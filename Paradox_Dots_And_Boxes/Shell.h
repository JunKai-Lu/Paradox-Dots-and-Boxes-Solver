#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <functional>

#include "GameDefine.h"

#pragma  once

namespace DAB
{
	namespace SHELL
	{
		void CleanScreen(str name);

	}

	//class ShellBase is the abstract class of shell.
	class ShellBase
	{
	private:
		str _name;
		ShellBase* _parent_shell;

	public:
		ShellBase(str name, ShellBase* parent_shell);
		ShellBase(ShellBase& sb) = delete;
		inline str name()
		{
			return _name;
		}
		
		inline ShellBase* parent_shell()
		{
			return _parent_shell;
		}
		inline void ShowPath()
		{
			if (_parent_shell == nullptr)
			{
				Cprintf("DAB", 5);
				std::cout << " @ ";
				Cprintf(_name, 2);
			}
			else
			{
				std::cout << "/";
				_parent_shell->ShowPath();
				Cprintf(_name, 2);
				
			}
		}
		virtual void Run() = NULL;
		~ShellBase();
	};

	//class Shell is used for generate all kinds of shell and it conclude extra data.
	template<typename datatype>
	class Shell : public ShellBase
	{
	private:
		std::map<str, std::function<void(datatype&)> > _func;
		std::map<str, std::string> _des;
		datatype _data;
		std::function<bool(str,datatype&)> _extra_command;
		
	public:
		Shell(str name, ShellBase* parent_shell, datatype data) :
			ShellBase(name, parent_shell),
			_data(data),
			_extra_command([](str a, datatype& b)->bool {return false; })
		{
			//default commands
			if (parent_shell != nullptr)
			{
				AddDescript("return", "return to previous menu.");
			}
			else
			{
				AddDescript("return", "exit DAB-Shell.");
			}

			AddFunction("help", [&](datatype& data)->void { ShowCommandList(); }, "get command list");
			AddFunction("cls", [&](datatype& data)->void { SHELL::CleanScreen(this->name()); }, "clean screen.");
		}
		~Shell() = default;

		//get data
		inline datatype& data()
		{
			return _data;
		}

		//get function or descript
		inline bool FuncExist(str command)
		{
			return _func.count(command) > 0;
		}
		inline bool DesExist(str command)
		{
			return _des.count(command) > 0;
		}
		inline std::function<void(datatype&)> GetFunc(str command)
		{
			WARNING_CHECK(!FuncExist(command), "command not exist");
			return _func[command];
		}
		inline std::string GetDes(str command)
		{
			WARNING_CHECK(!FuncExist(command), "command not exist");
			return _des[command];
		}

		//add function or descript
		inline void AddFunction(str command, std::function<void(datatype&)> func, str des)
		{
			_func[command] = func;
			_des[command] = des;
		}
		inline void AddDescript(str command, str des)
		{
			_des[command] = des;
		}
		inline void AddExtarCommand(std::function<bool(str, datatype&)> func)
		{
			_extra_command = func;
		}

		//Run Shell
		inline void ShowCommandList()
		{
			cout << endl << ">> ";
			Cprintf("[ COMMAND LIST ]\n\n", 14);
			for (auto command : _des)
			{
				cout << "   '";
				Cprintf(command.first, 12);
				cout << "'" << string("          ").substr(0, 10 - command.first.length()) << command.second << endl;
			}
			cout << endl << endl;
		}
		void Run()
		{
			ShellManager::set_current_shell(this);
			SHELL::CleanScreen(name());
			for (;;)
			{
				ShellManager::InputTip();
				str command = GetInput();
				if (command == "return")//check return command.
				{
					if (parent_shell() != nullptr)
					{
						SHELL::CleanScreen(parent_shell()->name());
					}
					break;
				}
				else
				{
					if (!_extra_command(command,_data))//check extra commands.
					{
						if (FuncExist(command))//check commands list.
						{
							GetFunc(command)(_data);
						}
						else
						{
							Error("command not found.");//error
						}
					}
				}
			}
		}
	};

	//class ShellManager is a static class which is used for manage shells.
	class ShellManager
	{
	private:
		static std::map<str, ShellBase*> _shell_list;
		static ShellBase* _current_shell;

	public:
		//Shell Manage
		static inline bool ShellExist(str name)
		{
			return _shell_list.count(name) > 0;
		}
		static inline ShellBase* GetShell(str name)
		{
			WARNING_CHECK(!ShellExist(name), "shell not exist");
			return _shell_list[name];
		}
		static inline void RegisterShell(ShellBase* shell)
		{
			_shell_list[shell->name()] = shell;
		}
		static inline void RemoveShell(ShellBase* shell)
		{
			_shell_list.erase(shell->name());
		}
		
		//current shell
		static inline ShellBase* current_shell()
		{
			return _current_shell;
		}
		static inline void set_current_shell(ShellBase* shell)
		{
			_current_shell = shell;
		}
		static inline bool exist_current_shell()
		{
			return _current_shell != nullptr;
		}
		static inline void InputTip(str tip = "")
		{
			WARNING_CHECK(!exist_current_shell(), "current shell not exist");
			_current_shell->ShowPath();
			if (tip != "")
			{
				std::cout <<"/";
			}
			Cprintf(tip, 10);
			std::cout << ": >> ";
		}
		
		//shell root
		static void RootInit();
	};
	
}

/*
template<typename F>
class CommandList
{
private:
std::map<std::string, F> _func;
std::map<std::string, std::string> _des;
public:
inline void Add(std::string command, F func, std::string des)
{
_func[command] = func;
_des[command] = des;
}
inline void AddDescript(std::string command, std::string des)
{
_des[command] = des;
}
inline bool Exist(std::string command)
{
return _func.count(command) > 0;
}
inline F Func(std::string command)
{
WARNING_CHECK(!Exist(command), "no command");
return _func[command];
}
inline std::string Des(std::string command)
{
WARNING_CHECK(!Exist(command),"no command");
return _des[command];
}
inline void ShowCommand()
{
cout << endl << ">> ";
Cprintf("[ COMMAND LIST ]\n\n", 14);
for (auto command : _des)
{
cout << "   '";
Cprintf(command.first, 12);
cout << "'" << string("          ").substr(0, 10 - command.first.length()) << command.second << endl;
}
cout << endl << endl;
}
};
void ShellStart();
*/