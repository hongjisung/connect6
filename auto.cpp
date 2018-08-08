#include <stdio.h>
#include <time.h>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

#define BOARD_SIZE 20
#define intmax 1000000000
#define intmin -1000000000
#define STATE_SIZE 12020
#define DANGER_STATE_SIZE 10
#define timebound 0.3


//---------

#define width 19
#define height 19

double s_time;
double limitTime=5;
//----------


char info[100] = { "TeamName:룰베이스>딥러닝,Department:고려대학교" };
// 바둑판 정보
int board[BOARD_SIZE][BOARD_SIZE];
// 기본 attribute개수세기
int state[STATE_SIZE];
// 다음차례에 END 되는 상황
// 내돌 4돌연속 한칸 빔-0, 내돌 5돌연속 한칸빔-1, 적돌 4돌연속 한칸 빔-2, 적돌 5돌연속 한칸빔-3
int danger_state[DANGER_STATE_SIZE];
// 0은 내돌 6개연속, 1은 적돌 6개연속
int end_state[2];


struct putstone{
	int x[2];
	int y[2];
	double score;
};

struct point{
	int x;
	int y;
};

struct compare
{
	bool operator()(const point &p1, const point &p2){
		if(p1.x<p2.x){
			return true;
		}
		else if(p1.x==p2.x){
			return p1.y<p2.y;
		}
		return false;
	}
};

void myturn(int cnt);
void board_init();
bool check_timeout(clock_t clock_start);
void count_state();
putstone put_first_stone(double alpha, double beta,int step, clock_t clock_start);
putstone alpha_beta_search(int step, clock_t clock_start);
putstone max_value(double alpha, double beta,int step, clock_t clock_start);
putstone min_value(double alpha, double beta,int step, clock_t clock_start);
bool terminal_test();
double utility();

//------------------
void init();
int showBoard(int x,int y);
void show();
void domymove(int x[], int y[], int cnt);
//------------------


void myturn(int cnt);
void board_init();
void state_init();
putstone put_first_stone(double alpha, double beta,int step, clock_t clock_start);
putstone alpha_beta_search(int step, clock_t clock_start);
putstone max_value(double alpha, double beta,int step, clock_t clock_start);
putstone min_value(double alpha, double beta,int step, clock_t clock_start);


//---------------------------


void myturn(int cnt) {
	clock_t clock_start = clock();
	int x[2], y[2];


	// 이 부분에서 알고리즘 프로그램(AI)을 작성하십시오. 기본 제공된 코드를 수정 또는 삭제하고 본인이 코드를 사용하시면 됩니다.
	// 현재 Sample code의 AI는 Random으로 돌을 놓는 Algorithm이 작성되어 있습니다.
	board_init();
	state_init();
	
	
	//첫 돌은 1개만 둠에 주의한다.
	//그냥 cnt = 1이면 대충 가운데 놓고 넘기는 건 어떤가?
	//하지만 장애물이 있다면? 
	if(cnt==1){
		int step = 1;
		double mxv = intmin+10;
		while(step<20){
			putstone ps = put_first_stone(intmin, intmax, step, clock_start);
			if(ps.score>mxv){
				x[0] = ps.x[0]; x[1] = ps.x[1];
				y[0] = ps.y[0]; y[1] = ps.y[1];	
				mxv = ps.score;
			}
			step++;
			if(check_timeout(clock_start)){
				break;
			}
		}
		domymove(x, y, cnt);
		return;
	}

	// iterative deepening search
	// 시간제한이 계속 바뀌는 건 어떻게? => s_time
	int step=1;
	double mxval = intmin;
	while( step<5){
		putstone ps = alpha_beta_search(step, clock_start);
		cout<<"time: "<<(float)(clock()-clock_start)/CLOCKS_PER_SEC<<"\n";
		cout<<"score: "<<ps.score<<"\n";
		if(mxval < intmin+10000){
			x[0] = ps.x[0]; x[1] = ps.x[1];
			y[0] = ps.y[0]; y[1] = ps.y[1];
			mxval = ps.score;
		}
		else if(ps.score > intmin+10000){
			x[0] = ps.x[0]; x[1] = ps.x[1];
			y[0] = ps.y[0]; y[1] = ps.y[1];
		}
		//step++;
		step+=1;
		if(check_timeout(clock_start)){
			break;
		}
	}


	// 이 부분에서 자신이 놓을 돌을 출력하십시오.
	// 필수 함수 : domymove(x배열,y배열,배열크기)
	// 여기서 배열크기(cnt)는 myturn()의 파라미터 cnt를 그대로 넣어야합니다.
	domymove(x, y, cnt);
}



