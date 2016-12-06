#pragma once

#include "stdafx.h"
#include "GameDefine.h"

using namespace std;

namespace DAB
{
		enum BoxType
		{
			BT_FULL_BOX = 4,
			BT_DEAD_BOX = 3,
			BT_CHAIN_BOX = 2,
			BT_FREE_BOX = 1
		};

		enum ChainType
		{
			CT_UNDEFINED = 0,
			CT_CHAIN = 1,
			CT_CIRCLE = 2,
			CT_OPEN_CHAIN = 3,
			CT_OPEN_CIRCLE = 4,
			CT_DEAD_CHAIN = 5,
			CT_DEAD_CIRCLE = 6
		};

		class BoxInfo
		{
		private:
			Edge _index;
			BoxType _type;
			size_t _belonging_chain;

			Edge _own_edge[4];
			Edge _neighbour_box[4];

		public:
			BoxInfo(BitBoard board, Edge index);


			inline Edge index()
			{
				return _index;
			}
			inline BoxType type()
			{
				return _type;
			}
			inline size_t belonging_chain()
			{
				return _belonging_chain;
			}
			inline Edge own_edge(size_t index)
			{
				return _own_edge[index];
			}
			inline Edge neighbour_box(size_t index)
			{
				return _neighbour_box[index];
			}

			inline void set_belonging_chain(size_t index)
			{
				_belonging_chain = index;
			}

			inline bool NoBelongingChain()
			{
				return _belonging_chain == MAX_CHAIN;
			}
			inline bool IsNotEmptyNeighbour(size_t index)
			{
				return _neighbour_box[index] != MAX_BOX;
			}
			inline Edge UpperEdge()
			{
				return _own_edge[0];
			}
			inline Edge LeftEdge()
			{
				return _own_edge[1];
			}
			inline Edge LowerEdge()
			{
				return _own_edge[2];
			}
			inline Edge RightEdge()
			{
				return _own_edge[3];
			}

		};

		class ChainInfo
		{
		private:
			ChainType _type;
			size_t _boxes_num;

		public:
			inline ChainInfo() :
				_type(CT_UNDEFINED),
				_boxes_num(0)
			{

			}
			inline ChainType type()
			{
				return _type;
			}
			inline size_t boxes_num()
			{
				return _boxes_num;
			}
			inline void set_type(ChainType type)
			{
				_type = type;
			}
			inline void add_box_num(size_t num = 1)
			{
				_boxes_num += num;
			}
			inline void Clear()
			{
				_type = CT_UNDEFINED;
				_boxes_num = 0;
			}
		};

		//an class that is used for analysis chain in a raer state.
		class ChainAnalyst
		{
		public:
			ChainAnalyst(BitBoard board);

		private:
			BitBoard _board;

			BoxInfo _boxes[MAX_BOX];
			ChainInfo _chains[MAX_CHAIN];

			//edge info
			bool _reasonable_edge[MAX_EDGE];

			//analysis chains in this state.
			void AnalysisChains();

			//get first undefined chain index.
			inline Edge GetFirUndefinedChainIndex()
			{
				for (Edge i = 0; i < MAX_CHAIN; i++)
				{
					if (_chains[i].type() == CT_UNDEFINED)
					{
						return i;
					}
				}
				Error("chain list is full!");
				return 0;
			}

			//add box to the chain.
			inline void AddBoxToChain(Edge box, Edge chain)
			{
				_boxes[box].set_belonging_chain(chain);
				_chains[chain].add_box_num();
			}

			//register a chain from a box.
			void RegisterChainFromBox(Edge start_box, Edge fir_box, Edge ignore_edge);

			//register a circle from a box.
			void RegisterCircleFromBox(Edge start_box, Edge fir_box, Edge ignore_edge);

			//merge two chains into a single chain.
			void MergeChain(Edge fir_chain, Edge sec_chain, ChainType new_chain_type);



		public:

			//judge whether a action is reasonable
			inline bool IsReasonableAction(Edge edge)
			{
				return _reasonable_edge[edge];
			}

			//show info
			void ShowInfo();
		};
}