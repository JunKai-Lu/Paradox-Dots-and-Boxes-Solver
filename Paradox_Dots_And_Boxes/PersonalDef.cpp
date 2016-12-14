#include "stdafx.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <sstream>
#include "PersonalDef.h"

namespace DAB
{
	void Cprintf(str tex, WORD color)
	{
#ifdef WINDOWS
		WORD colorOld;
		HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(handle, &csbi);
		colorOld = csbi.wAttributes;
		SetConsoleTextAttribute(handle, color);
		std::cout << tex;
		SetConsoleTextAttribute(handle, colorOld);
#endif
		/*
		Cprintf("o", 1);//����
		Cprintf("o", 2);//����
		Cprintf("o", 3);//����
		Cprintf("!", 4);//��
		Cprintf("r", 5);//��
		Cprintf("o", 6);//���
		Cprintf("o", 7);//����
		Cprintf("l", 8);//��
		Cprintf("e", 9);//��ɫ
		Cprintf("H", 10);//��ɫ
		Cprintf("l", 11);//��ɫ
		Cprintf("l", 12);//��ɫ
		Cprintf("o", 13);//��ɫ
		Cprintf("d", 14);//��
		Cprintf("W", 15);//��
		*/
	}
	void CprintNum(int num, WORD color)
	{
		char c[4];
		sprintf_s(c, "%d", num);
		Cprintf(str(c), color);
	}
}