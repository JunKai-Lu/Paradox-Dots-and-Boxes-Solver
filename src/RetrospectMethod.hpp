#include "DbInfo.h"
#include "LayerConatiner.hpp"
#include "StateDefine.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	constexpr char* DB_ITEM_SEPARATOR = " ";
	constexpr uint8_t RETROSPCECT_MAX_PARTITION_COUNT = 255;
	constexpr uint8_t RETROSPCECT_MAX_REDUCE_SEGMENT_COUNT = 255;

	//Map function, which need to write all resulted items into a file and return the count. 
	using RetrospectMapFunc = std::function<size_t(const DabStateItem&, LayerTable&)>;
	
	//Reduce function, which need to cover/write a item into a table and return true if it cover/write the item.
	using RetrospectReduceFunc = std::function<bool(const DabStateItem&, LayerTable&)>;

	//Filter function, which need to decide if a item is able to update the table.
	using RetrospectFilterFunc = std::function<bool(BoardValueType)>;

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	struct RetrospectFuncPackage
	{
		//generate items in new layer from item in layer whose edges is more.
		static size_t MapFunc(const DabStateItem & item, LayerTable& table)
		{
			DabBoard<WIDTH, HEIGHT> board(item.first);
			//std::stringstream buffer;
			size_t item_count = 0;
			for (EdgeIndex i = 0; i < EdgeCount<WIDTH, HEIGHT>(); i++)
			{
				if (board.edge_exist(i))//we can remove this edge.
				{
					MarginType box_count = static_cast<MarginType>(board.count_of_boxes_that_owns_edge(i));
					DabBoard<WIDTH, HEIGHT> new_board = board;
					new_board.reset_edge(i);
					bool filter = (BoxCount<WIDTH, HEIGHT>() - new_board.ExistingBoxCount()) <= 5;
					if (filter == false)
						filter = new_board.IsReasonable();
					if (filter)
					{
						MarginType new_margin = item.second;
						if (box_count == 0)
							new_margin = -new_margin;//m' = -m
						else
							new_margin += box_count;
						//buffer << new_board.to_ullong() << DB_ITEM_SEPARATOR << (int)new_margin << std::endl;
						table.UpdateIfLarger({ new_board.ToMinimalFormat().to_ullong(), new_margin });
						//cache << new_board.to_ullong() << DB_ITEM_SEPARATOR << (int)new_margin << std::endl;
						item_count++;
					}
				}
			}
			//writer.write(buffer.str());
			return item_count;
		}

		//sort all generated items and get their highest margin.
		static bool ReduceFunc(const DabStateItem & item, LayerTable & layer_table)
		{
			return layer_table.UpdateIfLarger(item);
		}

		static bool FilterFunc(BoardValueType)
		{
			return false;
		}
	};
}
