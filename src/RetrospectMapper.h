#include "RetrospectMethod.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	//from partition files to raw files. 
	class RetrospectMapper
	{
	private:

		using FileList = std::vector<std::string>;

	private:

		LayerInfo& _layer;	//layer info
		const size_t _thread_count;	//using multi threads for map process. 
		RetrospectMapFunc _MapFunc;

	private:

		//get part file list from previous layer info.
		FileList GetFileList() const;

		//each single map process would map all accepted files to target raw files. 
		void SingleMapProcess(FileList files, size_t* item_countm, size_t thread_index) const;

		//print setting
		void PrintSetting() const;

		//print result
		void PrintResult(double time, std::vector<FileList>& file_lists, std::vector<size_t>& item_counts) const;

	public:

		//get raw file path by index.
		inline std::string get_raw_file_path(size_t index) const
		{
			return _layer.GetRawFilePath(index);
		}

		//get ref of layer.
		inline const LayerInfo& layer() const
		{
			return _layer;
		}

	public:

		//constructor.
		RetrospectMapper(LayerInfo& layer, size_t thread_count, RetrospectMapFunc MapFunc);

		//run mapper.
		void Run() const;
	};
}