#include "stdafx.h"
#include "GameDefine.h"
#include "StateAnalyst.h"

using namespace std;

namespace dots_and_boxes
{
	namespace front_state
	{
		ActionVec GetFreeActions(const Board& board) 
		{
			ActionVec temp;
			for (Edge i = 0; i < MAX_EDGE; i++)
			{
				if (state::IsFreeEdge(board, i))
				{
					temp.set(i);
				}
			}
			return temp;

		}

		ActionVec GetFirDeadBoxAction(const Board& board)
		{
			ActionVec temp;
			Edge fir = state::GetDeadBoxRemainEdgeIndex(board);
			WARNING_CHECK(fir == MAX_EDGE, "no dead box exist");
			temp.set(fir);
			return temp;
		}
	}

	namespace rear_state
	{
		//constructor function.
		BoxInfo::BoxInfo(Board board, Edge index) :
			_index(index),
			_belonging_chain(MAX_CHAIN),
			_neighbour_box{ MAX_BOX,MAX_BOX,MAX_BOX,MAX_BOX }
		{
			//set own edges.
			_own_edge[0] = index;
			_own_edge[1] = state::UpperToLeftEdge(index);
			_own_edge[2] = index + 5;
			_own_edge[3] = state::UpperToLeftEdge(index) + 5;

			//set box type
			size_t edge_num = state::GetLowerBoxEdgeNum(board, index);
			if (edge_num == 4)
			{
				_type = box_type::FULL_BOX;
			}
			else if (edge_num == 3)
			{
				_type = box_type::DEAD_BOX;
			}
			else if (edge_num == 2)
			{
				_type = box_type::CHAIN_BOX;
			}
			else
			{
				_type = box_type::FREE_BOX;
			}

			//set neighbour boxes.
			if (state::IsNotUpperSideHorEdge(_own_edge[0]))
			{
				_neighbour_box[0] = index - 5;
			}
			if (state::IsNotLeftSideVecEdge(_own_edge[1]))
			{
				_neighbour_box[1] = index - 1;
			}
			if (state::IsNotLowerSideHorEdge(_own_edge[2]))
			{
				_neighbour_box[2] = index + 5;
			}
			if (state::IsNotRightSideVecEdge(_own_edge[3]))
			{
				_neighbour_box[3] = index + 1;
			}
		}

		//constructor function
		ChainAnalyst::ChainAnalyst(Board board) :
			_board(board),
			_boxes{
			BoxInfo(board,0),BoxInfo(board,1),BoxInfo(board,2),BoxInfo(board,3),BoxInfo(board,4),
			BoxInfo(board,5),BoxInfo(board,6),BoxInfo(board,7),BoxInfo(board,8),BoxInfo(board,9),
			BoxInfo(board,10),BoxInfo(board,11),BoxInfo(board,12),BoxInfo(board,13),BoxInfo(board,14),
			BoxInfo(board,15),BoxInfo(board,16),BoxInfo(board,17),BoxInfo(board,18),BoxInfo(board,19),
			BoxInfo(board,20),BoxInfo(board,21),BoxInfo(board,22),BoxInfo(board,23),BoxInfo(board,24)
		}
		{
			AnalysisChains();
		}

