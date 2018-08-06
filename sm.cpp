#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <vector>
#include <set>
#include <algorithm>
#include "Connect6Algo.h"
using namespace std;

#define BOARD_SIZE 20
#define intmax 1000000000
#define intmin -1000000000
#define STATE_SIZE 12020
#define DANGER_STATE_SIZE 10
#define timebound 0.3

char info[100] = { "TeamName:�꺣�̽�>������,Department:������б�" };
// �ٵ��� ����
int board[BOARD_SIZE][BOARD_SIZE];
// �⺻ attribute��������
int state[STATE_SIZE];
// �������ʿ� END �Ǵ� ��Ȳ
// ���� 4������ ��ĭ ��-0, ���� 5������ ��ĭ��-1, ���� 4������ ��ĭ ��-2, ���� 5������ ��ĭ��-3
int danger_state[DANGER_STATE_SIZE];
// 0�� ���� 6������, 1�� ���� 6������
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


void board_init();
void state_init();
bool check_timeout(clock_t clock_start);
void count_state();
putstone put_first_stone(double alpha, double beta,int step, clock_t clock_start);
putstone alpha_beta_search(int step, clock_t clock_start);
putstone max_value(double alpha, double beta,int step, clock_t clock_start);
putstone min_value(double alpha, double beta,int step, clock_t clock_start);
bool terminal_test();
double utility();



