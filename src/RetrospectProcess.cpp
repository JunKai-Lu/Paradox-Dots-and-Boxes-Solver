#include "RetrospectProcess.h"

namespace dots_and_boxes_solver
{
	//get part file list from previous layer info.
	RetrospectMapper::FileList RetrospectMapper::GetFileList() const
	{
		std::vector<std::string> file_path_vec;
		//get previous layer
		if (_layer.layer() < EdgeCount(_layer.width(), _layer.height()))
		{
			LayerInfo prev_layer(_layer.width(), _layer.height(), _layer.layer() + 1);
			for (auto index : prev_layer.part_files())
			{
				std::string path = prev_layer.GetPartitionFilePath(index);
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
			while (loader.is_eof() == false)
			{
				DabStateItem item = loader.LoadNextItem();
				size_t count = _MapFunc(item, writer);
				*item_count += count;
			}
		}
	}

	//run mapper.
	void RetrospectMapper::Run() const
	{
		PrintSetting();
		if (gadt::console::GetUserConfirm("Are you sure run mapper under this setting?"))
		{
			if (_layer.exist_raw_file())
			{
				if (!gadt::console::GetUserConfirm("all the existing raw files would be clear, continue?"))
				{
					gadt::console::ShowMessage("Operation canceled!");
					return;
				}
				//divede files into multi part.
				gadt::timer::TimePoint tp;
				std::vector<FileList> file_lists(_thread_count);
				auto files = GetFileList();
				for (size_t i = 0; i < files.size(); i++)
					file_lists[i % _thread_count].push_back(files[i]);
				std::vector<size_t> item_counts(_thread_count, 0);
				std::vector<std::thread> threads;
				for (size_t i = 0; i < _thread_count; i++)
					threads.push_back(std::thread([&](FileList files, size_t* count, size_t index)->void {
					SingleMapProcess(files, count, index); 
				}, file_lists[i], &item_counts[i], i));
				for (size_t i = 0; i < threads.size(); i++)
					threads[i].join();
				PrintResult(tp.time_since_created(), file_lists, item_counts);
			}
		}
	}

	//constructor.
	RetrospectMapper::RetrospectMapper(const LayerInfo & layer, size_t thread_count, RetrospectMapFunc MapFunc):
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
		tb.set_cell_in_column(0, {
				{ "Game", COLOR_GRAY, TABLE_ALIGN_MIDDLE },
				{ "Layer", COLOR_GRAY, TABLE_ALIGN_MIDDLE },
				{ "Thread", COLOR_GRAY, TABLE_ALIGN_MIDDLE }
			});
		tb.set_cell_in_column(1, {
			{ gadt::ToString(_layer.width()) + "-" + gadt::ToString(_layer.height()), TABLE_ALIGN_MIDDLE },
			{ gadt::ToString(_layer.layer()), TABLE_ALIGN_MIDDLE },
			{ _thread_count, TABLE_ALIGN_MIDDLE }
			});
		tb.Print();
	}

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
		tb.enable_title({ "time = " + gadt::ToString(time), COLOR_GRAY, TABLE_ALIGN_MIDDLE });
		tb.set_width({ 2, 4, 6 });
		tb.set_cell_in_row(0, {
				{ "Index", COLOR_GRAY, TABLE_ALIGN_MIDDLE },
				{ "Source", COLOR_GRAY, TABLE_ALIGN_MIDDLE },
				{ "Item Count", COLOR_GRAY, TABLE_ALIGN_MIDDLE }
			});
		tb.set_cell_in_row(_thread_count + 1, {
				{ "Total", COLOR_GRAY, TABLE_ALIGN_MIDDLE },
				{ source_count, COLOR_GRAY, TABLE_ALIGN_MIDDLE },
				{ item_count, COLOR_GRAY, TABLE_ALIGN_MIDDLE }
			});
		for (size_t i = 0; i < _thread_count; i++)
		{
			tb.set_cell_in_row(i + 1,
				{
					{ "Thread " + gadt::ToString(i), TABLE_ALIGN_MIDDLE },
					{ file_lists[i].size() } ,
					{ item_counts[i] }
				});
		}
		tb.Print();
	}

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

	//constructor.
	RetrospectReducer::RetrospectReducer(const LayerInfo & layer, size_t segment_count, RetrospectReduceFunc ReduceFunc):
		_layer(layer),
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