		//analysis chains in this state.
		void ChainAnalyst::AnalysisChains()
		{
			//register chain start from free box inside the grid.
			for (Edge i = 0; i < MAX_BOX; i++)
			{
				if (_boxes[i].type() == box_type::FREE_BOX && _boxes[i].NoBelongingChain())
				{
					for (Edge edge_i = 0; edge_i < 4; edge_i++)
					{
						if (!_board.get(_boxes[i].own_edge(edge_i)) && _boxes[i].IsNotEmptyNeighbour(edge_i))//exist empty edge and neighbour.
						{
							if (_boxes[_boxes[i].neighbour_box(edge_i)].NoBelongingChain())//neighbour still not difine belonging chain.
							{
								RegisterChainFromBox(i, _boxes[i].neighbour_box(edge_i), _boxes[i].own_edge(edge_i));
							}
						}
					}
				}
			}

			//register chain from chain box in the brim of grid.
			for (size_t i = 0; i < 5; i++)//upper boxes.
			{
				if (_boxes[i].type() == box_type::CHAIN_BOX && _boxes[i].NoBelongingChain() && !_board.get(_boxes[i].UpperEdge()))
				{
					RegisterChainFromBox(MAX_BOX, _boxes[i].index(), _boxes[i].UpperEdge());
				}
			}

			for (size_t i = 0; i < 25; i += 5)//left boxes.
			{
				if (_boxes[i].type() == box_type::CHAIN_BOX && _boxes[i].NoBelongingChain() && !_board.get(_boxes[i].LeftEdge()))
				{
					RegisterChainFromBox(MAX_BOX, _boxes[i].index(), _boxes[i].LeftEdge());
				}
			}

			for (size_t i = 20; i < 25; i++)//lower boxes.
			{
				if (_boxes[i].type() == box_type::CHAIN_BOX && _boxes[i].NoBelongingChain() && !_board.get(_boxes[i].LowerEdge()))
				{
					RegisterChainFromBox(MAX_BOX, _boxes[i].index(), _boxes[i].LowerEdge());
				}
			}

			for (size_t i = 4; i < 25; i += 5)//right boxes.
			{
				if (_boxes[i].type() == box_type::CHAIN_BOX && _boxes[i].NoBelongingChain() && !_board.get(_boxes[i].RightEdge()))
				{
					RegisterChainFromBox(MAX_BOX, _boxes[i].index(), _boxes[i].RightEdge());
				}
			}

			//register circle from chain box inside the grid.
			for (Edge i = 0; i < MAX_BOX; i++)
			{
				if (_boxes[i].type() == box_type::CHAIN_BOX && _boxes[i].NoBelongingChain())
				{
					for (Edge edge_i = 0; edge_i < 4; edge_i++)
					{
						if (!_board.get(_boxes[i].own_edge(edge_i)))
						{
							RegisterCircleFromBox(i, _boxes[i].neighbour_box(edge_i), _boxes[i].own_edge(edge_i));
							break;
						}
					}
				}
			}

			//combine chains from free box.
			for (Edge i = 0; i < MAX_BOX; i++)
			{
				if (_boxes[i].type() == box_type::FREE_BOX && _boxes[i].NoBelongingChain())
				{
					CombineChainsFromSourceBox(i);
				}
			}

		}

		//merge two chains into a single chain.
		void ChainAnalyst::MergeChain(size_t fir_chain, size_t sec_chain, chain_type::ChainType new_chain_type)
		{
			for (size_t i = 0; i < MAX_BOX; i++)
			{
				if (_boxes[i].belonging_chain() == sec_chain)
				{
					_boxes[i].set_belonging_chain(fir_chain);
				}
			}

			_chains[fir_chain].add_box_num(_chains[sec_chain].boxes_num());
			_chains[fir_chain].set_type(new_chain_type);

			_chains[sec_chain].Clear();
		}

