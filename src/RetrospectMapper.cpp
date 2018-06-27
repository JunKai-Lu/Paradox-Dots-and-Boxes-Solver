#include "RetrospectMapper.h"

namespace dots_and_boxes_solver
{
	//get file index lists.
	std::vector<RetrospectMapper::IndexList> RetrospectMapper::AssignFileIndexLists(size_t list_count) const
	{
		gadt::bitboard::ValueVector<RETROSPCECT_MAX_PARTITION_COUNT> indexs;
		std::vector<IndexList> index_lists(list_count, IndexList());
		if (_layer.index() < EdgeCount(_layer.width(), _layer.height()))
		{
			LayerInfo prev_layer(_layer.width(), _layer.height(), _layer.index() + 1);
			for (auto i : prev_layer.part_files())
				indexs.push((uint8_t)i);
			for (size_t i = 0; i < prev_layer.part_files().size(); i++)
				index_lists[i%list_count].push_back(indexs.draw_and_remove_value());
		}
		return index_lists;
	}

	//get file lists
	std::vector<RetrospectMapper::FileList> RetrospectMapper::AssignFileLists(const std::vector<RetrospectMapper::IndexList>& index_lists) const
	{
		std::vector<FileList> file_lists;
		if (_layer.index() < EdgeCount(_layer.width(), _layer.height()))
		{
			LayerInfo prev_layer(_layer.width(), _layer.height(), _layer.index() + 1);
			for (auto index_list : index_lists)
			{
				file_lists.push_back(FileList());
				for (auto index : index_list)
				{
					std::string path = prev_layer.GetPartitionFilePath(index, prev_layer.part_files().size());
					if (gadt::filesystem::exist_file(path))
						file_lists.back().push_back(path);
				}
			}
		}
		return file_lists;
	}

	//get part file list from previous layer info.
	RetrospectMapper::FileList RetrospectMapper::GetFileList() const
	{
		std::vector<std::string> file_path_vec;
		//get previous layer
		if (_layer.index() < EdgeCount(_layer.width(), _layer.height()))
		{
			LayerInfo prev_layer(_layer.width(), _layer.height(), _layer.index() + 1);
			for (auto index : prev_layer.part_files())
			{
				std::string path = prev_layer.GetPartitionFilePath(index, prev_layer.part_files().size());
				if (gadt::filesystem::exist_file(path))
					file_path_vec.push_back(path);
			}
		}
		return file_path_vec;
	}

	//assign raw file name by index.
	void RetrospectMapper::SingleMapProcess(FileList files, size_t* item_count, size_t thread_index) const
	{
		//get target file path
		std::string raw_file_path = get_raw_file_path(thread_index);
		for (auto file : files)
		{
			std::stringstream cache;
			DabFileLoader loader(file);
			for(;;)
			{
				DabStateItem item = loader.LoadNextItem();
				if (loader.is_eof())
					break;
				size_t count = _MapFunc(item, cache);
				*item_count += count;
			}
			//DabFileWriter writer(raw_file_path);
			//writer.write(cache.str());
			std::ofstream ofs(raw_file_path, std::ios::app);
			std::cout << raw_file_path << " had been outputed by thread " << thread_index << std::endl;
			ofs << cache.str();
		}
	}

