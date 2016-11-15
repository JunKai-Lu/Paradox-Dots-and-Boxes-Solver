#include "stdafx.h"
#include <iostream>
#include <string>
#include <stdlib.h>


#pragma  once

#define GAME_SIZE 5
#define MAX_EDGE 60
#define WARNING

#define EMPTY_BOARD 0

#define RED_BOX 1
#define BLUE_BOX -1
#define EMPTY_BOX 0

namespace DAB
{
	inline void Warning(std::string reason, std::string function)
	{
		std::cout << ">> WARNING: " << reason << " in function " << function << std::endl;
		system("pause");
	}

	typedef unsigned char Edge;
	typedef __int64 BitBoard;
	typedef __int64 ActionVec;
	typedef short Margin;

	//Data struct :bool[60]
	class State
	{
	private:
		bool _edge[MAX_EDGE];
	public:
		//生成空的棋盘
		State();	
		State(BitBoard bit_board);

		//显示棋盘
		void Visualization(ActionVec action_vec = 0);

		//取运算
		inline bool operator[](size_t index) const
		{
			#ifdef WARNING
				if (index >= MAX_EDGE || index < 0)
				{
					Warning("Wrong index", "State::operator[]");
				}
			#endif
			return _edge[index];
		}

		//正常取运算
		inline bool EdgeExist(size_t index) const 
		{
			return _edge[index];
		}

		//设置
		inline void EdgeSet(size_t index)
		{
			_edge[index] = true;
		}
	};

	namespace BOARD
	{
		//检查某一条边是否存在
		inline bool EdgeExist(const BitBoard& board, Edge index)
		{
		#ifdef WARNING
			if (index >= MAX_EDGE || index < 0)
			{
				Warning("Wrong index", "State::EdgeExist");
			}
		#endif
			return ((board >> index) & 0x1) == 1;
		}

		//设置某一条边
		inline void EdgeSet(BitBoard& board, Edge index)
		{
			#ifdef WARNING
				if (index >= MAX_EDGE || index < 0)
				{
					Warning("Wrong index", "State::EdgeSet");
				}
			#endif
			BitBoard temp = 1;
			temp = temp << index;
			board = board | temp;
		}

		inline void EdgeRemove(BitBoard& board, Edge index)
		{
#ifdef WARNING
			if (index >= MAX_EDGE || index < 0)
			{
				Warning("Wrong index", "State::EdgeRemove");
			}

			if (!BOARD::EdgeExist(board, index))
			{
				Warning("edge not exist", "State::EdgeRemove");
			}
#endif

			BitBoard temp = 1;
			temp = ~(temp << index);
			board = board & temp;
		}

		//打印
		inline void BitPrint(BitBoard board)
		{
			for (Edge i = MAX_EDGE - 1; i >= 0; i--)
			{
				std::cout << EdgeExist(board, i);
			}
			std::cout << std::endl;
		}

		//随机局面
		inline BitBoard RandomBitBoard()
		{
			BitBoard temp = 0;
			for (size_t i = 0; i < 60; i++)
			{
				temp |= ((rand() & 1)&0x1);//0或者1
				temp = temp << 1;
			}
			temp = temp >> 1;
			return temp;
		}

		
	}

	namespace ACTIONVEC
	{
		//创建一个动作容器
		inline ActionVec Create(bool edge[MAX_EDGE])
		{
			ActionVec temp = 0;
			for (int i = 0; i < MAX_EDGE; i++)
			{
				if (edge[i])
				{
					temp |= 0x1;
				}
				temp = temp << 1;
			}
			temp = temp >> 1;//多了一位。
			return temp;
		}

		//检查某个动作是否存在
		inline bool ActionExist(const BitBoard target, Edge index)
		{
			#ifdef WARNING
				if (index >= MAX_EDGE || index < 0)
				{
					Warning("Wrong index", "ACTIONVEC::ActionExist");
				}
			#endif
			return ((target >> index) & 0x1) == 1;
		}

		//给动作容器存入某一位
		inline void ActionSet(ActionVec& target, Edge index)
		{
			#ifdef WARNING
				if (index >= MAX_EDGE || index < 0)
				{
					Warning("Wrong index", "ACTIONVEC::ActionSet");
				}
			#endif
			BitBoard temp = 1;
			temp = temp << index;
			target = target | temp;
		}

