#include "BasicDefine.h"

#pragma once

namespace dots_and_boxes_solver
{
	/*
	* example of Json file structure:
	{
		width:5,
		height:5,
		layer:60,
		raw_files:[1,2,3,5,6...],
		partition_files:[4,5,6,7...]
	}
	*/

	constexpr char* DAB_DB_ROOT_PATH = "./db/";
	constexpr char* DAB_DB_RAW_PATH = "raw/";
	constexpr char* DAB_DB_PARTITION_PATH = "partition/";
	constexpr char* DAB_DB_LAYER_INFO_NAME = "layer.json";
	constexpr char* DAB_DB_RAW_PREFIX = "Raw_";
	constexpr char* DAB_DB_RAW_SUFFIX = ".DabRawFile";
	constexpr char* DAB_DB_PARTITION_PREFIX = "Part_";
	constexpr char* DAB_DB_PARTITION_SUFFIX = ".DabPartitionFile";
	constexpr char* DAB_DB_JSON_WIDTH_KEY = "width";
	constexpr char* DAB_DB_JSON_HEIGHT_KEY = "height";
	constexpr char* DAB_DB_JSON_LAYER_KEY = "index";
	constexpr char* DAB_DB_JSON_RAW_FILES_KEY = "raw_files";
	constexpr char* DAB_DB_JSON_PART_FILES_KEY = "partition_files";

	class LayerInfo
	{
	private:

		bool _is_init;
		const size_t _width;
		const size_t _height;
		const size_t _index;

		std::vector<size_t> _raw_files;
		std::vector<size_t> _part_files;

	private:

		//load json file.
		void LoadLayerInfoFile();

	public:
		
		inline size_t width() const
		{
			return _width;
		}

		inline size_t height() const
		{
			return _height;
		}

		inline size_t index() const
		{
			return _index;
		}

		inline const std::vector<size_t>& raw_files() const
		{
			return _raw_files;
		}

		inline const std::vector<size_t>& part_files() const
		{
			return _part_files;
		}

		//return true if raw process of this layer had completed.
		inline bool exist_raw_file() const
		{
			return _raw_files.size() != 0;
		}

		//return true if partition process of this layer had completed.
		inline bool exist_partition_file() const
		{
			return _part_files.size() != 0;
		}

		//add new index to raw files.
		inline void add_raw_file(size_t index)
		{
			_raw_files.push_back(index);
		}

		//add new index to part files.
		inline void add_part_file(size_t index)
		{
			_part_files.push_back(index);
		}

		//get raw file name by index.
		inline std::string index_to_raw_file_name(size_t index, size_t raw_file_count) const
		{
			return DAB_DB_RAW_PREFIX + gadt::ToString(index + 1) + "_" + gadt::ToString(raw_file_count) + DAB_DB_RAW_SUFFIX;
		}

		//get part file name by index.
		inline std::string index_to_part_file_name(size_t index, size_t partition_count) const
		{
			return DAB_DB_PARTITION_PREFIX + gadt::ToString(index + 1) + "_" + gadt::ToString(partition_count) + DAB_DB_PARTITION_SUFFIX;
		}

		//get string of layer info.
		inline std::string get_layer_str() const
		{
			std::stringstream ss;
			ss << "[" << _width << " x " << _height << "]: L" << _index;
			return ss.str();
		}

	public:

		//load layer info from file path.
		LayerInfo(size_t width, size_t height, size_t index);

		//save to file.
		void SaveToFile() const;

		//print info
		void PrintInfo() const;

		//claer all the raw files.
		bool ClearRawFiles();

		//claer all the raw files.
		bool ClearPartFiles();

		//get folder of root directory
		std::string GetRootDirectory() const;

		//get folder of layer directory
		std::string GetLayerDirectory() const;

		//get folder of raw files and ensure its existence.
		std::string GetRawDirectory() const;

		//get folder of partition files and ensure its existence.
		std::string GetPartitionDirectory() const;

		//get the path of file by layer index.
		std::string GetLayerInfoFilePath() const;

		//get the path of raw file.
		std::string GetRawFilePath(size_t index, size_t raw_file_count) const;

		//get the path of partition file.
		std::string GetPartitionFilePath(size_t index, size_t partition_count) const;

		//deconstructor, this class would be save to file before it was destroy.
		~LayerInfo();
	};
}
