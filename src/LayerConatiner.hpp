#include "GameDefine.h"
#include <unordered_map>
#include "../lib/gadt/lib/json11/json11.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	using DabStateItem = std::pair<BoardValueType, MarginType>;
	using DabStateTable = std::unordered_map<BoardValueType, MarginType>;

	//managing the input and output of a text file.
	class DabStoragedFile
	{
	private:
		std::ifstream _ifs;
		std::ofstream _ofs;

	public:
		//constuctor.
		DabStoragedFile(std::string file_name) :
			_ifs(),
			_ofs()
		{
			_ifs.open(file_name, std::ios::app);
			_ofs.open(file_name, std::ios::app);
		}

		//save item into the end of the file.
		inline void SaveItem(DabStateItem item)
		{
			_ofs << item.first << " " << (int)item.second << std::endl;
		}

		//save item into the end of the file.
		inline void SaveItem(BoardValueType board, MarginType margin)
		{
			_ofs << board << " " << (int)margin << std::endl;
		}

		//return next item of the file.
		inline DabStateItem LoadNextItem()
		{
			BoardValueType bv;
			int mv;
			_ifs >> bv >> mv;
			return { bv,mv };
		}

		//return true if it is eof.
		inline bool is_eof() const
		{
			return _ifs.eof() || !_ifs.is_open();
		}

	};

	using RawLayer = DabStoragedFile;
	using LayerPart = DabStoragedFile;

	class LayerTable
	{
	private:
		DabStateTable _table;

	public:

		//constuctor
		LayerTable():
			_table()
		{
		}

		//update if the margin in item is larger than the existing margin in table.
		void UpdateIfLarger(DabStateItem item)
		{
			if (_table.count(item.first) != 0)//exist
				if (_table[item.first] <= item.second)//if larger.
					_table[item.first] = item.second;//update
			else//not exist
				_table[item.first] = item.second;//save it.
		}

		//output this table to file.
		void OutputToFile(LayerPart& layer)
		{
			for (auto item : _table)
				layer.SaveItem(item);
		}
	};

}
