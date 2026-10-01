# Team Reference Document — NCPC / NWERC

`notebook.tex` is the file to paste into Overleaf; upload `kth.pdf` (the cover logo, from KACTL) next to it. Nothing under `Ruben/`,
`Ruiming/` or `martin/` was modified.

## Build

```
tools/build.sh    # src/*.tex  ->  notebook.tex   (run after any edit)
tools/pdf.sh      # notebook.tex -> build/pdf/notebook.pdf  (tectonic)
tools/lint.sh     # unescaped % _ # &, unbalanced $   -> 0 issues
tools/check.sh    # every listing compiled, gnu++17 + gnu++20 -> 182 ok, 0 failed
tools/smoke.sh    # templates vs brute force            -> 25 tests, 0 failed
tools/verbatim.sh # %FROM listings vs their source files -> 24 match, 0 changed
tools/pages.sh    # rough page estimate without compiling (reads high: 18.5 vs 15 content pages)
```

Edit `src/*.tex`, not `notebook.tex` — the latter is generated. Two ways to get a
PDF: `tools/pdf.sh` locally (tectonic, self-contained, downloads its TeX bundle on
first run), or paste `notebook.tex` into Overleaf and hit Recompile. Tectonic is
XeTeX-based and Overleaf defaults to pdfLaTeX; both compile this file cleanly.

**Current output: 25 pages (front page + 24 of content; the last page is full), 0 overfull hboxes,
116 index entries.**

Team name, university and members are three macros at the top of
`src/00-preamble.tex` (`\team`, `\uni`/`\unishort`, `\members`); the front page and
the running header both read from them, so change them in one place.

## Rules compliance (NWERC 2025, <https://2025.nwerc.eu/rules>)