		//register a chain from a box.
		void ChainAnalyst::RegisterChainFromBox(Edge start_box, Edge fir_box, Edge ignore_edge)
		{
			if (!(_boxes[fir_box].type() == box_type::CHAIN_BOX))
			{
				return;
			}

			Edge checking_box_index = fir_box;
			Edge ignoring_edge_index = ignore_edge;

			Edge new_chain = GetFirUndefinedChainIndex();

			for (size_t count = 0;; count++)
			{
				if (_boxes[checking_box_index].type() == box_type::CHAIN_BOX)//this box is part of chain.
				{
					AddBoxToChain(checking_box_index, new_chain);
					//find next box.
					for (size_t i = 0; i < 4; i++)
					{
						if (_boxes[checking_box_index].own_edge(i) != ignoring_edge_index && !_board.get(_boxes[checking_box_index].own_edge(i)))
						{
							if (_boxes[checking_box_index].IsNotEmptyNeighbour(i))//next_box
							{
								//next box.
								ignoring_edge_index = _boxes[checking_box_index].own_edge(i);
								checking_box_index = _boxes[checking_box_index].neighbour_box(i);
							}
							else//next box is out of grid. so current box is the end of this chain.
							{
								//register chain type.
								if (_chains[_boxes[start_box].belonging_chain()].type() == chain_type::OPEN_CIRCLE)
								{
									_chains[new_chain].set_type(chain_type::OPEN_CHAIN);
								}
								else
								{
									_chains[new_chain].set_type(chain_type::CHAIN);
								}
								return;
							}
							break;
						}
					}
				}
				else if (_boxes[checking_box_index].type() == box_type::FREE_BOX)//to the end of chain.
				{
					if (checking_box_index == start_box)//an open circle.
					{
						AddBoxToChain(checking_box_index, new_chain);
						_chains[new_chain].set_type(chain_type::OPEN_CIRCLE);
						for (size_t i = 0; i < 4; i++)
						{
							if (_boxes[checking_box_index].IsNotEmptyNeighbour(i))
							{
								size_t n = _boxes[_boxes[checking_box_index].neighbour_box(i)].belonging_chain();	//chain num of get neibought box 
								if (n != MAX_CHAIN && n != new_chain)
								{
									if (_chains[n].type() == chain_type::CHAIN)
									{
										_chains[n].set_type(chain_type::OPEN_CHAIN);
										break;
									}
								}
							}
						}
						return;
					}
					else//normal chain
					{
						_chains[new_chain].set_type(chain_type::CHAIN);
						return;
					}
				}
				else
				{
					WARNING_CHECK(true, "wrong state in box " + console::I2S(checking_box_index));
				}
				WARNING_CHECK(count >= MAX_BOX, "infinite loop");
			}
		}

		//register a circle from a box.
		void ChainAnalyst::RegisterCircleFromBox(Edge start_box, Edge fir_box, Edge ignore_edge)
		{
			if (!(_boxes[fir_box].type() == box_type::CHAIN_BOX))
			{
				return;
			}

			Edge checking_box_index = fir_box;
			Edge ignoring_edge_index = ignore_edge;

			Edge new_chain = GetFirUndefinedChainIndex();
			for (size_t count = 0;; count++)
			{
				if (_boxes[checking_box_index].type() == box_type::CHAIN_BOX)
				{
					AddBoxToChain(checking_box_index, new_chain);
					if (checking_box_index == start_box)//the end.
					{
						_chains[new_chain].set_type(chain_type::CIRCLE);
						return;
					}
					else//find next box.
					{
						for (size_t i = 0; i < 4; i++)
						{
							if (_boxes[checking_box_index].own_edge(i) != ignoring_edge_index && !_board.get(_boxes[checking_box_index].own_edge(i)))
							{
								WARNING_CHECK(!_boxes[checking_box_index].IsNotEmptyNeighbour(i), "empty box in circle");
								ignoring_edge_index = _boxes[checking_box_index].own_edge(i);
								checking_box_index = _boxes[checking_box_index].neighbour_box(i);
								break;
							}
						}
					}
				}
				else
				{
					WARNING_CHECK(true, "wrong state in box " + console::I2S(checking_box_index));
				}
				WARNING_CHECK(count >= MAX_BOX, "infinite loop");
			}
		}