// 바둑판 정보 초기화
void board_init(){
	for(int i=0; i<BOARD_SIZE; i++){
		for(int j=0; j<BOARD_SIZE; j++){
			board[i][j] = showBoard(i,j);
		}
	}
}

// 상태 초기화
void state_init(){
	for(int i=0; i<STATE_SIZE;i++){
		state[i] = 0;
	}
	for(int i=0; i<DANGER_STATE_SIZE; i++){
		danger_state[i]=0;
	}
	// memset(state, 0, (size_t)STATE_SIZE);
	// memset(danger_state, 0, (size_t)DANGER_STATE_SIZE);
	end_state[0]=0;
	end_state[1]=0;
}

//check timeout
bool check_timeout(clock_t clock_start){
	if(limitTime - (float)(clock()-clock_start)/CLOCKS_PER_SEC < timebound){
		return true;
	}
	return false;
}


// state 관련 값을 초기화 시키고 현재 판을 기준으로 값을 준다.
void count_state(){
	state_init();
	// 가로축에서 state 개수 세기
	for(int i=0; i<width; i++){
		int mine = 0, tot=0, enemy=0;
		int mycons=0, encons=0;
		int myblock = 0, enblock=0;
		// 가로축에서 내돌
		for(int j=0; j<height; j++){
			if(board[i][j]==1 || board[i][j]==3){
				mine++;
				tot++;
				mycons++;
			}
			else if(board[i][j]==2){
				if(tot>=6 && mycons<=6){
					state[tot*100+mine]++;
				}
				mine=0; tot=0;
				
				if(myblock==0){
					if(mycons==4 || mycons==5){
						danger_state[mycons-4]++;
					}
					if(mycons==6){
						end_state[0]++;
						return;
					}
				}
				myblock=1;
				mycons=0;
			}
			else{
				tot++;

				if(mycons==4 || mycons==5){
					danger_state[mycons-4]++;
				}
				if(mycons==6){
					end_state[0]++;
					return;
				}
				mycons = 0;
				myblock=0;
			}
		}
		if(tot>=6 && mycons<=6){
			state[tot*100+mine]++;
		}
		if(myblock==0){
			if(mycons==4 || mycons==5){
				danger_state[mycons-4]++;
			}
			if(mycons==6){
				end_state[0]++;
				return;
			}
		}
		mine=0; tot=0;enemy=0; mycons=0; encons=0; myblock = 0; enblock=0;

		//가로축에서 적돌
		for(int j=0; j<height; j++){
			if(board[i][j]==2){
				enemy++;
				tot++;
				encons++;
			}
			else if(board[i][j]==1|| board[i][j]==3){
				if(tot>=6 && encons<=6){
					state[10000+tot*100+enemy]++;
				}
				enemy=0; tot=0;

				if(enblock==0){
					if(encons==4 || encons==5){
						danger_state[encons-2]++;
					}
					if(encons==6){
						end_state[1]++;
						return;
					}
				}
				enblock=1;
				encons=0;
			}
			else{
				tot++;

				if(encons==4 || encons==5){
					danger_state[encons-2]++;
				}
				if(encons==6){
					end_state[1]++;
					return;
				}
				encons = 0;
				enblock=0;
			}
		}
		if(tot>=6 && encons<=6){
			state[10000+tot*100+enemy]++;
		}
		if(enblock==0){
			if(encons==4 || encons==5){
				danger_state[encons-2]++;
			}
			if(encons==6){
				end_state[1]++;
				return;
			}
		}
		// enemy=0; tot=0;mycons=0; encons=0; myblock = 0; enblock=0;
	}

	// 세로축에서 state 세기
	for(int j=0; j<height; j++){
		int mine = 0, tot=0, enemy=0;
		int mycons=0, encons=0;
		int myblock = 0, enblock=0;
		// 세로축에서 내돌
		for(int i=0; i<width; i++){
			if(board[i][j]==1|| board[i][j]==3){
				mine++;
				tot++;
				mycons++;
			}
			else if(board[i][j]==2){
				if(tot>=6&& mycons<=6){
					state[tot*100+mine]++;
				}
				mine=0; tot=0;

				if(myblock==0){
					if(mycons==4 || mycons==5){
						danger_state[mycons-4]++;
					}
					if(mycons==6){
						end_state[0]++;
						return;
					}
				}
				myblock=1;
				mycons=0;
			}
			else{
				tot++;

				if(mycons==4 || mycons==5){
					danger_state[mycons-4]++;
				}
				if(mycons==6){
					end_state[0]++;
					return;
				}
				mycons = 0;
				myblock=0;
			}
		}
		if(tot>=6&& mycons<=6){
			state[tot*100+mine]++;
		}
		if(myblock==0){
			if(mycons==4 || mycons==5){
				danger_state[mycons-4]++;
			}
			if(mycons==6){
				end_state[0]++;
				return;
			}
		}
		mine=0; tot=0; enemy=0; mycons=0; encons=0; myblock = 0; enblock=0;

		// 세로축에서 적돌
		for(int i=0; i<width; i++){
			if(board[i][j]==2){
				enemy++;
				tot++;
				encons++;
			}
			else if(board[i][j]==1|| board[i][j]==3){
				if(tot>=6&& encons<=6){
					state[10000+tot*100+enemy]++;
				}
				enemy=0; tot=0;

				if(enblock==0){
					if(encons==4 || encons==5){
						danger_state[encons-2]++;
					}
					if(encons==6){
						end_state[1]++;
						return;
					}
				}
				enblock=1;
				encons=0;
			}
			else{
				tot++;

				if(encons==4 || encons==5){
					danger_state[encons-2]++;
				}
				if(encons==6){
					end_state[1]++;
					return;
				}
				encons = 0;
				enblock=0;
			}
		}
		if(tot>=6&& encons<=6){
			state[10000+tot*100+enemy]++;
		}
		if(enblock==0){
			if(encons==4 || encons==5){
				danger_state[encons-2]++;
			}
			if(encons==6){
				end_state[1]++;
				return;
			}
		}
	}

	// 오른쪽아래로 대각선
	for(int i=-13; i<14; i++){
		int mine = 0, tot=0, enemy=0;
		int mycons=0, encons=0;
		int myblock = 0, enblock=0;
		// 오른쪽아래로 대각선에서 내돌
		for(int j=0; j<height; j++){
			if(i+j<0 || i+j>=width){
				continue;
			}

			if(board[i+j][j]==1 || board[i+j][j]==3){
				mine++;
				tot++;
				mycons++;
			}
			else if(board[i+j][j]==2){
				if(tot>=6&& mycons<=6){
					state[tot*100+mine]++;
				}
				mine=0; tot=0;
				
				if(myblock==0){
					if(mycons==4 || mycons==5){
						danger_state[mycons-4]++;
					}
					if(mycons==6){
						end_state[0]++;
						return;
					}
				}
				myblock=1;
				mycons=0;
			}
			else{
				tot++;

				if(mycons==4 || mycons==5){
					danger_state[mycons-4]++;
				}
				if(mycons==6){
					end_state[0]++;
					return;
				}
				mycons = 0;
				myblock=0;
			}
		}
		if(tot>=6&& mycons<=6){
			state[tot*100+mine]++;
		}
		if(myblock==0){
			if(mycons==4 || mycons==5){
				danger_state[mycons-4]++;
			}
			if(mycons==6){
				end_state[0]++;
				return;
			}
		}
		mine=0; tot=0; enemy=0; mycons=0; encons=0; myblock = 0; enblock=0;

		//오른쪽아래로 대각선에서 적돌
		for(int j=0; j<height; j++){
			if(i+j<0 || i+j>=width){
				continue;
			}
			if(board[i+j][j]==2){
				enemy++;
				tot++;
				encons++;
			}
			else if(board[i+j][j]==1|| board[i+j][j]==3){
				if(tot>=6&& encons<=6){
					state[10000+tot*100+enemy]++;
				}
				enemy=0; tot=0;

				if(enblock==0){
					if(encons==4 || encons==5){
						danger_state[encons-2]++;
					}
					if(encons==6){
						end_state[1]++;
						return;
					}
				}
				enblock=1;
				encons=0;
			}
			else{
				tot++;

				if(encons==4 || encons==5){
					danger_state[encons-2]++;
				}
				if(encons==6){
					end_state[1]++;
					return;
				}
				encons = 0;
				enblock=0;
			}
		}
		if(tot>=6&& encons<=6){
			state[10000+tot*100+enemy]++;
		}
		if(enblock==0){
			if(encons==4 || encons==5){
				danger_state[encons-2]++;
			}
			if(encons==6){
				end_state[1]++;
				return;
			}
		}
		// enemy=0; tot=0;mycons=0; encons=0; myblock = 0; enblock=0;
	}


	// 오른쪽위로 대각선
	for(int i=5; i<32; i++){
		int mine = 0, tot=0, enemy=0;
		int mycons=0, encons=0;
		int myblock = 0, enblock=0;
		// 오른쪽위로 대각선에서 내돌
		for(int j=0; j<height; j++){
			if(i-j<0 || i-j>=width){
				continue;
			}

			if(board[i-j][j]==1 || board[i-j][j]==3){
				mine++;
				tot++;
				mycons++;
			}
			else if(board[i-j][j]==2){
				if(tot>=6&& mycons<=6){
					state[tot*100+mine]++;
				}
				mine=0; tot=0;
				
				if(myblock==0){
					if(mycons==4 || mycons==5){
						danger_state[mycons-4]++;
					}
					if(mycons==6){
						end_state[0]++;
						return;
					}
				}
				myblock=1;
				mycons=0;
			}
			else{
				tot++;

				if(mycons==4 || mycons==5){
					danger_state[mycons-4]++;
				}
				if(mycons==6){
					end_state[0]++;
					return;
				}
				mycons = 0;
				myblock=0;
			}
		}
		if(tot>=6&& mycons<=6){
			state[tot*100+mine]++;
		}
		if(myblock==0){
			if(mycons==4 || mycons==5){
				danger_state[mycons-4]++;
			}
			if(mycons==6){
				end_state[0]++;
				return;
			}
		}
		mine=0; tot=0; enemy=0;mycons=0; encons=0; myblock = 0; enblock=0;

		//오른쪽위로 대각선에서 적돌
		for(int j=0; j<height; j++){
			if(i-j<0 || i-j>=width){
				continue;
			}
			if(board[i-j][j]==2){
				enemy++;
				tot++;
				encons++;
			}
			else if(board[i-j][j]==1|| board[i-j][j]==3){
				if(tot>=6&& encons<=6){
					state[10000+tot*100+enemy]++;
				}
				enemy=0; tot=0;

				if(enblock==0){
					if(encons==4 || encons==5){
						danger_state[encons-2]++;
					}
					if(encons==6){
						end_state[1]++;
						return;
					}
				}
				enblock=1;
				encons=0;
			}
			else{
				tot++;

				if(encons==4 || encons==5){
					danger_state[encons-2]++;
				}
				if(encons==6){
					end_state[1]++;
					return;
				}
				encons = 0;
				enblock=0;
			}
		}
		if(tot>=6&& encons<=6){
			state[10000+tot*100+enemy]++;
		}
		if(enblock==0){
			if(encons==4 || encons==5){
				danger_state[encons-2]++;
			}
			if(encons==6){
				end_state[1]++;
				return;
			}
		}
		// enemy=0; tot=0;mycons=0; encons=0; myblock = 0; enblock=0;
	}
}

