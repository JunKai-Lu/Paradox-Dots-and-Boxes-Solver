#include "RetrospectMethod.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	//merger RawLayers into a Layer
	class RetrospectReducer
	{
	private:

		using FileList = std::vector<std::string>;
		using FeatureList = std::vector<size_t>;
		using CountList = std::vector<size_t>;

	private:

		LayerInfo& _layer;
		const size_t _thread_count;			//count of available threads.
		const size_t _partition_count;		//devide reduce process into multi partition.
		RetrospectReduceFunc _ReduceFunc;	//reduce function.
		RetrospectFilterFunc _FilterFunc;	//filter function.

	private:

		//get raw file list from current layer info.
		FileList GetFileList() const;

		//get group feature.
		size_t GetGroupFeature(const DabStateItem& item) const;

		//print setting
		void PrintSetting() const;

		//print result
		void PrintResult(double time, std::vector<FeatureList>& feature_list, std::vector<CountList>& count_list) const;

		//handle file and use it to update layer. all the items in the file would be save to the table if possible.
		void FileToLayer(std::string file, size_t feature, LayerTable& layer_table) const;

		//each single reduce process would scan all the files and get the highest margin of items whose feature was included in the feature list.
		void SingleReduceProcess(FeatureList* feature_list, CountList* count_list, size_t thread_index) const;

	public:

		inline const LayerInfo& layer() const
		{
			return _layer;
		}

	public:

		//constructor.
		RetrospectReducer(LayerInfo& layer, size_t thread_count, size_t partition_count, RetrospectReduceFunc ReduceFunc);

		//run reducer.
		void Run() const;
	};
}