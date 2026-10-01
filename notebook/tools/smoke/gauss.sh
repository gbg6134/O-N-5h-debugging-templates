#!/usr/bin/env bash
# Ruiming's Gaussian elimination is a whole program: compile it and feed it
# systems with a known answer (unique / none / infinitely many).
set -u
cd "$(dirname "$0")/../.."
S=build/snips
src=$(grep -l 'swaped' "$S"/snip_*.cpp | head -1)
[ -n "$src" ] || { echo "[gauss] snippet not found"; exit 1; }
mkdir -p build/smoke
cat "$S/prelude.cpp" "$src" > build/smoke/gauss.cpp
g++ -std=gnu++17 -O2 -o build/smoke/gauss.exe build/smoke/gauss.cpp \
  2>build/smoke/gauss.log || { echo "[gauss] COMPILE FAIL"; exit 1; }
run() { printf '%s\n' "$1" | build/smoke/gauss.exe | tr -d '\r'; }
fail=0
check() {
  got=$(run "$1")
  if [ "$got" != "$2" ]; then
    echo "[gauss] MISMATCH input: $1"; echo "  got:  $got"; echo "  want: $2"
    fail=1
  fi
}
# x=1 y=2 z=3
check "3
2 1 -1 1
-3 -1 2 1
-2 1 2 6" "x1=1.00
x2=2.00
x3=3.00"
# zero pivot in the first column forces a row swap: x=4 y=5
check "2
0 1 5
1 0 4" "x1=4.00
x2=5.00"
# inconsistent
check "2
1 1 1
2 2 3" "-1"
# infinitely many
check "2
1 1 1
2 2 2" "0"
[ "$fail" = 0 ] && echo "[gauss] OK unique / row swap / no solution / infinitely many"
exit $fail
