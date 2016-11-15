#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include "GameDefine.h"
#include <map>

#pragma  once

namespace DAB
{
	namespace SHELL
	{
		struct OrderInfo
		{
			void (*_func)();
			std::string _descript;
		};
		extern std::map < std::string, OrderInfo > order_list;
		inline std::string B2S(bool b)
		{
			if (b)
			{
				return "true";
			}
			return "false";
		}
		inline void AddOrder(std::string order,void (*func)(),std::string des)
		{
			order_list[order] = { func, des };
		}

		void Start();
		void Info();
	}
}