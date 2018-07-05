#include "BasicDefine.h"
#include <unordered_map>

#pragma once

namespace dots_and_boxes_solver
{
	using DabStateItem = std::pair<BoardValueType, MarginType>;
	using DabStateTable = std::unordered_map<BoardValueType, MarginType>;

	/*loader for layer files.
	class DabFileLoader
	{
	private:
		std::ifstream _ifs;
		bool _eof;

	public:
		//constuctor.
		DabFileLoader(std::string file_name):
		_ifs()//gadt::filesystem::load_file_as_string(file_name))
		{
			_ifs.open(file_name, std::ios::in);
		}

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
	};*/

	

	/*writer for layer files.
	class DabFileWriter
	{
	private:
		std::ifstream _ifs;
		std::ofstream _ofs;

	public:
		//constuctor.
		DabFileWriter(std::string file_path) :
			_ofs()
		{
			_ofs.open(file_path, std::ios::out);
		}

		//save item into the end of the file.
		inline void save_item(const DabStateItem& item)
		{
			//int8_t must be convert to int due to the output mode of ios in char type.
			_ofs << item.first << " " << (int)item.second << std::endl;
		}

		//save item into the end of the file.
		inline void save_item(BoardValueType board, MarginType margin)
		{
			//int8_t must be convert to int due to the output mode of ios in char type.
			_ofs << board << " " << (int)margin << std::endl;
		}

		//write string to file.
		inline void write(std::string str)
		{
			_ofs << str;
		}
	};*/

	class DabFileLoader
	{
	private:

		FILE * _fp;
		bool _eof;

	public:

		DabFileLoader(std::string file_path) :
			_fp(),
			_eof(false)
		{
#ifdef __GADT_MSVC
			if (fopen_s(&_fp, file_path.c_str(), "rb") != 0 )
#else
			if ((_fp = fopen(file_path.c_str(), "rb")) == 0)
#endif
			{
				_eof = true;
			}
		}

		DabStateItem LoadNextItem()
		{
			DabStateItem item;
#ifdef __GADT_MSVC
			auto res = fread_s(&item, sizeof(DabStateItem), sizeof(DabStateItem), 1, _fp);
#else
			auto res = fread(&item, sizeof(DabStateItem), 1, _fp);
			
#endif
			if (res != 1)
				_eof = true;
			return item;
		}

		bool is_eof() const
		{
			return _eof;
		}

		~DabFileLoader()
		{
			fclose(_fp);
		}
	};

	class DabFileWriter
	{
	private:

		FILE * _fp;
		bool _exist;

	public:

		DabFileWriter(std::string file_path):
			_fp(),
			_exist(true)
		{
#ifdef __GADT_MSVC
			if (fopen_s(&_fp, file_path.c_str(), "wb") != 0)
#else
			if ((_fp = fopen(file_path.c_str(), "wb")) == 0)
#endif
			{
				_exist = false;
			}
		}

		//save item into the end of the file.
		inline void save_item(const DabStateItem& item)
		{
			if (_exist)
			{
				bool res = fwrite(&item, sizeof(DabStateItem), 1, _fp);
				if (res != 1)
					_exist = false;
			}
		}

		//save item into the end of the file.
		inline void save_item(BoardValueType board, MarginType margin)
		{
			save_item({ board, margin });
		}

		//write string to file.
		//inline void write(std::string str)
		//{
		//	_ofs << str;
		//}

		~DabFileWriter()
		{
			fclose(_fp);
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
		bool UpdateIfLarger(DabStateItem item)
		{
			if (_table.count(item.first) != 0)//exist
			{
				if (_table[item.first] <= item.second)//if larger.
				{
					_table[item.first] = item.second;//update
					return true;
				}
			}
			else//not exist
			{
				_table[item.first] = item.second;//save it.
				return true;
			}
			return false;
		}

		//output this table to file.
		size_t OutputToFile(DabFileWriter& writer)
		{
			for (auto item : _table)
				writer.save_item(item.first, item.second);
			return _table.size();
		}
	};

}