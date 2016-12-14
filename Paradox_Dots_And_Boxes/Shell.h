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

	//class ShellBase is the abstract class of shell.
	class ShellBase
	{
	private:
		str _name;
		str _path;
		ShellBase* _parent_shell;
		bool _exit_flag;

	public:
		ShellBase(str name, ShellBase* parent_shell);
		ShellBase(ShellBase& sb) = delete;
		inline str name()
		{
			return _name;
		}
		inline str path()
		{
			return _path;
		}
		virtual void Run() = delete;
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
			_data(data)
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

			
			AddFunction("help", [](datatype& data)->void {ShowCommandList()}, "get command list");
			AddFunction("show", [](datatype& data)->void {cout << data}, "demonstrate data.");
			//AddDescript("help", "get command list");
			//AddDescript("data", "demonstrate data.")
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

		}
	};

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
		static inline void Tip(str tip = "")
		{
			WARNING_CHECK(!exist_current_shell(), "current shell not exist");
			std::cout << _current_shell->path();
			Cprintf(tip, 8);
			std::cout << std::endl << " ~# ";
		}
		
		void Init();
	};
	
}