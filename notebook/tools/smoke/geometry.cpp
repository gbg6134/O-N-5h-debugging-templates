// DEPS: point seg poly hull lattice closest circle
// exact oracle for segment intersection, non-degenerate segments only
static ll cr(ll ax, ll ay, ll bx, ll by) { return ax * by - ay * bx; }
static bool oracle(PointLL a, PointLL b, PointLL c, PointLL d) {
    PointLL ab = b - a, cd = d - c, ac = c - a;
    ll den = cr(ab.x, ab.y, cd.x, cd.y);
    if (den != 0) {
        ll tn = cr(ac.x, ac.y, cd.x, cd.y), sn = cr(ac.x, ac.y, ab.x, ab.y);
        auto in01 = [](ll num, ll de) {
            if (de < 0) { num = -num; de = -de; }
            return num >= 0 && num <= de;
        };
        return in01(tn, den) && in01(sn, den);
    }
    if (cr(ac.x, ac.y, ab.x, ab.y) != 0) return false;   // parallel, apart
    ll R = ab.x * ab.x + ab.y * ab.y;
    ll p1 = ac.x * ab.x + ac.y * ab.y;
    PointLL ad = d - a;
    ll p2 = ad.x * ab.x + ad.y * ab.y;
    if (p1 > p2) swap(p1, p2);
    return max((ll)0, p1) <= min(R, p2);
}
static Point F(PointLL q) { return Point(q.x, q.y); }
// exact point-in-polygon oracle (crossing number), 0-indexed polygon
static int inPolyOracle(const vector<PointLL>& p, PointLL q) {
    int n = (int)p.size(), c = 0;
    rep(i, 0, n) {
        PointLL a = p[i], b = p[(i+1) % n];
        PointLL qa = a - q, qb = b - q;
        if (cr(qa.x, qa.y, qb.x, qb.y) == 0
            && qa.x * qb.x + qa.y * qb.y <= 0) return 2;  // boundary
        if (a.y > b.y) swap(a, b);
        PointLL ba = b - a, qa2 = q - a;
        if (a.y <= q.y && q.y < b.y && cr(ba.x, ba.y, qa2.x, qa2.y) > 0)
            c ^= 1;
    }
    return c;
}
Point P1[MAXN], CH[MAXN];
PointLL PL[MAXN];
int main(){
    mt19937 rng(161803);
    // --- Cross_Segment vs exact oracle, tiny coordinates so cases repeat
    rep(iter, 0, 200000) {
        auto rp = [&]{ return PointLL(rng() % 5, rng() % 5); };
        PointLL a = rp(), b = rp(), c = rp(), d = rp();
        if (a == b || c == d) continue;
        if (Cross_Segment(F(a), F(b), F(c), F(d)) != oracle(a, b, c, d)) {
            printf("MISMATCH Cross_Segment (%lld,%lld)-(%lld,%lld) "
                   "(%lld,%lld)-(%lld,%lld)\n", a.x, a.y, b.x, b.y,
                   c.x, c.y, d.x, d.y);
            return 1;
        }
    }
    // --- Convex_Hull keeps collinear boundary points: compare the point
    //     set with an O(n^3) oracle (q on the boundary iff some line
    //     through q has every point on one closed side)
    rep(iter, 0, 3000) {
        int n = 1 + rng() % 9;
        vector<PointLL> v;
        rep(i, 0, n) v.push_back(PointLL(rng() % 8, rng() % 8));
        sort(all(v), [](PointLL a, PointLL b){
            return tie(a.x, a.y) < tie(b.x, b.y); });
        v.erase(unique(all(v)), v.end());
        n = (int)v.size();
        set<pii> want;
        rep(i, 0, n) {
            bool on = (n <= 2);
            rep(j, 0, n) if (j != i && !on) {
                bool side = true;
                rep(k, 0, n)
                    if (Cross(v[j] - v[i], v[k] - v[i]) < 0) side = false;
                if (side) on = true;
            }
            if (on) want.insert({v[i].x, v[i].y});
        }
        rep(i, 0, n) P1[i+1] = F(v[i]);
        shuffle(P1 + 1, P1 + n + 1, rng);
        ll h = Convex_Hull(P1, n, CH);
        set<pii> got;
        rep(i, 1, h + 1) got.insert({(ll)CH[i].x, (ll)CH[i].y});
        if (got != want || (ll)got.size() != h) {
            printf("MISMATCH Convex_Hull n=%d got %zu want %zu\n",
                   n, got.size(), want.size());
            return 1;
        }
    }
    // --- Point_in_polygon (float and exact) vs oracle on star polygons
    rep(iter, 0, 1500) {
        int n = 3 + rng() % 7;
        vector<PointLL> v;
        set<pii> seen;
        while ((int)v.size() < n) {
            PointLL q((ll)(rng() % 21) - 10, (ll)(rng() % 21) - 10);
            if (q.x == 0 && q.y == 0) continue;
            if (seen.count({q.x, q.y})) continue;
            seen.insert({q.x, q.y}); v.push_back(q);
        }
        sort(all(v), [](PointLL a, PointLL b){
            return atan2l(a.y, a.x) < atan2l(b.y, b.x); });
        bool ok = true;                  // need distinct angles: simple
        rep(i, 0, n) {
            PointLL a = v[i], b = v[(i+1) % n];
            if (Cross(a, b) <= 0) ok = false;
        }
        if (!ok) continue;
        rep(i, 0, n) { PL[i+1] = v[i]; P1[i+1] = F(v[i]); }
        if (fabsl(Polygon_area(P1, n) - fabsl(Polygon_area2(PL, n)) / 2.0L)
            > 1e-9) {
            printf("MISMATCH Polygon_area %.3f %.3f n=%d\n",
                   (double)Polygon_area(P1, n),
                   (double)(fabsl(Polygon_area2(PL, n)) / 2.0L), n);
            return 1;
        }
        rep(qx, -11, 12) rep(qy, -11, 12) {
            PointLL q(qx, qy);
            int want = inPolyOracle(v, q);
            ll e = Point_in_polygon(q, PL, n);
            ll f = Point_in_polygon(F(q), P1, n);
            int ge = e >= 2 ? 2 : (int)e, gf = f >= 2 ? 2 : (int)f;
            if (ge != want) { puts("MISMATCH Point_in_polygon exact");
                              return 1; }
            if (gf != want) { puts("MISMATCH Point_in_polygon float");
                              return 1; }
        }
    }
    // --- closest pair vs O(n^2)
    rep(iter, 0, 2000) {
        int n = 2 + rng() % 40;
        vector<PointLL> v;
        rep(i, 0, n) v.push_back(PointLL(rng() % 200, rng() % 200));
        auto [best, pr] = closest(v);
        ll want = LLONG_MAX;
        rep(i, 0, n) rep(j, i+1, n) want = min(want, Dist2(v[i], v[j]));
        if (best != want) {
            printf("MISMATCH closest got=%lld want=%lld\n",
                   (long long)best, (long long)want); return 1;
        }
        if (Dist2(pr.first, pr.second) != best) {
            puts("MISMATCH closest returned pair"); return 1;
        }
    }
    // --- circles: every returned point lies on both objects
    rep(iter, 0, 20000) {
        Point c1((ld)(rng() % 21) - 10, (ld)(rng() % 21) - 10);
        Point c2((ld)(rng() % 21) - 10, (ld)(rng() % 21) - 10);
        ld r1 = 1 + rng() % 10, r2 = 1 + rng() % 10;
        for (Point q : circleCircle(c1, r1, c2, r2)) {
            if (fabsl(Dist(q, c1) - r1) > 1e-6
                || fabsl(Dist(q, c2) - r2) > 1e-6) {
                puts("MISMATCH circleCircle point"); return 1;
            }
        }
        Point a((ld)(rng() % 21) - 10, (ld)(rng() % 21) - 10);
        Point b((ld)(rng() % 21) - 10, (ld)(rng() % 21) - 10);
        if (a.x == b.x && a.y == b.y) continue;
        for (Point q : circleLine(c1, r1, a, b)) {
            if (fabsl(Dist(q, c1) - r1) > 1e-6
                || Dis_point_line(q, Line(a, b)) > 1e-6) {
                puts("MISMATCH circleLine point"); return 1;
            }
        }
    }
    puts("OK Cross_Segment vs exact oracle (200k), Convex_Hull vs O(n^3), "
         "Point_in_polygon float+exact vs oracle, Polygon_area, "
         "closest vs O(n^2), circles");
}
