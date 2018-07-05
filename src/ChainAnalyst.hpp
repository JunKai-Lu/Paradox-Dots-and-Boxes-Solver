#include "StateDefine.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	enum class ChainType
	{
		UNDEFINED = 0,
		CHAIN = 1,
		CIRCLE = 2,
		OPEN_CHAIN = 3,
		OPEN_CIRCLE = 4,
		DEAD_CHAIN = 5,
		DEAD_CIRCLE = 6
	};

	//box info that are included in chain analyst.
	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class BoxInfo :public DabBox<WIDTH, HEIGHT>
	{
	private:
		size_t _belonging_chain;

	public:

		BoxInfo():
			DabBox<WIDTH, HEIGHT>(DabBoard<WIDTH, HEIGHT>(), 0),
			_belonging_chain(BoxCount<WIDTH, HEIGHT>())
		{
		}

		BoxInfo(const DabBoard<WIDTH, HEIGHT>& board, EdgeIndex index):
			DabBox<WIDTH, HEIGHT>(board, index),
			_belonging_chain(BoxCount<WIDTH, HEIGHT>())
		{
		}

		inline size_t belonging_chain()
		{
			return _belonging_chain;
		}

		inline void set_belonging_chain(size_t index)
		{
			_belonging_chain = index;
		}

		inline bool no_belonging_chain()
		{
			return _belonging_chain == BoxCount<WIDTH, HEIGHT>();
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
			_type(ChainType::UNDEFINED),
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
		inline void clear()
		{
			_type = ChainType::UNDEFINED;
			_boxes_num = 0;
		}
	};


	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class ChainAnalyst
	{
	private:

		static constexpr const size_t _BOX_COUNT = BoxCount<WIDTH, HEIGHT>();

		const DabBoard<WIDTH, HEIGHT>& _board;
		std::array<BoxInfo<WIDTH, HEIGHT>, _BOX_COUNT> _boxes;
		std::array<ChainInfo, _BOX_COUNT> _chains;

	private:

		//analysis chains in this state.
		void AnalysisChains()
		{
			//register chain start from free box inside the grid.
			for (EdgeIndex i = 0; i < _BOX_COUNT; i++)
			{
				if ((_boxes[i].type() == DabBoxType::FREE_BOX || _boxes[i].type() == DabBoxType::EMPTY_BOX) && _boxes[i].no_belonging_chain())
				{
					for (EdgeIndex edge_i = 0; edge_i < 4; edge_i ++)
					{
						if (!_board.edge_exist(_boxes[i].own_edge(edge_i)) && _boxes[i].neighbour_exist(edge_i))//exist empty edge and neighbour.
						{
							if (_boxes[_boxes[i].neighbour_box(edge_i)].no_belonging_chain())//neighbour still not difine belonging chain.
							{
								RegisterChainFromBox(i, _boxes[i].neighbour_box(edge_i), _boxes[i].own_edge(edge_i));
							}
						}
					}
				}
			}

			//register chain from chain box in the brim of grid.
			for (size_t i = 0; i < WIDTH; i++)//upper boxes.
			{
				if (_boxes[i].type() == DabBoxType::CHAIN_BOX && _boxes[i].no_belonging_chain() && !_board.edge_exist(_boxes[i].the()))
				{
					RegisterChainFromBox(_BOX_COUNT, _boxes[i].index(), _boxes[i].the());
				}
			}

			for (size_t i = 0; i < WIDTH * HEIGHT; i += WIDTH)//left boxes.
			{
				if (_boxes[i].type() == DabBoxType::CHAIN_BOX && _boxes[i].no_belonging_chain() && !_board.edge_exist(_boxes[i].lve()))
				{
					RegisterChainFromBox(_BOX_COUNT, _boxes[i].index(), _boxes[i].lve());
				}
			}

			for (size_t i = (WIDTH * HEIGHT) - WIDTH; i < WIDTH * HEIGHT; i++)//lower boxes.
			{
				if (_boxes[i].type() == DabBoxType::CHAIN_BOX && _boxes[i].no_belonging_chain() && !_board.edge_exist(_boxes[i].bhe()))
				{
					RegisterChainFromBox(_BOX_COUNT, _boxes[i].index(), _boxes[i].bhe());
				}
			}

			for (size_t i = WIDTH - 1; i < WIDTH * HEIGHT; i += WIDTH)//right boxes.
			{
				if (_boxes[i].type() == DabBoxType::CHAIN_BOX && _boxes[i].no_belonging_chain() && !_board.edge_exist(_boxes[i].rve()))
				{
					RegisterChainFromBox(_BOX_COUNT, _boxes[i].index(), _boxes[i].rve());
				}
			}

			//register circle from chain box inside the grid.
			for (EdgeIndex i = 0; i < _BOX_COUNT; i++)
			{
				if (_boxes[i].type() == DabBoxType::CHAIN_BOX && _boxes[i].no_belonging_chain())
				{
					for (EdgeIndex edge_i = 0; edge_i < 4; edge_i++)
					{
						if (!_board.edge_exist(_boxes[i].own_edge(edge_i)))
						{
							RegisterCircleFromBox(i, _boxes[i].neighbour_box(edge_i), _boxes[i].own_edge(edge_i));
							break;
						}
					}
				}
			}

			//combine chains from free box.
			for (EdgeIndex i = 0; i < _BOX_COUNT; i++)
			{
				if (_boxes[i].type() == DabBoxType::FREE_BOX && _boxes[i].no_belonging_chain())
				{
					CombineChainsFromSourceBox(i);
				}
			}

		}

		//get first undefined chain index.
		inline EdgeIndex GetFirUndefinedChainIndex()
		{
			for (EdgeIndex i = 0; i < _BOX_COUNT; i++)
			{
				if (_chains[i].type() == ChainType::UNDEFINED)
				{
					return i;
				}
			}
			gadt::console::PrintError("chain list is full!");
			return 0;
		}

		//add box to the chain.
		inline void AddBoxToChain(size_t box, size_t chain)
		{
			_boxes[box].set_belonging_chain(chain);
			_chains[chain].add_box_num();
		}

		//register a chain from a box.
		void RegisterChainFromBox(EdgeIndex start_box, EdgeIndex fir_box, EdgeIndex ignore_edge)
		{
			if (!(_boxes[fir_box].type() == DabBoxType::CHAIN_BOX))
			{
				return;
			}

			EdgeIndex checking_box_index = fir_box;
			EdgeIndex ignoring_edge_index = ignore_edge;

			EdgeIndex new_chain = GetFirUndefinedChainIndex();

			for (size_t count = 0;; count++)
			{
				if (_boxes[checking_box_index].type() == DabBoxType::CHAIN_BOX)//this box is part of chain.
				{
					AddBoxToChain(checking_box_index, new_chain);
					//find next box.
					for (size_t i = 0; i < 4; i++)
					{
						if (_boxes[checking_box_index].own_edge(i) != ignoring_edge_index && !_board.edge_exist(_boxes[checking_box_index].own_edge(i)))
						{
							if (_boxes[checking_box_index].neighbour_exist(i))//next_box
							{
								//next box.
								ignoring_edge_index = _boxes[checking_box_index].own_edge(i);
								checking_box_index = _boxes[checking_box_index].neighbour_box(i);
							}
							else//next box is out of grid. so current box is the end of this chain.
							{
								//register chain type.
								_chains[new_chain].set_type(ChainType::CHAIN);
								if (start_box != _BOX_COUNT)
								{
									if (_boxes[start_box].no_belonging_chain() == false)
									{
										if (_chains[_boxes[start_box].belonging_chain()].type() == ChainType::OPEN_CIRCLE)
										{
											_chains[new_chain].set_type(ChainType::OPEN_CHAIN);
										}
									}
								}
								return;
							}
							break;
						}
					}
				}
				else if (_boxes[checking_box_index].type() == DabBoxType::FREE_BOX || _boxes[checking_box_index].type() == DabBoxType::EMPTY_BOX)//to the end of chain.
				{
					if (checking_box_index == start_box)//an open circle.
					{
						if (_boxes[checking_box_index].no_belonging_chain())
						{
							AddBoxToChain(checking_box_index, new_chain);
							_chains[new_chain].set_type(ChainType::OPEN_CIRCLE);
							for (size_t i = 0; i < 4; i++)
							{
								if (_boxes[checking_box_index].neighbour_exist(i))
								{
									size_t n = _boxes[_boxes[checking_box_index].neighbour_box(i)].belonging_chain();	//chain num of get neibought box 
									if (n != _BOX_COUNT && n != new_chain)
									{
										if (_chains[n].type() == ChainType::CHAIN)
										{
											_chains[n].set_type(ChainType::OPEN_CHAIN);
											break;
										}
									}
								}
							}
							return;
						}
						else
						{
							_chains[new_chain].set_type(ChainType::OPEN_CHAIN);
							return;
						}
					}
					else//normal chain
					{
						_chains[new_chain].set_type(ChainType::CHAIN);
						return;
					}
				}
				else
				{
					GADT_WARNING_IF(DAB_WARNING, true, "wrong state in box " + gadt::ToString(checking_box_index));
				}
				GADT_WARNING_IF(DAB_WARNING, count >= _BOX_COUNT, "infinite loop");
			}
		}

		//register a circle from a box.
		void RegisterCircleFromBox(EdgeIndex start_box, EdgeIndex fir_box, EdgeIndex ignore_edge)
		{
			if (!(_boxes[fir_box].type() == DabBoxType::CHAIN_BOX))
			{
				return;
			}

			EdgeIndex checking_box_index = fir_box;
			EdgeIndex ignoring_edge_index = ignore_edge;

			EdgeIndex new_chain = GetFirUndefinedChainIndex();
			for (size_t count = 0;; count++)
			{
				if (_boxes[checking_box_index].type() == DabBoxType::CHAIN_BOX)
				{
					AddBoxToChain(checking_box_index, new_chain);
					if (checking_box_index == start_box)//the end.
					{
						_chains[new_chain].set_type(ChainType::CIRCLE);
						return;
					}
					else//find next box.
					{
						for (size_t i = 0; i < 4; i++)
						{
							if (_boxes[checking_box_index].own_edge(i) != ignoring_edge_index && !_board.edge_exist(_boxes[checking_box_index].own_edge(i)))
							{
								GADT_WARNING_IF(DAB_WARNING, !_boxes[checking_box_index].neighbour_exist(i), "empty box in circle");
								ignoring_edge_index = _boxes[checking_box_index].own_edge(i);
								checking_box_index = _boxes[checking_box_index].neighbour_box(i);
								break;
							}
						}
					}
				}
				else
				{
					GADT_WARNING_IF(DAB_WARNING, true, "wrong state in box " + gadt::ToString(checking_box_index));
				}
				GADT_WARNING_IF(DAB_WARNING, count >= _BOX_COUNT, "infinite loop");
			}
		}

		//merge two chains into a single chain.
		void MergeChain(size_t fir_chain, size_t sec_chain, ChainType new_chain_type)
		{
			for (size_t i = 0; i < _BOX_COUNT; i++)
			{
				if (_boxes[i].belonging_chain() == sec_chain)
				{
					_boxes[i].set_belonging_chain(fir_chain);
				}
			}

			_chains[fir_chain].add_box_num(_chains[sec_chain].boxes_num());
			_chains[fir_chain].set_type(new_chain_type);

			_chains[sec_chain].clear();
		}

		//combine two chains from a source box they shared. the box must be the source of 3 or more chains.
		void CombineChainsFromSourceBox(EdgeIndex box_index)
		{
			//find two chains that is shorter than other
			size_t fir_chain_index = 0;
			size_t fir_chain_len = 0;
			for (EdgeIndex i = 0; i < 4; i++)
			{
				if (
					!_board.edge_exist(_boxes[box_index].own_edge(i)) &&
					_boxes[box_index].neighbour_exist(i)
					)
				{
					EdgeIndex neighbour = _boxes[box_index].neighbour_box(i);
					if (!_boxes[neighbour].no_belonging_chain())
					{
						if (_chains[_boxes[neighbour].belonging_chain()].boxes_num() > fir_chain_len)
						{
							fir_chain_len = _chains[_boxes[neighbour].belonging_chain()].boxes_num();
							fir_chain_index = _boxes[neighbour].belonging_chain();
						}
					}
				}
			}

			size_t sec_chain_index = 0;
			size_t sec_chain_len = 0;
			for (EdgeIndex i = 0; i < 4; i++)
			{
				if (
					!_board.edge_exist(_boxes[box_index].own_edge(i)) &&
					_boxes[box_index].neighbour_exist(i)
					)
				{
					EdgeIndex neighbour = _boxes[box_index].neighbour_box(i);
					if (!_boxes[neighbour].no_belonging_chain())
					{
						if (_chains[_boxes[neighbour].belonging_chain()].boxes_num() > sec_chain_len && _boxes[neighbour].belonging_chain() != fir_chain_index)
						{
							sec_chain_len = _chains[_boxes[neighbour].belonging_chain()].boxes_num();
							sec_chain_index = _boxes[neighbour].belonging_chain();
						}
					}
				}
			}

			AddBoxToChain(box_index, fir_chain_index);
			if (_chains[fir_chain_index].type() == ChainType::OPEN_CHAIN || _chains[sec_chain_index].type() == ChainType::OPEN_CHAIN)
			{
				MergeChain(fir_chain_index, sec_chain_index, ChainType::OPEN_CHAIN);
			}
			else
			{
				MergeChain(fir_chain_index, sec_chain_index, ChainType::CHAIN);
			}
		}

		//get the first edge of appointed chain.
		EdgeIndex GetFirstEdgeOfChain(size_t chain_index)
		{
			for (size_t i = 0; i < _BOX_COUNT; i++)
			{
				if (_boxes[i].belonging_chain() == chain_index)
				{
					for (size_t n = 0; n < 4; n++)
					{
						if (_boxes[i].neighbour_exist(n) && _boxes[_boxes[i].neighbour_box(n)].belonging_chain() == chain_index && _board.edge_exist(_boxes[i].own_edge(n)) == false)
						{
							return _boxes[i].own_edge(n);
						}

					}

					GADT_WARNING_IF(DAB_WARNING, true, "no find neighbour box that belong to same chain.");
				}
			}
			GADT_WARNING_IF(DAB_WARNING, true, "no find box belong to chain " + gadt::ToString(chain_index));
			return static_cast<EdgeIndex>(EdgeCount<WIDTH, HEIGHT>());
		}

	public:

		ChainAnalyst(const DabBoard<WIDTH, HEIGHT>& board) :
			_board(board),
			_boxes()
		{
			for (EdgeIndex i = 0; i < _BOX_COUNT; i++)
			{
				_boxes[i] = BoxInfo<WIDTH, HEIGHT>(board, i);
			}
			AnalysisChains();
		}

		DabActionList GetResult()
		{
			DabActionList actions;
			for (size_t i = 0; i < _BOX_COUNT; i++)
			{
				if (_chains[i].type() == ChainType::CIRCLE || _chains[i].type() == ChainType::CHAIN || _chains[i].type() == ChainType::OPEN_CHAIN)
				{
					auto edge = GetFirstEdgeOfChain(i);
					actions.push_back(DabAction());
					actions.back().set(edge);
				}
			}
			GADT_WARNING_IF(DAB_WARNING, actions.empty(), "no any action exist in this board");
			return actions;
		}

		//show boxes info.
		void ShowBoxInfo()
		{
			for (auto& box : _boxes)
			{
				if (box.index() % WIDTH == 0)
				{
					std::cout << std::endl;
				}
				std::cout << box.type() << " ";
			}
		}

		//show chains info.
		void ShowChainInfo()
		{
			using namespace gadt::console;
			gadt::console::ColorAllocator colors;
			for (size_t i = 0; i < _BOX_COUNT; i++)
			{
				if (i % WIDTH == 0)
				{
					PrintEndLine();
				}
				if (_boxes[i].no_belonging_chain())
				{
					std::cout << "   ";
				}
				else
				{
					if (_boxes[i].belonging_chain() <= 15)
					{
						Cprintf("■ ", colors.GetAnyByIndex(_boxes[i].belonging_chain() + 1));
					}
					else
					{
						std::cout << "■ ";
					}
				}

			}
			PrintEndLine<2>();
			for (size_t i = 0; i < _BOX_COUNT; i++)
			{
				if (_chains[i].type() != ChainType::UNDEFINED)
				{
					std::stringstream ss;
					ss << ">> chain " << i << " type = " << static_cast<int>(_chains[i].type()) << " box_num = " << _chains[i].boxes_num() << std::endl;
					Cprintf(ss.str(), colors.GetAnyByIndex(i + 1));
				}
			}
			PrintEndLine<2>();
		}
	};
}