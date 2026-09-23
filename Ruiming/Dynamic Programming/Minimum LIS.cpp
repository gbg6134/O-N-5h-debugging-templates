//UPDATERA ARRAY STORLEKEN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <bits/stdc++.h>
using namespace std;
 
typedef long long ll;
typedef long double ld;
typedef __int128 i128;
ld eps = 1e-9;
 
const ll INF=1e18;
const ll MAXN=206;//UPDATERA ARRAY STORLEKEN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
ll n,lst[MAXN],ans;
multiset<ll> maxi;

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cin>>n;
    for(int i=1;i<=n;i++){
        ll a;
        cin>>a;
        a*=-1;
        if(maxi.empty()||*maxi.lower_bound(a)==*maxi.begin()){
            maxi.insert(a);
        }else{
            auto temp=maxi.lower_bound(a);
            temp--;
            maxi.erase(maxi.find(*temp));
            maxi.insert(a);
        }
    }
    cout<<maxi.size()<<'\n';
}
