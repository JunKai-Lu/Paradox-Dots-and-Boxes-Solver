#include "stdafx.h"
#include "Shell.h"

using namespace DAB;
using namespace std;


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