putstone put_first_stone(double alpha, double beta,int step, clock_t clock_start){
	double val = intmin;
	putstone temp_stone;

	// make lookuptable for action
	set<point, compare> lookuptable;
	for(int i=0; i<width; i++){
		for(int j=0; j<height; j++){
			if(board[i][j]!=0){
				for(int si=i-2;si<=i+2; si++){
					for(int sj=j-2;sj<=j+2;sj++){
						if(si>=0 && si<width && sj>=0 && sj<height &&board[si][sj]==0){
							lookuptable.insert(point{si,sj});
						}
					}
				}
			}
		}
	}

	if(lookuptable.size()<1){
		return putstone{{width/2,-1},{height/2,-1},0};
	}
	
	// if(check_timeout(clock_start)){
	// 	return putstone{{-1,-1},{-1,-1},intmin};
	// }

	// 돌을 두개 놓는 action들을 돌면서 다음 step으로 간다.
	for(set<point, compare>::iterator si=lookuptable.begin(); si!=lookuptable.end(); si++){
			point p1 = *si;
			board[p1.x][p1.y] = 1;
			putstone nowput;
			nowput.x[0] = p1.x;
			nowput.y[0] = p1.y;

			putstone next =  min_value(alpha,beta,step-1,clock_start);
			board[p1.x][p1.y] = 0;
			
			val = max(val,next.score);
			if(val == next.score){
				nowput.score = val;
				temp_stone = nowput;
			}
			if(val>beta){
				nowput.score = val;
				return nowput;
			}
			alpha = max(alpha, val);
			
			if(check_timeout(clock_start)){
				nowput.score = intmin+10;
				return nowput;
			}
		
	}

	return temp_stone;
}


