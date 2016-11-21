#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

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
#ifdef WARNING
				if (!Exist(command))
				{
					Warning("no command", "CommandList::Func");
				}
#endif
				return _func[command];
			}
			inline std::string Des(std::string command)
			{
#ifdef WARNING
				if (!Exist(command))
				{
					Warning("no command", "CommandList::Func");
				}
#endif
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
	
}