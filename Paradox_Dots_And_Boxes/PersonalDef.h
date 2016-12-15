#include "stdafx.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <sstream>

#pragma once

//MACROS
#define WINDOWS
#define WARNING
#define WARNING_CHECK(condition,reason) WarningCheck(condition,reason,__FILE__,__LINE__,__FUNCTION__);

//ABBREVIATION
#define REP(n) for(size_t rep_i = 0; rep_i < n;rep_i++)
#define LOOP(i,n) for(size_t i = 0; i < n;i++)

namespace DAB
{
	typedef std::string str;

	//colorful print.
	void Cprintf(str tex, WORD color);
	void CprintNum(int num, WORD color);

	//output msg.
	inline str GetInput()
	{
		char buffer[50];
		std::cin.getline(buffer, 50);
		return std::string(buffer);
	}
	inline str B2S(bool b)
	{
		if (b)
		{
			return "true";
		}
		return "false";
	}
	inline str I2S(size_t i)
	{
		std::stringstream ss;
		ss << i;
		return ss.str();
	}

	inline void Error(std::string reason)
	{
		std::cout << std::endl;
		Cprintf(">> ERROR: ", 12);
		Cprintf(reason, 15);
		std::cout << std::endl << std::endl;
	}
	inline void Message(std::string message, bool show_MSG = true)
	{
		std::cout << ">> ";
		if (show_MSG)
		{
			Cprintf("MSG: ", 2);
		}
		Cprintf(message, 10);
		std::cout << std::endl << std::endl;
	}
	inline void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function)
	{
#ifdef WARNING
		if (condition)
		{
			Cprintf(">> WARNING: ", 5);
			Cprintf(reason, 12);
			std::cout << std::endl;
			Cprintf("[File]: " + file, 8);
			std::cout << std::endl;
			Cprintf("[Line]: " + I2S(line), 8);
			std::cout << std::endl;
			Cprintf("[Func]: " + function, 8);
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