| Rule | How |
| --- | --- |
| max **25 pages** | **25** incl. the front page, measured from the compiled PDF — exactly at the limit, so anything added means something removed |
| single-sided, A4 | A4 landscape (still "A4 size"); print one-sided |
| university name **upper left** | `\lhead{\uni}` = "KTH Royal Institute of Technology" |
| page number **upper right** | `\rhead{\thepage}` |
| front page | KTH logo, university, team name, members, "Team Reference Document · NCPC / NWERC" (the rules' own term, instead of "Notebook"), **and the same header as every page** — it counts toward the 25, and the rule asks for university + number on the pages, so it is page 1. Checked with `pdftotext`: all 25 pages start with the university and end with their own number |
| readable at 0.5 m | 8pt code, 7pt prose and tables. KACTL (9pt `extreport`, `\footnotesize` throughout) prints *everything* at 7pt, so nothing here is smaller than KACTL. The PathDP table used to be `\scriptsize` = 6pt; raised to 7pt |
| institution on the folder cover | do this when you print |

NCPC allows unlimited printed material, so this one document covers both contests.

**Print page 1 and read it at half a metre before trusting it.** The size knob is
one line in the preamble:

```latex
\newcommand{\codesize}{\fontsize{8}{8.8}\selectfont}
```

Raise both numbers if it looks tight. **Do not lower them.** KACTL, printed for NWERC
every year, uses `\footnotesize` in a 9pt `extreport`, which `size9.clo` defines as
**7pt**; our 8pt code is a point above that, a deliberate margin.

## Layout

Matched to KACTL deliberately, from `content/kactl.tex` + `content/tex/kactlpkg.sty`:

```latex
% KACTL's, for reference:
\documentclass[9pt,a4paper]{extreport}
\usepackage[a4paper,landscape,hmargin={0.5cm,0.5cm},vmargin={1.0cm,0.4cm}]{geometry}
\begin{multicols*}{3}
```

Ours is the same except `extarticle`, and `vmargin={0.4cm,0.4cm},includehead,headsep=4pt`
instead of KACTL's `vmargin={1.0cm,0.4cm}` — see *Verification* for why that
difference matters.

KACTL is **landscape**, which is the whole trick: landscape A4 with 0.5 cm side
margins gives 3 columns of ~93 mm — the same column width as Ruben's original
portrait 2-column layout, so every existing entry (including PathDP's 6-column
config table) still fits a column unchanged, at 1.5x the density.

Ruben's `\secbar` / `\entry` / `\code` macros and colours are untouched; `\entry`
now also emits a table-of-contents line, which is what builds the index.
If `tocloft` ever misbehaves, comment out `\tableofcontents` — nothing else
depends on it.

## Verification

| Tool | What it actually proves |
| --- | --- |
| `lint.sh` | no unescaped specials, inline math balanced, `tabular`/`ctab`/`begingroup` balanced |
| `check.sh` | all 182 listings compile under **both** gnu++17 and gnu++20 |
| `smoke.sh` | 18 randomised tests, each against an independent oracle |

`%HEADER`, `%NOCOMPILE`, `%LABEL <name>` and `%DEP <a> <b>` markers in `src/*.tex`
drive the extraction. `%NOCOMPILE` is Ruben's existing convention for fragments.

What the smoke tests check:

- **hungarian** — 400 rectangular cost matrices with negative costs, vs brute-force
  permutations
- **dinic** — 400 graphs: Dinic flow == MCMF flow == capacity of the cut `cut()` reports
- **matching** — Kuhn == unit-capacity Dinic, `matchL`/`matchR` consistent
- **euler** — 3000 multigraphs: existence matches a degree+connectivity oracle, and
  every returned path uses each edge exactly once
- **lazyseg** / **lazyseg_ap** — range add + range sum vs a naive array, *and* an
  arithmetic-progression config that actually exercises `sh(f,k)`
- **scc** — components == mutual reachability (Floyd closure), order is topological
- **paths** — Dijkstra == Floyd-Warshall, parent chains reach the source
- **wdsu** — contradictions detected, differences consistent
- **ntt** — 300 convolutions vs naive O(n²), plus n=5000
- **crt** — 4000 cases vs exhaustive search, non-coprime moduli included
- **pollard** — `miller_rabin` vs a sieve to 2e5; 304 factorisations multiply back, incl. 4e18
- **binom** — `C(n,k)` vs Pascal, out-of-range guards
- **gauss** — the whole program fed systems with a unique / no / infinitely many solutions
- **strings** — kmp, period, zfunc, Manacher `isPal`, hashing, suffix array + lcp, Aho
- **geometry** — `Cross_Segment` vs an exact integer oracle (200k cases), `Convex_Hull` vs an
  O(n³) oracle, both `Point_in_polygon`s vs an exact oracle, `Polygon_area`, `closest` vs O(n²), circles
- **hpi** — area of `HPI` over 1–4 random convex polygons vs Sutherland–Hodgman clipping
- **frac** — `zero_one_partition` vs brute force over every set of k items to drop
- **kactl_alg** — `modLog` for every a, b, m < 120; `modSqrt` for every a mod every prime < 600;
  Berlekamp–Massey recovers 500 random recurrences and `linearRec` reproduces them (and fib(10¹⁸));
  both `det`s vs permutation expansion
- **kactl_conv** — double `conv` exact vs naive, `convMod<1e9+7>` vs naive in i128, FST AND;
  **kactl_fst.sh** re-makes the OR and XOR variants exactly as the entry says and tests those
- **kactl_lp** — simplex vs vertex enumeration on 3000 random 2-variable LPs (820 infeasible),
  the unbounded case, KACTL's usage example (= −7/3); Simpson on known integrals
- **kactl_graph** — 2-SAT (with an `atMostOne` group) vs all assignments, and the returned
  assignment satisfies everything; max clique vs all subsets; König cover covers every
  edge and has the matching's size
- **kactl_misc** — Mo vs naive distinct count, Stoer–Wagner vs every bipartition (and the
  returned side achieves it), D&C DP vs the O(Gn²) DP, `minRotation` vs every rotation
- **kactl_geo** — `hullCCW` strictly convex and covering, `hullDiameter` vs all pairs, `inHull`
  strict / non-strict vs exact oracle, `mec` vs every 2- and 3-point circle, `polygonCut`
  areas partition the polygon, `tangents` touch both circles at right angles

The document itself compiles clean under tectonic: **25 pages** (front page + 24), **0 overfull
hboxes**, 116 index entries. Two layout bugs were found and fixed by actually looking
at the compiled output rather than trusting the source:

- Sections were numbered in the index, and since `\cftsecnumwidth` is `0em` the number
  overprinted the title ("T4rees"). Fixed with `\setcounter{secnumdepth}{0}`, which
  also matches the body, where the section bars show no number.
- 22 code lines exceeded the column width. The column fits **62 characters** at 8pt;
  the worst offender was 68. Anything over ~65 pushes past the 11.4pt gutter and
  collides with the next column. `tools/rewrap.awk` records those rewrites; they are
  pure reformatting apart from `closest` and `hungarian`, whose loop bodies were
  restructured equivalently and are both covered by the smoke tests.

**Keep code lines at or under 62 characters** when adding anything — `tools/pdf.sh`
reports the overfull count, and it must stay 0.

Two more, found only by looking at the printed header:

- **The header was sliced in half by the page edge.** With plain
  `vmargin={1.0cm,0.4cm}` LaTeX put the header box top at
  `1in + \topmargin = 72.27 - 80.82 = -8.55pt`, i.e. *above* the paper. KACTL hides
  this by setting `\headheight` to 25pt, which just pushes the text back down inside
  an oversized box. The fix here is `includehead` in `geometry`, so the header is
  reserved *inside* the top margin and the arithmetic comes out consistent
  (`\topmargin` now `-60.89pt`, header top at `+11.38pt` = the requested 0.4cm, and
  the bottom margin lands on 0.4cm too). If you ever change `vmargin`, re-check
  `\topmargin` in `build/pdf/notebook.log` — `1in + \topmargin` must be positive.
- **`tocloft` forces `\thispagestyle{plain}` on the table-of-contents page**, which
  silently removed the header from the index page (and only that page). Neither
  `\pagestyle` nor `\thispagestyle` *before* `\tableofcontents` helps; the undo has
  to come after it. Bisected against a minimal file — `titlesec` is innocent.

The one thing not verified is how it *looks*: there is no PDF rasteriser on this
machine (`pdftotext` is present but `pdftoppm` is not, and `convert` here is the
Windows filesystem tool, not ImageMagick). Open `build/pdf/notebook.pdf` and check
the print by eye, as the rules demand anyway.

## Bugs found in the original files (fixed here, originals untouched)

| File | Problem |
| --- | --- |
| `Ruiming/geometry/Convex_Hull.cpp` | two files concatenated, duplicate `#include`/`Point` — **does not compile** |
| `Ruiming/geometry/Lattic_Point.cpp` | uses `Point` and `Cross_point` before they are declared — **does not compile** |
| `Ruiming/Dynamic Programming/Bitmask DP.cpp` | `dp[MAXN]` is 1-D but indexed `dp[s][j]` — **does not compile** |
| `Ruiming/graph/Floyd-Warshall.cpp` | `dist[b][a]=min(dist[a][b],c)` reads the just-written cell; wrong when an earlier edge already set `b,a` |
| `Ruiming/String/Hash.cpp` | `get_hash` returns `ll` from `ull` arithmetic (sign bug); single overflow-mod hash |
| `Ruiming/Data Structures/Weighted_union_find.cpp` | `merge_set` never sets the weight, so the potentials are never usable |
| `Ruiming/graph/Hugarian_algorithm.cpp` | is **Kuhn's unweighted matching**, not the Hungarian algorithm |
| `martin/claude/strings/aho_corasick.cpp` | `search` walks the whole fail chain per position: O(n·depth), not the advertised O(n + matches) |
| `martin/claude/strings/suffix_array.cpp` | O(n log²n) via comparator sort |
| `martin/claude/math/number_theory.cpp` | `crt` arithmetic is fragile |

Two more were found *by the tooling* while building this, worth knowing:

- `bits/stdc++.h` on GCC 16 no longer pulls in `<cassert>`, so the header includes it
  explicitly. Judges on older GCC do not need this; it costs nothing.
- A `#define sz(v)` in the header **breaks `Treap`**, which has its own `sz()` member.
  That macro is deliberately *not* in the header. Do not add it.

## What changed vs. your own files

Ruben's entries are verbatim. For the others, the shared prelude that used to sit at
the top of every one of Ruiming's files (`#include`, typedefs, `MAXN=206`, `eps`) now
appears **once**, in *Contest setup*, and loose `main()` bodies became callable
functions. Names are kept wherever there was a real interface.

| Entry | From | Call site change |
| --- | --- | --- |
| `Tree` (order statistics) | `Ruiming/.../OrderStatisticTree.cpp` | none — his `Tree<T>` alias kept |
| `lis` | `martin/dp/lis.cpp` | none — `lis(a)` still returns the length |
| `kmp`, `failure` | `martin/string/kmp.cpp` | none — `kmp(txt, pat)` still returns start indices |
| `precompute`, `C` | `Ruben/PrecomputeBinCoeff.cpp` | none |
| `power`, `ext_gcd`, `inv` | `Ruben/InverseModulu.cpp` | none |
| `SpTab` | Ruiming + martin sparse tables | **was** global `st[j][i]`/`back[j][i]` 1-indexed arrays + `log2()`. Now `SpTab s(a); s.query(l, r)`, half-open, configurable `op` |
| `WDSU` | `Ruiming/.../Weighted_union_find.cpp` | **was** `find_set`/`merge_set` on globals `s[]`, `d[]`, `height[]`. Now `join(a,b,w)` returns false on a contradiction, `diff(a,b)` reads the difference |
| `diameter` | `Ruiming/graph/Tree_Diamter.cpp` | **was** two recursive DFS writing globals `place`/`ans`. Now returns `{length, a, b}`, iterative |
| `centroids` | `Ruiming/graph/Centroid.cpp` | **was** recursive DFS filling a global `cand`. Now returns the vector, iterative |
| `sumOfDists` | `Ruiming/.../Tree DP.cpp` | **was** `dfs`/`dfs1` on globals. Now returns `ans`, iterative |
| `kruskal` | Ruben's `.tex` snippet | wrapped in a function returning `{cost, edges}` instead of inline statements |
| `floyd` | `Ruiming/graph/Floyd-Warshall.cpp` | takes a `matrix&`, 0-indexed, `INF` guards added; **the min bug is fixed** |
| `dijkstra`, `bellman` | Ruiming + martin | **was** `Link`/`Edge`/`insert` linked-list adjacency, 1-indexed globals. Now `vector<vector<pii>>`, 0-indexed; `dijkstra` can fill a parent array |
| `kShortest` | `Ruiming/graph/kth_longest_path.cpp` | **was** a per-node `priority_queue` array. Now one function returning the k lengths |
| `eulerPath` | Ruiming's two `Eulertour` files | **was** recursive, adjacency-matrix or `multiset`, 1-indexed, with the degree checks inline in `main`. Now one iterative function for both directions, returning `{}` when no path exists |
| `Matching` | `martin/claude/.../bipartite_matching.cpp` | renamed from `BipartiteMatching`; same members |
| `Dinic` | `martin/claude/.../max_flow_dinic.cpp` | `addEdge` now returns an edge id; added `cut()` and `flowOn(id)` |
| `MCMF` | `martin/claude/.../mcmf.cpp` | added the `maxf` cap argument |
| `Hash` | `Ruiming/String/Hash.cpp` | **was** `Bhash(s)` + `get_hash(v, a, b)`, 1-indexed inclusive, single overflow hash. Now `Hash h(s); h.get(l, r)`, 0-indexed half-open, two moduli |
| `Manacher` | Ruiming + martin | **was** inline in `main` over a `#`-padded buffer. Now a struct with `d1`, `d2` and `isPal(l, r)` |
| `SuffixArray` | `martin/claude/.../suffix_array.cpp` | O(n log n) instead of O(n log²n); one struct holding `sa`, `rnk`, `lcp` |
| `Aho` | `martin/claude/.../aho_corasick.cpp` | output links added (so it hits its advertised complexity) and duplicate patterns now work; `match(txt, f)` callback instead of a returned matrix |

### Ruiming's math and geometry: his code, verbatim

Every file in `Ruiming/math/` and `Ruiming/geomtery/` (his spelling) is in the notebook as **his
actual code**, and `tools/verbatim.sh` proves it: each such listing carries a
`%FROM <file>` marker, and the script checks that every statement of the listing
appears in that file, in order (comments and whitespace ignored). Run it after any
edit; it must say `0 changed`. The only differences it allows, and the only ones made:

- the prelude each file repeated (`#include`, typedefs, `INF`, `MAXN`, `eps`) is
  dropped, because the merged header in *Contest setup* carries it, including his
  `//UPDATERA ARRAY STORLEKEN` comment and `MAXN`. Exception: `miller_rabin.cpp` is
  printed whole (it has `typedef __int128 ll`, so it is a standalone file, not pasted
  under the header);
- code repeated across his files is printed **once**: the geometry base (`ldcmp`,
  `Point`, `POF`, `Dist`, `Dot`, `Len`, `Cross`, ..., `Rotate`) as *Point / vector*,
  and `fast` as *Fast power*, which *Euler's theorem* and *Binomials (inverse)* use;
- example `main()`/`solve()` bodies are dropped where the template is a set of
  functions. Where the algorithm *is* `main()` the whole program is kept;
- whitespace: completely empty lines are removed from all listings (everyone's, to save
  space), long lines re-wrapped to the 62-character column;
- comments: Chinese comments
  translated (pdfLaTeX cannot print them); his `ld eps=1e-6;` lines (geometry,
  `zero_one_partition`) kept as comments
  since `eps` is declared once in the header.

| Entry | From `Ruiming/` |
| --- | --- |
| Fast power | `math/Fastmod.cpp` |
| Fast gcd | `math/Fast_gcd.cpp` |
| CRT | `math/CRT.cpp` |
| Euler sieve | `math/Euler_sieve.cpp` |
| Linear sieve + Euler phi | `math/Linearphi.cpp` |
| Euler phi of one number | `math/Phi.cpp` |
| Prime factorization | `math/Prime_factorization.cpp` |
| Factorization table | `math/Factorization.cpp` |
| Euler's theorem (huge exponent) | `math/Eulerthm.cpp` |
| Miller-Rabin (standalone) | `math/miller_rabin.cpp` |
| Miller-Rabin + Pollard rho | `math/Pollard_Rho.cpp` |
| Binomials (Pascal) | `math/Combinumber.cpp` |
| Binomials (inverse) | `math/Combinumber_inv.cpp` |
| Linear inverses | `math/Linearinverse.cpp` |
| Matrix power | `math/Matrix.cpp`. `struct matrix` clashes with Ruben's `matrix` typedef, so delete the typedef when you use it |
| Gaussian elimination | `math/gaussjordanelimination.cpp` |
| 0/1 fractional programming | `math/zero_one_partition.cpp` (his POJ 2976 `main` dropped; returns 100 x the ratio, rounded) |
| Big integers | `math/Big_Integer.cpp` |
| Point / vector, Lines and segments | `geomtery/Line_and_Vectors.cpp` |
| Polygons | `geomtery/Polygon.cpp` |
| Convex hull | `geomtery/Convex_Hull.cpp` |
| Half-plane intersection | `geomtery/Half_Plane_Intersect.cpp`. Its own `Line` (adds `v`, `ang`, `operator<`) replaces the one in *Lines and segments* when you use it |
| Lattice points (exact) | `geomtery/Lattic_points.cpp` |

Not his, and labelled as such in the notebook: *Modular basics* and *Binomials mod p*
(Ruben), *CRT (general)* and *Sieves* (Martin), *FFT / NTT*, and `closest` and the
circle functions, which were written on top of his `PointLL` / `Point` so the geometry
section has one point type.

### New — nobody had these

`LineContainer` (CHT, adapted from KACTL, CC0), `hungarian` (real weighted
assignment), `ntt`/`conv` (FFT), `closest` (closest pair), `circleLine`,
`circleCircle`, `sos` (subset sums), `compress`, and the whole
*Reference* section.

### Cut, and why

| Cut | Reason |
| --- | --- |
| `Scapegoattree.cpp` | superseded by `Tree` (pbds) + `Treap` |
| `Big_Integer.cpp` | 205 lines, and Python 3 / Java `BigInteger` are both available |
| `Johnsons.cpp` | `floyd` covers n ≤ 500; Johnson only wins on sparse APSP with negatives |
| `mst_prim.cpp` | `kruskal` + `DSU` covers it |
| `Modifyheap`, `DFS_standard`, `Edges_Link` | idioms, not algorithms — a comparator and a loop |
| `Minimum LIS.cpp` | folded into the `lis` notes (Dilworth) |
| `Built_In_Treap.cpp` (rope) | non-standard, and `Treap` does the same job |
| plain `Trie` | noted in the `XorTrie` entry — same shape, 26 children |
| `Eulerthm.cpp` | folded into the *Modular basics* prose |
| `Fast_gcd`, `Fastmod`, `Combinumber*`, `Factorization`, `Prime_factorization`, `Linearinverse` | duplicates of `__gcd`, `power`, `C`, `spf`, `factor`, `inv_fact` |
| contest skeletons (`Codeforces.cpp`, `luogu.cpp`, `USACO.cpp`, `NewCodeforces.cpp`) | one header is enough |

## Trim order

If Overleaf reports more than 25 pages, delete from the top. Each `\entry` plus its
following `lstlisting` is independently removable.

1. `SuffixArray` — `Hash` + binary search usually replaces it (~45 lines)
2. `Aho` — rare relative to its size (~45 lines)
3. `Treap` — needed only for insert/erase in the middle of a sequence (~60 lines)
4. `kShortest`
5. `sumOfDists` (rerooting) — the pattern is described in the prose anyway
6. `inConvex` — `inPoly` is O(n) but usually fast enough
7. `LineContainer` — only for a specific DP shape
8. `gauss` — drop last; it is short and shows up in odd places

## KACTL entries

Taken from KACTL (`kth-competitive-programming/kactl`), each tagged *KACTL* in its
header bar and given a longer explanation than KACTL's, since the team has not used
them before. Changes are mechanical: `sz(x)` written out (a `sz` macro breaks `Treap`),
`int` → `ll` where the header's `vi`/`pii` are `ll`, names that clashed with existing
entries renamed. Geometry is ported onto Ruiming's `Point` / `PointLL` instead of
adding a second point type.

| Entry | KACTL file(s) | Licence | Changed |
| --- | --- | --- | --- |
| Mo's algorithm | `MoQueries.h` | CC0 | example `add`/`del`/`calc` filled in; `moTree` left out |
| 2-SAT | `2sat.h` | CC0 | `val`/`comp`/`z` are `vector<int>` (`min(int, ll)` would not compile) |
| Maximum clique | `MaximumClique.h` | CC0 | verbatim, wrapped in `#define sz` … `#undef sz` |
| Minimum vertex cover | `MinimumVertexCover.h` | CC0 | runs on our `Matching` instead of `DFSMatching` |
| Global min cut | `GlobalMinCut.h` | CC0 | `ll` weights, `INT_MIN` → `-INF` |
| Divide and conquer DP | `DivideAndConquerDP.h` | CC0 | `f`/`store` wired to `prv`/`cur` |
| Discrete log, modular sqrt | `ModLog.h`, `ModSqrt.h` | CC0 | `sqrt` → `modSqrt`, returns −1 instead of asserting, uses `power` |
| FFT, convolution mod any m | `FastFourierTransform.h`, `FastFourierTransformMod.h` | CC0 | `C` → `cd` (clashes with binomial `C`) |
| AND / OR / XOR convolution | `FastSubsetTransform.h` | GFDL 1.2 | `int&` → `ll&`, `conv` → `fstConv` |
| Berlekamp–Massey + k-th term | `BerlekampMassey.h`, `LinearRecurrence.h` | CC0 | `mod` = 1e9+7 |
| Determinant | `Determinant.h`, `IntDeterminant.h` | CC0 / none stated | none |
| Simplex | `Simplex.h` (from the Stanford notebook) | MIT | `eps` → `EPS` (header has `eps`) |
| Numerical integration | `Integrate.h` | CC0 | none |
| Minimum rotation | `MinRotation.h` | Unlicense | `max(0, …)` → `max(0LL, …)` |
| Hull diameter, point in hull | `ConvexHull.h`, `HullDiameter.h`, `PointInsideHull.h` | CC0 | on `PointLL`; `max` of pairs → explicit compare (`PointLL` has no `<`) |
| Enclosing circle, polygon cut, tangents | `MinimumEnclosingCircle.h`, `circumcircle.h`, `PolygonCut.h`, `CircleTangents.h` | CC0 | on Ruiming's `Point` |

Skipped because the notebook already covers them: LCA, HLD,
Dinic, MCMF, Hungarian, KMP/Z/Manacher/suffix array/Aho–Corasick, NTT, CRT,
Miller–Rabin/Pollard, Gaussian elimination, LineContainer, treap, closest pair.

## Still missing, deliberately

Suffix automaton, persistent segment tree, Gomory-Hu, link-cut tree, 3D hull.
Ported but cut to stay at 25 pages (never smoke-tested, so test before relying on
them): general (non-bipartite) matching via the Tutte matrix (`GeneralMatching.h` +
`MatrixInverse-mod.h`), circle∩polygon area (`CirclePolygonIntersection.h`), floor sums
(`ModSum.h`), angle sweep (`Angle.h`). They are the next candidates if something else is
removed.
