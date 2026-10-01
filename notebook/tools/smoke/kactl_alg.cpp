// DEPS: modbasics modlog bm det
// KACTL algebra: modLog, modSqrt, Berlekamp-Massey + linearRec, both dets,
// each against brute force.
static ll permDet(vector<vector<ll>> a) {        // sum over permutations
    int n = (int)a.size();
    vector<int> p(n); iota(all(p), 0);
    i128 s = 0;
    do {
        int inv = 0;
        rep(i, 0, n) rep(j, i + 1, n) if (p[i] > p[j]) inv++;
        i128 t = 1;
        rep(i, 0, n) t *= a[i][p[i]];
        s += inv % 2 ? -t : t;
    } while (next_permutation(all(p)));
    return (ll)(((s % mod) + mod) % mod);
}
int main(){
    mt19937 rng(2718);
    int cnt = 0;
    // modLog: every a, b for small m, prime or not
    rep(m, 2, 120) rep(a, 0, m) rep(b, 0, m) {
        ll want = -1, e = 1;
        rep(x, 1, 2 * m + 2) { e = e * a % m; if (e == b) { want = x; break; } }
        ll got = modLog(a, b, m);
        if (got != want) {
            printf("modLog MISMATCH a=%lld b=%lld m=%lld got=%lld want=%lld\n",
                   a, b, m, got, want);
            return 1;
        }
        cnt++;
    }
    // modSqrt: every a for primes below 600
    rep(p, 2, 600) {
        bool pr = true;
        for (ll d = 2; d * d <= p; d++) if (p % d == 0) pr = false;
        if (!pr) continue;
        rep(a, 0, p) {
            bool sq = false;
            rep(x, 0, p) if (x * x % p == a) sq = true;
            ll r = modSqrt(a, p);
            if (sq ? (r < 0 || r * r % p != a) : r != -1) {
                printf("modSqrt MISMATCH a=%lld p=%lld got=%lld\n", a, p, r);
                return 1;
            }
            cnt++;
        }
    }
    // BM: random recurrences of order <= 8, recover from 2n terms + check kth
    rep(it, 0, 500) {
        int n = 1 + rng() % 8;
        vi tr(n), S(n);
        rep(i, 0, n) { tr[i] = rng() % mod; S[i] = rng() % mod; }
        int T = 60;
        vi seq(S);
        rep(i, n, T) {
            ll v = 0;
            rep(j, 0, n) v = (v + tr[j] * seq[i - j - 1]) % mod;
            seq.push_back(v);
        }
        vi got = berlekampMassey(vi(seq.begin(), seq.begin() + 2 * n + 4));
        // the recovered recurrence must reproduce the whole sequence
        rep(i, (ll)got.size(), T) {
            ll v = 0;
            rep(j, 0, (ll)got.size()) v = (v + got[j] * seq[i - j - 1]) % mod;
            if (v != seq[i]) { printf("BM MISMATCH it=%lld\n", it); return 1; }
        }
        rep(q, 0, 5) {
            ll k = rng() % T;
            if (linearRec(vi(seq.begin(), seq.begin() + got.size()), got, k)
                != seq[k]) { printf("linearRec MISMATCH\n"); return 1; }
        }
        cnt++;
    }
    // Fibonacci at 10^18 vs fast doubling
    {
        ll k = 1000000000000000000LL;
        function<pii(ll)> fib = [&](ll x) -> pii {
            if (!x) return {0, 1};
            auto [a, b] = fib(x / 2);
            ll c = a * ((2 * b - a + mod) % mod) % mod, d = (a * a + b * b) % mod;
            return x % 2 ? pii{d, (c + d) % mod} : pii{c, d};
        };
        if (linearRec({0, 1}, {1, 1}, k) != fib(k).first) {
            printf("linearRec fib MISMATCH\n"); return 1;
        }
    }
    // determinants: mod version vs permutation expansion, double vs the same
    rep(it, 0, 3000) {
        int n = 1 + rng() % 6;
        vector<vector<ll>> a(n, vector<ll>(n));
        vector<vector<double>> d(n, vector<double>(n));
        rep(i, 0, n) rep(j, 0, n) { a[i][j] = (ll)(rng() % 11) - 5; d[i][j] = a[i][j]; }
        ll want = permDet(a);
        // exact integer value (|det| <= 6! * 5^6 fits easily) for the double check
        ll exact = want > mod / 2 ? want - mod : want;
        auto a2 = a;
        if (det(a2) != want) { printf("det(ll) MISMATCH n=%d\n", n); return 1; }
        double dd = det(d);                     // det destroys d
        if (fabs(dd - exact) > 1e-6 * max(1.0, fabs((double)exact))) {
            printf("det(double) MISMATCH n=%d got=%f want=%lld\n", n, dd, exact);
            return 1;
        }
        cnt++;
    }
    printf("OK %d cases: modLog, modSqrt, BM + linearRec (incl. fib(1e18)), det ll/double\n", cnt);
}
