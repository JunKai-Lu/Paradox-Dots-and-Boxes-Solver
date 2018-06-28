#include "RetrospectMethod.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	struct MapTask
	{
		size_t index;
		std::string source_file;
		std::string target_file;
	};


	//from partition files to raw files. 
	class RetrospectMapper
	{
	private:

		using TaskList = std::vector<MapTask>;
		using ItemVector = std::vector<DabStateItem>;
		using IndexList = std::vector<size_t>;

	private:

		LayerInfo& _layer;	//layer info
		const size_t _thread_count;	//using multi threads for map process. 
		RetrospectMapFunc _MapFunc;

	private:

		//get file index lists.
		std::vector<IndexList> AssignFileIndexLists(size_t list_count) const;

		//get file lists
		std::vector<TaskList> AssignTaskLists(const std::vector<IndexList>& index_lists) const;

		//each single map process would map all accepted files to target raw files. 
		void SingleMapProcess(TaskList tasks, size_t* item_countm, size_t thread_index) const;

		//print setting
		void PrintSetting(const std::vector<IndexList> indexs) const;

		//print result
		void PrintResult(double time, std::vector<IndexList>& index_lists, std::vector<size_t>& item_counts) const;

	public:

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