#include "stdafx.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <sstream>
#include "GeneralGameShell.h"

#pragma once

//MACROS
#define WINDOWS
#define WARNING
#define WARNING_CHECK(condition,reason) console::WarningCheck(condition,reason,__FILE__,__LINE__,__FUNCTION__);

namespace console
{
	//output msg.
	inline std::string B2S(bool b)
	{
		if (b)
		{
			return "true";
		}
		return "false";
	}
	inline std::string I2S(size_t i)
	{
		std::stringstream ss;
		ss << i;
		return ss.str();
	}

	inline void Error(std::string reason)
	{
		std::cout << std::endl;
		Cprintf(">> ERROR: ", color::red);
		Cprintf(reason, color::white);
		std::cout << std::endl << std::endl;
	}
	inline void Message(std::string message, bool show_MSG = true)
	{
		std::cout << ">> ";
		if (show_MSG)
		{
			Cprintf("MSG: ", color::deep_green);
		}
		Cprintf(message, color::green);
		std::cout << std::endl << std::endl;
	}
	inline void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function)
	{
#ifdef WARNING
		if (condition)
		{
			Cprintf(">> WARNING: ", color::purple);
			Cprintf(reason, color::red);
			std::cout << std::endl;
			Cprintf("[File]: " + file, color::gray);
			std::cout << std::endl;
			Cprintf("[Line]: " + I2S(line), color::gray);
			std::cout << std::endl;
			Cprintf("[Func]: " + function, color::gray);
			std::cout << std::endl;
			system("pause");
		}
#endif
	}

	//Singleton
	template<class value_type>
	class Singleton final : public value_type
	{
	private:
		Singleton() = default;
		Singleton(const Singleton&) = delete;
		~Singleton() = default;
	public:

		static Singleton* instance()
		{
			static Singleton only_one;
			return &only_one;
		}
	};
}


	