// alpha-beta-pruning start
// parameter
// int step(iterative deepening search를 위해)
// int clock_start  : 시간 제약조건을 위해
putstone alpha_beta_search(int step, clock_t clock_start){
	return max_value(intmin, intmax, step, clock_start);
}

// int alpha : max bound
// int beta : min bound
putstone max_value(double alpha, double beta,int step, clock_t clock_start){
	if(step==0 ){ 
		return putstone{{-1,-1},{-1,-1}, utility()};
	}
	double val = intmin;
	putstone temp_stone;

	// make lookuptable for action
	set<point, compare> lookuptable;
	for(int i=0; i<width; i++){
		for(int j=0; j<height; j++){
			if(board[i][j]!=0){
				for(int si=i-2;si<=i+2; si++){
					for(int sj=j-2;sj<=j+2;sj++){
						if(abs(si-i)+abs(sj-j)==3){
							continue;
						}
						if(si>=0 && si<width && sj>=0 && sj<height &&board[si][sj] == 0){
							lookuptable.insert(point{si,sj});
						}
					}
				}
			}
		}
	}

	//cout<<"look size: "<<lookuptable.size()<<"\n";
	// 돌을 둘 수 있는 자리가 1개 남았을 때
	if(lookuptable.size()==1){
		set<point,compare>::iterator sit = lookuptable.begin();
		point ps = *sit;
		return putstone{{ps.x,-1},{ps.y,-1}, utility()};
	}

	
	

	// 돌을 두개 놓는 action들을 돌면서 다음 step으로 간다.
	// int testidx = 0;
	for(set<point, compare>::iterator si=lookuptable.begin(); si!=lookuptable.end(); si++){
		set<point, compare>::iterator sj = si;
		sj++;
		for(sj; sj!=lookuptable.end(); sj++){
			point p1 = *si;
			point p2 = *sj;
			board[p1.x][p1.y] = 1;
			board[p2.x][p2.y] = 1;
			putstone nowput;
			nowput.x[0] = p1.x; nowput.x[1] = p2.x;
			nowput.y[0] = p1.y; nowput.y[1] = p2.y;

			putstone next =  min_value(alpha,beta,step-1,clock_start);
			board[p1.x][p1.y] = 0;
			board[p2.x][p2.y] = 0;
			
			val = max(val,next.score);
			if(val == next.score){
				nowput.score = val;
				temp_stone = nowput;
			}
			if(val>beta){
				nowput.score = val;
				return nowput;
			}
			alpha = max(alpha, val);
			

			// cout<<testidx<<"\n";
			// testidx++;
			if(check_timeout(clock_start)){
				nowput.score = intmin+10;
				cout<<"timeout\n";
				return nowput;
			}
		}
	}

	return temp_stone;
}



