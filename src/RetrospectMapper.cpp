#include "RetrospectMapper.h"

namespace dots_and_boxes_solver
{
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
		DabFileWriter writer(raw_file_path);
		for (auto file : files)
		{
			DabFileLoader loader(file);
			for(;;)
			{
				DabStateItem item = loader.LoadNextItem();
				if (loader.is_eof())
					break;
				size_t count = _MapFunc(item, writer);
				*item_count += count;
			}
		}
	}

	//run mapper.
	void RetrospectMapper::Run() const
	{
		PrintSetting();
		if (gadt::console::GetUserConfirm("Do you confirm that to run mapper under this setting?"))
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

			//assign files for each thread.
			std::vector<FileList> file_lists(_thread_count);
			auto files = GetFileList();
			for (size_t i = 0; i < files.size(); i++)
				file_lists[i % _thread_count].push_back(files[i]);
			std::vector<size_t> item_counts(_thread_count, 0);

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
			PrintResult(tp.time_since_created(), file_lists, item_counts);
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
	void RetrospectMapper::PrintSetting() const
	{
		using namespace gadt::console;
		Table tb(2, 3);
		tb.set_width({ 5,5 });
		tb.set_cell_in_column(0, {
			{ "Game", ConsoleColor::Gray, TableAlign::Middle },
			{ "Layer",ConsoleColor::Gray, TableAlign::Middle },
			{ "Thread", ConsoleColor::Gray, TableAlign::Middle }
			});
		tb.set_cell_in_column(1, {
			{ gadt::ToString(_layer.width()) + "-" + gadt::ToString(_layer.height()), TableAlign::Middle },
			{ gadt::ToString(_layer.index()), TableAlign::Middle },
			{ _thread_count, TableAlign::Middle }
			});
		tb.Print();
	}

	//print result
	void RetrospectMapper::PrintResult(double time, std::vector<FileList>& file_lists, std::vector<size_t>& item_counts) const
	{
		using namespace gadt::console;
		size_t source_count = 0;
		size_t item_count = 0;
		for (size_t i = 0; i < _thread_count; i++)
		{
			source_count += file_lists[i].size();
			item_count += item_counts[i];
		}
		Table tb(3, _thread_count + 2);
		tb.enable_title({ "time = " + gadt::ToString(time), ConsoleColor::Gray, TableAlign::Middle });
		tb.set_width({ 4, 4, 8 });
		tb.set_cell_in_row(0, {
			{ "Index", ConsoleColor::Gray, TableAlign::Middle },
			{ "Source", ConsoleColor::Gray, TableAlign::Middle },
			{ "Item Count", ConsoleColor::Gray, TableAlign::Middle }
			});
		tb.set_cell_in_row(_thread_count + 1, {
			{ "Total", ConsoleColor::Gray, TableAlign::Middle },
			{ source_count, ConsoleColor::Gray, TableAlign::Middle },
			{ item_count, ConsoleColor::Gray, TableAlign::Middle }
			});
		for (size_t i = 0; i < _thread_count; i++)
		{
			tb.set_cell_in_row(i + 1,
				{
					{ "Thread " + gadt::ToString(i), TableAlign::Middle },
					{ file_lists[i].size(), TableAlign::Middle } ,
					{ item_counts[i], TableAlign::Middle }
				});
		}
		tb.Print();
	}
}