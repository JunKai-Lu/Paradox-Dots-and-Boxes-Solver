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
	}

	class ShellBase
	{
	private:
		str _name;
		str _path;
		ShellBase* _parent_shell;

	public:
		ShellBase(str name, ShellBase* parent_shell);
		inline void InputTip(str tip = "")
		{
			std::cout << _path;
			Cprintf(tip, 8);
			std::cout << std::endl << " ~# ";
		}
		inline str name()
		{
			return _name;
		}
		str GetPath()
		{
			if (_parent_shell == nullptr)
			{
				return "DAB@root/";
			}
			return _parent_shell->GetPath() + _name + "/";
		}
		
	};

	template<typename data_A, typename data_B>
	class Shell : public ShellBase
	{
	private:
		std::map<str, std::function<void(data_A,data_B)> > _func;
		std::map<str, std::string> _des;

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
		inline F GetFunc(std::string command)
		{
			WARNING_CHECK(!Exist(command), "no command");
			return _func[command];
		}
		inline std::string GetDes(std::string command)
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

	
	namespace SHELL
	{
		extern std::map<str, ShellBase*> shell_list;
		extern ShellBase* current_shell;

		inline ShellBase* GetShell(str name)
		{
			WARNING_CHECK(shell_list.count(name) > 0, "empty");
			return shell_list[name];
		}
		inline void RegisterShell(ShellBase* shell)
		{
			shell_list[shell->name()] = shell;
		}
	}

}