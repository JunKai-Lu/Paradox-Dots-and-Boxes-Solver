#include "DbInfo.h"
#include "LayerConatiner.hpp"
#include "StateRepresentation.hpp"

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
		}

		//get the reference of focus layer.
		inline LayerPointer& focus_layer()
		{
			return _focus_layer;
		}

		//get current layer.
		inline size_t focus_layer_index() const
		{
			return _focus_layer->layer();
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

		//TODO
		//run map process for focus layer from previous layer.
		void RunMapProcess(size_t thread_count) const
		{

		}

		//TODO
		//run reduce process for focus layer.
		void RunReduceProcess(size_t partition_count) const
		{

		}

		//clear db.
		void ClearDB() const
		{
			gadt::filesystem::remove_directory(focus_layer()->GetRootDirectory());
			focus_layer()->clear_raw_files();
			focus_layer()->clear_part_files();
			focus_layer()->SaveToFile();//recreate focus layer.
		}

		//claer db of layer.
		void ClearLayer() const
		{
			gadt::filesystem::remove_directory(focus_layer()->GetLayerDirectory());
			focus_layer()->clear_raw_files();
			focus_layer()->clear_part_files();
			focus_layer()->SaveToFile();//recreate focus layer.
		}

		//clear db of raw files
		void ClearRawFiles() const
		{
			gadt::filesystem::remove_directory(focus_layer()->GetRawDirectory());
			focus_layer()->clear_raw_files();
			focus_layer()->SaveToFile();//recreate focus layer.
		}

		//clear db of partition files
		void ClearPartFile() const
		{
			gadt::filesystem::remove_directory(focus_layer()->GetPartitionDirectory());
			focus_layer()->clear_part_files();
			focus_layer()->SaveToFile();//recreate focus layer.
		}
	};
}
