// DEPS: frac
// zero_one_partition vs brute force over every set of k items to drop.
int main(){
    mt19937 rng(14142);
    eps = 1e-6;                                  // what his file uses
    int tested = 0;
    rep(iter, 0, 3000) {
        n = 1 + rng() % 10;
        k = rng() % n;                           // keep at least one item
        rep(i, 1, n + 1) {
            b[i] = 1 + rng() % 20;
            a[i] = rng() % (b[i] + 1);           // ratio in [0,1] <= n
        }
        ld best = -1;
        rep(mask, 0, 1LL << n) {
            if (__builtin_popcountll(mask) != n - k) continue;
            ll sa = 0, sb = 0;
            rep(i, 0, n) if (mask >> i & 1) { sa += a[i+1]; sb += b[i+1]; }
            best = max(best, (ld)sa / sb);
        }
        ld want100 = 100 * best;
        ll got = zero_one_partition();
        // skip answers that sit on a rounding boundary (x.5)
        if (fabsl(want100 - floorl(want100) - 0.5) < 1e-4) continue;
        tested++;
        if (got != (ll)floorl(want100 + 0.5)) {
            printf("MISMATCH n=%lld k=%lld got=%lld want=%.6f\n",
                   n, k, got, (double)want100);
            return 1;
        }
    }
    printf("OK %d cases: zero_one_partition == brute force over subsets\n", tested);
}
