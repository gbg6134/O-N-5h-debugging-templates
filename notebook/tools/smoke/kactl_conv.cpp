// DEPS: fft fst
// KACTL FFT: conv on doubles (exact ints) and convMod<1e9+7>, vs naive O(n^2).
// FST is tested here in its printed (AND) form; kactl_fst.sh does OR and XOR.
int main(){
    mt19937 rng(31415);
    const int M = 1000000007;
    int cnt = 0;
    rep(it, 0, 300) {
        int n = 1 + rng() % 200, m = 1 + rng() % 200;
        vd a(n), b(m);
        vi A(n), B(m);
        rep(i, 0, n) { a[i] = rng() % 100000; A[i] = rng() % M; }
        rep(i, 0, m) { b[i] = rng() % 100000; B[i] = rng() % M; }
        vd c = conv(a, b);
        vi C2 = convMod<M>(A, B);
        if ((int)c.size() != n + m - 1 || (int)C2.size() != n + m - 1) {
            printf("conv size MISMATCH\n"); return 1;
        }
        rep(k, 0, n + m - 1) {
            ll w = 0; i128 w2 = 0;
            rep(i, max(0LL, k - m + 1), min((ll)n, k + 1)) {
                w += (ll)a[i] * (ll)b[k - i];
                w2 += (i128)A[i] * B[k - i];
            }
            if (llround(c[k]) != w) { printf("conv MISMATCH\n"); return 1; }
            if (C2[k] != (ll)(w2 % M)) { printf("convMod MISMATCH\n"); return 1; }
        }
        cnt++;
    }
    // FST, AND convolution
    rep(it, 0, 300) {
        int lg = rng() % 8, n = 1 << lg;
        vi a(n), b(n);
        rep(i, 0, n) { a[i] = (ll)(rng() % 21) - 10; b[i] = (ll)(rng() % 21) - 10; }
        vi c = fstConv(a, b), w(n);
        rep(x, 0, n) rep(y, 0, n) w[x & y] += a[x] * b[y];
        if (c != w) { printf("FST(AND) MISMATCH n=%d\n", n); return 1; }
        cnt++;
    }
    printf("OK %d cases: conv(double) exact, convMod<1e9+7>, FST AND\n", cnt);
}
