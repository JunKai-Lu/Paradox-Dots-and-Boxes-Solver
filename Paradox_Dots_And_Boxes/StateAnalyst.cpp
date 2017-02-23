#include "GameDefine.h"
#include "StateAnalyst.h"

using namespace std;
using namespace gadt;

namespace dots_and_boxes
{
	namespace action_analyst
	{
		//Get free actions.
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

		//get the actions that appointed the edge can take the fir dead box.
		ActionVec GetFirDeadBoxAction(const Board& board)
		{
			ActionVec temp;
			Edge fir = state::GetFirstFreeEdgeInDeadBox(board);
			GADT_WARNING_CHECK(fir == MAX_EDGE, "no dead box exist");
			temp.set(fir);
			return temp;
		}

		//get the possible actions for dead chain.
		ActionVec GetDeadChainAction(const Board & board)
		{
			ActionVec temp_vec;
			Board temp_board = board;
			Edge fir = state::GetFirstFreeEdgeInDeadChain(board);
			temp_vec.set(fir);
			temp_board.set(fir);
			if (!state::ExistDeadChain(temp_board))
			{
				//find next edge.
				if (state::IsHorEdge(fir))
				{
					//horizon edge.
					if (state::IsNotUpperSideHorEdge(fir))
					{
						//check upper box.
						state::Box box(temp_board, fir - 5);
						if (box.type() == state::box_type::DEAD_BOX)
						{
							for (size_t i = 0; i < 4; i++)
							{
								if (!temp_board.get(box.own_edge(i)))
								{
									temp_vec.set(box.own_edge(i));
									return temp_vec;
								}
							}
						}
					}
					if(state::IsNotLowerSideHorEdge(fir))
					{
						//check lower box.
						state::Box box(temp_board, fir);
						if (box.type() == state::box_type::DEAD_BOX)
						{
							for (size_t i = 0; i < 4; i++)
							{
								if (!temp_board.get(box.own_edge(i)))
								{
									temp_vec.set(box.own_edge(i));
									return temp_vec;
								}
							}
						}
					}
				}
				else
				{
					//vecitical edge.
					if (state::IsNotRightSideVecEdge(fir))
					{
						//check right box.
						state::Box box(temp_board, state::LeftToUpperEdge(fir));
						if (box.type() == state::box_type::DEAD_BOX)
						{
							for (size_t i = 0; i < 4; i++)
							{
								if (!temp_board.get(box.own_edge(i)))
								{
									temp_vec.set(box.own_edge(i));
									return temp_vec;
								}
							}
						}
						
					}
					if (state::IsNotLeftSideVecEdge(fir))
					{
						//check left box.
						state::Box box(temp_board, state::LeftToUpperEdge(fir - 5));
						if (box.type() == state::box_type::DEAD_BOX)
						{
							for (size_t i = 0; i < 4; i++)
							{
								if (!temp_board.get(box.own_edge(i)))
								{
									temp_vec.set(box.own_edge(i));
									return temp_vec;
								}
							}
						}
					}
				}
			}
			return temp_vec;
		}
	}

	namespace rear_state
	{
		//constructor function.
		BoxInfo::BoxInfo(const Board& board, Edge index) :
			Box(board,index),
			_belonging_chain(MAX_CHAIN)
		{
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
				if (_boxes[i].type() == state::box_type::FREE_BOX && _boxes[i].NoBelongingChain())
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
				if (_boxes[i].type() == state::box_type::CHAIN_BOX && _boxes[i].NoBelongingChain() && !_board.get(_boxes[i].UpperEdge()))
				{
					RegisterChainFromBox(MAX_BOX, _boxes[i].index(), _boxes[i].UpperEdge());
				}
			}

			for (size_t i = 0; i < 25; i += 5)//left boxes.
			{
				if (_boxes[i].type() == state::box_type::CHAIN_BOX && _boxes[i].NoBelongingChain() && !_board.get(_boxes[i].LeftEdge()))
				{
					RegisterChainFromBox(MAX_BOX, _boxes[i].index(), _boxes[i].LeftEdge());
				}
			}

			for (size_t i = 20; i < 25; i++)//lower boxes.
			{
				if (_boxes[i].type() == state::box_type::CHAIN_BOX && _boxes[i].NoBelongingChain() && !_board.get(_boxes[i].LowerEdge()))
				{
					RegisterChainFromBox(MAX_BOX, _boxes[i].index(), _boxes[i].LowerEdge());
				}
			}

			for (size_t i = 4; i < 25; i += 5)//right boxes.
			{
				if (_boxes[i].type() == state::box_type::CHAIN_BOX && _boxes[i].NoBelongingChain() && !_board.get(_boxes[i].RightEdge()))
				{
					RegisterChainFromBox(MAX_BOX, _boxes[i].index(), _boxes[i].RightEdge());
				}
			}

