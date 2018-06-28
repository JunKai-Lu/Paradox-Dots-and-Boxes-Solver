#include "RetrospectReducer.h"

namespace dots_and_boxes_solver
{
	std::vector<RetrospectReducer::FeatureList> RetrospectReducer::AssignFeatureList(size_t feature_count, size_t list_count) const
	{
		gadt::bitboard::ValueVector<RETROSPCECT_MAX_REDUCE_SEGMENT_COUNT> features;
		for (size_t i = 0; i < feature_count; i++)
			features.push((uint8_t)i);

		std::vector<FeatureList> feature_lists(list_count, FeatureList());
		for (size_t i = 0; i < feature_count; i++)
			feature_lists[i % list_count].push_back(features.draw_and_remove_value());
		
		return feature_lists;
	}

	//get raw file list from current layer info.
	RetrospectReducer::FileList RetrospectReducer::GetFileList() const
	{
		std::vector<std::string> file_path_vec;
		for (auto index : _layer.raw_files())
		{
			std::string path = _layer.GetRawFilePath(index, _layer.raw_files().size());
			if (gadt::filesystem::exist_file(path))
				file_path_vec.push_back(path);
		}
		return file_path_vec;
	}

	//load items into item_vec;
	void RetrospectReducer::LoadItems(RetrospectReducer::ItemVector& item_vec) const
	{
		auto file_list = GetFileList();
		for (auto file : file_list)
		{
			DabFileLoader loader(file);
			for (;;)
			{
				auto item = loader.LoadNextItem();
				if (loader.is_eof())
					break;
				item_vec.push_back(item);
			}
		}
	}

	//get group feature of a item.
	size_t RetrospectReducer::GetGroupFeature(const DabStateItem & item) const
	{
		return size_t(item.first % BoardValueType(_partition_count));
	}

	//print setting
	void RetrospectReducer::PrintSetting(const std::vector<FeatureList>& feature_lists) const
	{
		using namespace gadt::console;
		Table tb(2, _thread_count + 1);
		tb.set_width({ 4, 14 });
		tb.enable_title({ _layer.get_layer_str() + " Reducer", ConsoleColor::Yellow, TableAlign::Middle });
		tb.set_cell_in_row(0, {
			{ "Thread", ConsoleColor::Gray, TableAlign::Middle },
			{ "Partitions", ConsoleColor::Gray, TableAlign::Middle }
			});
		for (size_t ti = 0; ti < feature_lists.size(); ti++)
		{
			tb.set_cell({ "thread " + gadt::ToString(ti) }, 0, ti + 1);
			std::stringstream ss;
			ss << "[ ";
			for (size_t n = 0; n < feature_lists[ti].size(); n++)
				ss << (feature_lists[ti])[n] << (n + 1 == feature_lists[ti].size() ? "" : ", ");
			ss << " ]";
			tb.set_cell({ " " + ss.str() }, 1, ti + 1);
		}
		tb.Print();
	}

	//print result
	void RetrospectReducer::PrintResult(double time, const std::vector<FeatureList>& feature_lists, const std::vector<CountList>& count_lists) const
	{
		using namespace gadt::console;
		size_t item_count = 0;

		Table tb(3, _partition_count + 2);
		tb.set_width({ 4, 10, 6 });
		tb.enable_title({ _layer.get_layer_str() + " Reducer, [ t = " + gadt::ToString(time) + "]", ConsoleColor::Yellow, TableAlign::Middle });
		tb.set_cell_in_row(0, {
			{ "Thread", ConsoleColor::Gray, TableAlign::Middle },
			{ "Partitions", ConsoleColor::Gray, TableAlign::Middle },
			{ "Item Count", ConsoleColor::Gray, TableAlign::Middle }
			});
		size_t row = 1;
		for (size_t ti = 0; ti < feature_lists.size(); ti++)
		{
			tb.set_cell({ "thread " + gadt::ToString(ti) }, 0, row);
			if (feature_lists[ti].size() != count_lists[ti].size())
				std::cout << "WARNING!!!!!";
			for (size_t fi = 0; fi < feature_lists[ti].size(); fi++)
			{
				tb.set_cell({"[" + gadt::ToString((feature_lists[ti])[fi]) + "]", TableAlign::Middle }, 1, row + fi);
			}
			for (size_t ci = 0; ci < count_lists[ti].size(); ci++)
			{
				tb.set_cell({(count_lists[ti])[ci], TableAlign::Middle }, 2, row);
				item_count += (count_lists[ti])[ci];
				row++;
			}
		}
		tb.set_cell_in_row(_partition_count + 1, {
			{ "Total", ConsoleColor::Gray, TableAlign::Middle },
			{ _partition_count, ConsoleColor::Gray, TableAlign::Middle },
			{ item_count, ConsoleColor::Gray, TableAlign::Middle }
			});
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
	void RetrospectReducer::SingleReduceProcess(const FeatureList* feature_list,const ItemVector* item_vec, CountList* count_list, size_t thread_index) const
	{
		for (size_t i = 0; i < feature_list->size();i++)
		{
			auto feature = (*feature_list)[i];
			gadt::timer::TimePoint tp;
			std::string file_path = _layer.GetPartitionFilePath(feature, _partition_count);
			LayerTable layer_table;
			for (auto item : *item_vec)
			{
				if (GetGroupFeature(item) == feature)
					_ReduceFunc(item, layer_table);
			}
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
		//assign feature for each thread.
		std::vector<FeatureList> feature_lists = AssignFeatureList(_partition_count, _thread_count);

		//print setting.
		PrintSetting(feature_lists);
		if (true)//gadt::console::GetUserConfirm("Do you confirm that running reducer under this setting?"))
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
			gadt::console::PrintMessage("loading raw files...");
			
			//item from raw files.
			ItemVector item_vec;
			LoadItems(item_vec);
			gadt::console::PrintMessage("loading raw files completed, item count = " + gadt::ToString(item_vec.size()));
			
			//item counts for features in each thread.
			std::vector<CountList> item_counts;
			for (size_t i = 0; i < _thread_count ;i++)
				item_counts.push_back(std::vector<size_t>(feature_lists[i].size(), 0));

			//run multi threads.
			std::vector<std::thread> threads;
			for (size_t i = 0; i < _thread_count; i++)
				threads.push_back(std::thread([&](const FeatureList* features, const ItemVector* items, CountList* count, size_t index)->void {
				SingleReduceProcess(features, items ,count, index);
			}, &feature_lists[i], &item_vec, &item_counts[i], i));
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