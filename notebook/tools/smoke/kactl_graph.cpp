// DEPS: twosat clique Matching cover
// KACTL 2-SAT (incl. atMostOne), Maxclique, vertexCover, each vs brute force.
int main(){
    mt19937 rng(8128);
    int sat = 0, unsat = 0;
    rep(it, 0, 3000) {
        int n = 1 + rng() % 8, m = rng() % 14;
        vector<array<int, 2>> cl(m);
        TwoSat ts(n);
        for (auto& c : cl) {
            rep(k, 0, 2) { int v = rng() % n; c[k] = rng() % 2 ? v : ~v; }
            ts.either(c[0], c[1]);
        }
        vi amo;                                   // one atMostOne group
        rep(v, 0, n) if (rng() % 3 == 0) amo.push_back(rng() % 2 ? v : ~v);
        ts.atMostOne(amo);
        auto lit = [&](int mask, int l) { return l >= 0 ? (mask >> l & 1) : !(mask >> ~l & 1); };
        auto ok = [&](int mask) {
            for (auto& c : cl) if (!lit(mask, c[0]) && !lit(mask, c[1])) return false;
            int t = 0;
            for (ll l : amo) t += lit(mask, (int)l);
            return t <= 1;
        };
        bool any = false;
        rep(mask, 0, 1 << n) if (ok(mask)) any = true;
        bool got = ts.solve();
        if (got != any) { printf("2-SAT MISMATCH n=%d\n", n); return 1; }
        if (got) {
            int mask = 0;
            rep(i, 0, n) if (ts.values[i]) mask |= 1 << i;
            if (!ok(mask)) { printf("2-SAT assignment invalid\n"); return 1; }
            sat++;
        } else unsat++;
    }
    rep(it, 0, 300) {                             // max clique vs all subsets
        int n = 1 + rng() % 14;
        vb g(n);
        int p = rng() % 100;
        rep(i, 0, n) rep(j, i + 1, n) if ((int)(rng() % 100) < p) g[i][j] = g[j][i] = 1;
        int best = 0;
        rep(mask, 1, 1 << n) {
            bool cl = true;
            rep(i, 0, n) rep(j, i + 1, n) if ((mask >> i & 1) && (mask >> j & 1) && !g[i][j]) cl = false;
            if (cl) best = max(best, __builtin_popcount(mask));
        }
        vi c = Maxclique(g).maxClique();
        for (ll a : c) for (ll b : c) if (a != b && !g[a][b]) { printf("clique not a clique\n"); return 1; }
        if ((int)c.size() != best) { printf("clique MISMATCH got=%d want=%d\n", (int)c.size(), best); return 1; }
    }
    rep(it, 0, 500) {                             // vertex cover: valid, size == matching
        int n1 = 1 + rng() % 7, n2 = 1 + rng() % 7;
        Matching M(n1, n2);
        vector<pii> ed;
        rep(u, 0, n1) rep(v, 0, n2) if (rng() % 3 == 0) { M.addEdge(u, v); ed.push_back({u, v}); }
        int mm = M.maxMatching();
        auto [L, R] = vertexCover(M);
        set<ll> sl(all(L)), sr(all(R));
        for (auto [u, v] : ed) if (!sl.count(u) && !sr.count(v)) { printf("cover misses an edge\n"); return 1; }
        if ((int)(L.size() + R.size()) != mm) { printf("cover size != matching\n"); return 1; }
    }
    printf("OK %d sat + %d unsat 2-SAT (with atMostOne), max clique vs subsets, Konig cover\n", sat, unsat);
}
