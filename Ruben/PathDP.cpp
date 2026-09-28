// ===========================================================================
// PathDP (bitmask)   O(2^n * n^2)   mem 2^n * n
// Best simple path covering exactly a given vertex set, n <= ~18. Directed;
// for undirected fill both directions. No recursion. The constructor does
// all the work, every call below is a lookup.
//
//   PathDP p(w, PathDP::MAX);      // w[u][v] = weight, PathDP::INF = no edge
//   PathDP p(w, PathDP::MIN, s);   // 3rd arg = fixed start, -1 (default) = any
//
//   p.best(mask, v)   // best path covering EXACTLY mask, ending at v
//   p.hamPath()       // best over v of best(full, v)
//   p.hamCycle(s)     // best(full, v) + w[v][s]; needs the fixed start s
//   p.longestAny()    // best over every mask and every v
//   p.path(mask, v)   // the vertex order behind best(mask, v), in order
//   p.bad             // "no such path": what every query returns when none
//   p.bt(a, b)        // is a better than b under the mode, for your own folds
//   p.dp[mask][v]     // the table; p.par[mask][v] = predecessor, -1 at the end
//
//   PathDP::induced(g, vs)  // cut a piece out of a big graph, see below
//
// MODE -- PathDP::MIN or PathDP::MAX, fixed at construction, decides what
// "best" means in every query. Shortest hamiltonian path is MIN; longest
// simple path counted in edges is every weight 1 with MAX.
// bad is INF under MIN and -INF under MAX. Test a result against p.bad
// before using it, and never call path() on a bad state.
//
// INDUCED -- runs the dp on a subset of a big graph. g is the global
// adjacency list (a LIST), vs the global ids you keep, in any order. Back
// comes the k x k weight MATRIX of just those vertices, which is the form
// the constructor wants, renumbered so that local i is vs[i]:
//
//   vi vs = {9, 2, 5};
//   PathDP p(PathDP::induced(g, vs), PathDP::MAX);
//   //  local  0  1  2   ==   global  9  2  5
//
// From there EVERYTHING is local: n is k, bit i of a mask is local i, and
// best / path / hamPath all speak local ids. vs is the way back, vs[i]
// being the global id of local i:
//
//   for (ll x : p.path(bm, bv)) cout << vs[x] << ' ';   // global order
//
// Keep vs in a variable: SCC::members() rebuilds the whole grouping on
// every call, so take it once with auto mem = s.members(), then pass
// mem[c] and translate with mem[c][x].
//
// An edge survives only if BOTH ends are in vs; direction is kept, every
// survivor gets weight 1, parallel edges collapse into one. g and vs hold
// any integer type, so SCC::members()[c] and 2ecc vertex lists go in as is.
// vs must hold DISTINCT ids, each < g.size(): a repeat silently becomes two
// vertices that a path may visit both of. A self-loop lands on w[i][i] and
// is never read by the dp, so it is harmless.
//
// GOTCHAS
//   best(mask, v) is EXACTLY mask, not a subset; v must be inside mask.
//   longestAny() is a MAX tool -- under MIN it is 0, one vertex beats all.
//   hamCycle(s) means nothing unless the table was built with start == s.
//   induced() hardcodes weight 1; for real weights build w yourself, and
//   remember its output is local while your problem is global.
//   A fixed start changes the whole table, so sweeping starts rebuilds it:
//   k starts cost k * 2^n * n^2.
//
// MEM -- 2^n * n ll for dp plus the same count of char for par. Measured at
// -O2: n=16 0.03 s 18 MB, n=18 0.16 s 64 MB, n=20 1.7 s 260 MB. So n <= 18
// in practice; n = 20 needs a flat array and an int dp, if at all.
// ===========================================================================
struct PathDP {
    enum { MIN, MAX };
    static constexpr ll INF = 1e18;

    int n, mode, start; ll bad;
    matrix w, dp; vector<vector<char>> par;

    PathDP(const matrix& wt, int md, int st = -1)
            : n(wt.size()), mode(md), start(st),
              bad(md == MIN ? INF : -INF), w(wt) {
        dp.assign(1 << n, vi(n, bad));
        par.assign(1 << n, vector<char>(n, -1));
        rep(v, 0, n) if (start < 0 || v == start)
            dp[1 << v][v] = 0;
        rep(m, 1, 1 << n) rep(u, 0, n) {
            ll cur = dp[m][u];
            if (!(m >> u & 1) || cur == bad) continue;
            rep(v, 0, n) if (!(m >> v & 1) && w[u][v] < INF) {
                ll c = cur + w[u][v], t = m | 1LL << v;
                if (bt(c, dp[t][v])) {
                    dp[t][v] = c; par[t][v] = (char)u;
                }
            }
        }
    }
    bool bt(ll a, ll b) {
        return mode == MIN ? a < b : a > b;
    }
    ll best(int m, int v) { return dp[m][v]; }
    ll hamPath() {
        ll r = bad, f = (1 << n) - 1;
        rep(v, 0, n) if (bt(dp[f][v], r)) r = dp[f][v];
        return r;
    }
    ll hamCycle(int s) {
        ll r = bad, f = (1 << n) - 1;
        rep(v, 0, n) if (dp[f][v] != bad && w[v][s] < INF)
            if (bt(dp[f][v] + w[v][s], r))
                r = dp[f][v] + w[v][s];
        return r;
    }
    ll longestAny() {
        ll r = bad;
        rep(m, 1, 1 << n) rep(v, 0, n)
            if (bt(dp[m][v], r)) r = dp[m][v];
        return r;
    }
    vi path(int m, int v) {
        vi p;
        while (v >= 0) {
            p.push_back(v);
            int u = par[m][v]; m ^= 1 << v; v = u;
        }
        reverse(p.begin(), p.end());
        return p;
    }
    template<class G, class V>    // g, vs: any integer type
    static matrix induced(const G& g, const V& vs) {
        int k = (int)vs.size();
        vi idx(g.size(), -1);
        rep(i, 0, k) idx[vs[i]] = i;
        matrix w(k, vi(k, INF));
        rep(i, 0, k) for (ll u : g[vs[i]])
            if (idx[u] >= 0) w[i][idx[u]] = 1;
        return w;
    }
};