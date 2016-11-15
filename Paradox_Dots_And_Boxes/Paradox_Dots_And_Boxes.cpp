// Paradox_Dots_And_Boxes.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Shell.h"

using namespace DAB;
using namespace std;

//Program Info
#include <map>



int _tmain(int argc, _TCHAR* argv[])
{
	/*BitBoard board = BOARD::RandomBitBoard();
	//board = STATE::CaptureAllBoxes(board);
	State(board).Visualization();
	cout << "\n\n\n\n\n\n\n";
	BitBoard next = board;
	for (int i = 0; i < 2; i++)
	{
		next = STATE::ReverseBox(next);
	}
	State(next).Visualization();
	cout << "\n\n\n\n\n\n\n";
	State(STATE::MinimalForm(next)).Visualization();
	*/
	SHELL::Info();
	SHELL::Start();

	return 0;
	
}

