#!/usr/bin/env bash
# FST is printed with the AND line active. Re-make the OR and XOR versions
# exactly the way the entry says to (swap which line is commented, and for XOR
# uncomment the division) and test each against an O(n^2) oracle.
# No backslashes on purpose: the tooling here eats them.
set -u
cd "$(dirname "$0")/../.."
S=build/snips
[ -f "$S/lib_fst.cpp" ] || { echo "[kactl_fst] lib_fst.cpp missing"; exit 1; }
mkdir -p build/smoke
fail=0
for op in OR XOR; do
  tu=build/smoke/fst_$op.cpp
  cat "$S/prelude.cpp" > "$tu"
  awk -v op="$op" '
    index($0, "// AND") { print "//" $0; next }
    index($0, "// " op) && !index($0, "XOR only") && (op == "XOR" || !index($0, "XOR")) {
        sub("// ", ""); print; next }
    op == "XOR" && index($0, "XOR only") { sub("// ", ""); print; next }
    { print }
  ' "$S/lib_fst.cpp" >> "$tu"
  if [ "$op" = OR ]; then sym='|'; else sym='^'; fi
  cat >> "$tu" <<EOF
int main(){
    mt19937 rng(7);
    rep(it, 0, 300) {
        int n = 1 << (rng() % 8);
        vi a(n), b(n), w(n);
        rep(i, 0, n) { a[i] = (ll)(rng() % 21) - 10; b[i] = (ll)(rng() % 21) - 10; }
        vi c = fstConv(a, b);
        rep(x, 0, n) rep(y, 0, n) w[x $sym y] += a[x] * b[y];
        if (c != w) { printf("MISMATCH n=%d\n", n); return 1; }
    }
    printf("OK 300 cases\n");
}
EOF
  if ! g++ -std=gnu++17 -O2 -o "build/smoke/fst_$op.exe" "$tu" 2>"build/smoke/fst_$op.log"; then
    echo "[kactl_fst] $op COMPILE FAIL"; head -5 "build/smoke/fst_$op.log"; fail=1; continue
  fi
  if out=$("build/smoke/fst_$op.exe"); then echo "[kactl_fst $op] $out"
  else echo "[kactl_fst $op] FAIL: $out"; fail=1; fi
done
exit $fail
