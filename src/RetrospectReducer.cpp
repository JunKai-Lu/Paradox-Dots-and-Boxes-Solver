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

	size_t RetrospectReducer::GetGroupFeature(const DabStateItem & item) const
	{
		return size_t(item.first % BoardValueType(_partition_count));
	}

	//print setting
	void RetrospectReducer::PrintSetting() const
	{
		using namespace gadt::console;
		Table tb(2, 4);
		tb.set_width({5,5});
		tb.set_cell_in_column(0, {
			{ "Game", ConsoleColor::Gray, TableAlign::Middle },
			{ "Layer",ConsoleColor::Gray, TableAlign::Middle },
			{ "Thread", ConsoleColor::Gray, TableAlign::Middle },
			{ "Partition", ConsoleColor::Gray, TableAlign::Middle }
		});
		tb.set_cell_in_column(1, {
			{ gadt::ToString(_layer.width()) + "-" + gadt::ToString(_layer.height()), TableAlign::Middle },
			{ gadt::ToString(_layer.index()), TableAlign::Middle },
			{ _thread_count, TableAlign::Middle },
			{ _partition_count, TableAlign::Middle }
		});
		tb.Print();
	}

	//print result
	void RetrospectReducer::PrintResult(double time, std::vector<FeatureList>& feature_lists, std::vector<CountList>& count_lists) const
	{
		using namespace gadt::console;
		Table tb(3, _partition_count + 1);
		tb.set_width({ 4, 4, 8 });
		tb.enable_title({ "time = " + gadt::ToString(time), ConsoleColor::Gray, TableAlign::Middle });
		tb.set_cell_in_row(0, {
			{ "Thread", ConsoleColor::Gray, TableAlign::Middle },
			{ "Feature", ConsoleColor::Gray, TableAlign::Middle },
			{ "Item Count", ConsoleColor::Gray, TableAlign::Middle }
			});
		size_t row = 1;
		for (size_t ti = 0; ti < feature_lists.size(); ti++)
		{
			tb.set_cell({ "thread " + gadt::ToString(ti) }, 0, row);
			
			for (size_t fi = 0; fi < feature_lists[ti].size(); fi++)
			{
				tb.set_cell({(feature_lists[ti])[fi], TableAlign::Middle }, 1, row);
				
			}
			for (size_t ci = 0; ci < count_lists[ti].size(); ci++)
			{
				tb.set_cell({(count_lists[ti])[ci], TableAlign::Middle }, 2, row);
				row++;
			}
		}
		tb.Print();
	}

	//handle file and use it to update layer. all the items in the file would be save to the table if possible.
	void RetrospectReducer::FileToLayer(std::string file, size_t feature, LayerTable & layer_table) const
	{
		DabFileLoader loader(file);
		for (;;)
		{
			auto item = loader.LoadNextItem();
			if (loader.is_eof())
				break;
			if (GetGroupFeature(item) == feature)
				_ReduceFunc(item, layer_table);
		}
	}

	//each single reduce process would scan all the files and get the highest margin of items whose feature was included in the feature list.
	void RetrospectReducer::SingleReduceProcess(FeatureList* feature_list, CountList* count_list, size_t thread_index) const
	{
		for (size_t i = 0; i < feature_list->size();i++)
		{
			auto feature = (*feature_list)[i];
			gadt::timer::TimePoint tp;
			std::string file_path = _layer.GetPartitionFilePath(feature, _partition_count);
			LayerTable layer_table;
			auto file_list = GetFileList();
			for (auto file : file_list)
				FileToLayer(file, feature, layer_table);
			DabFileWriter writer(file_path);
			(*count_list)[i] = layer_table.OutputToFile(writer);
			std::cout << ">>[ thread " << thread_index << "] had completed the work of feature " << feature << " of " << _partition_count << ", time = "<< tp.time_since_created() << std::endl;
		}
	}

	//constructor.
	RetrospectReducer::RetrospectReducer(LayerInfo & layer, size_t thread_count, size_t partition_count, RetrospectReduceFunc ReduceFunc) :
		_layer(layer),
		_thread_count(thread_count),
		_partition_count(partition_count),
		_ReduceFunc(ReduceFunc),
		_FilterFunc([](BoardValueType)->bool {return true; })
	{
	}

	//run reducer.
	void RetrospectReducer::Run() const
	{
		PrintSetting();
		if (gadt::console::GetUserConfirm("Do you confirm that to run reducer under this setting?"))
		{
			if (_layer.exist_partition_file())
			{
				if (gadt::console::GetUserConfirm("all the existing partition files would be clear, continue?"))
				{
					_layer.ClearPartFiles();
					gadt::console::PrintMessage("partition files had been cleared.");
				}
				else
				{
					gadt::console::PrintMessage("Operation canceled!");
					return;
				}
			}

			gadt::timer::TimePoint tp;
			//assign feature for each thread.
			std::vector<FeatureList> feature_lists(_thread_count);
			for (size_t i = 0; i < _thread_count; i++)
				feature_lists[i%_thread_count].push_back(i);

			//item counts for features in each thread.
			std::vector<CountList> item_counts;
			for (size_t i = 0; i < feature_lists.size();i++)
				item_counts.push_back(std::vector<size_t>(feature_lists[i].size(), 0));

			//run multi threads.
			std::vector<std::thread> threads;
			for (size_t i = 0; i < _thread_count; i++)
				threads.push_back(std::thread([&](FeatureList* features, std::vector<size_t>* count, size_t index)->void {
				SingleReduceProcess(features, count, index);
			}, &feature_lists[i], &item_counts[i], i));
			for (size_t i = 0; i < threads.size(); i++)
				threads[i].join();

			//save file to json.
			for (size_t i = 0; i < _partition_count; i++)
				_layer.add_part_file(i);
			_layer.SaveToFile();

			//print result.
			PrintResult(tp.time_since_created(), feature_lists, item_counts);
		}
		else
		{
			gadt::console::PrintMessage("Operation canceled!");
		}
	}
}