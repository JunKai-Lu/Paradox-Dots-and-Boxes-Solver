#include "GameDefine.h"
#include "BasicDefine.h"

namespace dots_and_boxes
{
	State::State(Board board) :
		_board(board)
	{
	}

	void State::Print(ActionVec action_vec) const
	{
		using namespace gadt::console;
		const ConsoleColor edge_color = ConsoleColor::Yellow;
		const ConsoleColor action_color = ConsoleColor::White;
		const ConsoleColor dot_color = ConsoleColor::Gray;
		PrintEndLine();
		std::string interval = "     ";
		for (Edge y = 0; y < GAME_SIZE; y++)
		{
			//首先得到横边初始坐标
			std::cout << interval;
			for (Edge index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "○";
				Cprintf("+", dot_color);
				if (_board.get(index))
				{
					Cprintf("---", edge_color);
					//cout << "━";
				}
				else
				{
					if (action_vec.get(index))
					{
						Cprintf("---", action_color);
						//cout << "┈";
					}
					else
					{
						std::cout << "   ";
					}

				}
			}
			Cprintf("+\n", dot_color);
			//cout << "○" << endl;

			//然后得到纵边坐标
			std::cout << interval;
			for (Edge index = 34 - y; index < 59 - y; index += 5)
			{
				if (_board.get(index))
				{
					//cout << "┃";
					Cprintf("|", edge_color);
					Edge upper_right_hor_edge = state::LeftToUpperEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					Edge right_vec_edge = index + 5;
					if (_board.get(upper_right_hor_edge) && _board.get(lower_right_hor_edge) && _board.get(right_vec_edge))
					{
						std::cout << " X ";
					}
					else
					{
						std::cout << "   ";
					}
				}
				else
				{
					if (action_vec.get(index))
					{
						//cout << "┊";
						Cprintf("|", action_color);
					}
					else
					{
						std::cout << " ";
					}
					std::cout << "   ";
				}
			}

			if (_board.get(59 - y))
			{
				//cout << "┃" << endl;
				Cprintf("|\n", edge_color);
			}
			else
			{
				if (action_vec.get(59 - y))
				{
					//cout << "┊" << endl;
					Cprintf("|\n", action_color);
				}
				else
				{
					std::cout << " " << std::endl;
				}
			}
		}

		//打印最下面的一个边
		std::cout << interval;
		for (Edge index = 25; index < 30; index++)
		{
			//cout << "○";
			Cprintf("+", dot_color);
			if (_board.get(index))
			{
				//cout << "━";
				Cprintf("---", edge_color);
			}
			else
			{
				if (action_vec.get(index))
				{
					//cout << "┈";
					Cprintf("---", action_color);
				}
				else
				{
					std::cout << "   ";
				}
			}
		}
		//cout << "○" << endl;
		Cprintf("+", dot_color);
		PrintEndLine<2>();
	}

