#include "stdafx.h"
#include <iostream>
#include <vector>

#include "GameDefine.h"


using namespace std;

namespace dots_and_boxes
{
	State::State()
	{
		for (size_t i = 0; i < MAX_EDGE; i++)
		{
			_edge[i] = false;
		}
	}

	State::State(BitBoard bit_board)
	{
		for (Edge i = 0; i < MAX_EDGE; i++)
		{
			_edge[i] = board::EdgeExist(bit_board, i);
		}
	}

	void State::Visualization(ActionVec action_vec)
	{
		const console::color::Color edge_color = console::color::yellow;
		const console::color::Color action_color = console::color::white;
		const console::color::Color dot_color = console::color::gray;

		cout << endl;
		string interval = "     ";
		for (Edge y = 0; y < GAME_SIZE; y++)
		{
			//首先得到横边初始坐标
			cout << interval;
			for (Edge index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "○";
				console::Cprintf("○", dot_color);
				if (EdgeExist(index))
				{
					console::Cprintf("━", edge_color);
					//cout << "━";
				}
				else
				{
					if (action_vector::ActionExist(action_vec, index))
					{
						console::Cprintf("┈", action_color);
						//cout << "┈";
					}
					else
					{
						cout << "  ";
					}

				}
			}
			console::Cprintf("○\n", dot_color);
			//cout << "○" << endl;

			//然后得到纵边坐标
			cout << interval;
			for (Edge index = 34 - y; index < 59 - y; index += 5)
			{
				if (EdgeExist(index))
				{
					//cout << "┃";
					console::Cprintf("┃", edge_color);
					Edge upper_right_hor_edge = state::GetUpperRightHorEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					Edge right_vec_edge = index + 5;
					if (EdgeExist(upper_right_hor_edge) && EdgeExist(lower_right_hor_edge) && EdgeExist(right_vec_edge))
					{
						cout << "■";
					}
					else
					{
						cout << "  ";
					}
				}
				else
				{
					if (action_vector::ActionExist(action_vec, index))
					{
						//cout << "┊";
						console::Cprintf("┊", action_color);
					}
					else
					{
						cout << "  ";
					}
					cout << "  ";
				}
			}

			if (EdgeExist(59 - y))
			{
				//cout << "┃" << endl;
				console::Cprintf("┃\n", edge_color);
			}
			else
			{
				if (action_vector::ActionExist(action_vec, 59 - y))
				{
					//cout << "┊" << endl;
					console::Cprintf("┊\n", action_color);
				}
				else
				{
					cout << "  " << endl;
				}
			}
		}

		//打印最下面的一个边
		cout << interval;
		for (Edge index = 25; index < 30; index++)
		{
			//cout << "○";
			console::Cprintf("○", dot_color);
			if (EdgeExist(index))
			{
				//cout << "━";
				console::Cprintf("━", edge_color);
			}
			else
			{
				if (action_vector::ActionExist(action_vec, index))
				{
					//cout << "┈";
					console::Cprintf("┈", action_color);
				}
				else
				{
					cout << "  ";
				}
			}
		}
		//cout << "○" << endl;
		console::Cprintf("○\n\n", dot_color);
	}

