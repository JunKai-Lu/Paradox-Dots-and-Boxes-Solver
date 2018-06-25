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
			if (_focus_layer->exist_raw_file() == false)
			{
				std::string path = _focus_layer->GetRawFilePath(0);
				DabFileWriter writer(path);
				DabState<WIDTH, HEIGHT> end_state;
				end_state.BeFull();
				writer.save_item(end_state.board().to_ullong(), 0);
				_focus_layer->add_raw_file(0);
				_focus_layer->SaveToFile();
			}
			if (_focus_layer->exist_partition_file() == false)
			{
				std::string path = _focus_layer->GetPartitionFilePath(0,1);
				DabFileWriter writer(path);
				DabState<WIDTH, HEIGHT> end_state;
				end_state.BeFull();
				writer.save_item(end_state.board().to_ullong(), 0);
				_focus_layer->add_part_file(0);
				_focus_layer->SaveToFile();
			}
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
		}

	public:

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

		//clear db.
		void ClearDB() const
		{
			gadt::filesystem::remove_directory(focus_layer()->GetRootDirectory());
			focus_layer()->ClearRawFiles();
			focus_layer()->ClearPartFiles();
			focus_layer()->SaveToFile();//recreate focus layer.
		}

		//claer db of layer.
		void ClearLayer() const
		{
			gadt::filesystem::remove_directory(focus_layer()->GetLayerDirectory());
			focus_layer()->ClearPartFiles();
			focus_layer()->ClearRawFiles();
			focus_layer()->SaveToFile();//recreate focus layer.
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
