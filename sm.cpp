#include <iostream>
#include <vector>
using namespace std;

#define line 19


/*
six mok rule
put the stone 2 times at once;
But first turn only put 1 stone.
there can be obstacles.
obstacles are treated as the ally stone
*/
enum STONE{
	NONE,
	BLACK,
	WHITE
};


void show_board(vector<vector<STONE> > &board){
	for(int i=0; i<line; i++){
		for(int j=0; j<line; j++){
			cout<<board[i][j]<<" ";
		}
		cout<<"\n";
	}
}

int main(){
	vector<vector<STONE> > board(line, vector<STONE>(line,NONE));
	show_board(board);

	return 0;
}
