#include "RetrospectReducer.h"

namespace dots_and_boxes_solver
{
	//get raw file list from current layer info.
	RetrospectReducer::FileList RetrospectReducer::GetFileList() const
	{
		std::vector<std::string> file_path_vec;
		for (auto index : _layer.raw_files())
		{
			std::string path = _layer.GetRawFilePath(index);
			if (gadt::filesystem::exist_file(path))
				file_path_vec.push_back(path);
		}
		return file_path_vec;
	}

	//handle file and use it to update layer. all the items in the file would be save to the table if possible.
	void RetrospectReducer::FileToLayer(std::string file, size_t feature, LayerTable & layer_table)
	{
		DabFileLoader loader(file);
		if (loader.is_eof() == false)
		{
			auto item = loader.LoadNextItem();
			if (GetGroupFeature(item) == feature)
				_ReduceFunc(item, layer_table);
		}
	}

	//each single reduce process would scan all the files and get the highest margin of items whose feature was included in the feature list.
	void RetrospectReducer::SingleReduceProcess(std::vector<size_t>* feature_list, size_t thread_index)
	{
		for (auto feature : *feature_list)
		{
			gadt::timer::TimePoint tp;
			std::string file_path = _layer.GetPartitionFilePath(feature, _segment_count);
			LayerTable layer_table;
			auto file_list = GetFileList();
			for (auto file : file_list)
				FileToLayer(file, feature, layer_table);
			DabFileWriter writer(file_path);
			layer_table.OutputToFile(writer);
			std::cout << ">>[ thread " << thread_index << "] had completed the work of feature " << feature << " of " << _segment_count << ", time = "<< tp.time_since_created() << std::endl;
		}
	}

	//constructor.
	RetrospectReducer::RetrospectReducer(LayerInfo & layer, size_t thread_count, size_t segment_count, RetrospectReduceFunc ReduceFunc) :
		_layer(layer),
		_thread_count(thread_count),
		_segment_count(segment_count),
		_ReduceFunc(ReduceFunc),
		_FilterFunc([](BoardValueType)->bool {return true; })
	{
	}

	//run reducer.
	void RetrospectReducer::Run() const
	{
		
	}
}