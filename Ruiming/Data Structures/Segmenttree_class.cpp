//UPDATERA ARRAY STORLEKEN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <bits/stdc++.h>
using namespace std;
 
typedef long long ll;
typedef long double ld;
typedef __int128 i128;
ld eps = 1e-9;
 
const ll MOD=998244353;
const ll INF=1e15;
const ll MAXN=206;//UPDATERA ARRAY STORLEKEN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
ll t,a[MAXN],ans,n,q;

struct SegTree {
    int n; // number of elements (0-indexed)
    vector<ll> tree, tag;
 
    SegTree() {}
    SegTree(const vector<ll>& arr) { init(arr); }
 
    // Helper: ensure value is in [0, MOD-1]
    static ll norm(ll x) {
        x %= MOD;
        if (x < 0) x += MOD;
        return x;
    }
 
    void init(const vector<ll>& arr) {
        n = (int)arr.size();
        tree.assign(4 * n + 5, 0);
        tag.assign(4 * n + 5, 0);
        build(1, 0, n - 1, arr);
    }
 
    inline int lc(int p) { return p << 1; }
    inline int rc(int p) { return p << 1 | 1; }
 
    void push_up(int p) {
        tree[p] = (tree[lc(p)] + tree[rc(p)]) % MOD;
    }
 
    void build(int p, int l, int r, const vector<ll>& arr) {
        tag[p] = 0;
        if (l == r) {
            tree[p] = norm(arr[l]); // store positive modulo
            return;
        }
        int mid = (l + r) >> 1;
        build(lc(p), l, mid, arr);
        build(rc(p), mid + 1, r, arr);
        push_up(p);
    }
 
    void apply_add(int p, int l, int r, ll k) {
        k = norm(k); // normalize k to positive
        tag[p] = (tag[p] + k) % MOD;
        tree[p] = (tree[p] + k * ((r - l + 1) % MOD)) % MOD;
    }
 
    void push_down(int p, int l, int r) {
        if (tag[p] == 0) return;
        int mid = (l + r) >> 1;
        apply_add(lc(p), l, mid, tag[p]);
        apply_add(rc(p), mid + 1, r, tag[p]);
        tag[p] = 0;
    }
 
    void update(int ql, int qr, ll k, int p, int l, int r) {
        if (ql <= l && r <= qr) {
            apply_add(p, l, r, k);
            return;
        }
        push_down(p, l, r);
        int mid = (l + r) >> 1;
        if (ql <= mid) update(ql, qr, k, lc(p), l, mid);
        if (qr > mid)  update(ql, qr, k, rc(p), mid + 1, r);
        push_up(p);
    }
 
    // public update: add k to range [l, r] (0-indexed, inclusive)
    void update(int l, int r, ll k) {
        if (l > r) return;
        k = norm(k); // ensure k is positive modulo
        update(l, r, k, 1, 0, n - 1);
    }
 
    ll query(int ql, int qr, int p, int l, int r) {
        if (ql <= l && r <= qr) return tree[p];
        push_down(p, l, r);
        int mid = (l + r) >> 1;
        ll res = 0;
        if (ql <= mid) res = (res + query(ql, qr, lc(p), l, mid)) % MOD;
        if (qr > mid)  res = (res + query(ql, qr, rc(p), mid + 1, r)) % MOD;
        return res;
    }
 
    // public query: sum over [l, r] (0-indexed, inclusive)
    ll query(int l, int r) {
        if (l > r) return 0;
        return query(l, r, 1, 0, n - 1);
    }
};

int main(){
    SegTree T;

    //1 to n all add 1
    T.update(1,n,1);

    //querying sum from 1 to n
    T.query(1,n);
}