		//打印
		inline void Print(ActionVec target)
		{
			for (Edge i = MAX_EDGE -1; i >= 0; i--)
			{
				std::cout << ActionExist(target, i);
			}
			std::cout << std::endl;
		}
	}

	namespace STATE
	{
		//得到某个横边左下的竖边的编号（限制为0~24）
		inline Edge GetLowerLeftVecEdge(Edge hor_edge)
		{
#ifdef WARNING
			if (hor_edge > 24 || hor_edge < 0)
			{
				Warning("Wrong index", "STATE::GetLowerLeftVecEdge");
			}
#endif
			return (34 - (hor_edge / 5)) + 5 * (hor_edge % 5);
		}

		//得到某个竖边右上的横边的编号（限制为30~54）
		inline Edge GetUpperRightHorEdge(Edge vec_edge)
		{
#ifdef WARNING
			if (vec_edge > 54 || vec_edge < 30)
			{
				Warning("Wrong index", "STATE::GetUpperRightHorEdge");
			}
#endif
			return (14 - 5 * (vec_edge % 5)) + (vec_edge / 5);
		}

		//判断某个边是否不是最上方的边
		inline bool IsNotUpperSideHorEdge(Edge index)
		{
			return index > 4;
		}

		//判断某个边是否不是最下方的边
		inline bool IsNotLowerSideHorEdge(Edge index)
		{
			return index < 25;
		}

		//判断某个边是否不是最左方的边
		inline bool IsNotLeftSideVecEdge(Edge index)
		{
			return index > 34;
		}

		//判断某个边是否不是最右方的边
		inline bool IsNotRightSideVecEdge(Edge index)
		{
			return index < 55;
		}

		//判断某个边是否是横边
		inline bool IsHorEdge(Edge index)
		{
			return index >= 0 && index < 30;
		}

		//判断某个边是否是竖边
		inline bool IsVecEdge(Edge index)
		{
			return index >= 30 && index < 60;
		}

		//Get the edge num of the box below a horizon edge.
		inline size_t GetLowerBoxEdgeNum(BitBoard board, Edge hor_edge)
		{
#ifdef WARNING
			if (hor_edge > 24)
			{
				Warning("Wrong index", "STATE::GetLowerBoxEdgeNum");
			}
#endif
			Edge lower_left_edge = GetLowerLeftVecEdge(hor_edge);
			return BOARD::EdgeExist(board, hor_edge) + BOARD::EdgeExist(board, lower_left_edge) + BOARD::EdgeExist(board, lower_left_edge + 5) + BOARD::EdgeExist(board, hor_edge + 5);
		}

		//得到某个局面下第一个DEAD BOX(已经被占领了三条边)的空边的编号。没有的话则返回MAX_EDGE。
		Edge GetDeadBoxRemainEdgeIndex(BitBoard board);

		//判断某个局面是否没有DEAD BOX
		bool ExistDeadBox(BitBoard board);

		//capture all possible box in this action.
		BitBoard CaptureAllBoxes(BitBoard board);