	void State::EdgePrint() const
	{
		using namespace gadt::console;
		const ConsoleColor edge_color = ConsoleColor::Yellow;
		const ConsoleColor action_color = ConsoleColor::Red;
		const ConsoleColor dot_color = ConsoleColor::Gray;
		const ConsoleColor box_color = ConsoleColor::Blue;

		PrintEndLine();
		std::string interval = "     ";
		for (Edge y = 0; y < GAME_SIZE; y++)
		{
			//horizon edges, first grid.
			std::cout << interval;
			for (Edge index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "○";
				Cprintf("+", dot_color);
				if (_board.get(index))
				{
					Cprintf("---", edge_color);
					//cout << "━";
				}
				else
				{
					std::cout << "   ";
				}
			}
			Cprintf("+" + interval, dot_color);
			//cout << "○" << endl;

			//horizon edges, second grid.
			for (Edge index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "○";
				Cprintf("+ ", dot_color);
				if (_board.get(index))
				{
					Cprintf("--", edge_color);
					//cout << "━";
				}
				else
				{
					if (index > 9)
					{
						std::cout << (size_t)index;
					}
					else
					{
						std::cout << 0 << (size_t)index;
					}
				}
			}
			Cprintf("+ \n", dot_color);

			//vertical edges, first grid.
			std::cout << interval;
			for (Edge index = 34 - y; index < 59 - y; index += 5)
			{
				if (_board.get(index))
				{
					//cout << "┃";
					Cprintf("|", edge_color);
					Edge upper_right_hor_edge = state::LeftToUpperEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					Edge right_vec_edge = index + 5;
					if (_board.get(upper_right_hor_edge) && _board.get(lower_right_hor_edge) && _board.get(right_vec_edge))
					{
						Cprintf(" X ", box_color);
					}
					else
					{
						std::cout << "   ";
					}
				}
				else
				{
					std::cout << "    ";
				}
			}
			if (_board.get(59 - y))
			{
				//cout << "┃" << endl;
				Cprintf("|" + interval, edge_color);
			}
			else
			{
				std::cout << " " + interval;
			}

			//vertical edges, second grid.
			for (Edge index = 34 - y; index < 59 - y; index += 5)
			{
				if (_board.get(index))
				{
					//cout << "┃";
					Cprintf("| ", edge_color);
					Edge upper_right_hor_edge = state::LeftToUpperEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					Edge right_vec_edge = index + 5;
					if (_board.get(upper_right_hor_edge) && _board.get(lower_right_hor_edge) && _board.get(right_vec_edge))
					{
						Cprintf("X ", box_color);
					}
					else
					{
						std::cout << "  ";
					}
				}
				else
				{
					if (index > 9)
					{
						std::cout << (size_t)index;
					}
					else
					{
						std::cout << 0 << (size_t)index;
					}
					std::cout << "  ";
				}
			}

			if (_board.get(59 - y))
			{
				//cout << "┃" << endl;
				Cprintf("|\n", edge_color);
			}
			else
			{
				std::cout << 59 - y << std::endl;
			}
		}

		//print lower horizon edges, first grid.
		std::cout << interval;
		for (Edge index = 25; index < 30; index++)
		{
			//cout << "○";
			Cprintf("+", dot_color);
			if (_board.get(index))
			{
				//cout << "━";
				Cprintf("---", edge_color);
			}
			else
			{
				std::cout << "   ";
			}
		}
		//cout << "○" << endl;
		Cprintf("+" + interval, dot_color);

		//print lower horizon edges, second grid.
		for (Edge index = 25; index < 30; index++)
		{
			//cout << "○";
			Cprintf("+ ", dot_color);
			if (_board.get(index))
			{
				//cout << "━";
				Cprintf("--", edge_color);
			}
			else
			{
				std::cout << (size_t)index;
			}
		}
		//cout << "○" << endl;
		Cprintf("+", dot_color);
		PrintEndLine<2>();
	}

	State State::RandomState(size_t edge_num)
	{
		GADT_WARNING_IF(g_DAB_DEFINE_WARNING, edge_num >= MAX_EDGE, "out of range");
		Board board;
		std::vector<Edge> moves(60);
		for (Edge i = 0; i < edge_num; i++)
		{
			for (Edge edge = 0; edge < MAX_EDGE; edge++)
			{
				if (state::IsFreeEdge(board, edge))
				{
					moves.push_back(edge);
				}
			}
			if (moves.size() != 0)
			{
				size_t rnd = rand() % moves.size();
				board.set(moves[rnd]);
				moves.clear();
			}
			else
			{
				break;
			}
		}
		return State(board);
	}

	namespace state
	{
		//class box contains the information of a box in a board.
		Box::Box(const Board& board, Edge index) :
			_index(index),
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

		//get the free edge of first dead-box.
		Edge GetFirstFreeEdgeInDeadBox(const Board & board)
		{
			for (Edge fir_box_i = 0; fir_box_i < MAX_BOX; fir_box_i++)
			{
				Box fir_box(board, fir_box_i);
				if (fir_box.type() == box_type::DEAD_BOX)
				{
					//the second box is not CHAIN-BOX or ( second box is CHAIN-BOX and trd box is DEAD-BOX)
					for (size_t f = 0; f < 4; f++)
					{
						if (!board.get(fir_box.own_edge(f)))
						{
							//now f is the next box.
							if (fir_box.IsNotEmptyNeighbour(f))
							{
								Edge fir_sec = fir_box.own_edge(f);
								Box sec_box(board, fir_box.neighbour_box(f));
								if (sec_box.type() != box_type::CHAIN_BOX)
								{
									return fir_sec;
								}
								else
								{
									//find trd box. return true if its type is dead box.
									for (size_t s = 0; s < 4; s++)
									{
										if (!board.get(sec_box.own_edge(s)) && sec_box.own_edge(s) != fir_sec && sec_box.IsNotEmptyNeighbour(s))
										{
											Box trd_box(board, sec_box.neighbour_box(s));
											if (trd_box.type() == box_type::DEAD_BOX)
											{
												return fir_sec;
											}
										}
									}
								}
							}
							else
							{
								//the box is in the border and open to outside.so this is a dead box.
								return fir_box.own_edge(f);
							}
						}
					}
				}
			}
			return MAX_EDGE;
		}