			//register circle from chain box inside the grid.
			for (Edge i = 0; i < MAX_BOX; i++)
			{
				if (_boxes[i].type() == state::box_type::CHAIN_BOX && _boxes[i].NoBelongingChain())
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
				if (_boxes[i].type() == state::box_type::FREE_BOX && _boxes[i].NoBelongingChain())
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
			if (!(_boxes[fir_box].type() == state::box_type::CHAIN_BOX))
			{
				return;
			}

			Edge checking_box_index = fir_box;
			Edge ignoring_edge_index = ignore_edge;

			Edge new_chain = GetFirUndefinedChainIndex();

			for (size_t count = 0;; count++)
			{
				if (_boxes[checking_box_index].type() == state::box_type::CHAIN_BOX)//this box is part of chain.
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
								_chains[new_chain].set_type(chain_type::CHAIN);
								if (start_box != MAX_BOX)
								{
									if (_chains[_boxes[start_box].belonging_chain()].type() == chain_type::OPEN_CIRCLE)
									{
										_chains[new_chain].set_type(chain_type::OPEN_CHAIN);
									}
								}
								return;
							}
							break;
						}
					}
				}
				else if (_boxes[checking_box_index].type() == state::box_type::FREE_BOX)//to the end of chain.
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
					GADT_WARNING_CHECK(true, "wrong state in box " + console::I2S(checking_box_index));
				}
				GADT_WARNING_CHECK(count >= MAX_BOX, "infinite loop");
			}
		}

		//register a circle from a box.
		void ChainAnalyst::RegisterCircleFromBox(Edge start_box, Edge fir_box, Edge ignore_edge)
		{
			if (!(_boxes[fir_box].type() == state::box_type::CHAIN_BOX))
			{
				return;
			}

			Edge checking_box_index = fir_box;
			Edge ignoring_edge_index = ignore_edge;

			Edge new_chain = GetFirUndefinedChainIndex();
			for (size_t count = 0;; count++)
			{
				if (_boxes[checking_box_index].type() == state::box_type::CHAIN_BOX)
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
								GADT_WARNING_CHECK(!_boxes[checking_box_index].IsNotEmptyNeighbour(i), "empty box in circle");
								ignoring_edge_index = _boxes[checking_box_index].own_edge(i);
								checking_box_index = _boxes[checking_box_index].neighbour_box(i);
								break;
							}
						}
					}
				}
				else
				{
					GADT_WARNING_CHECK(true, "wrong state in box " + console::I2S(checking_box_index));
				}
				GADT_WARNING_CHECK(count >= MAX_BOX, "infinite loop");
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

		//get the first edge of appointed chain.
		Edge ChainAnalyst::GetFirstEdgeOfChain(size_t chain_index)
		{
			for (size_t i = 0; i < MAX_BOX; i++)
			{
				if (_boxes[i].belonging_chain() == chain_index)
				{
					for (size_t n = 0; n < 4; n++)
					{
						if (_boxes[i].IsNotEmptyNeighbour(n) && _boxes[_boxes[i].neighbour_box(n)].belonging_chain() == chain_index)
						{
							return _boxes[i].own_edge(n);
						}

					}

					GADT_WARNING_CHECK(true, "no find neighbour box that belong to same chain.");
				}
			}
			GADT_WARNING_CHECK(true, "no find box belong to chain " + console::I2S(chain_index));
			return MAX_EDGE;
		}

		//get result
		ActionVec ChainAnalyst::result()
		{
			ActionVec temp;
			for (size_t i = 0; i < MAX_CHAIN; i++)
			{
				if (_chains[i].type() == chain_type::CIRCLE || _chains[i].type() == chain_type::CHAIN || _chains[i].type() == chain_type::OPEN_CHAIN)
				{
					temp.set(GetFirstEdgeOfChain(i));
				}
			}
			GADT_WARNING_CHECK(temp.none(), "no any action exist in this board");
			return temp;
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
						console::Cprintf("�� ", console::ConsoleColor(_boxes[i].belonging_chain() + 1));
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
					console::Cprintf(ss.str(), console::ConsoleColor(i + 1));
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
	state_type::StateType Analyst::DetermindStateType(const Board& board)
	{
		if (board.to_ullong() == 0xFFFFFFFFFFFFFFF)
		{
			return state_type::FINISH;
		}
		if (state::ExistDeadBox(board))
		{
			return state_type::DEAD_BOX;
		}
		if (state::ExistDeadChain(board))
		{
			return state_type::DEAD_CHAIN;
		}
		if (state::ExistFreeEdge(board))//front state.
		{
			return state_type::FRONT;
		}
		return state_type::REAR;
	}

	//TODO Action Analyst
	ActionAnalyst::ActionAnalyst(Board board) :
		Analyst(board)
	{
		if (_state_type == state_type::DEAD_BOX)
		{
			_result = action_analyst::GetFirDeadBoxAction(_board);
		}
		else if (_state_type == state_type::FRONT)
		{
			_result = action_analyst::GetFreeActions(_board);
		}
		else if (_state_type == state_type::REAR)
		{
			_result = rear_state::ChainAnalyst(_board).result();
		}
		else if (_state_type == state_type::DEAD_CHAIN)
		{
			_result = action_analyst::GetDeadChainAction(_board);
		}
		else
		{
			_result = ActionVec();
		}
	}

	//Get random action in action analyst.
	Edge ActionAnalyst::RandomAction()
	{
		GADT_WARNING_CHECK(_result.none(), "no any action is legal.");
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
		const console::ConsoleColor edge_color = console::YELLOW;
		const console::ConsoleColor action_color = console::WHITE;
		const console::ConsoleColor dot_color = console::GRAY;
		const console::ConsoleColor tag_color = console::GREEN;
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
		if (_state_type == state_type::DEAD_BOX)
		{
		}
		else if (_state_type == state_type::FRONT)
		{
		}
		else if (_state_type == state_type::REAR)
		{

		}
		else if (_state_type == state_type::DEAD_CHAIN)
		{

		}
	}
}