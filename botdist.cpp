#include "botdist.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cassert>

using namespace std;

inline int sqd(int W,int a,int b){
	return (a%W-b%W)*(a%W-b%W)+(a/W-b/W)*(a/W-b/W);
}

inline double d(int W,int a,int b){return sqrt(sqd(W,a,b));}

vector<int> makeBotDistribution(const int W,const int H,const int B){
	assert(B<=W*H);
	bool bd[W*H];
	memset(bd,0,W*H*sizeof(bool));
	int bots[B];
	int i,j;
	int newpos;
	for(i=0;i<B;i++){
		bd[i]=true;
		bots[i]=i;
	}
	if(B==W*H)return vector<int>(bots,bots+B);
	/*for(int y=0;y<H;y++){
		for(int x=0;x<W;x++){
			for(i=0;i<B;i++){
				if(bots[i]==W*y+x)break;
			}
			cerr<<(bd[W*y+x]?'#':' ');
			if(i==B)cerr<<". "; else cerr<<i<<' ';
		}
		cerr<<endl;
	}
	cerr<<endl;*/
#if 0
	double dist,maxdist; int maxatdx,maxatdy;
	for(int iter=0;iter<10;iter++){
		for(i=0;i<B;i++){
			maxdist=-1;
			maxatdx=0; maxatdy=0;
			for(dy=-1;dy<=1;dy++)for(dx=-1;dx<=1;dx++){
				if(bots[i]/W+dy<0||bots[i]/W+dy>=H||bots[i]%W+dx<0||bots[i]%W+dx>=W){
					cerr<<"-1 ";
					if(dx==1)cerr<<endl;
					continue;
				}
				newpos=W*(bots[i]/W+dy)+bots[i]%W+dx;
				if(bd[newpos]&&(dx!=0||dy!=0)){
					cerr<<"-1 ";
					if(dx==1)cerr<<endl;
					continue;
				}
				dist=0;
				for(j=0;j<B;j++){
					if(i==j)continue;
					dist+=1.0/d(W,newpos,bots[j]);
				}
				cerr<<dist<<' ';
				if(dx==1)cerr<<endl;
				if(maxdist==-1||dist<maxdist){
					maxdist=dist;
					maxatdx=dx;
					maxatdy=dy;
				}
			}
			if(maxdist!=-1){
				bd[bots[i]]=false;
				newpos=W*(bots[i]/W+maxatdy)+bots[i]%W+maxatdx;
				cerr<<"bot "<<i<<", "<<bots[i]<<"->"<<newpos<<" with maxdist="<<maxdist<<" maxat{dx="<<maxatdx<<",dy="<<maxatdy<<'}'<<endl;
				bd[newpos]=true;
				bots[i]=newpos;
			}
		}

		for(int y=0;y<H;y++){
			for(int x=0;x<W;x++){
				for(i=0;i<B;i++){
					if(bots[i]==W*y+x)break;
				}
				cerr<<(bd[W*y+x]?'#':' ');
				if(i==B)cerr<<". "; else cerr<<i<<' ';
			}
			cerr<<endl;
		}
		cerr<<endl;
	}
#else
	double dist,mindist; int minat;
	for(int iter=0;iter<10;iter++){
		for(i=0;i<B;i++){
			mindist=-1;
			minat=-1;
			for(newpos=0;newpos<W*H;newpos++){
				if(bd[newpos]&&newpos!=bots[i]){
					//cerr<<"-- ";
					//if(newpos%W==W-1)cerr<<endl;
					continue;
				}
				dist=0;
				for(j=0;j<B;j++){
					if(i==j)continue;
					dist+=1.0/d(W,newpos,bots[j]);
				}
				//cerr<<dist<<' ';
				//if(newpos%W==W-1)cerr<<endl;
				if(mindist==-1||dist<mindist){
					mindist=dist;
					minat=newpos;
				}
			}
			if(mindist!=-1){
				bd[bots[i]]=false;
				//cerr<<"bot "<<i<<", "<<bots[i]<<"->"<<newpos<<" with mindist="<<mindist<<" minat="<<minat<<endl;
				bd[minat]=true;
				bots[i]=minat;
			}
		}

		/*for(int y=0;y<H;y++){
			for(int x=0;x<W;x++){
				for(i=0;i<B;i++){
					if(bots[i]==W*y+x)break;
				}
				cerr<<(bd[W*y+x]?'#':' ');
				if(i==B)cerr<<". "; else cerr<<i<<' ';
			}
			cerr<<endl;
		}
		cerr<<endl;*/
	}
#endif
	return vector<int>(bots,bots+B);
}