	void State::ActionVisualization(ActionVec action_vec)
	{
		const console::color::Color edge_color = console::color::yellow;
		const console::color::Color action_color = console::color::white;
		const console::color::Color dot_color = console::color::gray;
		cout << endl;
		string interval = "     ";
		for (Edge y = 0; y < GAME_SIZE; y++)
		{
			//horizon edges, first grid.
			cout << interval;
			for (Edge index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "○";
				console::Cprintf("○", dot_color);
				if (EdgeExist(index))
				{
					console::Cprintf("━", edge_color);
					//cout << "━";
				}
				else
				{
					cout << "  ";
				}
			}
			console::Cprintf("○" + interval, dot_color);
			//cout << "○" << endl;

			//horizon edges, second grid.
			for (Edge index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "○";
				console::Cprintf("○", dot_color);
				if (EdgeExist(index))
				{
					console::Cprintf("━", edge_color);
					//cout << "━";
				}
				else
				{
					if (action_vector::ActionExist(action_vec, index))
					{
						console::Cprintf("┈", action_color);
						//cout << "┈";
					}
					else
					{
						cout << "  ";
					}

				}
			}
			console::Cprintf("○\n", dot_color);

			//vertical edges, first grid.
			cout << interval;
			for (Edge index = 34 - y; index < 59 - y; index += 5)
			{
				if (EdgeExist(index))
				{
					//cout << "┃";
					console::Cprintf("┃", edge_color);
					Edge upper_right_hor_edge = state::GetUpperRightHorEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					Edge right_vec_edge = index + 5;
					if (EdgeExist(upper_right_hor_edge) && EdgeExist(lower_right_hor_edge) && EdgeExist(right_vec_edge))
					{
						cout << "■";
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
			if (EdgeExist(59 - y))
			{
				//cout << "┃" << endl;
				console::Cprintf("┃" + interval, edge_color);
			}
			else
			{
				cout << "  " + interval;
			}

			//vertical edges, second grid.
			for (Edge index = 34 - y; index < 59 - y; index += 5)
			{
				if (EdgeExist(index))
				{
					//cout << "┃";
					console::Cprintf("┃", edge_color);
					Edge upper_right_hor_edge = state::GetUpperRightHorEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					Edge right_vec_edge = index + 5;
					if (EdgeExist(upper_right_hor_edge) && EdgeExist(lower_right_hor_edge) && EdgeExist(right_vec_edge))
					{
						cout << "■";
					}
					else
					{
						cout << "  ";
					}
				}
				else
				{
					if (action_vector::ActionExist(action_vec, index))
					{
						//cout << "┊";
						console::Cprintf("┊", action_color);
					}
					else
					{
						cout << "  ";
					}
					cout << "  ";
				}
			}



			if (EdgeExist(59 - y))
			{
				//cout << "┃" << endl;
				console::Cprintf("┃\n", edge_color);
			}
			else
			{
				if (action_vector::ActionExist(action_vec, 59 - y))
				{
					//cout << "┊" << endl;
					console::Cprintf("┊\n", action_color);
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
			//cout << "○";
			console::Cprintf("○", dot_color);
			if (EdgeExist(index))
			{
				//cout << "━";
				console::Cprintf("━", edge_color);
			}
			else
			{
				cout << "  ";
			}
		}
		//cout << "○" << endl;
		console::Cprintf("○" + interval, dot_color);

		//print lower horizon edges, second grid.
		for (Edge index = 25; index < 30; index++)
		{
			//cout << "○";
			console::Cprintf("○", dot_color);
			if (EdgeExist(index))
			{
				//cout << "━";
				console::Cprintf("━", edge_color);
			}
			else
			{
				if (action_vector::ActionExist(action_vec, index))
				{
					//cout << "┈";
					console::Cprintf("┈", action_color);
				}
				else
				{
					cout << "  ";
				}
			}
		}
		//cout << "○" << endl;
		console::Cprintf("○\n\n", dot_color);
	}

	void State::EdgeVisualization()
	{
		const console::color::Color edge_color = console::color::yellow;
		const console::color::Color action_color = console::color::white;
		const console::color::Color dot_color = console::color::gray;
		cout << endl;
		string interval = "     ";
		for (Edge y = 0; y < GAME_SIZE; y++)
		{
			//horizon edges, first grid.
			cout << interval;
			for (Edge index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "○";
				console::Cprintf("○", dot_color);
				if (EdgeExist(index))
				{
					console::Cprintf("━", edge_color);
					//cout << "━";
				}
				else
				{
					cout << "  ";
				}
			}
			console::Cprintf("○" + interval, dot_color);
			//cout << "○" << endl;

			//horizon edges, second grid.
			for (Edge index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "○";
				console::Cprintf("○", dot_color);
				if (EdgeExist(index))
				{
					console::Cprintf("━", edge_color);
					//cout << "━";
				}
				else
				{
					if (index > 9)
					{
						cout << index;
					}
					else
					{
						cout << 0 << index;
					}
				}
			}
			console::Cprintf("○\n", dot_color);

			//vertical edges, first grid.
			cout << interval;
			for (Edge index = 34 - y; index < 59 - y; index += 5)
			{
				if (EdgeExist(index))
				{
					//cout << "┃";
					console::Cprintf("┃", edge_color);
					Edge upper_right_hor_edge = state::GetUpperRightHorEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					Edge right_vec_edge = index + 5;
					if (EdgeExist(upper_right_hor_edge) && EdgeExist(lower_right_hor_edge) && EdgeExist(right_vec_edge))
					{
						cout << "■";
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
			if (EdgeExist(59 - y))
			{
				//cout << "┃" << endl;
				console::Cprintf("┃" + interval, edge_color);
			}
			else
			{
				cout << "  " + interval;
			}

			//vertical edges, second grid.
			for (Edge index = 34 - y; index < 59 - y; index += 5)
			{
				if (EdgeExist(index))
				{
					//cout << "┃";
					console::Cprintf("┃", edge_color);
					Edge upper_right_hor_edge = state::GetUpperRightHorEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					Edge right_vec_edge = index + 5;
					if (EdgeExist(upper_right_hor_edge) && EdgeExist(lower_right_hor_edge) && EdgeExist(right_vec_edge))
					{
						cout << "■";
					}
					else
					{
						cout << "  ";
					}
				}
				else
				{
					if (index > 9)
					{
						cout << index;
					}
					else
					{
						cout << 0 << index;
					}
					cout << "  ";
				}
			}



			if (EdgeExist(59 - y))
			{
				//cout << "┃" << endl;
				console::Cprintf("┃\n", edge_color);
			}
			else
			{
				cout << 59 - y << endl;
			}
		}

		//print lower horizon edges, first grid.
		cout << interval;
		for (Edge index = 25; index < 30; index++)
		{
			//cout << "○";
			console::Cprintf("○", dot_color);
			if (EdgeExist(index))
			{
				//cout << "━";
				console::Cprintf("━", edge_color);
			}
			else
			{
				cout << "  ";
			}
		}
		//cout << "○" << endl;
		console::Cprintf("○" + interval, dot_color);

		//print lower horizon edges, second grid.
		for (Edge index = 25; index < 30; index++)
		{
			//cout << "○";
			console::Cprintf("○", dot_color);
			if (EdgeExist(index))
			{
				//cout << "━";
				console::Cprintf("━", edge_color);
			}
			else
			{
				cout << index;
			}
		}
		//cout << "○" << endl;
		console::Cprintf("○\n\n", dot_color);
	}

	State State::RandomState(size_t edge_num)
	{
		BitBoard board = EMPTY_BOARD;
		vector<Edge> moves(60);
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
				board::EdgeSet(board, moves[rnd]);
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
		//得到某个局面下第一个DEAD BOX(已经被占领了三条边)的空边的编号。没有的话则返回MAX_EDGE。
		Edge GetDeadBoxRemainEdgeIndex(BitBoard board)
		{
			for (Edge hor_edge = 0; hor_edge < 25; hor_edge++)
			{
				Edge lower_hor_edge = hor_edge + 5;
				Edge lower_left_vec_edge = state::GetLowerLeftVecEdge(hor_edge);
				Edge lower_right_vec_edge = lower_left_vec_edge + 5;
				if ((board::EdgeExist(board, hor_edge) + board::EdgeExist(board, lower_hor_edge) + board::EdgeExist(board, lower_left_vec_edge) + board::EdgeExist(board, lower_right_vec_edge)) == 3)
				{
					if (!board::EdgeExist(board, hor_edge))
					{
						return hor_edge;
					}
					else if (!board::EdgeExist(board, lower_hor_edge))
					{
						return lower_hor_edge;
					}
					else if (!board::EdgeExist(board, lower_left_vec_edge))
					{
						return lower_left_vec_edge;
					}
					else if (!board::EdgeExist(board, lower_right_vec_edge))
					{
						return lower_right_vec_edge;
					}
				}
			}
			return MAX_EDGE;
		}

		//return true if any dead box exist in this state.
		bool ExistDeadBox(BitBoard board)
		{
			return GetDeadBoxRemainEdgeIndex(board) != MAX_EDGE;
		}

		//capture all possible box in this action.
		BitBoard CaptureAllBoxes(BitBoard board)
		{
			BitBoard temp = board;
			for (;;)
			{
				Edge edge = GetDeadBoxRemainEdgeIndex(temp);
				if (edge != MAX_EDGE)
				{
					board::EdgeSet(temp, edge);
				}
				else
				{
					break;
				}
			}
			return temp;
		}

		//get the minimal state of the box.
		BitBoard MinimalForm(BitBoard target)
		{

			BitBoard min = target;
			BitBoard prototype = target;
			BitBoard reversed = ReverseBox(min);
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

		//check whether exist dead chain in a board.
		bool ExistDeadChain(BitBoard board)
		{
			for (Edge fir_box_upper_edge = 0; fir_box_upper_edge < 25; fir_box_upper_edge++)
			{
				Edge fir_box_lower_edge = fir_box_upper_edge + 5;
				Edge fir_box_left_edge = GetLowerLeftVecEdge(fir_box_upper_edge);
				Edge fir_box_right_edge = fir_box_left_edge + 5;
				if ((board::EdgeExist(board, fir_box_upper_edge) + board::EdgeExist(board, fir_box_lower_edge) + board::EdgeExist(board, fir_box_left_edge) + board::EdgeExist(board, fir_box_right_edge)) == 3)
				{
					if (!board::EdgeExist(board, fir_box_upper_edge))
					{
						//check upper box
						if (IsNotUpperSideHorEdge(fir_box_upper_edge))
						{
							Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_upper_edge - 5);
							if (board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_left_edge + 5) + board::EdgeExist(board, fir_box_upper_edge - 5) == 2)
							{
								return true;
							}
						}
					}
					else if (!board::EdgeExist(board, fir_box_lower_edge))
					{
						//check lower box
						if (IsNotLowerSideHorEdge(fir_box_lower_edge))
						{
							Edge sec_box_left_edge = state::GetLowerLeftVecEdge(fir_box_lower_edge);
							if (board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_left_edge + 5) + board::EdgeExist(board, fir_box_lower_edge + 5) == 2)
							{
								return true;
							}
						}
					}
					else if (!board::EdgeExist(board, fir_box_left_edge))
					{
						//check left box
						if (IsNotLeftSideVecEdge(fir_box_left_edge))
						{
							Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge - 5);
							if (board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_upper_edge + 5) + board::EdgeExist(board, fir_box_left_edge - 5) == 2)
							{
								return true;
							}
						}
					}
					else if (!board::EdgeExist(board, fir_box_right_edge))
					{
						//check right box
						if (IsNotRightSideVecEdge(fir_box_right_edge))
						{
							Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_right_edge);
							if (board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_upper_edge + 5) + board::EdgeExist(board, fir_box_right_edge + 5) == 2)
							{
								return true;
							}
						}
					}
				}
			}
			return false;
		}

		//check whether exist free edge in a board.
		bool ExistFreeEdge(BitBoard board)
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

		//get not reasonable state
		bool IsReasonable(BitBoard board)
		{
			size_t dead_box = 0;
			size_t dead_chain = 0;
			Edge fir_dead_box_upper_edge = MAX_EDGE;
			for (Edge edge = 0; edge < 25; edge++)
			{
				Edge fir_box_lower_edge = edge + 5;
				Edge fir_box_left_edge = state::GetLowerLeftVecEdge(edge);
				Edge fir_box_right_edge = fir_box_left_edge + 5;
				if (board::EdgeExist(board, edge) + board::EdgeExist(board, fir_box_lower_edge) + board::EdgeExist(board, fir_box_left_edge) + board::EdgeExist(board, fir_box_right_edge) == 3)
				{
					if (board::EdgeExist(board, edge) == false)
					{
						if (IsNotUpperSideHorEdge(edge))
						{
							Edge sec_box_upper_edge = edge - 5;
							Edge sec_box_left_edge = GetLowerLeftVecEdge(edge - 5);
							Edge sec_box_right_edge = GetLowerLeftVecEdge(edge - 5) + 5;
							if (board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_right_edge) == 2)
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
					else if (board::EdgeExist(board, fir_box_lower_edge) == false)
					{
						if (IsNotLowerSideHorEdge(fir_box_lower_edge))
						{
							Edge sec_box_lower_edge = fir_box_lower_edge + 5;
							Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_lower_edge);
							Edge sec_box_right_edge = GetLowerLeftVecEdge(fir_box_lower_edge) + 5;
							if (board::EdgeExist(board, sec_box_lower_edge) + board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_right_edge) == 2)
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
					else if (board::EdgeExist(board, fir_box_left_edge) == false)
					{
						if (IsNotLeftSideVecEdge(fir_box_left_edge))
						{
							Edge sec_box_left_edge = fir_box_left_edge - 5;
							Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge - 5);
							Edge sec_box_lower_edge = GetUpperRightHorEdge(fir_box_left_edge - 5) + 5;
							if (board::EdgeExist(board, sec_box_lower_edge) + board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_left_edge) == 2)
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
					else if (board::EdgeExist(board, fir_box_right_edge) == false)
					{
						if (IsNotRightSideVecEdge(fir_box_right_edge))
						{
							Edge sec_box_right_edge = fir_box_right_edge + 5;
							Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_right_edge);
							Edge sec_box_lower_edge = GetUpperRightHorEdge(fir_box_right_edge) + 5;
							if (board::EdgeExist(board, sec_box_lower_edge) + board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_right_edge) == 2)
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
		bool IsFreeEdge(BitBoard board, Edge edge)
		{
			//Check horizon edges.
			if (IsHorEdge(edge))
			{
				if (board::EdgeExist(board, edge) == false)
				{
					//check the box below the edge.
					bool affect_upper_box = false;
					bool affect_lower_box = false;
					if (IsNotLowerSideHorEdge(edge))
					{
						Edge fir_box_lower_edge = edge + 5;
						Edge fir_box_left_edge = GetLowerLeftVecEdge(edge);
						Edge fir_box_right_edge = fir_box_left_edge + 5;

						int fir_box_edge_num = board::EdgeExist(board, fir_box_lower_edge) + board::EdgeExist(board, fir_box_left_edge) + board::EdgeExist(board, fir_box_right_edge);
						if (fir_box_edge_num == 2)
						{
							if (board::EdgeExist(board, fir_box_lower_edge) == false)
							{
								if (IsNotLowerSideHorEdge(fir_box_lower_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_lower_edge);
									if (board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_left_edge + 5) + board::EdgeExist(board, fir_box_lower_edge + 5) == 2)
									{
										affect_lower_box = true;
									}
								}
							}
							else if (board::EdgeExist(board, fir_box_left_edge) == false)
							{
								if (IsNotLeftSideVecEdge(fir_box_left_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge - 5);
									if (board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_upper_edge + 5) + board::EdgeExist(board, fir_box_left_edge - 5) == 2)
									{
										affect_lower_box = true;
									}
								}
							}
							else if (board::EdgeExist(board, fir_box_right_edge) == false)
							{
								if (IsNotRightSideVecEdge(fir_box_right_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_right_edge);
									if (board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_upper_edge + 5) + board::EdgeExist(board, fir_box_right_edge + 5) == 2)
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
						Edge fir_box_left_edge = GetLowerLeftVecEdge(fir_box_upper_edge);
						Edge fir_box_right_edge = fir_box_left_edge + 5;

						int fir_box_edge_num = board::EdgeExist(board, fir_box_upper_edge) + board::EdgeExist(board, fir_box_left_edge) + board::EdgeExist(board, fir_box_right_edge);
						if (fir_box_edge_num == 2)
						{
							if (board::EdgeExist(board, fir_box_upper_edge) == false)
							{
								if (IsNotUpperSideHorEdge(fir_box_upper_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_upper_edge - 5);
									if (board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_left_edge + 5) + board::EdgeExist(board, fir_box_upper_edge - 5) == 2)
									{
										affect_upper_box = true;
									}
								}
							}
							else if (board::EdgeExist(board, fir_box_left_edge) == false)
							{
								if (IsNotLeftSideVecEdge(fir_box_left_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge - 5);
									if (board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_upper_edge + 5) + board::EdgeExist(board, fir_box_left_edge - 5) == 2)
									{
										affect_upper_box = true;
									}
								}
							}
							else if (board::EdgeExist(board, fir_box_right_edge) == false)
							{
								if (IsNotRightSideVecEdge(fir_box_right_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_right_edge);
									if (board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_upper_edge + 5) + board::EdgeExist(board, fir_box_right_edge + 5) == 2)
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
				if (board::EdgeExist(board, edge) == false)
				{
					//check the box on the left of the edge.
					bool affect_left_box = false;
					bool affect_right_box = false;
					if (IsNotLeftSideVecEdge(edge))
					{
						Edge fir_box_left_edge = edge - 5;
						Edge fir_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge);
						Edge fir_box_lower_edge = fir_box_upper_edge + 5;

						int fir_box_edge_num = board::EdgeExist(board, fir_box_lower_edge) + board::EdgeExist(board, fir_box_upper_edge) + board::EdgeExist(board, fir_box_left_edge);
						if (fir_box_edge_num == 2)
						{
							if (board::EdgeExist(board, fir_box_left_edge) == false)
							{
								if (IsNotLeftSideVecEdge(fir_box_left_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge - 5);
									if (board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_upper_edge + 5) + board::EdgeExist(board, fir_box_left_edge - 5) == 2)
									{
										affect_left_box = true;
									}
								}
							}
							else if (board::EdgeExist(board, fir_box_upper_edge) == false)
							{
								if (IsNotUpperSideHorEdge(fir_box_upper_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_upper_edge - 5);
									if (board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_left_edge + 5) + board::EdgeExist(board, fir_box_upper_edge - 5) == 2)
									{
										affect_left_box = true;
									}
								}
							}
							else if (board::EdgeExist(board, fir_box_lower_edge) == false)
							{
								if (IsNotLowerSideHorEdge(fir_box_lower_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_lower_edge);
									if (board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_left_edge + 5) + board::EdgeExist(board, fir_box_lower_edge + 5) == 2)
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
						Edge fir_box_upper_edge = GetUpperRightHorEdge(edge);
						Edge fir_box_lower_edge = fir_box_upper_edge + 5;

						int fir_box_edge_num = board::EdgeExist(board, fir_box_lower_edge) + board::EdgeExist(board, fir_box_upper_edge) + board::EdgeExist(board, fir_box_right_edge);
						if (fir_box_edge_num == 2)
						{
							if (board::EdgeExist(board, fir_box_right_edge) == false)
							{
								if (IsNotRightSideVecEdge(fir_box_right_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_right_edge);
									if (board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_upper_edge + 5) + board::EdgeExist(board, fir_box_right_edge + 5) == 2)
									{
										affect_right_box = true;
									}
								}
							}
							else if (board::EdgeExist(board, fir_box_upper_edge) == false)
							{
								if (IsNotUpperSideHorEdge(fir_box_upper_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_upper_edge - 5);
									if (board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_left_edge + 5) + board::EdgeExist(board, fir_box_upper_edge - 5) == 2)
									{
										affect_right_box = true;
									}
								}
							}
							else if (board::EdgeExist(board, fir_box_lower_edge) == false)
							{
								if (IsNotLowerSideHorEdge(fir_box_lower_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_lower_edge);
									if (board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_left_edge + 5) + board::EdgeExist(board, fir_box_lower_edge + 5) == 2)
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

		//judge whether a edge is the upper edge of a box that is first box of a dead chain.
		bool IsUpperEdgeOfFirstBoxOfDeadChain(BitBoard board, Edge edge)
		{
			WARNING_CHECK(edge > 25, "wrong edge");

			Edge fir_box_lower_edge = edge + 5;
			Edge fir_box_left_edge = state::GetLowerLeftVecEdge(edge);
			Edge fir_box_right_edge = fir_box_left_edge + 5;
			if (board::EdgeExist(board, edge) + board::EdgeExist(board, fir_box_lower_edge) + board::EdgeExist(board, fir_box_left_edge) + board::EdgeExist(board, fir_box_right_edge) == 3)
			{
				if (board::EdgeExist(board, edge) == false)
				{
					if (IsNotUpperSideHorEdge(edge))
					{
						Edge sec_box_upper_edge = edge - 5;
						Edge sec_box_left_edge = GetLowerLeftVecEdge(edge - 5);
						Edge sec_box_right_edge = GetLowerLeftVecEdge(edge - 5) + 5;
						return (board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_right_edge) == 2);
					}
					return false;
				}
				else if (board::EdgeExist(board, fir_box_lower_edge) == false)
				{
					if (IsNotLowerSideHorEdge(fir_box_lower_edge))
					{
						Edge sec_box_lower_edge = fir_box_lower_edge + 5;
						Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_lower_edge);
						Edge sec_box_right_edge = GetLowerLeftVecEdge(fir_box_lower_edge) + 5;
						return (board::EdgeExist(board, sec_box_lower_edge) + board::EdgeExist(board, sec_box_left_edge) + board::EdgeExist(board, sec_box_right_edge) == 2);
					}
					return false;
				}
				else if (board::EdgeExist(board, fir_box_left_edge) == false)
				{
					if (IsNotLeftSideVecEdge(fir_box_left_edge))
					{
						Edge sec_box_left_edge = fir_box_left_edge - 5;
						Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge - 5);
						Edge sec_box_lower_edge = GetUpperRightHorEdge(fir_box_left_edge - 5) + 5;
						return (board::EdgeExist(board, sec_box_lower_edge) + board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_left_edge) == 2);
					}
					return false;
				}
				else if (board::EdgeExist(board, fir_box_right_edge) == false)
				{
					if (IsNotRightSideVecEdge(fir_box_right_edge))
					{
						Edge sec_box_right_edge = fir_box_right_edge + 5;
						Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_right_edge);
						Edge sec_box_lower_edge = GetUpperRightHorEdge(fir_box_right_edge) + 5;
						return (board::EdgeExist(board, sec_box_lower_edge) + board::EdgeExist(board, sec_box_upper_edge) + board::EdgeExist(board, sec_box_right_edge) == 2);
					}
					return false;
				}
			}
			return false;
		}

		//judge whether two edge is upper edge of two neighbour box.
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

	namespace game
	{
		GameState::GameState() :
			State(),
			_fir_player{ 0,nullptr,"MCTS" },
			_sec_player{ 0,nullptr,"MCTS" },
			_last_action(0)
		{

		}

		GameState::GameState(BitBoard board, Player fir_player, Player sec_player) :
			State(board),
			_fir_player(fir_player),
			_sec_player(sec_player),
			_last_action(0)
		{

		}
	}
}
