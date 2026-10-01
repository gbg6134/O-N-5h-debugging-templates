#!/usr/bin/env bash
# Prove that listings copied from someone's own files were not rewritten.
# Every listing preceded by a marker line
#   %FROM <path relative to the repo root>
# is compared with that file: comments and whitespace are ignored, and every
# statement of the listing must appear in the original, in the same order.
# (Dropping a repeated prelude or an example main() is allowed; changing,
# reordering or adding code is not.)
set -u
cd "$(dirname "$0")/.."
OUT=build/verbatim
rm -rf "$OUT"; mkdir -p "$OUT"
g++ -O2 -static -o "$OUT/verbatim.exe" tools/verbatim.cpp || exit 1

# No backslashes in this awk program on purpose (the tooling eats them).
awk -v out="$OUT" '
  BEGIN { B = "begin{lstlisting}"; E = "end{lstlisting}" }
  index($0, "%FROM") == 1 { src = $0; sub(/^%FROM[ ]*/, "", src); next }
  index($0, B) > 0 {
      if (src != "") { n++; f = sprintf("%s/v_%03d.cpp", out, n)
                       printf "%s|%s\n", f, src > (out "/list.txt") }
      inl = 1; src = ""; next
  }
  index($0, E) > 0 { if (f != "") close(f); inl = 0; f = ""; next }
  inl && f != "" { print > f }
' notebook.tex

ok=0; bad=0
while IFS='|' read -r snip src; do
  if res=$("$OUT/verbatim.exe" "$snip" "../$src"); then ok=$((ok+1))
  else bad=$((bad+1)); echo "=== $src"; echo "$res"; fi
done < "$OUT/list.txt"
echo "-----------------------------------------"
echo "verbatim: $ok listings match their source files, $bad changed"
[ "$bad" -eq 0 ]