void myturn(int cnt) {
	clock_t clock_start = clock();
	int x[2], y[2];


	// �� �κп��� �˰��� ���α׷�(AI)�� �ۼ��Ͻʽÿ�. �⺻ ������ �ڵ带 ���� �Ǵ� �����ϰ� ������ �ڵ带 ����Ͻø� �˴ϴ�.
	// ���� Sample code�� AI�� Random���� ���� ���� Algorithm�� �ۼ��Ǿ� �ֽ��ϴ�.
	board_init();
	state_init();
	
	
	//ù ���� 1���� �ҿ� �����Ѵ�.
	//�׳� cnt = 1�̸� ���� ��� ���� �ѱ�� �� ���?
	//������ ��ֹ��� �ִٸ�? 
	if(cnt==1){
		int step = 1;
		double mxv = intmin+10;
		while(step<20){
			putstone ps = put_first_stone(intmin, intmax, step, clock_start);
			if(ps.score>mxv){
				x[0] = ps.x[0]; x[1] = ps.x[1];
				y[0] = ps.y[0]; y[1] = ps.y[1];	
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
	// �ð������� ��� �ٲ�� �� ���? => s_time
	int step=1;
	double mxval = intmin+10;
	while( step<20){
		putstone ps = alpha_beta_search(step, clock_start);
		if(ps.score > mxval){
			x[0] = ps.x[0]; x[1] = ps.x[1];
			y[0] = ps.y[0]; y[1] = ps.y[1];
		}
		step++;
		if(check_timeout(clock_start)){
			break;
		}
	}


	// �� �κп��� �ڽ��� ���� ���� ����Ͻʽÿ�.
	// �ʼ� �Լ� : domymove(x�迭,y�迭,�迭ũ��)
	// ���⼭ �迭ũ��(cnt)�� myturn()�� �Ķ���� cnt�� �״�� �־���մϴ�.
	domymove(x, y, cnt);
}



// �ٵ��� ���� �ʱ�ȭ
void board_init(){
	for(int i=0; i<BOARD_SIZE; i++){
		for(int j=0; j<BOARD_SIZE; j++){
			board[i][j] = showBoard(i,j);
		}
	}
}

// ���� �ʱ�ȭ
void state_init(){
	memset((void*)state, 0, (size_t)STATE_SIZE);
	memset((void*)danger_state, 0, (size_t)DANGER_STATE_SIZE);
	end_state[0]=0;
	end_state[1]=0;
}

//check timeout
bool check_timeout(clock_t clock_start){
	if(s_time - (float)(clock()-clock_start)/CLOCKS_PER_SEC < timebound){
		return true;
	}
	return false;
}


// state ���� ���� �ʱ�ȭ ��Ű�� ���� ���� �������� ���� �ش�.
void count_state(){
	state_init();
	// �����࿡�� state ���� ����
	for(int i=0; i<width; i++){
		int mine = 0, tot=0, enemy=0;
		int mycons=0, encons=0;
		int myblock = 0, enblock=0;
		// �����࿡�� ����
		for(int j=0; j<height; j++){
			if(board[i][j]==1 || board[i][j]==3){
				mine++;
				tot++;
				mycons++;
			}
			else if(board[i][j]==2){
				if(tot>=6){
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
		if(tot>=6){
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
		mine=0; tot=0; mycons=0; encons=0; myblock = 0; enblock=0;

		//�����࿡�� ����
		for(int j=0; j<height; j++){
			if(board[i][j]==2){
				enemy++;
				tot++;
				encons++;
			}
			else if(board[i][j]==1|| board[i][j]==3){
				if(tot>=6){
					state[10000+tot*100+enemy]++;
				}
				enemy=0; tot=0;

				if(enblock==0){
					if(encons==4 || encons==5){
						danger_state[encons-4]++;
					}
					if(encons==6){
						end_state[0]++;
						return;
					}
				}
				enblock=1;
				encons=0;
			}
			else{
				tot++;

				if(encons==4 || encons==5){
					danger_state[encons-4]++;
				}
				if(encons==6){
					end_state[0]++;
					return;
				}
				encons = 0;
				enblock=0;
			}
		}
		if(tot>=6){
			state[10000+tot*100+enemy]++;
		}
		if(enblock==0){
			if(encons==4 || encons==5){
				danger_state[encons-4]++;
			}
			if(encons==6){
				end_state[0]++;
				return;
			}
		}
		// enemy=0; tot=0;mycons=0; encons=0; myblock = 0; enblock=0;
	}

	// �����࿡�� state ����
	for(int j=0; j<height; j++){
		int mine = 0, tot=0, enemy=0;
		int mycons=0, encons=0;
		int myblock = 0, enblock=0;
		// �����࿡�� ����
		for(int i=0; i<width; i++){
			if(board[i][j]==1|| board[i][j]==3){
				mine++;
				tot++;
				mycons++;
			}
			else if(board[i][j]==2){
				if(tot>=6){
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
		if(tot>=6){
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
		mine=0; tot=0; mycons=0; encons=0; myblock = 0; enblock=0;

		// �����࿡�� ����
		for(int i=0; i<width; i++){
			if(board[i][j]==2){
				enemy++;
				tot++;
				encons++;
			}
			else if(board[i][j]==1|| board[i][j]==3){
				if(tot>=6){
					state[10000+tot*100+enemy]++;
				}
				enemy=0; tot=0;

				if(enblock==0){
					if(encons==4 || encons==5){
						danger_state[encons-4]++;
					}
					if(encons==6){
						end_state[0]++;
						return;
					}
				}
				enblock=1;
				encons=0;
			}
			else{
				tot++;

				if(encons==4 || encons==5){
					danger_state[encons-4]++;
				}
				if(encons==6){
					end_state[0]++;
					return;
				}
				encons = 0;
				enblock=0;
			}
		}
		if(tot>=6){
			state[10000+tot*100+enemy]++;
		}
		if(enblock==0){
			if(encons==4 || encons==5){
				danger_state[encons-4]++;
			}
			if(encons==6){
				end_state[0]++;
				return;
			}
		}
	}

	// �����ʾƷ��� �밢��
	for(int i=-13; i<14; i++){
		int mine = 0, tot=0, enemy=0;
		int mycons=0, encons=0;
		int myblock = 0, enblock=0;
		// �����ʾƷ��� �밢������ ����
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
				if(tot>=6){
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
		if(tot>=6){
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
		mine=0; tot=0; mycons=0; encons=0; myblock = 0; enblock=0;

		//�����ʾƷ��� �밢������ ����
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
				if(tot>=6){
					state[10000+tot*100+enemy]++;
				}
				enemy=0; tot=0;

				if(enblock==0){
					if(encons==4 || encons==5){
						danger_state[encons-4]++;
					}
					if(encons==6){
						end_state[0]++;
						return;
					}
				}
				enblock=1;
				encons=0;
			}
			else{
				tot++;

				if(encons==4 || encons==5){
					danger_state[encons-4]++;
				}
				if(encons==6){
					end_state[0]++;
					return;
				}
				encons = 0;
				enblock=0;
			}
		}
		if(tot>=6){
			state[10000+tot*100+enemy]++;
		}
		if(enblock==0){
			if(encons==4 || encons==5){
				danger_state[encons-4]++;
			}
			if(encons==6){
				end_state[0]++;
				return;
			}
		}
		// enemy=0; tot=0;mycons=0; encons=0; myblock = 0; enblock=0;
	}


	// ���������� �밢��
	for(int i=5; i<32; i++){
		int mine = 0, tot=0, enemy=0;
		int mycons=0, encons=0;
		int myblock = 0, enblock=0;
		// ���������� �밢������ ����
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
				if(tot>=6){
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
		if(tot>=6){
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
		mine=0; tot=0; mycons=0; encons=0; myblock = 0; enblock=0;

		//���������� �밢������ ����
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
				if(tot>=6){
					state[10000+tot*100+enemy]++;
				}
				enemy=0; tot=0;

				if(enblock==0){
					if(encons==4 || encons==5){
						danger_state[encons-4]++;
					}
					if(encons==6){
						end_state[0]++;
						return;
					}
				}
				enblock=1;
				encons=0;
			}
			else{
				tot++;

				if(encons==4 || encons==5){
					danger_state[encons-4]++;
				}
				if(encons==6){
					end_state[0]++;
					return;
				}
				encons = 0;
				enblock=0;
			}
		}
		if(tot>=6){
			state[10000+tot*100+enemy]++;
		}
		if(enblock==0){
			if(encons==4 || encons==5){
				danger_state[encons-4]++;
			}
			if(encons==6){
				end_state[0]++;
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
						if(board[si][sj]==0){
							lookuptable.insert(point{si,sj});
						}
					}
				}
			}
		}
	}

	
	if(check_timeout(clock_start)){
		return putstone{{-1,-1},{-1,-1},intmin};
	}

	// ���� �ΰ� ���� action���� ���鼭 ���� step���� ����.
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
			if(val>=beta){
				nowput.score = val;
				return nowput;
			}
			alpha = max(alpha, val);
			
			if(check_timeout(clock_start)){
				return putstone{{-1,-1},{-1,-1},intmin};
			}
		
	}

	return temp_stone;
}


// alpha-beta-pruning start
// parameter
// int step(iterative deepening search�� ����)
// int clock_start  : �ð� ���������� ����
putstone alpha_beta_search(int step, clock_t clock_start){
	return max_value(intmin, intmax, step, clock_start);
}

// int alpha : max bound
// int beta : min bound
putstone max_value(double alpha, double beta,int step, clock_t clock_start){
	if(step==0 || terminal_test()){
		if(end_state[0]>0){
			return putstone{{-1,-1},{-1,-1},intmax-100000};
		}
		if(end_state[1]>0){
			return putstone{{-1,-1},{-1,-1},intmin+100000};
		}
		
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
						if(board[si][sj] == 0){
							lookuptable.insert(point{si,sj});
						}
					}
				}
			}
		}
	}

	
	if(check_timeout(clock_start)){
		return putstone{{-1,-1},{-1,-1},intmin};
	}

	// ���� �ΰ� ���� action���� ���鼭 ���� step���� ����.
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
			if(val>=beta){
				nowput.score = val;
				return nowput;
			}
			alpha = max(alpha, val);
			
			if(check_timeout(clock_start)){
				return putstone{{-1,-1},{-1,-1},intmin};
			}
		}
	}

	return temp_stone;
}

putstone min_value(double alpha, double beta,int step, clock_t clock_start){
	// ������ iterative deepening���� ���� ������ �ܰ�� ����
	// ���� 6�� �ο������� �����ε� �� �� ��� ó��?
	if(step==0 || terminal_test()){
		if(end_state[0]>0){
			return putstone{{-1,-1},{-1,-1},intmax-100000};
		}
		if(end_state[1]>0){
			return putstone{{-1,-1},{-1,-1},intmin+100000};
		}

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
						if(board[si][sj] == 0){
							lookuptable.insert(point{si,sj});
						}
					}
				}
			}
		}
	}

	
	if(check_timeout(clock_start)){
		return putstone{{-1,-1},{-1,-1},intmin};
	}

	// ���� �ΰ� ���� action���� ���鼭 ���� step���� ����.
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
			if(val<=alpha){
				nowput.score = val;
				return nowput;
			}
			beta = min(beta, val);

			
			if(check_timeout(clock_start)){
				return putstone{{-1,-1},{-1,-1},intmin};
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


	return 0;
}