// DEPS: simplex quad
// Simplex vs vertex enumeration on random 2-variable LPs (box-bounded, so the
// optimum is a vertex; negative b forces phase 1; some are infeasible).
// Plus KACTL's own usage example and Simpson on known integrals.
int main(){
    mt19937 rng(5772);
    int cnt = 0, infeas = 0;
    rep(it, 0, 3000) {
        int m = 1 + rng() % 5;
        vvd A; vd b;
        rep(i, 0, m) {
            A.push_back({(double)((ll)(rng() % 11) - 5), (double)((ll)(rng() % 11) - 5)});
            b.push_back((double)((ll)(rng() % 41) - 10));
        }
        A.push_back({1, 0}); b.push_back(20);       // box: x, y <= 20
        A.push_back({0, 1}); b.push_back(20);
        vd c = {(double)((ll)(rng() % 11) - 5), (double)((ll)(rng() % 11) - 5)};
        // brute: every intersection of two boundary lines (incl. x=0, y=0)
        vvd L = A; vd R = b;
        L.push_back({-1, 0}); R.push_back(0);
        L.push_back({0, -1}); R.push_back(0);
        double want = -1e18; bool any = false;
        rep(i, 0, (ll)L.size()) rep(j, i + 1, (ll)L.size()) {
            double det = L[i][0] * L[j][1] - L[i][1] * L[j][0];
            if (fabs(det) < 1e-12) continue;
            double x = (R[i] * L[j][1] - L[i][1] * R[j]) / det;
            double y = (L[i][0] * R[j] - R[i] * L[j][0]) / det;
            bool ok = true;
            rep(k, 0, (ll)L.size()) if (L[k][0] * x + L[k][1] * y > R[k] + 1e-7) ok = false;
            if (ok) { any = true; want = max(want, c[0] * x + c[1] * y); }
        }
        vd x;
        T got = LPSolver(A, b, c).solve(x);
        if (!any) {
            infeas++;
            if (got != -inf) { printf("simplex: expected infeasible, got %f\n", got); return 1; }
            continue;
        }
        if (fabs(got - want) > 1e-6) { printf("simplex MISMATCH got=%f want=%f\n", got, want); return 1; }
        // the returned x must be feasible and achieve the value
        rep(k, 0, m + 2) if (A[k][0] * x[0] + A[k][1] * x[1] > b[k] + 1e-6) {
            printf("simplex x infeasible\n"); return 1; }
        if (x[0] < -1e-6 || x[1] < -1e-6 || fabs(c[0] * x[0] + c[1] * x[1] - got) > 1e-6) {
            printf("simplex x wrong\n"); return 1; }
        cnt++;
    }
    {   // KACTL usage example: min x+y s.t. |x-y| <= 1, x+2y >= 4  ->  7/3
        vvd A = {{1,-1}, {-1,1}, {-1,-2}};
        vd b = {1,1,-4}, c = {-1,-1}, x;
        T val = LPSolver(A, b, c).solve(x);
        if (fabs(val + 7.0 / 3) > 1e-9) { printf("simplex example got %f\n", val); return 1; }
        vvd A2 = {{1, -1}}; vd b2 = {1}, c2 = {1, 1};        // unbounded
        if (LPSolver(A2, b2, c2).solve(x) != inf) { printf("simplex: expected unbounded\n"); return 1; }
    }
    double q1 = quad(0, 3, [](double x) { return x * x; });
    double q2 = quad(0, acos(-1.0), [](double x) { return sin(x); });
    if (fabs(q1 - 9) > 1e-9 || fabs(q2 - 2) > 1e-9) { printf("quad MISMATCH %f %f\n", q1, q2); return 1; }
    printf("OK %d feasible + %d infeasible LPs vs vertex enumeration, unbounded, quad\n", cnt, infeas);
}
