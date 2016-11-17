#include "stdafx.h"
#include <iostream>
#include <vector>

#include "GameDefine.h"


using namespace std;

namespace DAB
{
	void Cprintf(string str, WORD color)
	{
		WORD colorOld;
		HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(handle, &csbi);
		colorOld = csbi.wAttributes;
		SetConsoleTextAttribute(handle, color);
		cout << str;
		SetConsoleTextAttribute(handle, colorOld);
		/*
		Cprintf("o", 1);//深蓝
		Cprintf("o", 2);//军绿
		Cprintf("o", 3);//暗青
		Cprintf("!", 4);//棕
		Cprintf("r", 5);//紫
		Cprintf("o", 6);//深黄
		Cprintf("o", 7);//暗白
		Cprintf("l", 8);//灰
		Cprintf("e", 9);//蓝色
		Cprintf("H", 10);//绿色
		Cprintf("l", 11);//青色
		Cprintf("l", 12);//红色
		Cprintf("o", 13);//粉色
		Cprintf("d", 14);//黄
		Cprintf("W", 15);//白
		
		
		
		
		
		*/
	}
	void CprintNum(int num, int color)
	{
		char str[4];
		sprintf_s(str, "%d", num);
		Cprintf(string(str), color);
	}

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
			_edge[i] = BOARD::EdgeExist(bit_board,i);
		}
	}

	void State::Visualization(ActionVec action_vec)
	{
		const WORD edge_color = 14;
		const WORD action_color = 15;
		const WORD dot_color = 8;
		for (int y = 0; y < GAME_SIZE; y++)
		{
			//首先得到横边初始坐标
			for (int index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "○";
				Cprintf("○", dot_color);
				if (EdgeExist(index))
				{
					Cprintf("━", edge_color);
					//cout << "━";
				}
				else
				{
					if (ACTIONVEC::ActionExist(action_vec,index))
					{
						Cprintf("┈", action_color);
						//cout << "┈";
					}
					else
					{
						cout << "  ";
					}
					
				}
			}
			Cprintf("○\n", dot_color);
			//cout << "○" << endl;

			//然后得到纵边坐标
			for (int index = 34 - y; index < 59 - y; index+=5)
			{
				if (EdgeExist(index))
				{
					//cout << "┃";
					Cprintf("┃", edge_color);
					int upper_right_hor_edge = (14 - 5 * (index % 5)) + (index / 5);
					int lower_right_hor_edge = upper_right_hor_edge + 5;
					int right_vec_edge = index + 5;
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
					if (ACTIONVEC::ActionExist(action_vec, index))
					{
						//cout << "┊";
						Cprintf("┊", action_color);
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
				Cprintf("┃\n", edge_color);
			}
			else
			{
				if (ACTIONVEC::ActionExist(action_vec, 59-y))
				{
					//cout << "┊" << endl;
					Cprintf("┊\n", action_color);
				}
				else
				{
					cout << "  " << endl;
				}
			}
		}

		//打印最下面的一个边
		for (int index = 25; index < 30; index++)
		{
			//cout << "○";
			Cprintf("○", dot_color);
			if (EdgeExist(index))
			{
				//cout << "━";
				Cprintf("━", edge_color);
			}
			else
			{
				if (ACTIONVEC::ActionExist(action_vec, index))
				{
					//cout << "┈";
					Cprintf("┈", action_color);
				}
				else
				{
					cout << "  ";
				}
			}
		}
		//cout << "○" << endl;
		Cprintf("○\n", dot_color);
	}

	void State::ActionVisualization(ActionVec action_vec)

	{
		const WORD edge_color = 14;
		const WORD action_color = 15;
		const WORD dot_color = 8;
		cout << endl;
		string interval = "     ";
		for (int y = 0; y < GAME_SIZE; y++)
		{
			//horizon edges, first grid.
			cout << interval;
			for (int index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "○";
				Cprintf("○", dot_color);
				if (EdgeExist(index))
				{
					Cprintf("━", edge_color);
					//cout << "━";
				}
				else
				{	
					cout << "  ";
				}
			}
			Cprintf("○" + interval, dot_color);
			//cout << "○" << endl;

			//horizon edges, second grid.
			for (int index = y * 5; index < (y * 5) + 5; index++)
			{
				//cout << "○";
				Cprintf("○", dot_color);
				if (EdgeExist(index))
				{
					Cprintf("━", edge_color);
					//cout << "━";
				}
				else
				{
					if (ACTIONVEC::ActionExist(action_vec, index))
					{
						Cprintf("┈", action_color);
						//cout << "┈";
					}
					else
					{
						cout << "  ";
					}

				}
			}
			Cprintf("○\n", dot_color);

			//vertical edges, first grid.
			cout << interval;
			for (int index = 34 - y; index < 59 - y; index += 5)
			{
				if (EdgeExist(index))
				{
					//cout << "┃";
					Cprintf("┃", edge_color);
					int upper_right_hor_edge = (14 - 5 * (index % 5)) + (index / 5);
					int lower_right_hor_edge = upper_right_hor_edge + 5;
					int right_vec_edge = index + 5;
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
				Cprintf("┃" + interval, edge_color);
			}
			else
			{
				cout << "  " + interval;
			}

			//vertical edges, second grid.
			for (int index = 34 - y; index < 59 - y; index += 5)
			{
				if (EdgeExist(index))
				{
					//cout << "┃";
					Cprintf("┃", edge_color);
					int upper_right_hor_edge = (14 - 5 * (index % 5)) + (index / 5);
					int lower_right_hor_edge = upper_right_hor_edge + 5;
					int right_vec_edge = index + 5;
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
					if (ACTIONVEC::ActionExist(action_vec, index))
					{
						//cout << "┊";
						Cprintf("┊", action_color);
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
				Cprintf("┃\n", edge_color);
			}
			else
			{
				if (ACTIONVEC::ActionExist(action_vec, 59 - y))
				{
					//cout << "┊" << endl;
					Cprintf("┊\n", action_color);
				}
				else
				{
					cout << "  " << endl;
				}
			}
		}

		//print lower horizon edges, first grid.
		cout << interval;
		for (int index = 25; index < 30; index++)
		{
			//cout << "○";
			Cprintf("○", dot_color);
			if (EdgeExist(index))
			{
				//cout << "━";
				Cprintf("━", edge_color);
			}
			else
			{	
				cout << "  ";
			}
		}
		//cout << "○" << endl;
		Cprintf("○" + interval, dot_color);

		//print lower horizon edges, second grid.
		for (int index = 25; index < 30; index++)
		{
			//cout << "○";
			Cprintf("○", dot_color);
			if (EdgeExist(index))
			{
				//cout << "━";
				Cprintf("━", edge_color);
			}
			else
			{
				if (ACTIONVEC::ActionExist(action_vec, index))
				{
					//cout << "┈";
					Cprintf("┈", action_color);
				}
				else
				{
					cout << "  ";
				}
			}
		}
		//cout << "○" << endl;
		Cprintf("○\n\n", dot_color);
	}

	State State::RandomState(size_t edge_num)
	{
		BitBoard board = EMPTY_BOARD;
		vector<Edge> moves(60);
		for (Edge i = 0; i < edge_num; i++)
		{
			for (Edge edge = 0; edge < MAX_EDGE; edge++)
			{
				if (STATE::IsFreeEdge(board, edge))
				{
					moves.push_back(edge);
				}
			}
			if (moves.size() != 0)
			{
				size_t rnd = rand() % moves.size();
				BOARD::EdgeSet(board, moves[rnd]);
				moves.clear();
			}
			else
			{
				break;
			}
		}
		return State(board);
	}

	namespace STATE
	{
		//得到某个局面下第一个DEAD BOX(已经被占领了三条边)的空边的编号。没有的话则返回MAX_EDGE。
		Edge GetDeadBoxRemainEdgeIndex(BitBoard board)
		{
			for (Edge hor_edge = 0; hor_edge < 25; hor_edge++)
			{
				Edge lower_hor_edge = hor_edge + 5;
				Edge lower_left_vec_edge = STATE::GetLowerLeftVecEdge(hor_edge);
				Edge lower_right_vec_edge = lower_left_vec_edge + 5;
				if ((BOARD::EdgeExist(board, hor_edge) + BOARD::EdgeExist(board, lower_hor_edge) + BOARD::EdgeExist(board, lower_left_vec_edge) + BOARD::EdgeExist(board, lower_right_vec_edge)) == 3)
				{
					if (!BOARD::EdgeExist(board, hor_edge))
					{
						return hor_edge;
					}
					else if (!BOARD::EdgeExist(board, lower_hor_edge))
					{
						return lower_hor_edge;
					}
					else if (!BOARD::EdgeExist(board, lower_left_vec_edge))
					{
						return lower_left_vec_edge;
					}
					else if (!BOARD::EdgeExist(board, lower_right_vec_edge))
					{
						return lower_right_vec_edge;
					}
				}
			}
			return MAX_EDGE;
		}
		
		//判断某个局面是否没有DEAD BOX
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
					BOARD::EdgeSet(temp,edge);
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
				if (prototype < min)
				{
					min = prototype;
				}

				reversed = RotateBox(reversed);
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
				if ((BOARD::EdgeExist(board, fir_box_upper_edge) + BOARD::EdgeExist(board, fir_box_lower_edge) + BOARD::EdgeExist(board, fir_box_left_edge) + BOARD::EdgeExist(board, fir_box_right_edge)) == 3)
				{
					if (!BOARD::EdgeExist(board, fir_box_upper_edge))
					{
						//check upper box
						if (IsNotUpperSideHorEdge(fir_box_upper_edge))
						{
							Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_upper_edge - 5);
							if (BOARD::EdgeExist(board, sec_box_left_edge) + BOARD::EdgeExist(board, sec_box_left_edge + 5) + BOARD::EdgeExist(board, fir_box_upper_edge - 5) == 2)
							{
								return true;
							}
						}
					}
					else if (!BOARD::EdgeExist(board, fir_box_lower_edge))
					{
						//check lower box
						if (IsNotLowerSideHorEdge(fir_box_lower_edge))
						{
							Edge sec_box_left_edge = STATE::GetLowerLeftVecEdge(fir_box_lower_edge);
							if (BOARD::EdgeExist(board, sec_box_left_edge) + BOARD::EdgeExist(board, sec_box_left_edge + 5) + BOARD::EdgeExist(board, fir_box_lower_edge + 5) == 2)
							{
								return true;
							}
						}
					}
					else if (!BOARD::EdgeExist(board, fir_box_left_edge))
					{
						//check left box
						if (IsNotLeftSideVecEdge(fir_box_left_edge))
						{
							Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge - 5);
							if (BOARD::EdgeExist(board, sec_box_upper_edge) + BOARD::EdgeExist(board, sec_box_upper_edge + 5) + BOARD::EdgeExist(board, fir_box_left_edge - 5) == 2)
							{
								return true;
							}
						}
					}
					else if (!BOARD::EdgeExist(board, fir_box_right_edge))
					{
						//check right box
						if (IsNotRightSideVecEdge(fir_box_right_edge))
						{
							Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_right_edge);
							if (BOARD::EdgeExist(board, sec_box_upper_edge) + BOARD::EdgeExist(board, sec_box_upper_edge + 5) + BOARD::EdgeExist(board, fir_box_right_edge + 5) == 2)
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
			if (ExistDeadBox(board) && ExistFreeEdge(board))
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
				if (BOARD::EdgeExist(board, edge) == false)
				{
					//check the box below the edge.
					bool affect_upper_box = false;
					bool affect_lower_box = false;
					if (IsNotLowerSideHorEdge(edge))
					{
						Edge fir_box_lower_edge = edge + 5;
						Edge fir_box_left_edge = GetLowerLeftVecEdge(edge);
						Edge fir_box_right_edge = fir_box_left_edge + 5;
						if (BOARD::EdgeExist(board, fir_box_lower_edge) + BOARD::EdgeExist(board, fir_box_left_edge) + BOARD::EdgeExist(board, fir_box_right_edge) == 2)
						{
							if (BOARD::EdgeExist(board, fir_box_lower_edge) == false)
							{
								if (IsNotLowerSideHorEdge(fir_box_lower_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_lower_edge);
									if (BOARD::EdgeExist(board, sec_box_left_edge) + BOARD::EdgeExist(board, sec_box_left_edge + 5) + BOARD::EdgeExist(board, fir_box_lower_edge + 5) == 2)
									{
										affect_lower_box = true;
									}
								}
							}
							else if (BOARD::EdgeExist(board, fir_box_left_edge) == false)
							{
								if (IsNotLeftSideVecEdge(fir_box_left_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge - 5);
									if (BOARD::EdgeExist(board, sec_box_upper_edge) + BOARD::EdgeExist(board, sec_box_upper_edge + 5) + BOARD::EdgeExist(board, fir_box_left_edge - 5) == 2)
									{
										affect_lower_box = true;
									}
								}
							}
							else if (BOARD::EdgeExist(board, fir_box_right_edge) == false)
							{
								if (IsNotRightSideVecEdge(fir_box_right_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_right_edge);
									if (BOARD::EdgeExist(board, sec_box_upper_edge) + BOARD::EdgeExist(board, sec_box_upper_edge + 5) + BOARD::EdgeExist(board, fir_box_right_edge + 5) == 2)
									{
										affect_lower_box = true;
									}
								}
							}
						}
					}
					//check the box above the edge
					if (IsNotUpperSideHorEdge(edge))
					{
						Edge fir_box_upper_edge = edge - 5;
						Edge fir_box_left_edge = GetLowerLeftVecEdge(fir_box_upper_edge);
						Edge fir_box_right_edge = fir_box_left_edge + 5;
						if (BOARD::EdgeExist(board, fir_box_upper_edge) + BOARD::EdgeExist(board, fir_box_left_edge) + BOARD::EdgeExist(board, fir_box_right_edge) == 2)
						{
							if (BOARD::EdgeExist(board, fir_box_upper_edge) == false)
							{
								if (IsNotUpperSideHorEdge(fir_box_upper_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_upper_edge - 5);
									if (BOARD::EdgeExist(board, sec_box_left_edge) + BOARD::EdgeExist(board, sec_box_left_edge + 5) + BOARD::EdgeExist(board, fir_box_upper_edge - 5) == 2)
									{
										affect_upper_box = true;
									}
								}
							}
							else if (BOARD::EdgeExist(board, fir_box_left_edge) == false)
							{
								if (IsNotLeftSideVecEdge(fir_box_left_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge - 5);
									if (BOARD::EdgeExist(board, sec_box_upper_edge) + BOARD::EdgeExist(board, sec_box_upper_edge + 5) + BOARD::EdgeExist(board, fir_box_left_edge - 5) == 2)
									{
										affect_upper_box = true;
									}
								}
							}
							else if (BOARD::EdgeExist(board, fir_box_right_edge) == false)
							{
								if (IsNotRightSideVecEdge(fir_box_right_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_right_edge);
									if (BOARD::EdgeExist(board, sec_box_upper_edge) + BOARD::EdgeExist(board, sec_box_upper_edge + 5) + BOARD::EdgeExist(board, fir_box_right_edge + 5) == 2)
									{
										affect_upper_box = true;
									}
								}
							}
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
				if (BOARD::EdgeExist(board, edge) == false)
				{
					//check the box on the left of the edge.
					bool affect_left_box = false;
					bool affect_right_box = false;
					if (IsNotLeftSideVecEdge(edge))
					{
						Edge fir_box_left_edge = edge - 5;
						Edge fir_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge);
						Edge fir_box_lower_edge = fir_box_upper_edge + 5;
						if (BOARD::EdgeExist(board, fir_box_lower_edge) + BOARD::EdgeExist(board, fir_box_upper_edge) + BOARD::EdgeExist(board, fir_box_left_edge) == 2)
						{
							if (BOARD::EdgeExist(board, fir_box_left_edge) == false)
							{
								if (IsNotLeftSideVecEdge(fir_box_left_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_left_edge - 5);
									if (BOARD::EdgeExist(board, sec_box_upper_edge) + BOARD::EdgeExist(board, sec_box_upper_edge + 5) + BOARD::EdgeExist(board, fir_box_left_edge - 5) == 2)
									{
										affect_left_box = true;
									}
								}
							}
							else if (BOARD::EdgeExist(board, fir_box_upper_edge) == false)
							{
								if (IsNotUpperSideHorEdge(fir_box_upper_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_upper_edge - 5);
									if (BOARD::EdgeExist(board, sec_box_left_edge) + BOARD::EdgeExist(board, sec_box_left_edge + 5) + BOARD::EdgeExist(board, fir_box_upper_edge - 5) == 2)
									{
										affect_left_box = true;
									}
								}
							}
							else if (BOARD::EdgeExist(board, fir_box_lower_edge) == false)
							{
								if (IsNotLowerSideHorEdge(fir_box_lower_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_lower_edge);
									if (BOARD::EdgeExist(board, sec_box_left_edge) + BOARD::EdgeExist(board, sec_box_left_edge + 5) + BOARD::EdgeExist(board, fir_box_lower_edge + 5) == 2)
									{
										affect_left_box = true;
									}
								}
							}
						}
					}

					//check the box on the right of the edge.
					if (IsNotRightSideVecEdge(edge))
					{
						Edge fir_box_right_edge = edge + 5;
						Edge fir_box_upper_edge = GetUpperRightHorEdge(edge);
						Edge fir_box_lower_edge = fir_box_upper_edge + 5;
						if (BOARD::EdgeExist(board, fir_box_lower_edge) + BOARD::EdgeExist(board, fir_box_upper_edge) + BOARD::EdgeExist(board, fir_box_right_edge) == 2)
						{
							if (BOARD::EdgeExist(board, fir_box_right_edge) == false)
							{
								if (IsNotRightSideVecEdge(fir_box_right_edge))
								{
									Edge sec_box_upper_edge = GetUpperRightHorEdge(fir_box_right_edge);
									if (BOARD::EdgeExist(board, sec_box_upper_edge) + BOARD::EdgeExist(board, sec_box_upper_edge + 5) + BOARD::EdgeExist(board, fir_box_right_edge + 5) == 2)
									{
										affect_right_box = true;
									}
								}
							}
							else if (BOARD::EdgeExist(board, fir_box_upper_edge) == false)
							{
								if (IsNotUpperSideHorEdge(fir_box_upper_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_upper_edge - 5);
									if (BOARD::EdgeExist(board, sec_box_left_edge) + BOARD::EdgeExist(board, sec_box_left_edge + 5) + BOARD::EdgeExist(board, fir_box_upper_edge - 5) == 2)
									{
										affect_right_box = true;
									}
								}
							}
							else if (BOARD::EdgeExist(board, fir_box_lower_edge) == false)
							{
								if (IsNotLowerSideHorEdge(fir_box_lower_edge))
								{
									Edge sec_box_left_edge = GetLowerLeftVecEdge(fir_box_lower_edge);
									if (BOARD::EdgeExist(board, sec_box_left_edge) + BOARD::EdgeExist(board, sec_box_left_edge + 5) + BOARD::EdgeExist(board, fir_box_lower_edge + 5) == 2)
									{
										affect_right_box = true;
									}
								}
							}
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
	}
}
