#include "botdist.h"
#include <cmath>
#include <cstring>
#include <cassert>

using namespace std;

inline double sqr(double x){return x*x;}

inline double d(int W,int a,int b){
	return sqrt((a%W-b%W)*(a%W-b%W)+(a/W-b/W)*(a/W-b/W));
}

vector<bool> makeBotDistribution(const int W,const int H,const int B){
	assert(B<=W*H);
	vector<bool> bd;
	int bots[B];
	double force[B];
	bd.resize(W*H);
	int i,j;
	for(i=0;i<B;i++){
		bd[i]=true;
		bots[i]=i;
	}
	if(B==W*H)return bd;
	return bd;
	for(int iter=0;iter<100;iter++){
		memset(force,0,B*sizeof(double));
		for(i=0;i<B;i++){
			for(j=0;j<B;j++){
				force[i]+=sqr(d(W,bots[i],bots[j]));
			}
		}
		;
	}
}
