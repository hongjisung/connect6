
#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include "Connect6Algo.h"
#define BOARD_SIZE 20
#define intmax 1000000000
#define intmin -1000000000

char info[] = { "TeamName:�꺣�̽�>������,Department:������б�" };
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

	// �� �κп��� �˰��� ���α׷�(AI)�� �ۼ��Ͻʽÿ�. �⺻ ������ �ڵ带 ���� �Ǵ� �����ϰ� ������ �ڵ带 ����Ͻø� �˴ϴ�.
	// ���� Sample code�� AI�� Random���� ���� ���� Algorithm�� �ۼ��Ǿ� �ֽ��ϴ�.

	srand((unsigned)time(NULL));

	for (int i = 0; i < cnt; i++) {
		do {
			x[i] = rand() % width;
			y[i] = rand() % height;
			if (terminateAI) return;
		} while (!isFree(x[i], y[i]));

		if (x[1] == x[0] && y[1] == y[0]) i--;
	}

	// �� �κп��� �ڽ��� ���� ���� ����Ͻʽÿ�.
	// �ʼ� �Լ� : domymove(x�迭,y�迭,�迭ũ��)
	// ���⼭ �迭ũ��(cnt)�� myturn()�� �Ķ���� cnt�� �״�� �־���մϴ�.
	domymove(x, y, cnt);
}