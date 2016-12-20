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
		Cprintf("o", 1);//深蓝
		Cprintf("o", 2);//军绿
		Cprintf("o", 3);//暗青
		Cprintf("!", 4);//棕
		Cprintf("r", 5);//紫
		Cprintf("o", 6);//深黄
		Cprintf("o", 7);//暗白
		Cprintf("l", 8);//灰
		Cprintf("e", 9);//蓝色
		Cprintf("H", 10);//绿色
		Cprintf("l", 11);//青色
		Cprintf("l", 12);//红色
		Cprintf("o", 13);//粉色
		Cprintf("d", 14);//黄
		Cprintf("W", 15);//白
		*/
	}
	void CprintNum(int num, WORD color)
	{
		char c[4];
		sprintf_s(c, "%d", num);
		Cprintf(str(c), color);
	}
}