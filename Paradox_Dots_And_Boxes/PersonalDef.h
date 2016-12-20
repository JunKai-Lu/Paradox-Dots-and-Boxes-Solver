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