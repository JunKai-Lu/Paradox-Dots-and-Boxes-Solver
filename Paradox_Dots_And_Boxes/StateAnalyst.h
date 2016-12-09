#pragma once

#include "stdafx.h"
#include "GameDefine.h"

using namespace std;

namespace DAB
{
	/*
								free-edge	dead-box		dead-chain

	FrontState						Yes			No			No
	FrontStateWithDeadBox			Yes			Yes			No
	RearState						No			No			No
	RearStateWithDeadBox			No			Yes			No
	RearStateWithDeadChain			No			Yes			No

	*/

	//state type means different period of state.
	enum StateType
	{
		ST_FrontState = 0,
		ST_FrontStateWithDeadBox = 1,

		ST_RearState = 2,
		ST_RearStateWithDeadBox = 3,
		ST_RearStateWithDeadChain = 4
	};

	//box type means boxes with different number of edges.
	enum BoxType
	{
		BT_FULL_BOX = 4,
		BT_DEAD_BOX = 3,
		BT_CHAIN_BOX = 2,
		BT_FREE_BOX = 1
	};

	//chain type is used to difine the prop of chain.
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

	//box info that are included in chain analyst.
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

	//chain info that are included in chain analyst.
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

	//an class that is used for analysing chain in a rear-state.
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

		//show boxes info.
		inline void ShowBoxInfo()
		{
			for (auto& box : _boxes)
			{
				if (box.index() % 5 == 0)
				{
					cout << endl;
				}
				cout << box.type() << " ";
			}
		}

		//show chains info.
		void ShowChainInfo();

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

	//determind the type of this state.
	StateType DetermindStateType(BitBoard board);

	//a action analyst is used for generate reasonable actions with filter.
	class ActionAnalyst
	{
	private:
		BitBoard _board;
		bool _action[MAX_EDGE];

	public:
		ActionAnalyst(BitBoard board);
		inline bool operator[](size_t index)
		{
			return action(index);
		}
		inline void set_action(size_t index)
		{
			WarningCheck(index < MAX_EDGE, "wrong index", __FILE__ , __LINE__ , __FUNCTION__);
			_action[index] = true;
		}
		inline bool action(size_t index)
		{
			WarningCheck(index < MAX_EDGE, "wrong index", __FILE__ , __LINE__ , __FUNCTION__);
			return _action[index];
		}
	};
}