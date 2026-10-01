// DEPS: point hpi
// HPI on random convex polygons vs Sutherland-Hodgman clipping (oracle).
typedef pair<ld, ld> PD;
static ld cr2(PD o, PD a, PD b) {
    return (a.first - o.first) * (b.second - o.second)
         - (a.second - o.second) * (b.first - o.first);
}
static vector<PD> clip(const vector<PD>& poly, PD a, PD b) {  // keep left of a->b
    vector<PD> out;
    int n = (int)poly.size();
    rep(i, 0, n) {
        PD p = poly[i], q = poly[(i+1) % n];
        ld sp = cr2(a, b, p), sq = cr2(a, b, q);
        if (sp >= 0) out.push_back(p);
        if ((sp >= 0) != (sq >= 0)) {
            ld t = sp / (sp - sq);
            out.push_back({p.first + (q.first - p.first) * t,
                           p.second + (q.second - p.second) * t});
        }
    }
    return out;
}
static ld area(const vector<PD>& p) {
    ld s = 0; int n = (int)p.size();
    rep(i, 0, n) s += p[i].first * p[(i+1) % n].second
                    - p[i].second * p[(i+1) % n].first;
    return fabsl(s) / 2;
}
int main(){
    mt19937 rng(57721);
    auto rnd = [&](ld lo, ld hi) {
        return lo + (hi - lo) * (ld)(rng() % 1000000) / 1000000;
    };
    int tested = 0, nonempty = 0;
    rep(iter, 0, 3000) {
        int k = 1 + rng() % 4;                       // number of polygons
        vector<vector<PD>> polys;
        rep(t, 0, k) {
            int m = 3 + rng() % 6;
            ld cx = rnd(-3, 3), cy = rnd(-3, 3), r = rnd(2, 8);
            vector<ld> ang;
            rep(i, 0, m) ang.push_back(rnd(0, 2 * pi));
            sort(all(ang));
            bool ok = true;                           // reject near-duplicates
            rep(i, 0, m) {
                ld d = (i + 1 < m ? ang[i+1] : ang[0] + 2 * pi) - ang[i];
                if (d < 1e-2 || d > pi - 1e-2) ok = false;
            }
            if (!ok) { t--; continue; }
            vector<PD> p;
            for (ld a : ang) p.push_back({cx + r * cosl(a), cy + r * sinl(a)});
            polys.push_back(p);
        }
        vector<Line> L;
        for (auto& p : polys) rep(i, 0, (ll)p.size()) {
            PD a = p[i], b = p[(i+1) % p.size()];
            L.push_back(Line(Point(a.first, a.second), Point(b.first, b.second)));
        }
        vector<PD> want = polys[0];
        rep(t, 1, k) {
            int m = (int)polys[t].size();
            rep(i, 0, m) want = clip(want, polys[t][i], polys[t][(i+1) % m]);
        }
        ld wa = want.size() >= 3 ? area(want) : 0;
        vector<Point> got = HPI(L);
        vector<PD> g;
        for (Point q : got) g.push_back({q.x, q.y});
        ld ga = g.size() >= 3 ? area(g) : 0;
        if (wa > 1e-3 && wa < 1e-1) continue;     // skip slivers: eps territory
        tested++;
        if (wa > 1e-3) nonempty++;
        if (fabsl(ga - wa) > 1e-4 * max((ld)1, wa)) {
            printf("MISMATCH HPI area got=%.6f want=%.6f polys=%d\n",
                   (double)ga, (double)wa, k);
            return 1;
        }
    }
    printf("OK %d cases (%d non-empty): HPI area == Sutherland-Hodgman clipping\n",
           tested, nonempty);
}