		//get the first free edge in dead-chain.
		Edge GetFirstFreeEdgeInDeadChain(const Board & board)
		{
			//dead-chain means its first box is dead-box and second box is chain-box and trd box is not dead-box. 
			for (Edge i = 0; i < MAX_BOX; i++)
			{
				Box fir_box(board, i);
				if (fir_box.type() == box_type::DEAD_BOX)
				{
					for (size_t n = 0; n < 4; n++)
					{
						if (!board.get(fir_box.own_edge(n)) && fir_box.IsNotEmptyNeighbour(n))
						{
							//get the sec box.
							Edge edge_bet_fir_and_sec = fir_box.own_edge(n);
							Box sec_box(board, fir_box.neighbour_box(n));
							if (sec_box.type() == box_type::CHAIN_BOX)
							{
								//notice: the next box can not be DEAD-BOX!
								for (size_t e = 0; e < 4; e++)
								{
									if (!board.get(sec_box.own_edge(e)) && sec_box.own_edge(e) != edge_bet_fir_and_sec)
									{
										if (sec_box.IsNotEmptyNeighbour(e))
										{
											Box trd_box(board, sec_box.neighbour_box(e));
											if (trd_box.type() != box_type::DEAD_BOX)
											{
												return edge_bet_fir_and_sec;
											}
										}
										else
										{
											return edge_bet_fir_and_sec;
										}
									}
								}
							}
						}
					}
				}
			}
			return MAX_EDGE;
		}

		//return true if any dead box exist in this state.
		bool ExistDeadBox(const Board& board)
		{
			return GetFirstFreeEdgeInDeadBox(board) != MAX_EDGE;
		}

		//check whether exist dead chain in a board.
		bool ExistDeadChain(const Board& board)
		{
			return GetFirstFreeEdgeInDeadChain(board) != MAX_EDGE;
		}

		//check whether exist free edge in a board.
		bool ExistFreeEdge(const Board& board)
		{
			for (Edge edge = 0; edge < MAX_EDGE; edge++)
			{
				if (IsFreeEdge(board, edge))
				{
					return true;
				}
			}
			return false;
		}

		//get the minimal state of the box.
		Board MinimalForm(const Board& target)
		{
			Board min = target;
			Board prototype = target;
			Board reversed = ReverseBox(min);
			if (reversed < min)
			{
				min = reversed;
			}
			for (size_t i = 0; i < 3; i++)
			{
				prototype = RotateBox(prototype);
				ChangeBoxCorner(prototype);
				if (prototype < min)
				{
					min = prototype;
				}

				reversed = RotateBox(reversed);
				ChangeBoxCorner(reversed);
				if (reversed < min)
				{
					min = reversed;
				}
			}
			return min;
		}

