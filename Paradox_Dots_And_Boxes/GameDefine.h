#include "stdafx.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <Windows.h>
#include <vector>

#pragma  once

#define GAME_SIZE 5
#define MAX_EDGE 60
#define MAX_BOX 25
#define MAX_CHAIN 10
#define WARNING

#define EMPTY_BOARD 0

#define RED_BOX 1
#define BLUE_BOX -1
#define EMPTY_BOX 0

namespace DAB
{
	//colorful print.
	void Cprintf(std::string str, WORD color);
	void CprintNum(int num, int color);

	/*output message.
	inline void WarningCheck(std::string reason, std::string function)
	{
		Cprintf(">> WARNING: ", 5);
		Cprintf(reason, 12);
		Cprintf(" in function ", 8);
		Cprintf(function + "\n", 7);
		system("pause");
	}*/

	//output msg.
	inline void Error(std::string reason)
	{
		Cprintf(">> ERROR: ", 5);
		Cprintf(reason, 14);
		std::cout << std::endl;
	}
	inline void Message(std::string message,bool show_MSG = true)
	{
		std::cout << ">> ";
		if (show_MSG)
		{
			Cprintf("MSG: ", 2);
		}
		Cprintf(message, 10);
		std::cout << std::endl << std::endl;
	}
	inline void InputTip(std::string tip)
	{
		std::cout << ">> ";
		Cprintf(tip, 8);
		std::cout << std::endl << ">>> ";
	}
	inline void WarningCheck(bool condition, std::string reason, std::string function_name)
	{
#ifdef WARNING
		if (condition)
		{
			Cprintf(">> WARNING: ", 5);
			Cprintf(reason, 12);
			Cprintf(" in function ", 8);
			Cprintf(function_name + "\n", 7);
			system("pause");
		}
#endif
	}
	inline std::string GetInput()
	{
		char buffer[50];
		std::cin.getline(buffer, 50);
		return std::string(buffer);
	}
	inline std::string B2S(bool b)
	{
		if (b)
		{
			return "true";
		}
		return "false";
	}
	inline std::string I2S(size_t i)
	{
		std::stringstream ss;
		ss << i;
		return ss.str();
	}
	
	//type define.
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
		//create a empty state.
		State();

		//create a state by bit board.
		State(BitBoard bit_board);

		//Show State
		void Visualization(ActionVec action_vec = 0);
		void ActionVisualization(ActionVec action_vec = 0);
		void EdgeVisualization();

		//operator[]
		inline bool operator[](Edge index) const
		{
			WarningCheck(index >= MAX_EDGE || index < 0,"Wrong index", "State::operator[]");
			return _edge[index];
		}

		//get whether a edge exist in this state.
		inline bool EdgeExist(Edge index) const
		{

			WarningCheck(index >= MAX_EDGE,"Wrong index", "State::EdgeExist");
			return _edge[index];
		}

		//set edge.
		inline void EdgeSet(Edge index)
		{
			WarningCheck(index >= MAX_EDGE, "Wrong index", "State::EdgeSet");
			_edge[index] = true;
		}

		//remove edge.
		inline void EdgeRemove(Edge index)
		{
			WarningCheck(index >= MAX_EDGE,"Wrong index", "State::EdgeRemove");
			_edge[index] = false;
		}

