// DEPS: mo mincut dncdp minrot
// KACTL: Mo (distinct count), Stoer-Wagner, D&C DP, minRotation, each vs brute.
vi pre;                                     // prefix sums for cost
ll cost(int k, int i) { ll s = pre[i] - pre[k]; return s * s; }
int main(){
    mt19937 rng(1618);

    // Mo: distinct values in [l, r)
    rep(it, 0, 200) {
        int n = 1 + rng() % 60, q = 1 + rng() % 60, V = 1 + rng() % 10;
        a.assign(n, 0); cnt.assign(V, 0); nd = 0;
        rep(i, 0, n) a[i] = rng() % V;
        vector<pii> Q(q);
        for (auto& [l, r] : Q) {
            l = rng() % (n + 1); r = rng() % (n + 1);
            if (l > r) swap(l, r);
        }
        vi got = mo(Q);
        rep(i, 0, q) {
            set<ll> s(a.begin() + Q[i].first, a.begin() + Q[i].second);
            if (got[i] != (ll)s.size()) { printf("Mo MISMATCH\n"); return 1; }
        }

    }
    // Stoer-Wagner vs every bipartition
    rep(it, 0, 500) {
        int n = 2 + rng() % 7;
        vector<vi> mat(n, vi(n));
        rep(i, 0, n) rep(j, i + 1, n) if (rng() % 2)
            mat[i][j] = mat[j][i] = rng() % 1000000000000LL;
        ll want = LLONG_MAX;
        rep(mask, 1, (1LL << n) - 1) {
            ll w = 0;
            rep(i, 0, n) rep(j, 0, n) if ((mask >> i & 1) && !(mask >> j & 1)) w += mat[i][j];
            want = min(want, w);
        }
        auto [w, side] = globalMinCut(mat);
        ll chk = 0;                          // the returned side must achieve it
        vector<char> in(n, 0);
        for (ll v : side) in[v] = 1;
        rep(i, 0, n) rep(j, 0, n) if (in[i] && !in[j]) chk += mat[i][j];
        if (w != want || chk != want || side.empty() || (int)side.size() == n) {
            printf("mincut MISMATCH n=%d got=%lld want=%lld\n", n, w, want);
            return 1;
        }
    }
    // D&C DP: split into G groups minimising sum of (group sum)^2
    rep(it, 0, 300) {
        int n = 1 + rng() % 40, G = 1 + rng() % n;
        pre.assign(n + 1, 0);
        rep(i, 0, n) pre[i + 1] = pre[i] + rng() % 100;
        vector<vi> dp(G + 1, vi(n + 1, INF));
        dp[0][0] = 0;
        rep(g, 1, G + 1) rep(i, 1, n + 1) rep(k, 0, i)
            if (dp[g - 1][k] < INF) dp[g][i] = min(dp[g][i], dp[g - 1][k] + cost(k, i));
        prv.assign(n + 1, INF); prv[0] = 0;
        rep(g, 1, G + 1) {
            cur.assign(n + 1, INF);
            DP().solve(1, n + 1);
            prv = cur;
        }
        if (prv[n] != dp[G][n]) {
            printf("D&C DP MISMATCH n=%d G=%d got=%lld want=%lld\n", n, G, prv[n], dp[G][n]);
            return 1;
        }
    }
    // minRotation vs trying every rotation
    rep(it, 0, 3000) {
        int n = 1 + rng() % 12;
        string s(n, 'a');
        for (char& c : s) c = 'a' + rng() % 3;
        string best = s;
        rep(i, 0, n) best = min(best, s.substr(i) + s.substr(0, i));
        int r = minRotation(s);
        if (s.substr(r) + s.substr(0, r) != best) { printf("minRotation MISMATCH %s\n", s.c_str()); return 1; }
    }
    printf("OK: Mo == naive distinct, Stoer-Wagner == all cuts, D&C DP == O(Gn^2), minRotation\n");
}
