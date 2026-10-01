// usage: verbatim snippet.cpp original.cpp   (driven by tools/verbatim.sh)
// Strips // comments and all whitespace from both, splits the snippet into
// statements (ending at ; { }), and checks they occur in the original in
// order. Prints any statement that is not found.
#include <bits/stdc++.h>
using namespace std;
string norm(const string& path) {
    ifstream f(path); stringstream ss; ss << f.rdbuf();
    string s = ss.str(), o;
    for (size_t i = 0; i < s.size(); i++) {
        if (i + 1 < s.size() && s[i] == '/' && s[i+1] == '/') {
            while (i < s.size() && s[i] != 10) i++;
            continue;
        }
        if (!isspace((unsigned char)s[i])) o += s[i];
    }
    return o;
}
int main(int argc, char** argv) {
    string a = norm(argv[1]), b = norm(argv[2]);
    if (b.empty()) { printf("  cannot read %s\n", argv[2]); return 1; }
    vector<string> st; string cur;
    for (char c : a) {
        cur += c;
        if (c == ';' || c == '{' || c == '}') { st.push_back(cur); cur.clear(); }
    }
    if (!cur.empty()) st.push_back(cur);
    size_t pos = 0; int bad = 0;
    for (auto& x : st) {
        size_t q = b.find(x, pos);
        if (q == string::npos) { printf("  NOT IN ORIGINAL: %s\n", x.c_str()); bad++; }
        else pos = q + x.size();
    }
    printf("  -> %s (%zu statements)\n", bad ? "CHANGED" : "verbatim", st.size());
    return bad != 0;
}
