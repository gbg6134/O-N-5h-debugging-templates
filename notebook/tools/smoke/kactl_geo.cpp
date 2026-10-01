// DEPS: point lattice hullkactl mec
// KACTL geometry ported onto Ruiming's points, each vs brute force:
// hullCCW, hullDiameter, inHull (strict and not), mec, polygonCut, tangents.
static ld areaOf(const vector<Point>& p) {
    ld s = 0;
    rep(i, 0, (ll)p.size()) s += Cross(p[i], p[(i + 1) % p.size()]);
    return s / 2;
}
int main(){
    mt19937 rng(4669);
    eps = 1e-6;
    int cnt = 0;
    rep(it, 0, 2000) {
        int n = 1 + rng() % 25, C = 1 + rng() % 12;
        vector<PL> p(n);
        for (auto& q : p) q = PL((ll)(rng() % (2 * C + 1)) - C, (ll)(rng() % (2 * C + 1)) - C);
        vector<PL> h = hullCCW(p);
        // hull: strictly convex CCW, every input point inside or on it
        int m = (int)h.size();
        if (m >= 3) {
            rep(i, 0, m) if (Area2(h[i], h[(i + 1) % m], h[(i + 2) % m]) <= 0) {
                printf("hullCCW not strictly convex CCW\n"); return 1; }
            for (auto& q : p) rep(i, 0, m) if (Area2(h[i], h[(i + 1) % m], q) < 0) {
                printf("hullCCW misses a point\n"); return 1; }
        }
        // diameter vs all pairs
        ll want = 0;
        for (auto& a : p) for (auto& b : p) want = max(want, Dist2(a, b));
        auto d = hullDiameter(h);
        if (Dist2(d[0], d[1]) != want) {
            printf("hullDiameter MISMATCH got=%lld want=%lld\n", Dist2(d[0], d[1]), want);
            return 1;
        }
        // inHull vs the exact oracle, on proper (>= 3 point) hulls
        if (m >= 3) rep(t, 0, 30) {
            PL q((ll)(rng() % (2 * C + 3)) - C - 1, (ll)(rng() % (2 * C + 3)) - C - 1);
            bool inS = true, inN = true;
            rep(i, 0, m) {
                ll c = Area2(h[i], h[(i + 1) % m], q);
                if (c <= 0) inS = false;
                if (c < 0) inN = false;
            }
            if (inHull(h, q, true) != inS || inHull(h, q, false) != inN) {
                printf("inHull MISMATCH\n"); return 1; }
        }
        cnt++;
    }
    // mec: contains every point, and no circle through 2 or 3 of them that
    // contains everything is smaller
    rep(it, 0, 300) {
        int n = 1 + rng() % 12;
        vector<Point> p(n);
        for (auto& q : p) q = Point(rng() % 1000 / 10.0, rng() % 1000 / 10.0);
        auto [o, r] = mec(p);
        for (auto& q : p) if (Dist(o, q) > r + 1e-7) { printf("mec misses a point\n"); return 1; }
        ld best = n == 1 ? 0 : 1e18;
        auto tryC = [&](Point c) {
            ld rr = 0;
            for (auto& q : p) rr = max(rr, Dist(c, q));
            best = min(best, rr);
        };
        rep(i, 0, n) rep(j, i + 1, n) {
            tryC((p[i] + p[j]) / 2);
            rep(k, j + 1, n) if (fabsl(Cross(p[j] - p[i], p[k] - p[i])) > 1e-9)
                tryC(ccCenter(p[i], p[j], p[k]));
        }
        if (fabsl(r - best) > 1e-7) { printf("mec MISMATCH got=%Lf want=%Lf\n", r, best); return 1; }
        cnt++;
    }
    // polygonCut: the two sides partition the area
    rep(it, 0, 1000) {
        vector<PL> pl(3 + rng() % 20);
        for (auto& q : pl) q = PL(rng() % 200, rng() % 200);
        vector<PL> h = hullCCW(pl);
        if (h.size() < 3) continue;
        vector<Point> poly;
        for (auto& q : h) poly.push_back(Point(q.x, q.y));
        Point s(rng() % 200, rng() % 200), e(rng() % 200, rng() % 200);
        if (s == e) continue;
        ld A = areaOf(poly), R = areaOf(polygonCut(poly, s, e)), L = areaOf(polygonCut(poly, e, s));
        if (fabsl(A - R - L) > 1e-6) { printf("polygonCut MISMATCH\n"); return 1; }
        // every vertex of the kept part is right of (or on) s->e
        for (auto& q : polygonCut(poly, s, e)) if (Cross(e - s, q - s) > 1e-6) {
            printf("polygonCut kept the wrong side\n"); return 1; }
        cnt++;
    }
    // tangents: touch both circles, perpendicular to both radii
    rep(it, 0, 1000) {
        Point c1(rng() % 100, rng() % 100), c2(rng() % 100, rng() % 100);
        ld r1 = 1 + rng() % 30, r2 = (ll)(rng() % 31) - 15;   // r2 < 0: inner
        for (auto [t1, t2] : tangents(c1, r1, c2, r2)) {
            if (fabsl(Dist(t1, c1) - r1) > 1e-6 || fabsl(Dist(t2, c2) - fabsl(r2)) > 1e-6
                || fabsl(Dot(t2 - t1, t1 - c1)) > 1e-5 || fabsl(Dot(t2 - t1, t2 - c2)) > 1e-5) {
                printf("tangents MISMATCH\n"); return 1; }
        }
        cnt++;
    }
    printf("OK %d cases: hullCCW, hullDiameter, inHull, mec, polygonCut, tangents\n", cnt);
}