		//get not reasonable state
		bool IsReasonable(const Board& board)
		{
			size_t dead_box = 0;
			size_t dead_chain = 0;
			Edge fir_dead_box_upper_edge = MAX_EDGE;
			for (Edge edge = 0; edge < 25; edge++)
			{
				Edge fir_box_lower_edge = edge + 5;
				Edge fir_box_left_edge = state::UpperToLeftEdge(edge);
				Edge fir_box_right_edge = fir_box_left_edge + 5;
				if (board.get(edge) + board.get(fir_box_lower_edge) + board.get(fir_box_left_edge) + board.get(fir_box_right_edge) == 3)
				{
					if (board.get(edge) == false)
					{
						if (IsNotUpperSideHorEdge(edge))
						{
							Edge sec_box_upper_edge = edge - 5;
							Edge sec_box_left_edge = UpperToLeftEdge(edge - 5);
							Edge sec_box_right_edge = UpperToLeftEdge(edge - 5) + 5;
							if (board.get(sec_box_upper_edge) + board.get(sec_box_left_edge) + board.get(sec_box_right_edge) == 2)
							{
								dead_chain++;
								if (dead_chain > 2)
								{
									return false;
								}
							}
							else
							{
								dead_box++;
								if (dead_box == 1)
								{
									fir_dead_box_upper_edge = edge;
								}
								else if (dead_box == 2)
								{
									if (IsUpperEdgeOfNeighbourBox(edge, fir_dead_box_upper_edge) == false)
									{
										return false;
									}
								}
								else
								{
									return false;
								}
							}
						}
						else
						{
							dead_box++;
							if (dead_box == 1)
							{
								fir_dead_box_upper_edge = edge;
							}
							else if (dead_box == 2)
							{
								if (IsUpperEdgeOfNeighbourBox(edge, fir_dead_box_upper_edge) == false)
								{
									return false;
								}
							}
							else
							{
								return false;
							}
						}
					}
					else if (board.get(fir_box_lower_edge) == false)
					{
						if (IsNotLowerSideHorEdge(fir_box_lower_edge))
						{
							Edge sec_box_lower_edge = fir_box_lower_edge + 5;
							Edge sec_box_left_edge = UpperToLeftEdge(fir_box_lower_edge);
							Edge sec_box_right_edge = UpperToLeftEdge(fir_box_lower_edge) + 5;
							if (board.get(sec_box_lower_edge) + board.get(sec_box_left_edge) + board.get(sec_box_right_edge) == 2)
							{
								dead_chain++;
								if (dead_chain > 2)
								{
									return false;
								}
							}
							else
							{
								dead_box++;
								if (dead_box == 1)
								{
									fir_dead_box_upper_edge = edge;
								}
								else if (dead_box == 2)
								{
									if (IsUpperEdgeOfNeighbourBox(edge, fir_dead_box_upper_edge) == false)
									{
										return false;
									}
								}
								else
								{
									return false;
								}
							}
						}
						else
						{
							dead_box++;
							if (dead_box == 1)
							{
								fir_dead_box_upper_edge = edge;
							}
							else if (dead_box == 2)
							{
								if (IsUpperEdgeOfNeighbourBox(edge, fir_dead_box_upper_edge) == false)
								{
									return false;
								}
							}
							else
							{
								return false;
							}
						}
					}
					else if (board.get(fir_box_left_edge) == false)
					{
						if (IsNotLeftSideVecEdge(fir_box_left_edge))
						{
							Edge sec_box_left_edge = fir_box_left_edge - 5;
							Edge sec_box_upper_edge = LeftToUpperEdge(fir_box_left_edge - 5);
							Edge sec_box_lower_edge = LeftToUpperEdge(fir_box_left_edge - 5) + 5;
							if (board.get(sec_box_lower_edge) + board.get(sec_box_upper_edge) + board.get(sec_box_left_edge) == 2)
							{
								dead_chain++;
								if (dead_chain > 2)
								{
									return false;
								}
							}
							else
							{
								dead_box++;
								if (dead_box == 1)
								{
									fir_dead_box_upper_edge = edge;
								}
								else if (dead_box == 2)
								{
									if (IsUpperEdgeOfNeighbourBox(edge, fir_dead_box_upper_edge) == false)
									{
										return false;
									}
								}
								else
								{
									return false;
								}
							}
						}
						else
						{
							dead_box++;
							if (dead_box == 1)
							{
								fir_dead_box_upper_edge = edge;
							}
							else if (dead_box == 2)
							{
								if (IsUpperEdgeOfNeighbourBox(edge, fir_dead_box_upper_edge) == false)
								{
									return false;
								}
							}
							else
							{
								return false;
							}
						}
					}
					else if (board.get(fir_box_right_edge) == false)
					{
						if (IsNotRightSideVecEdge(fir_box_right_edge))
						{
							Edge sec_box_right_edge = fir_box_right_edge + 5;
							Edge sec_box_upper_edge = LeftToUpperEdge(fir_box_right_edge);
							Edge sec_box_lower_edge = LeftToUpperEdge(fir_box_right_edge) + 5;
							if (board.get(sec_box_lower_edge) + board.get(sec_box_upper_edge) + board.get(sec_box_right_edge) == 2)
							{
								dead_chain++;
								if (dead_chain > 2)
								{
									return false;
								}
							}
							else
							{
								dead_box++;
								if (dead_box == 1)
								{
									fir_dead_box_upper_edge = edge;
								}
								else if (dead_box == 2)
								{
									if (IsUpperEdgeOfNeighbourBox(edge, fir_dead_box_upper_edge) == false)
									{
										return false;
									}
								}
								else
								{
									return false;
								}
							}
						}
						else
						{
							dead_box++;
							if (dead_box == 1)
							{
								fir_dead_box_upper_edge = edge;
							}
							else if (dead_box == 2)
							{
								if (IsUpperEdgeOfNeighbourBox(edge, fir_dead_box_upper_edge) == false)
								{
									return false;
								}
							}
							else
							{
								return false;
							}
						}
					}
				}
			}

			//if dead chain and dead box coexist, this is a unreasonable state.
			if (dead_chain > 0 && dead_box > 0)
			{
				return false;
			}
			if (dead_chain > 0 && ExistFreeEdge(board))
			{
				return false;
			}
			return true;
		}