putstone min_value(double alpha, double beta,int step, clock_t clock_start){
	// 지금이 iterative deepening으로 보는 마지막 단계면 종료
	// 돌이 6개 두여있으면 종료인데 이 때 어떻게 처리?
	if(step==0){

		return putstone{{-1,-1}, {-1,-1}, utility()};
	}

	double val = intmax;
	putstone temp_stone;

	// make lookuptable for action
	set<point, compare> lookuptable;
	for(int i=0; i<width; i++){
		for(int j=0; j<height; j++){
			if(board[i][j]!=0){
				for(int si=i-2;si<=i+2; si++){
					for(int sj=j-2;sj<=j+2;sj++){
						if(abs(si-i)+abs(sj-j)==3){
							continue;
						}
						if(si>=0 && si<width && sj>=0 && sj<height &&board[si][sj] == 0){
							lookuptable.insert(point{si,sj});
						}
					}
				}
			}
		}
	}


	// 돌을 둘 수 있는 자리가 1개남음
	if(lookuptable.size()==1){
		set<point,compare>::iterator sit = lookuptable.begin();
		point ps = *sit;
		return putstone{{ps.x,-1},{ps.y,-1}, utility()};
	}


	// 돌을 두개 놓는 action들을 돌면서 다음 step으로 간다.
	for(set<point, compare>::iterator si=lookuptable.begin(); si!=lookuptable.end(); si++){
		set<point, compare>::iterator sj = si;
		sj++;
		for(sj; sj!=lookuptable.end(); sj++){
			point p1 = *si;
			point p2 = *sj;
			board[p1.x][p1.y] = 2;
			board[p2.x][p2.y] = 2;
			putstone nowput;
			nowput.x[0] = p1.x; nowput.x[1] = p2.x;
			nowput.y[0] = p1.y; nowput.y[1] = p2.y;

			putstone next =  max_value(alpha,beta,step-1,clock_start);
			board[p1.x][p1.y] = 0;
			board[p2.x][p2.y] = 0;

			val = min(val,next.score);
			if(val == next.score){
				nowput.score = val;
				temp_stone = nowput;
			}
			if(val<alpha){
				nowput.score = val;
				return nowput;
			}
			beta = min(beta, val);

			
			if(check_timeout(clock_start)){
				nowput.score = intmin+10;
				cout<<"timeout\n";
				return nowput;
			}
		}
	}

	return temp_stone;
}

