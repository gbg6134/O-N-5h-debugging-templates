// ===========================================================================
// PathDP (bitmask)   O(2^n * n^2)   mem 2^n * n
// Folds every simple path covering exactly a vertex set into one value,
// n <= ~18. Directed; for undirected fill both directions. No recursion.
// The constructor does all the work, every call below is a lookup.
//
//   PathDP p(w);           // w[u][v] = weight, PathDP::INF = no edge
//   PathDP p(w, s);        // 2nd arg = fixed start, -1 (default) = any
//
//   p.val(mask, v)    // fold of the paths covering EXACTLY mask, ending at v
//   p.hamPath()       // fold over v of val(full, v)
//   p.hamCycle(s)     // fold of val(full, v) extended by w[v][s]; needs start s
//   p.anyPath()       // fold over every mask and every v
//   p.path(mask, v)   // vertex order behind val(mask, v); MIN / MAX only
//   p.dp[mask][v]     // the table; p.par[mask][v] = predecessor, -1 at the end
//
//   PathDP::induced(g, vs)  // cut a piece out of a big graph, see below
//
// CONFIG -- edit the 5 lines below, like Seg.
//   T           what one path is worth, and what every call returns.
//   ext(x, wt)  a path worth x is extended by one edge of weight wt;
//               return what the longer path is worth.
//   merge(a,b)  two different paths reach the same (mask, v); return the one
//               value that stands for both. Associative AND commutative.
//   UNIT        what a one-vertex path is worth, before any edge.
//   NONE        "no path at all": merge(x, NONE) = x and ext(NONE, wt) = NONE.
//               Every call returns it when nothing exists.
//   ext must distribute over merge -- ext(merge(a,b),wt) =
//   merge(ext(a,wt),ext(b,wt)) -- or the fold is wrong.
//
//   want                T       UNIT  NONE   ext(x,wt)    merge(a,b)
//   shortest path       ll      0     INF    x + wt       min(a, b)
//   longest path        ll      0     -INF   x + wt       max(a, b)
//   count paths         ll      1     0      x            a + b
//   count mod p         ll      1     0      x            (a + b) % p
//   min bottleneck      ll      0     INF    max(x, wt)   min(a, b)
//   max probability     double  1     0      x * wt       max(a, b)
// Counting ignores wt, so w only has to say which edges exist. On an
// undirected graph every path is found in both directions, so counts of
// hamPath / hamCycle come out 2x -- halve them.
// path() needs merge to SELECT one of its arguments, so it is for MIN / MAX
// only; under a summing merge par holds the last contributor, not a path.
//
// INDUCED -- runs the dp on a subset of a big graph. g is the global
// adjacency list (a LIST), vs the global ids you keep, in any order. Back
// comes the k x k weight MATRIX of just those vertices, which is the form
// the constructor wants, renumbered so that local i is vs[i]:
//
//   vi vs = {9, 2, 5};
//   PathDP p(PathDP::induced(g, vs));
//   //  local  0  1  2   ==   global  9  2  5
//
// From there EVERYTHING is local: n is k, bit i of a mask is local i, and
// val / path / hamPath all speak local ids. vs is the way back, vs[i]
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
//   val(mask, v) is EXACTLY mask, not a subset; v must be inside mask.
//   anyPath() also folds in the n one-vertex paths -- under a counting
//   config that is n extra, under MAX it makes the answer at least UNIT.
//   hamCycle(s) means nothing unless the table was built with start == s.
//   induced() hardcodes weight 1; for real weights build w yourself, and
//   remember its output is local while your problem is global.
//   A fixed start changes the whole table, so sweeping starts rebuilds it:
//   k starts cost k * 2^n * n^2.
//
// MEM -- 2^n * n of T for dp plus the same count of char for par. Measured
// at -O2 with T = ll: n=16 0.03 s 18 MB, n=18 0.16 s 64 MB, n=20 1.7 s
// 260 MB. So n <= 18 in practice.
// ===========================================================================
struct PathDP {
    using T = ll;
    static T ext(T x, ll wt) { return x + wt; }
    static T merge(T a, T b) { return min(a, b); }
    static constexpr T UNIT = 0;
    static constexpr T NONE = 1e18;
    static constexpr ll INF = 1e18;      // missing edge in w

    int n, start;
    matrix w; vector<vector<T>> dp; vector<vector<char>> par;

    PathDP(const matrix& wt, int st = -1)
            : n(wt.size()), start(st), w(wt) {
        dp.assign(1 << n, vector<T>(n, NONE));
        par.assign(1 << n, vector<char>(n, -1));
        rep(v, 0, n) if (start < 0 || v == start)
            dp[1 << v][v] = UNIT;
        rep(m, 1, 1 << n) rep(u, 0, n) {
            T cur = dp[m][u];
            if (!(m >> u & 1) || cur == NONE) continue;
            rep(v, 0, n) if (!(m >> v & 1) && w[u][v] < INF) {
                ll t = m | 1LL << v;
                T nv = merge(dp[t][v], ext(cur, w[u][v]));
                if (nv != dp[t][v]) {
                    dp[t][v] = nv; par[t][v] = (char)u;
                }
            }
        }
    }
    T val(int m, int v) { return dp[m][v]; }
    T hamPath() {
        T r = NONE; ll f = (1 << n) - 1;
        rep(v, 0, n) r = merge(r, dp[f][v]);
        return r;
    }
    T hamCycle(int s) {
        T r = NONE; ll f = (1 << n) - 1;
        rep(v, 0, n) if (dp[f][v] != NONE && w[v][s] < INF)
            r = merge(r, ext(dp[f][v], w[v][s]));
        return r;
    }
    T anyPath() {
        T r = NONE;
        rep(m, 1, 1 << n) rep(v, 0, n) r = merge(r, dp[m][v]);
        return r;
    }
    vi path(int m, int v) {          // MIN / MAX only
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