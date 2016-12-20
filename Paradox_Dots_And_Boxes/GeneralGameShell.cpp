#include "stdafx.h"
#include <Windows.h>
#include "GeneralGameShell.h"

/*
* General game shell is used for game AI developing and this shell is consist of the concept i called 'page',
* each page is an single set of command and different type of data can be binded to each page by use template.
* An easy example have been showed in the ShellExample.cpp.
*
* version: 2016/12/20
* copyright: Junkai Lu
* email: Junkai-Lu@outlook.com
*/

namespace console
{
	//colorful print
	void Cprintf(std::string tex, console::color::Color color)
	{
		WORD colorOld;
		HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(handle, &csbi);
		colorOld = csbi.wAttributes;
		SetConsoleTextAttribute(handle, color);
		std::cout << tex;
		SetConsoleTextAttribute(handle, colorOld);
	}
}
namespace general_game_shell
{
	//ShellPageBase
	ShellPageBase::ShellPageBase(GameShell* belonging_shell, std::string name) :
		_belonging_shell(belonging_shell),
		_name(name),
		_index(AllocPageIndex()),
		_info([]()->void {})
	{
	}
	void ShellPageBase::ShowPath() const
	{
		if (_call_source == nullptr)
		{
			console::Cprintf(GameShell::focus_game()->name(), console::color::purple);
			std::cout << " @ ";
			console::Cprintf(_name, console::color::yellow);
		}
		else
		{
			_call_source->ShowPath();
			std::cout << "/";
			console::Cprintf(_name, console::color::green);
		}
	}
	void ShellPageBase::CleanScreen() const
	{
		system("cls");
		_info();
		std::cout << ">> ";
		console::Cprintf("[ Shell ", console::color::deep_yellow);
		console::Cprintf("<" + _name + ">", console::color::yellow);
		console::Cprintf(" ]\n", console::color::deep_yellow);

		std::cout << ">> ";
		console::Cprintf("use 'help' to get more command\n\n", console::color::deep_green);
	}
	void ShellPageBase::BeFocus()
	{
		belonging_shell()->_focus_page = this;
	}

	//GameShell
	GameShell* GameShell::_g_focus_game = nullptr;
	GameShell::GameShell(std::string name):
		_name(name)
	{
		BeFocus();
	}
}