bool terminal_test(){
	count_state();
	if(end_state[0]>0 || end_state[1]>0){
		return true;
	}
	return false;
}

double utility(){
	count_state();
	double value=0.0;
	value += end_state[0] * 5000000;
	value -= end_state[1] * 10000000;
	value += ((danger_state[0]+ danger_state[1])*5*5*20);
	value -= ((danger_state[2]+ danger_state[3])*5*5*20);	

	// state : 00601 ~ 01919 10601 ~ 11919
	
	// benefit to me
	for(int i=6; i<=19; i++){
		for(int j=1; j<=i; j++){
			if(j<4){
				value += state[i*100+j]*j*j;// + (i);
			}
			else if(6>j>=4){
				value += state[i*100+j]*5*5*20;// + (i)*(i);
			}
			else{
				value += state[i*100+j]*5*5*20;//+ (i)*(i);
			}
		}
	}
	// bad to me
	
	for(int i=6; i<=19; i++){
		for(int j=1; j<=i; j++){if(j<4){
				value -= state[10000+i*100+j]*j*j;//+ (i);
			}
			else if(6>j>=4){
				value -= state[10000+i*100+j]*5*5*20*8;//+ (i)*(i);
			}
			else{
				value -= state[10000+i*100+j]*5*5*20*8;//+ (i)*(i);
			}
		}
	}

	//show();
	return value;
}


