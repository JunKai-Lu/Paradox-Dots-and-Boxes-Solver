#include "GameDefine.h"
#include <unordered_map>


#pragma once

namespace dots_and_boxes_solver
{
	using DabStateItem = std::pair<BoardValueType, MarginType>;
	using DabStateTable = std::unordered_map<BoardValueType, MarginType>;

	//loader for layer files.
	class DabFileLoader
	{
	private:
		std::ifstream _ifs;

	public:
		//constuctor.
		DabFileLoader(std::string file_name);

		//return next item of the file.
		inline DabStateItem LoadNextItem()
		{
			BoardValueType bv;
			int mv;
			_ifs >> bv >> mv;
			return { bv, (MarginType)mv };
		}

		//return true if it is eof.
		inline bool is_eof() const
		{
			return _ifs.eof() || !_ifs.is_open();
		}
	};

	//writer for layer files.
	class DabFileWriter
	{
	private:
		std::ifstream _ifs;
		std::ofstream _ofs;

	public:
		//constuctor.
		DabFileWriter(std::string file_path);

		//save item into the end of the file.
		inline void SaveItem(const DabStateItem& item)
		{
			//int8_t must be convert to int due to the output mode of ios in char type.
			_ofs << item.first << " " << (int)item.second << std::endl;
		}

		//save item into the end of the file.
		inline void SaveItem(BoardValueType board, MarginType margin)
		{
			//int8_t must be convert to int due to the output mode of ios in char type.
			_ofs << board << " " << (int)margin << std::endl;
		}
	};

	//hash table of layer.
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
		void OutputToFile(DabFileWriter& writer)
		{
			for (auto item : _table)
				writer.SaveItem(item);
		}
	};

}