		//create a random state with appointed edge number.
		static State RandomState(size_t edge_num);
	};

	

	namespace BOARD
	{
		//get whether a edge exist in this bit board.
		inline bool EdgeExist(const BitBoard& board, Edge index)
		{
			WarningCheck(index >= MAX_EDGE || index < 0,"Wrong index", "BOARD::EdgeExist");
			return ((board >> index) & 0x1) == 1;
		}

		//set edge in bit board.
		inline void EdgeSet(BitBoard& board, Edge index)
		{
			WarningCheck(index >= MAX_EDGE || index < 0,"Wrong index", "BOARD::EdgeSet");
			BitBoard temp = 1;
			temp = temp << index;
			board = board | temp;
		}

		//remove edge in bit board.
		inline void EdgeRemove(BitBoard& board, Edge index)
		{
			WarningCheck(index >= MAX_EDGE || index < 0,"Wrong index", "BOARD::EdgeRemove");
			WarningCheck(!BOARD::EdgeExist(board, index),"edge not exist", "BOARD::EdgeRemove");
			BitBoard temp = 1;
			temp = ~(temp << index);
			board = board & temp;
		}

		//print bit board.
		inline void BitPrint(BitBoard board)
		{
			for (Edge i = MAX_EDGE - 1; i >= 0 && i <= MAX_EDGE; i--)
			{
				std::cout << EdgeExist(board, i);
			}
			std::cout << std::endl;
		}

		//get a random bit board.
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

		//create a bit board by a State.
		inline BitBoard Create(State& state)
		{
			BitBoard temp = 0;
			for (Edge i = 59; i >= 0 && i < 60; i--)
			{
				temp = temp << 1;
				if (state[i])
				{
					temp = temp | 0x1;
				}
			}
			return temp;
		}
	}

	namespace ACTIONVEC
	{
		//create a action vector.
		inline ActionVec Create(bool edge[MAX_EDGE])
		{
			ActionVec temp = 0;
			for (Edge i = 59; i >= 0 && i < 60; i--)
			{
				temp = temp << 1;
				if (edge[i])
				{
					temp = temp | 0x1;
				}
			}
			return temp;
		}

		//get whether a action exist in this action vec.
		inline bool ActionExist(const BitBoard target, Edge index)
		{
			WarningCheck(index >= MAX_EDGE || index < 0,"Wrong index", "ACTIONVEC::ActionExist");
			return ((target >> index) & 0x1) == 1;
		}

		//set action in action vector.
		inline void ActionSet(ActionVec& target, Edge index)
		{
			WarningCheck(index >= MAX_EDGE || index < 0, "Wrong index", "ACTIONVEC::ActionSet");
			BitBoard temp = 1;
			temp = temp << index;
			target = target | temp;
		}

		//print action vector.
		inline void Print(ActionVec target)
		{
			for (Edge i = MAX_EDGE -1; i >= 0 && i <= MAX_EDGE; i--)
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
			WarningCheck(hor_edge > 24 || hor_edge < 0,"Wrong index", "STATE::GetLowerLeftVecEdge");
			return (34 - (hor_edge / 5)) + 5 * (hor_edge % 5);
		}

		//得到某个竖边右上的横边的编号（限制为30~54）
		inline Edge GetUpperRightHorEdge(Edge vec_edge)
		{
			WarningCheck(vec_edge > 54 || vec_edge < 30, "Wrong index", "STATE::GetUpperRightHorEdge");
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
			WarningCheck(hor_edge > 24, "Wrong index", "STATE::GetLowerBoxEdgeNum");
			Edge lower_left_edge = GetLowerLeftVecEdge(hor_edge);
			return BOARD::EdgeExist(board, hor_edge) + BOARD::EdgeExist(board, lower_left_edge) + BOARD::EdgeExist(board, lower_left_edge + 5) + BOARD::EdgeExist(board, hor_edge + 5);
		}

		//得到某个局面下第一个DEAD BOX(已经被占领了三条边)的空边的编号。没有的话则返回MAX_EDGE.
		Edge GetDeadBoxRemainEdgeIndex(BitBoard board);

		//judge whether any dead box exists in a board.
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

		//minimize state by change corner
		inline void ChangeBoxCorner(BitBoard& board)
		{
			if ((board & 0x400000001) == 0x400000000)
			{
				BOARD::EdgeRemove(board, 34);
				BOARD::EdgeSet(board, 0);
			}

			if ((board & 0x800000000000010) == 0x800000000000000)
			{
				BOARD::EdgeRemove(board, 59);
				BOARD::EdgeSet(board, 4);
			}

			if ((board & 0x42000000) == 0x40000000)
			{
				BOARD::EdgeRemove(board, 30);
				BOARD::EdgeSet(board, 25);
			}

			if ((board & 0x80000020000000) == 0x80000000000000)
			{
				BOARD::EdgeRemove(board, 55);
				BOARD::EdgeSet(board, 29);
			}
		}

		//get the box that have been rotate for 90'.
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

		//get not reasonable state.
		bool IsReasonable(BitBoard board);

		//judge whether a edge in a board is a free-edge.
		bool IsFreeEdge(BitBoard board, Edge edge);

		//judge whether a edge is the upper edge of a box that is first box of a dead chain.
		bool IsUpperEdgeOfFirstBoxOfDeadChain(BitBoard board, Edge edge);

		//judge whether two edge is upper edge of two neighbour box.
		bool IsUpperEdgeOfNeighbourBox(Edge a, Edge b);
	}

	

	namespace GAME
	{
		typedef std::vector<Edge> ActionDes;

		struct Player
		{
			size_t _score;
			ActionDes(*_func)(State&);
			std::string _des;
		};

		class GameState :public State
		{
		private:
			Player _fir_player;
			Player _sec_player;
			bool _auto_game;

			Edge _last_action;
			bool _last_player;

		public:
			GameState();
			GameState(BitBoard board, Player fir_player, Player sec_player);

			//variable
			inline Margin get_margin()
			{
				return Margin(_fir_player._score - _sec_player._score);
			}
			inline const Player& player(bool is_sec_player)
			{
				if (is_sec_player)
				{
					return _sec_player;
				}
				else
				{
					return _fir_player;
				}
			}
			inline const Player& operator[](bool is_sec_player)
			{
				return player(is_sec_player);
			}
			inline Edge last_action()
			{
				return _last_action;
			}
			inline bool last_player()
			{
				return _last_player;
			}
			inline void set_player(bool is_sec_action, Player player)
			{
				if (is_sec_action)
				{
					_sec_player = player;
				}
				else
				{
					_fir_player = player;
				}
			}
			inline void set_auto_game(bool auto_game)
			{
				_auto_game = auto_game;
			}

			//func
			inline void GameEdgeRemove(bool is_sec_player, Edge edge)
			{
				if (is_sec_player)
				{
					_sec_player._score -= STATE::TheNumOfFullBoxWithTheEdge(BOARD::Create(*this), edge);
				}
				else
				{
					_fir_player._score -= STATE::TheNumOfFullBoxWithTheEdge(BOARD::Create(*this), edge);
				}
				EdgeRemove(edge);
			}
			inline void GameEdgeSet(bool is_sec_player, Edge edge)
			{
				EdgeSet(edge);
				_last_action = edge;
				_last_player = is_sec_player;
				if (is_sec_player)
				{
					_sec_player._score += STATE::TheNumOfFullBoxWithTheEdge(BOARD::Create(*this), edge);
				}
				else
				{
					_fir_player._score += STATE::TheNumOfFullBoxWithTheEdge(BOARD::Create(*this), edge);
				}
			}
			void DoActions(bool is_sec_player, ActionDes actions)
			{
				for (auto edge : actions)
				{
					GameEdgeSet(is_sec_player, edge);
					Message("Edge " + I2S(edge), false);
				}
			}
		};
	}
}