		//combine two chains from a source box they shared. the box must be source of 3 or more chains.
		void ChainAnalyst::CombineChainsFromSourceBox(Edge box_index)
		{
			//find two chains that is shorter than other
			size_t fir_chain_index = 0;
			size_t fir_chain_len = 0;
			for (Edge i = 0; i < 4; i++)
			{
				if ( 
					!_board.get(_boxes[box_index].own_edge(i)) &&
					_boxes[box_index].IsNotEmptyNeighbour(i) 
					)
				{
					Edge neighbour = _boxes[box_index].neighbour_box(i);
					if (!_boxes[neighbour].NoBelongingChain()) 
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
			for (Edge i = 0; i < 4; i++)
			{
				if (
					!_board.get(_boxes[box_index].own_edge(i)) &&
					_boxes[box_index].IsNotEmptyNeighbour(i)
					)
				{
					Edge neighbour = _boxes[box_index].neighbour_box(i);
					if (!_boxes[neighbour].NoBelongingChain())
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
			if (_chains[fir_chain_index].type() == chain_type::OPEN_CHAIN || _chains[sec_chain_index].type() == chain_type::OPEN_CHAIN)
			{
				MergeChain(fir_chain_index, sec_chain_index, chain_type::OPEN_CHAIN);
			}
			else
			{
				MergeChain(fir_chain_index, sec_chain_index, chain_type::CHAIN);
			}
		}

		//show chain info
		void ChainAnalyst::ShowChainInfo()
		{
			for (size_t i = 0; i < MAX_BOX; i++)
			{
				if (i % 5 == 0)
				{
					cout << endl;
				}
				if (_boxes[i].NoBelongingChain())
				{
					cout << "   ";
				}
				else
				{
					if (_boxes[i].belonging_chain() <= 15)
					{
						console::Cprintf("�� ", console::color::Color(_boxes[i].belonging_chain() + 1));
					}
					else
					{
						cout << "�� ";
					}
				}

			}
			cout << endl;
			cout << endl;
			for (size_t i = 0; i < MAX_CHAIN; i++)
			{
				if (_chains[i].type() != chain_type::UNDEFINED)
				{
					stringstream ss;
					ss << ">> chain " << i << " type = " << chain_type::ToString(_chains[i].type()) << " box_num = " << _chains[i].boxes_num() << endl;
					console::Cprintf(ss.str(), console::color::Color(i + 1));
				}
			}
			cout << endl;
			cout << endl;
		}
	}

	//constructor function
	Analyst::Analyst(Board board):
		_board(board),
		_state_type(DetermindStateType(board))
	{
	}

	//determind state type.
	state_type::StateType Analyst::DetermindStateType(Board board)
	{
		if (state::ExistDeadChain(board))
		{
			return state_type::REAR_WITH_DEAD_CHAIN;
		}

		if (state::ExistFreeEdge(board))//front state.
		{
			if (state::ExistDeadBox(board))
			{
				return state_type::FRONT_WITH_DEAD_BOX;
			}
			return state_type::FRONT;
		}
		//rear state
		
		if (state::ExistDeadBox(board))
		{
			return state_type::REAR_WITH_DEAD_BOX;
		}
		return state_type::REAR;
	}

	//TODO Action Analyst
	ActionAnalyst::ActionAnalyst(Board board) :
		Analyst(board)
	{
		if (_state_type == state_type::FRONT_WITH_DEAD_BOX || _state_type == state_type::REAR_WITH_DEAD_BOX)
		{
			_result = front_state::GetFirDeadBoxAction(_board);
		}
		else if (_state_type == state_type::FRONT)
		{
			_result = front_state::GetFreeActions(_board);
		}
		else if (_state_type == state_type::REAR)
		{

		}
		else if (_state_type == state_type::REAR_WITH_DEAD_CHAIN)
		{

		}
	}

	//Get random action in action analyst.
	Edge ActionAnalyst::RandomAction()
	{
		WARNING_CHECK(_result.none(), "no any action is legal.");
		Edge actions[MAX_EDGE];
		size_t c_index = 0;
		for (Edge edge = 0; edge < MAX_EDGE; edge++)
		{
			if (_result.get(edge))
			{
				actions[c_index] = edge;
				c_index++;
			}
		}
		size_t rnd = rand() % c_index;
		return actions[rnd];
	}

	//visualization.
	void ActionAnalyst::Visualization(Edge tag_edge) const
	{
		const console::color::Color edge_color = console::color::yellow;
		const console::color::Color action_color = console::color::white;
		const console::color::Color dot_color = console::color::gray;
		const console::color::Color tag_color = console::color::green;
		cout << endl;
		string interval = "     ";
		for (Edge y = 0; y < GAME_SIZE; y++)
		{
			//horizon edges, first grid.
			cout << interval;
			for (Edge index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "��";
				console::Cprintf("��", dot_color);
				if (_board.get(index))
				{
					if(index == tag_edge)
					{
						console::Cprintf("��", tag_color);
					}
					else
					{
						console::Cprintf("��", edge_color);
					}
				}
				else
				{
					cout << "  ";
				}
			}
			console::Cprintf("��" + interval, dot_color);
			//cout << "��" << endl;

			//horizon edges, second grid.
			for (Edge index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "��";
				console::Cprintf("��", dot_color);
				if (_board.get(index))
				{
					if (index == tag_edge)
					{
						console::Cprintf("��", tag_color);
					}
					else
					{
						console::Cprintf("��", edge_color);
					}
				}
				else
				{
					if (_result.get(index))
					{
						console::Cprintf("��", action_color);
						//cout << "��";
					}
					else
					{
						cout << "  ";
					}

				}
			}
			console::Cprintf("��\n", dot_color);

			//vertical edges, first grid.
			cout << interval;
			for (Edge index = 34 - y; index < 59 - y; index += 5)
			{
				if (_board.get(index))
				{
					if(index == tag_edge)
					{ 
						console::Cprintf("��", tag_color);
					}
					else
					{
						console::Cprintf("��", edge_color);
					}
					Edge upper_right_hor_edge = state::LeftToUpperEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					Edge right_vec_edge = index + 5;
					if (_board.get(upper_right_hor_edge) && _board.get(lower_right_hor_edge) && _board.get(right_vec_edge))
					{
						cout << "��";
					}
					else
					{
						cout << "  ";
					}
				}
				else
				{
					cout << "    ";
				}
			}
			if (_board.get(59 - y))
			{
				if ((59 - y) == tag_edge)
				{
					console::Cprintf("��" + interval, tag_color);
				}
				else
				{
					console::Cprintf("��" + interval, edge_color);
				}
			}
			else
			{
				cout << "  " + interval;
			}

			//vertical edges, second grid.
			for (Edge index = 34 - y; index < 59 - y; index += 5)
			{
				if (_board.get(index))
				{
					if (index == tag_edge)
					{
						console::Cprintf("��", tag_color);
					}
					else
					{
						console::Cprintf("��", edge_color);
					}
					Edge upper_right_hor_edge = state::LeftToUpperEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					Edge right_vec_edge = index + 5;
					if (_board.get(upper_right_hor_edge) && _board.get(lower_right_hor_edge) && _board.get(right_vec_edge))
					{
						cout << "��";
					}
					else
					{
						cout << "  ";
					}
				}
				else
				{
					if (_result.get(index))
					{
						//cout << "��";
						console::Cprintf("��", action_color);
					}
					else
					{
						cout << "  ";
					}
					cout << "  ";
				}
			}



			if (_board.get(59 - y))
			{
				if ((59-y) == tag_edge)
				{
					console::Cprintf("��\n", tag_color);
				}
				else
				{
					console::Cprintf("��\n", edge_color);
				}
			}
			else
			{
				if (_result.get(59 - y))
				{
					//cout << "��" << endl;
					console::Cprintf("��\n", action_color);
				}
				else
				{
					cout << "  " << endl;
				}
			}
		}

		//print lower horizon edges, first grid.
		cout << interval;
		for (Edge index = 25; index < 30; index++)
		{
			//cout << "��";
			console::Cprintf("��", dot_color);
			if (_board.get(index))
			{
				if (index == tag_edge)
				{
					console::Cprintf("��", tag_color);
				}
				else
				{
					console::Cprintf("��", edge_color);
				}
			}
			else
			{
				cout << "  ";
			}
		}
		//cout << "��" << endl;
		console::Cprintf("��" + interval, dot_color);

		//print lower horizon edges, second grid.
		for (Edge index = 25; index < 30; index++)
		{
			//cout << "��";
			console::Cprintf("��", dot_color);
			if (_board.get(index))
			{
				if (index == tag_edge)
				{
					console::Cprintf("��", tag_color);
				}
				else
				{
					console::Cprintf("��", edge_color);
				}
			}
			else
			{
				if (_result.get(index))
				{
					//cout << "��";
					console::Cprintf("��", action_color);
				}
				else
				{
					cout << "  ";
				}
			}
		}
		//cout << "��" << endl;
		console::Cprintf("��\n\n", dot_color);
	}

	//TODO Retro Analyst
	RetroAnalyst::RetroAnalyst(Board board) :
		Analyst(board)
	{
		//TODO
		if (_state_type == state_type::FRONT_WITH_DEAD_BOX || _state_type == state_type::REAR_WITH_DEAD_BOX)
		{
		}
		else if (_state_type == state_type::FRONT)
		{

		}
		else if (_state_type == state_type::REAR)
		{
		}
		else if (_state_type == state_type::REAR_WITH_DEAD_CHAIN)
		{
		}
	}
}