double utility_lr(){
	count_state();
	double value=0.0;
	value += end_state[0] * 5000000;
	value -= end_state[1] * 10000000;
	// value += ((danger_state[0]+ danger_state[1])*5*5*20);
	// value -= ((danger_state[2]+ danger_state[3])*5*5*20);	

	// state : 00601 ~ 01919 10601 ~ 11919
	double params[350] ={-0.8891095601161974,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.036759477542832646,-0.25570107964729805,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.13165871977059246,0.23209720567210684,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.34921258480861034,0.2306669937899741,0.08846119455706077,0.07737924909221651,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,1.0208864721040063,-1.408644882346492,-0.5897395840364056,-0.13708229804939048,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.7127159095418679,-0.8010598924441749,0.08973203990203275,-0.0019123813708573363,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.21905379483000928,-0.33131915539562495,-0.0001563585159340058,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.10623734183587398,0.0037050609478995813,-3.794873126868967e-06,1.7673265508428467e-05,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.4210263736729063,0.12500867823095468,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.16976965467459704,-0.27955658576806636,-0.012367100938175892,0.14913332205901153,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.13101420721284396,-0.5829595429175439,-0.06715912558430112,0.044360056849942646,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.9667268586456733,-0.03264654709860013,0.017040074118175975,0.05424250817190563,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.017045065915997283,-0.6040552221404106,-0.21901665327041048,-0.18423524424465051,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.6633257171226549,0.5493868736192139,-0.15996156924419924,0.04343951838502567,0.0013947120098435951,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.8891200775739664,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.036732351754626004,0.2556830294637375,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.1316773030192859,-0.23211300815946795,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.34924545986688205,-0.2306314188147076,-0.08846340543524148,-0.07739415377755764,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-1.0208471857131658,1.408619820192942,0.5897356355237752,0.13707315857909497,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.7126852202361343,0.801075013474841,-0.0897411019616433,0.0019057846134873573,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.21905936151708402,0.3313184875915292,0.0001487424572426502,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.10624096576289555,-0.003715424770049056,-3.794873126868967e-06,-2.5353417049578438e-05,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.4210385558278946,-0.12501462359241425,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.16981291909042787,0.27956481898092844,0.01235549725752136,-0.14914545805429308,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.13099138902929278,0.582958118279031,0.06715505279157863,-0.04436505112441696,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.9667136520719242,0.03263682919341224,-0.01704453671151024,-0.05424987668605011,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,0.017044516998515637,0.604055157025332,0.21900637990478583,0.18422780330528415,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-0.6633037485292109,-0.5494747227259177,0.15995423101550424,-0.043459986658551115,-0.0014036924113267366,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06,-3.794873126868967e-06};


	// benefit to me
	int pidx = 0;
	for(int i=6; i<=19; i++){
		for(int j=1; j<=i; j++){
			value += params[pidx] * state[i*100+j];
			pidx++;
		}
	}
	// bad to me
	
	for(int i=6; i<=19; i++){
		for(int j=1; j<=i; j++){
			value += params[pidx]*state[10000+i*100+j];
			pidx++;
		}
	}

	//show();
	return value;
}










//---------------------------
void init(){
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            board[i][j]=0;
        }
    }
}

int showBoard(int x,int y){
    return board[x][y];
}

void show(){
    cout<<" ";
    for(int i=0; i<width; i++){
        cout<<char(i+65)<<" ";
    }
    cout<<"\n";
    for(int i=0; i<width; i++){
        cout<<char(i+65);
        for(int j=0; j<height; j++){
            if(board[i][j]==0){
                cout<<". ";
            }
            if(board[i][j]==1){
                cout<<"o ";
            }
            if(board[i][j]==2){
                cout<<"x ";
            }
        }
        cout<<"\n";
    } 
    cout<<"\n";
}


void domymove(int x[], int y[], int cnt) {
    for(int i=0; i<cnt; i++){
		cout<<x[i]<<", "<<y[i]<<"\n";
        board[x[i]][y[i]] = 1;
    }
}