	//run mapper.
	void RetrospectMapper::Run() const
	{
		//assign files for each thread.
		auto index_lists = AssignFileIndexLists(_thread_count);
		auto file_lists = AssignFileLists(index_lists);
		std::vector<size_t> item_counts(_thread_count, 0);
		PrintSetting(index_lists);
		if (true)//gadt::console::GetUserConfirm("Do you confirm that running mapper under this setting?"))
		{
			//clear db if need.
			if (_layer.exist_raw_file())
			{
				if (gadt::console::GetUserConfirm("all the existing raw files would be clear, continue?"))
				{
					_layer.ClearRawFiles();
					gadt::console::PrintMessage("raw files had been cleared.");
				}
				else
				{
					gadt::console::PrintMessage("Operation canceled!");
					return;
				}
			}
			//divede files into multi part.
			gadt::timer::TimePoint tp;
			//run multi thread.
			std::vector<std::thread> threads;
			for (size_t i = 0; i < _thread_count; i++)
				threads.push_back(std::thread([&](FileList files, size_t* count, size_t index)->void {
				SingleMapProcess(files, count, index);
			}, file_lists[i], &item_counts[i], i));
			for (size_t i = 0; i < threads.size(); i++)
				threads[i].join();

			//save layer json.
			for (size_t i = 0; i < _thread_count; i++)
				_layer.add_raw_file(i);
			_layer.SaveToFile();

			//print result.
			PrintResult(tp.time_since_created(), index_lists, item_counts);
		}
		else
		{
			gadt::console::PrintMessage("Operation canceled!");
		}
	}

	//constructor.
	RetrospectMapper::RetrospectMapper(LayerInfo & layer, size_t thread_count, RetrospectMapFunc MapFunc) :
		_layer(layer),
		_thread_count(thread_count),
		_MapFunc(MapFunc)
	{
	}

	//print setting
	void RetrospectMapper::PrintSetting(const std::vector<IndexList> indexs) const
	{
		using namespace gadt::console;
		Table tb(2, _thread_count + 1);
		tb.set_width({ 5,15 });
		tb.enable_title({ _layer.get_layer_str() + " Mapper Info", ConsoleColor::Yellow, TableAlign::Middle });
		tb.set_cell_in_row(0, {
			{ "Thread", ConsoleColor::Gray, TableAlign::Middle },
			{ "Source Files",ConsoleColor::Gray, TableAlign::Middle }
			});
		for (size_t i = 0; i < _thread_count; i++)
		{
			tb.set_cell({ "thread " + gadt::ToString(i), TableAlign::Middle }, 0, i + 1);
			std::stringstream ss;
			ss << "[ ";
			for (size_t n = 0; n < indexs[i].size(); n++)
				ss << (indexs[i])[n] << (n + 1 == indexs[i].size() ? "": ", ");
			ss << " ]";
			tb.set_cell({ " " + ss.str() }, 1, i + 1);
		}
		tb.Print();
	}

	//print result
	void RetrospectMapper::PrintResult(double time, std::vector<IndexList>& index_lists, std::vector<size_t>& item_counts) const
	{
		using namespace gadt::console;
		size_t source_count = 0;
		size_t item_count = 0;
		for (size_t i = 0; i < _thread_count; i++)
		{
			source_count += index_lists[i].size();
			item_count += item_counts[i];
		}

		Table tb(3, _thread_count + 2);
		tb.enable_title({ _layer.get_layer_str() + " Mapper, [ t = " + gadt::ToString(time) + "]", ConsoleColor::Yellow, TableAlign::Middle });
		tb.set_width({ 4, 15, 6 });
		tb.set_cell_in_row(0, {
			{ "Thread", ConsoleColor::Gray, TableAlign::Middle },
			{ "Source Files", ConsoleColor::Gray, TableAlign::Middle },
			{ "Item Count", ConsoleColor::Gray, TableAlign::Middle }
			});
		tb.set_cell_in_row(_thread_count + 1, {
			{ "Total", ConsoleColor::Gray, TableAlign::Middle },
			{ source_count, ConsoleColor::Gray, TableAlign::Middle },
			{ item_count, ConsoleColor::Gray, TableAlign::Middle }
			});
		for (size_t i = 0; i < _thread_count; i++)
		{
			std::stringstream ss;
			ss << "[ ";
			for (size_t n = 0; n < index_lists[i].size(); n++)
				ss << (index_lists[i])[n] << (n + 1 == index_lists[i].size() ? "" : ", ");
			ss << " ]";
			tb.set_cell_in_row(i + 1,
				{
					{ "Thread " + gadt::ToString(i), TableAlign::Middle },
					{ ss.str(), TableAlign::Middle } ,
					{ item_counts[i], TableAlign::Middle }
				});
		}
		tb.Print();
	}
}