
#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include "Connect6Algo.h"
#define BOARD_SIZE 20
#define intmax 1000000000
#define intmin -1000000000

char info[] = { "TeamName:룰베이스>딥러닝,Department:고려대학교" };
int board[BOARD_SIZE][BOARD_SIZE];

int board_init(){
	for(int i=0; i<BOARD_SIZE; i++){
		for(int j=0; j<BOARD_SIZE; j++){
			board[i][j] = showBoard(i,j);
		}
	}
}

struct putstone{
	int x[2];
	int y[2];
	int score;
};

putstone alpha_beta_search(){
	return max_value();
}

putstone max_value(){
	if(terminal_test()){
		return putstone{{-1,-1},{-1,-1}, utility()};
	}
}

putstone min_value(){
	if(terminal_test()){
		return putstone{{-1,-1}, {-1,-1}, utility()};
	}
}

bool terminal_test(){
	return false;
}

int utility(){
	return 0;
}

void myturn(int cnt) {
	
	int x[2], y[2];

	// 이 부분에서 알고리즘 프로그램(AI)을 작성하십시오. 기본 제공된 코드를 수정 또는 삭제하고 본인이 코드를 사용하시면 됩니다.
	// 현재 Sample code의 AI는 Random으로 돌을 놓는 Algorithm이 작성되어 있습니다.

	srand((unsigned)time(NULL));

	for (int i = 0; i < cnt; i++) {
		do {
			x[i] = rand() % width;
			y[i] = rand() % height;
			if (terminateAI) return;
		} while (!isFree(x[i], y[i]));

		if (x[1] == x[0] && y[1] == y[0]) i--;
	}

	// 이 부분에서 자신이 놓을 돌을 출력하십시오.
	// 필수 함수 : domymove(x배열,y배열,배열크기)
	// 여기서 배열크기(cnt)는 myturn()의 파라미터 cnt를 그대로 넣어야합니다.
	domymove(x, y, cnt);
}