		//judge whether a edge in a board is a free-edge.
		bool IsFreeEdge(const Board& board, Edge edge)
		{
			//Check horizon edges.
			if (IsHorEdge(edge))
			{
				if (board.get(edge) == false)
				{
					//check the box below the edge.
					bool affect_upper_box = false;
					bool affect_lower_box = false;
					if (IsNotLowerSideHorEdge(edge))
					{
						Edge fir_box_lower_edge = edge + 5;
						Edge fir_box_left_edge = UpperToLeftEdge(edge);
						Edge fir_box_right_edge = fir_box_left_edge + 5;

						int fir_box_edge_num = board.get(fir_box_lower_edge) + board.get(fir_box_left_edge) + board.get(fir_box_right_edge);
						if (fir_box_edge_num == 2)
						{
							if (board.get(fir_box_lower_edge) == false)
							{
								if (IsNotLowerSideHorEdge(fir_box_lower_edge))
								{
									Edge sec_box_left_edge = UpperToLeftEdge(fir_box_lower_edge);
									if (board.get(sec_box_left_edge) + board.get(sec_box_left_edge + 5) + board.get(fir_box_lower_edge + 5) == 2)
									{
										affect_lower_box = true;
									}
								}
							}
							else if (board.get(fir_box_left_edge) == false)
							{
								if (IsNotLeftSideVecEdge(fir_box_left_edge))
								{
									Edge sec_box_upper_edge = LeftToUpperEdge(fir_box_left_edge - 5);
									if (board.get(sec_box_upper_edge) + board.get(sec_box_upper_edge + 5) + board.get(fir_box_left_edge - 5) == 2)
									{
										affect_lower_box = true;
									}
								}
							}
							else if (board.get(fir_box_right_edge) == false)
							{
								if (IsNotRightSideVecEdge(fir_box_right_edge))
								{
									Edge sec_box_upper_edge = LeftToUpperEdge(fir_box_right_edge);
									if (board.get(sec_box_upper_edge) + board.get(sec_box_upper_edge + 5) + board.get(fir_box_right_edge + 5) == 2)
									{
										affect_lower_box = true;
									}
								}
							}
						}
						else if (fir_box_edge_num == 3)
						{
							//dead box.
							return false;
						}

					}
					//check the box above the edge
					if (IsNotUpperSideHorEdge(edge))
					{
						Edge fir_box_upper_edge = edge - 5;
						Edge fir_box_left_edge = UpperToLeftEdge(fir_box_upper_edge);
						Edge fir_box_right_edge = fir_box_left_edge + 5;

						int fir_box_edge_num = board.get(fir_box_upper_edge) + board.get(fir_box_left_edge) + board.get(fir_box_right_edge);
						if (fir_box_edge_num == 2)
						{
							if (board.get(fir_box_upper_edge) == false)
							{
								if (IsNotUpperSideHorEdge(fir_box_upper_edge))
								{
									Edge sec_box_left_edge = UpperToLeftEdge(fir_box_upper_edge - 5);
									if (board.get(sec_box_left_edge) + board.get(sec_box_left_edge + 5) + board.get(fir_box_upper_edge - 5) == 2)
									{
										affect_upper_box = true;
									}
								}
							}
							else if (board.get(fir_box_left_edge) == false)
							{
								if (IsNotLeftSideVecEdge(fir_box_left_edge))
								{
									Edge sec_box_upper_edge = LeftToUpperEdge(fir_box_left_edge - 5);
									if (board.get(sec_box_upper_edge) + board.get(sec_box_upper_edge + 5) + board.get(fir_box_left_edge - 5) == 2)
									{
										affect_upper_box = true;
									}
								}
							}
							else if (board.get(fir_box_right_edge) == false)
							{
								if (IsNotRightSideVecEdge(fir_box_right_edge))
								{
									Edge sec_box_upper_edge = LeftToUpperEdge(fir_box_right_edge);
									if (board.get(sec_box_upper_edge) + board.get(sec_box_upper_edge + 5) + board.get(fir_box_right_edge + 5) == 2)
									{
										affect_upper_box = true;
									}
								}
							}
						}
						else if (fir_box_edge_num == 3)
						{
							//dead box.
							return false;
						}
					}
					if (affect_upper_box || affect_lower_box)
					{
						return false;
					}
					else
					{
						return true;
					}
				}
			}
			else if (IsVecEdge(edge))
			{
				if (board.get(edge) == false)
				{
					//check the box on the left of the edge.
					bool affect_left_box = false;
					bool affect_right_box = false;
					if (IsNotLeftSideVecEdge(edge))
					{
						Edge fir_box_left_edge = edge - 5;
						Edge fir_box_upper_edge = LeftToUpperEdge(fir_box_left_edge);
						Edge fir_box_lower_edge = fir_box_upper_edge + 5;

						int fir_box_edge_num = board.get(fir_box_lower_edge) + board.get(fir_box_upper_edge) + board.get(fir_box_left_edge);
						if (fir_box_edge_num == 2)
						{
							if (board.get(fir_box_left_edge) == false)
							{
								if (IsNotLeftSideVecEdge(fir_box_left_edge))
								{
									Edge sec_box_upper_edge = LeftToUpperEdge(fir_box_left_edge - 5);
									if (board.get(sec_box_upper_edge) + board.get(sec_box_upper_edge + 5) + board.get(fir_box_left_edge - 5) == 2)
									{
										affect_left_box = true;
									}
								}
							}
							else if (board.get(fir_box_upper_edge) == false)
							{
								if (IsNotUpperSideHorEdge(fir_box_upper_edge))
								{
									Edge sec_box_left_edge = UpperToLeftEdge(fir_box_upper_edge - 5);
									if (board.get(sec_box_left_edge) + board.get(sec_box_left_edge + 5) + board.get(fir_box_upper_edge - 5) == 2)
									{
										affect_left_box = true;
									}
								}
							}
							else if (board.get(fir_box_lower_edge) == false)
							{
								if (IsNotLowerSideHorEdge(fir_box_lower_edge))
								{
									Edge sec_box_left_edge = UpperToLeftEdge(fir_box_lower_edge);
									if (board.get(sec_box_left_edge) + board.get(sec_box_left_edge + 5) + board.get(fir_box_lower_edge + 5) == 2)
									{
										affect_left_box = true;
									}
								}
							}
						}
						else if (fir_box_edge_num == 3)
						{
							//dead box.
							return false;
						}
					}

					//check the box on the right of the edge.
					if (IsNotRightSideVecEdge(edge))
					{
						Edge fir_box_right_edge = edge + 5;
						Edge fir_box_upper_edge = LeftToUpperEdge(edge);
						Edge fir_box_lower_edge = fir_box_upper_edge + 5;

						int fir_box_edge_num = board.get(fir_box_lower_edge) + board.get(fir_box_upper_edge) + board.get(fir_box_right_edge);
						if (fir_box_edge_num == 2)
						{
							if (board.get(fir_box_right_edge) == false)
							{
								if (IsNotRightSideVecEdge(fir_box_right_edge))
								{
									Edge sec_box_upper_edge = LeftToUpperEdge(fir_box_right_edge);
									if (board.get(sec_box_upper_edge) + board.get(sec_box_upper_edge + 5) + board.get(fir_box_right_edge + 5) == 2)
									{
										affect_right_box = true;
									}
								}
							}
							else if (board.get(fir_box_upper_edge) == false)
							{
								if (IsNotUpperSideHorEdge(fir_box_upper_edge))
								{
									Edge sec_box_left_edge = UpperToLeftEdge(fir_box_upper_edge - 5);
									if (board.get(sec_box_left_edge) + board.get(sec_box_left_edge + 5) + board.get(fir_box_upper_edge - 5) == 2)
									{
										affect_right_box = true;
									}
								}
							}
							else if (board.get(fir_box_lower_edge) == false)
							{
								if (IsNotLowerSideHorEdge(fir_box_lower_edge))
								{
									Edge sec_box_left_edge = UpperToLeftEdge(fir_box_lower_edge);
									if (board.get(sec_box_left_edge) + board.get(sec_box_left_edge + 5) + board.get(fir_box_lower_edge + 5) == 2)
									{
										affect_right_box = true;
									}
								}
							}
						}
						else if (fir_box_edge_num == 3)
						{
							//dead box.
							return false;
						}
					}
					if (affect_left_box || affect_right_box)
					{
						return false;
					}
					else
					{
						return true;
					}

				}
			}

			return false;
		}

