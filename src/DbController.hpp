#include "DbInfo.h"
#include "LayerConatiner.hpp"
#include "StateDefine.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DbController
	{
	private:

		using LayerPointer = std::unique_ptr<LayerInfo>;

	private:

		LayerPointer _focus_layer;

	public:

		//default constructor.
		DbController() :
			_focus_layer(new LayerInfo(WIDTH, HEIGHT, EdgeCount<WIDTH, HEIGHT>()))
		{
			CreateOriginLayer();
		}

		//get the reference of focus layer.
		inline const LayerPointer& focus_layer() const
		{
			return _focus_layer;
		}

		//get current layer.
		inline size_t focus_layer_index() const
		{
			return _focus_layer->index();
		}

		//check out to another layer.
		inline void checkout(size_t layer)
		{
			_focus_layer = LayerPointer(new LayerInfo(WIDTH, HEIGHT, layer));
			CreateOriginLayer();
		}

	public:

		void CreateOriginLayer() const
		{
			if (_focus_layer->index() == EdgeCount<WIDTH, HEIGHT>())
			{
				if (_focus_layer->exist_raw_file() == false)
				{
					std::string path = _focus_layer->GetRawFilePath(0, 1);
					DabFileWriter writer(path);
					DabState<WIDTH, HEIGHT> end_state;
					end_state.BeFull();
					writer.save_item(end_state.board().to_ullong(), 0);
					_focus_layer->add_raw_file(0);
					_focus_layer->SaveToFile();
				}
				if (_focus_layer->exist_partition_file() == false)
				{
					std::string path = _focus_layer->GetPartitionFilePath(0, 1);
					DabFileWriter writer(path);
					DabState<WIDTH, HEIGHT> end_state;
					end_state.BeFull();
					writer.save_item(end_state.board().to_ullong(), 0);
					_focus_layer->add_part_file(0);
					_focus_layer->SaveToFile();
				}
			}
		}

		//print info of all existing layers in this db. 
		void PrintInfo() const
		{
			focus_layer()->PrintInfo();
		}

		//run map process for focus layer from previous layer.
		void RunMapProcess(size_t thread_count) const
		{
			RetrospectMapper mapper(*focus_layer(), thread_count, RetrospectFuncPackage<WIDTH, HEIGHT>::MapFunc);
			mapper.Run();
		}

		//run reduce process for focus layer.
		void RunReduceProcess(size_t thread_count, size_t segment_count) const
		{
			RetrospectReducer reducer(*focus_layer(), thread_count, segment_count, RetrospectFuncPackage<WIDTH, HEIGHT>::ReduceFunc);
			reducer.Run();
		}

		//print items in raw file.
		void PrintRaw(size_t index, bool enable_minimax) const
		{
			std::string path = focus_layer()->GetRawFilePath(index, focus_layer()->raw_files().size());
			if (gadt::filesystem::exist_file(path) == false)
			{
				gadt::console::PrintError("raw file not found!");
				return;
			}
			DabFileLoader loader(path);
			for (size_t count = 0;; count++)
			{
				auto item = loader.LoadNextItem();
				if (loader.is_eof())
				{
					gadt::console::PrintMessage("EOF, item count = " + gadt::ToString(count));
					return;
				}
				DabState<WIDTH, HEIGHT>(DabBoard<WIDTH, HEIGHT>(item.first), (int)item.second).Print();
				if (enable_minimax)
				{
					DabState<WIDTH, HEIGHT> state(DabBoard<WIDTH, HEIGHT>(item.first), 0);
					std::cout << "eval = " << (int)EvalMinimax<WIDTH, HEIGHT>(state) << std::endl;
				}
				if (!gadt::console::GetUserConfirm("continue?"))
				{
					gadt::console::PrintMessage("print completed, item count = " + gadt::ToString(count));
					return;
				}
			}
		}

		//print items in partition file.
		void PrintPartition(size_t index, bool enable_minimax) const
		{
			std::string path = focus_layer()->GetPartitionFilePath(index, focus_layer()->part_files().size());
			if (gadt::filesystem::exist_file(path) == false)
			{
				gadt::console::PrintError("partition file not found!");
				return;
			}
			DabFileLoader loader(path);
			for (size_t count = 0;; count ++)
			{
				auto item = loader.LoadNextItem();
				if (loader.is_eof())
				{
					gadt::console::PrintMessage("EOF, item count = " + gadt::ToString(count));
					return;
				}
				
				if (enable_minimax)
				{
					DabState<WIDTH, HEIGHT> state(DabBoard<WIDTH, HEIGHT>(item.first), 0);
					auto margin = EvalMinimax<WIDTH, HEIGHT>(state);
					if (margin != item.second)
					{
						DabState<WIDTH, HEIGHT>(DabBoard<WIDTH, HEIGHT>(item.first), (int)item.second).Print();
						std::cout << "eval = " << (int)margin << std::endl;
						std::cout << "feature = " << state.board().to_ullong() << std::endl;
						gadt::console::PrintMessage("Assert Failed!");
						gadt::console::SystemPause();
					}
				}
				else
				{
					DabState<WIDTH, HEIGHT>(DabBoard<WIDTH, HEIGHT>(item.first), (int)item.second).Print();
					if (!gadt::console::GetUserConfirm("continue?"))
					{
						gadt::console::PrintMessage("print completed, item count = " + gadt::ToString(count));
						return;
					}
				}
			}
		}

		//clear db.
		void ClearDB()
		{
			checkout(EdgeCount<WIDTH, HEIGHT>());
			for (size_t i = 0; i <= EdgeCount<WIDTH, HEIGHT>(); i++)
			{
				std::string info_path = focus_layer()->GetLayerInfoFilePath();
				std::string layer_dir = focus_layer()->GetLayerDirectory();
				checkout(i);
				gadt::filesystem::remove_file(info_path);
				gadt::filesystem::remove_directory(layer_dir);
				ClearLayer();
			}
			checkout(EdgeCount<WIDTH, HEIGHT>());
			focus_layer()->SaveToFile();//recreate focus layer.
		}

		//claer db of layer.
		void ClearLayer() const
		{
			focus_layer()->ClearPartFiles();
			focus_layer()->ClearRawFiles();
			if (gadt::filesystem::exist_file(focus_layer()->GetLayerInfoFilePath()))
				gadt::filesystem::remove_file(focus_layer()->GetLayerInfoFilePath());
			gadt::filesystem::remove_directory(focus_layer()->GetLayerDirectory());
			CreateOriginLayer();
		}

		//clear db of raw files
		void ClearRawFiles() const
		{
			focus_layer()->ClearRawFiles();
			focus_layer()->SaveToFile();//recreate focus layer.
		}

		//clear db of partition files
		void ClearPartFiles() const
		{
			focus_layer()->ClearPartFiles();
			focus_layer()->SaveToFile();//recreate focus layer.
		}
	};
}