		//return whether how many full-box as the edge as their part.
		inline Margin TheNumOfFullBoxWithTheEdge(const BitBoard& board, Edge index)
		{
			
			if (!BOARD::EdgeExist(board, index))
			{
				return 0;
			}
			Margin num = 0;
			//for those horrizon edges
			if (IsHorEdge(index))
			{
				if (IsNotUpperSideHorEdge(index))
				{
					if (IsNotLowerSideHorEdge(index))
					{
						//middle side hor edges

						//check lower box.
						Edge lower_hor_edge = index + 5;
						Edge lower_left_vec_edge = GetLowerLeftVecEdge(index);
						Edge lower_right_vec_edge = lower_left_vec_edge + 5;
						if (BOARD::EdgeExist(board, lower_hor_edge) && BOARD::EdgeExist(board, lower_left_vec_edge) && BOARD::EdgeExist(board, lower_right_vec_edge))
						{
							num++;
						}
						
						//check upper box.
						Edge upper_hor_edge = index - 5;
						Edge upper_left_vec_edge = GetLowerLeftVecEdge(upper_hor_edge);
						Edge upper_right_vec_edge = upper_left_vec_edge + 5;
						if (BOARD::EdgeExist(board, upper_hor_edge) && BOARD::EdgeExist(board, upper_left_vec_edge) && BOARD::EdgeExist(board, upper_right_vec_edge))
						{
							num++;
						}

					}
					else
					{
						//lower side hor edges

						//check upper box.
						Edge upper_hor_edge = index - 5;
						Edge upper_left_vec_edge = GetLowerLeftVecEdge(upper_hor_edge);
						Edge upper_right_vec_edge = upper_left_vec_edge + 5;
						if (BOARD::EdgeExist(board, upper_hor_edge) && BOARD::EdgeExist(board, upper_left_vec_edge) && BOARD::EdgeExist(board, upper_right_vec_edge))
						{
							num++;
						}
					}
				}
				else
				{
					//upper side hor edges

					//check lower box.
					Edge lower_hor_edge = index + 5;
					Edge lower_left_vec_edge = GetLowerLeftVecEdge(index);
					Edge lower_right_vec_edge = lower_left_vec_edge + 5;
					if (BOARD::EdgeExist(board, lower_hor_edge) && BOARD::EdgeExist(board, lower_left_vec_edge) && BOARD::EdgeExist(board, lower_right_vec_edge))
					{
						num++;
					}
				}
				return num;
			}
			//for those vectical edges
			if (IsVecEdge(index))
			{
				if (IsNotLeftSideVecEdge(index))
				{
					if (IsNotRightSideVecEdge(index))
					{
						//middle side vec edges

						//check right box.
						Edge right_vec_edge = index + 5;
						Edge upper_right_hor_edge = GetUpperRightHorEdge(index);
						Edge lower_right_hor_edge = upper_right_hor_edge + 5;
						if (BOARD::EdgeExist(board, right_vec_edge) && BOARD::EdgeExist(board, upper_right_hor_edge) && BOARD::EdgeExist(board, lower_right_hor_edge))
						{
							num++;
						}

						//check left box.
						Edge left_vec_edge = index - 5;
						Edge upper_left_hor_edge = GetUpperRightHorEdge(left_vec_edge);
						Edge lower_left_hor_edge = upper_left_hor_edge + 5;
						if (BOARD::EdgeExist(board, left_vec_edge) && BOARD::EdgeExist(board, upper_left_hor_edge) && BOARD::EdgeExist(board, lower_left_hor_edge))
						{
							num++;
						}
					}
					else
					{
						//right side vec edges

						//check left box.
						Edge left_vec_edge = index - 5;
						Edge upper_left_hor_edge = GetUpperRightHorEdge(left_vec_edge);
						Edge lower_left_hor_edge = upper_left_hor_edge + 5;
						if (BOARD::EdgeExist(board, left_vec_edge) && BOARD::EdgeExist(board, upper_left_hor_edge) && BOARD::EdgeExist(board, lower_left_hor_edge))
						{
							num++;
						}
					}
				}
				else
				{
					//left side vec edges

					//check right box.
					Edge right_vec_edge = index + 5;
					Edge upper_right_hor_edge = GetUpperRightHorEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					if (BOARD::EdgeExist(board, right_vec_edge) && BOARD::EdgeExist(board, upper_right_hor_edge) && BOARD::EdgeExist(board, lower_right_hor_edge))
					{
						num++;
					}
				}
				return num;
			}
			return num;
		}

		//get the box that have been rotate for 90'
		inline BitBoard RotateBox(BitBoard board)
		{
			BitBoard temp = board & 0x3FFFFFFF;
			for (Edge i = 30; i < 60; i++)
			{
				temp = temp << 1;
				if (BOARD::EdgeExist(board, i))
				{
					temp = temp | 1;
				}
			}
			return temp;
		}

		//get the box that have been reversald.
		inline BitBoard ReverseBox(BitBoard board)
		{
			BitBoard temp = 0;
			for (Edge i = 0; i < 6; i++)
			{
				Edge begin = 55 - (i * 5);
				for (Edge n = 0; n < 5; n++)
				{
					temp = temp << 1;
					if (BOARD::EdgeExist(board, begin + n))
					{
						temp = temp | 1;
					}
				}
			}

			for (Edge i = 0; i < 6; i++)
			{
				BitBoard part = (board >> (i * 5)) & 0x1F;
				temp = (temp << 5) | part;
			}
			return temp;
		}

		//get the minimal state of the box.
		BitBoard MinimalForm(BitBoard board);

		//check whether exist dead chain in a board.
		bool ExistDeadChain(BitBoard board);
		
		//check whether exist free edge in a board.
		bool ExistFreeEdge(BitBoard board);

		//get not reasonable state
		bool IsReasonable(BitBoard board);
	}

}