bool checkinput(char x[2], char y[2]){
	if(!(x[0]>='A' && x[0]<='Z')){
		return false;
	}
	if(!(x[1]>='A' && x[1]<='Z')){
		return false;
	}
	if(!(y[0]>='A' && y[0]<='Z')){
		return false;
	}
	if(!(y[1]>='A' && y[1]<='Z')){
		return false;
	}
	return true;
}

void writeData(vector<vector<int> > &data, int mode){
	count_state();
	data.push_back(vector<int>());
	int s = data.size()-1;

	int m = (mode&1);
	for(int i=6; i<=19; i++){
		for(int j=1; j<=i; j++){
			// mode : 0->0, 1->1
			data[s].push_back(state[10000*m+i*100+j]);		
		}
	}

	m = (mode^1);
	for(int i=6; i<=19; i++){
		for(int j=1; j<=i; j++){
			// mode : 0->1, 1->0
			data[s].push_back(state[10000*m+i*100+j]);		
		}
	}
}

void change_board(){
	for(int i=0; i<width; i++){
		for(int j=0; j<height; j++){
			if(board[i][j] == 1){
				board[i][j]=2;
			}	
			else if(board[i][j]==2){
				board[i][j]=1;
			}
		}
	}
}

int main(){
	string filePath = "./data2.csv";
	vector<vector<int> > blackdata;
	vector<vector<int> > whitedata;


    init();
    int cnt=1;
    while(true){
        myturn(cnt);
        if(cnt==1){
            cnt=2;
        }

        show();
		writeData(blackdata, 0);
		writeData(whitedata, 1);

		terminal_test();
		if(end_state[0]>0){
			cout<<"computer win\n";
			break;
		}
        
        // char x[2]={0,},y[2]={0,};
		// cout<<"insert stone\n";
        // cin>>x[0]>>y[0]>>x[1]>>y[1];
		// while(!checkinput(x,y) || !(board[x[0]-65][y[0]-65] == 0 && board[x[1]-65][y[1]-65]==0)){
		// 	cout<<"wrong input, redo\n";
		// 	cin>>x[0]>>y[0]>>x[1]>>y[1];
		// }
        // for(int i=0; i<cnt; i++){
        //     board[x[i]-65][y[i]-65] = 2;
        // }

		change_board();
		myturn(cnt);
		change_board();

        show();
		writeData(blackdata,0);
		writeData(whitedata,1);
		terminal_test();
		if(end_state[1]>0){
			cout<<"user win\n";
			break;
		}
    }

	ofstream fout;
	ifstream fin;
	fin.open(filePath);
	if(!fin.good()){
		fin.close();
		
		fout.open(filePath);
		for(int i=6; i<=19; i++){
			for(int j=1; j<=i; j++){
				int temp = i*100+j;
				fout<<temp<<",";	
			}
		}
		for(int i=6; i<=19; i++){
			for(int j=1; j<=i; j++){
				int temp = 10000+i*100+j;
				fout<<temp<<",";
			}
		}
		fout<<"result\n";
		fout.close();
	}

	fout.open(filePath, ios::app);
	if(end_state[0]>0){
		for(int i=2; i<blackdata.size(); i++){
			for(int j=0;j<blackdata[i].size(); j++){
				fout<<(blackdata[i][j])<<",";
			}
			fout<<"WIN\n";
		}
		for(int i=2; i<whitedata.size(); i++){
			for(int j=0;j<whitedata[i].size(); j++){
				fout<<whitedata[i][j]<<",";
			}
			fout<<"LOSE\n";
		}
	}
	else{
		for(int i=2; i<whitedata.size(); i++){
			for(int j=0;j<whitedata[i].size(); j++){
				fout<<whitedata[i][j]<<",";
			}
			fout<<"WIN\n";
		}
		for(int i=2; i<blackdata.size(); i++){
			for(int j=0;j<blackdata[i].size(); j++){
				fout<<blackdata[i][j]<<",";
			}
			fout<<"LOSE\n";
		}
		
	}
	
	fout.close();
    return 0;
}





