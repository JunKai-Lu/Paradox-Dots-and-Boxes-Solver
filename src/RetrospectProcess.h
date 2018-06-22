#include "DbInfo.h"
#include "LayerConatiner.hpp"
#include "StateRepresentation.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	

	//Map function, which need to write all resulted items into a file and return the count. 
	using RetrospectMapFunc = std::function<size_t(const DabStateItem&, DabFileWriter&)>;
	
	//Reduce function, which need to cover/write a item into a table and return true if it cover/write the item.
	using RetrospectReduceFunc = std::function<bool(const DabStateItem&, LayerTable&)>;

	//Filter function, which need to decide if a item is able to update the table.
	using RetrospectFilterFunc = std::function<bool(BoardValueType)>;

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	struct RetrospectFuncPackage
	{
		static size_t MapFunc(const DabStateItem & item, DabFileWriter & writer)
		{
			return 0;
		}

		static bool ReduceFunc(const DabStateItem & item, LayerTable & layer_table)
		{
			return false;
		}

		static bool FilterFunc(BoardValueType)
		{
			return false;
		}
	};

	//from partition files to raw files. 
	class RetrospectMapper
	{
	private:

		using FileList = std::vector<std::string>;

	private:
		
		const LayerInfo& _layer;	//layer info
		const size_t _thread_count;	//using multi threads for map process. 
		RetrospectMapFunc _MapFunc;

	private:

		//get part file list from previous layer info.
		FileList GetFileList() const;

		//each single map process would map all accepted files to target raw files. 
		void SingleMapProcess(FileList files, size_t* item_countm, size_t thread_index) const;

		//print setting
		void PrintSetting() const;

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
		RetrospectMapper(const LayerInfo& layer, size_t thread_count, RetrospectMapFunc MapFunc);

		//run mapper.
		void Run() const;
	};

	//merger RawLayers into a Layer
	class RetrospectReducer
	{
	private:

		using FileList = std::vector<std::string>;

	private:

		const LayerInfo& _layer;	
		const size_t _segment_count;		//devide reduce process into multi segment.
		RetrospectReduceFunc _ReduceFunc;	//reduce function.
		RetrospectFilterFunc _FilterFunc;	//filter function.

	private:

		//get raw file list from current layer info.
		FileList GetFileList() const;

	public:

		inline const LayerInfo& layer() const
		{
			return _layer;
		}

	public:

		//constructor.
		RetrospectReducer(const LayerInfo& layer, size_t segment_count, RetrospectReduceFunc ReduceFunc);

		//run reducer.
		void Run() const;
	};

	
}
