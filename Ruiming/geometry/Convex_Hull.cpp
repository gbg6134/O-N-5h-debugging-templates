//UPDATERA ARRAY STORLEKEN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <bits/stdc++.h>
using namespace std;
 
typedef long long ll;
typedef long double ld;
 
const ll INF=1e18;
const ll MAXN=200006;//UPDATERA ARRAY STORLEKEN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
ll t,n;
ld ans;

const ld pi=acos(-1.0);
ld eps=1e-6;//MIGHT GET WA PGA EPS TOO SMALL!!!!!!!!!!!!!!!!!!!!!!!!!!!!

ll ldcmp(ld x,ld y){
    if(fabs(x-y)<eps) return 0;
    else return x<y?-1:1;
    //is x<y return -1, x>y return 1;
}

struct Point{
    ld x,y;
    Point(): x(0),y(0){}
    Point(ld x,ld y): x(x),y(y) {}

    Point operator + (Point B){return Point(x+B.x,y+B.y);}
    Point operator - (Point B){return Point(x-B.x,y-B.y);}
    Point operator * (ld k){return Point(x*k,y*k);}
    Point operator / (ld k){return Point(x/k,y/k);}
    bool operator == (Point B){return ((ldcmp(x-B.x,0)==0)&&(ldcmp(y-B.y,0)==0));}
    bool operator < (Point B){
        if (x != B.x) return x < B.x;
        return y < B.y;
    }
};

ld Dist(Point A,Point B){
    return sqrt((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y));
}

typedef Point Vector;

ld Dot(Vector A,Vector B){return A.x*B.x+A.y*B.y;}
ld Len(Vector A){return sqrt(Dot(A,A));}
ld Angle(Vector A,Vector B){return acos(Dot(A,B)/Len(A)/Len(B));}
ld Cross(Vector A,Vector B){return A.x*B.y-A.y*B.x;}

ll Convex_Hull(Point *p, ll n,Point *ch){
    sort(p+1,p+n+1);
    n=unique(p+1,p+n+1)-(p+1);
    if (n == 1) { ch[1] = p[1]; return 1; }
    if (n == 2) { ch[1] = p[1]; ch[2] = p[2]; return 2; }

    ll v=0;
    
    for(int i=1;i<=n;i++){
        while(v>1&&ldcmp(Cross(ch[v]-ch[v-1],p[i]-ch[v]),0)<0){
            v--;
        }
        ch[++v]=p[i];
    }

    ll j=v;
    for(int i=n-1;i>=1;i--){
        //<=0 do not count points on the side of convex hull
        while(v>j&&ldcmp(Cross(ch[v]-ch[v-1],p[i]-ch[v]),0)<0){
            v--;
        }
        ch[++v]=p[i];
    }

    //might include double points on the line case
    return v-1;
}

Point poly[MAXN],ch[MAXN];

void solve(){
    cin>>n;
    for(int i=1;i<=n;i++) cin>>poly[i].x>>poly[i].y;
    ll cnt=Convex_Hull(poly,n,ch);
    
    sort(ch+1,ch+cnt+1);
    cnt=unique(ch+1,ch+cnt+1)-(ch+1);

    cout<<cnt<<'\n';
    cout<<fixed<<setprecision(0);
    for(int i=1;i<=cnt;i++){
        cout<<ch[i].x<<' '<<ch[i].y<<'\n';
    }
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    solve();
}