		//return true if a edge is the upper edge of a box that is the first box of a dead chain.
		bool IsUpperEdgeOfFirstBoxOfDeadChain(const Board& board, Edge edge)
		{
			GADT_WARNING_IF(g_DAB_DEFINE_WARNING, edge > 25, "wrong edge");

			Edge fir_box_lower_edge = edge + 5;
			Edge fir_box_left_edge = state::UpperToLeftEdge(edge);
			Edge fir_box_right_edge = fir_box_left_edge + 5;
			if (board.get(edge) + board.get(fir_box_lower_edge) + board.get(fir_box_left_edge) + board.get(fir_box_right_edge) == 3)
			{
				if (board.get(edge) == false)
				{
					if (IsNotUpperSideHorEdge(edge))
					{
						Edge sec_box_upper_edge = edge - 5;
						Edge sec_box_left_edge = UpperToLeftEdge(edge - 5);
						Edge sec_box_right_edge = UpperToLeftEdge(edge - 5) + 5;
						return (board.get(sec_box_upper_edge) + board.get(sec_box_left_edge) + board.get(sec_box_right_edge) == 2);
					}
					return false;
				}
				else if (board.get(fir_box_lower_edge) == false)
				{
					if (IsNotLowerSideHorEdge(fir_box_lower_edge))
					{
						Edge sec_box_lower_edge = fir_box_lower_edge + 5;
						Edge sec_box_left_edge = UpperToLeftEdge(fir_box_lower_edge);
						Edge sec_box_right_edge = UpperToLeftEdge(fir_box_lower_edge) + 5;
						return (board.get(sec_box_lower_edge) + board.get(sec_box_left_edge) + board.get(sec_box_right_edge) == 2);
					}
					return false;
				}
				else if (board.get(fir_box_left_edge) == false)
				{
					if (IsNotLeftSideVecEdge(fir_box_left_edge))
					{
						Edge sec_box_left_edge = fir_box_left_edge - 5;
						Edge sec_box_upper_edge = LeftToUpperEdge(fir_box_left_edge - 5);
						Edge sec_box_lower_edge = LeftToUpperEdge(fir_box_left_edge - 5) + 5;
						return (board.get(sec_box_lower_edge) + board.get(sec_box_upper_edge) + board.get(sec_box_left_edge) == 2);
					}
					return false;
				}
				else if (board.get(fir_box_right_edge) == false)
				{
					if (IsNotRightSideVecEdge(fir_box_right_edge))
					{
						Edge sec_box_right_edge = fir_box_right_edge + 5;
						Edge sec_box_upper_edge = LeftToUpperEdge(fir_box_right_edge);
						Edge sec_box_lower_edge = LeftToUpperEdge(fir_box_right_edge) + 5;
						return (board.get(sec_box_lower_edge) + board.get(sec_box_upper_edge) + board.get(sec_box_right_edge) == 2);
					}
					return false;
				}
			}
			return false;
		}

		//reture if those edge are the upper edge of two neighbour box respectively.
		bool IsUpperEdgeOfNeighbourBox(Edge a, Edge b)
		{
			//not left side box.
			if (a % 5 != 0)
			{
				if (a - 1 == b)
				{
					return true;
				}
			}

			//not right side box.
			if (a % 5 != 4)
			{
				if (a + 1 == b)
				{
					return true;
				}
			}

			//not upper side box.
			if (a >= 5)
			{
				if (a - 5 == b)
				{
					return true;
				}
			}

			//not lower side box.
			if (a < 20)
			{
				if (a + 5 == b)
				{
					return true;
				}
			}

			return false;
		}

	}
}



