#include "stdafx.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <vector>

#include "GeneralGameShell.h"

#pragma  once

//MACROS
#define WINDOWS
#define WARNING
#define WARNING_CHECK(warning_condition,reason) console::WarningCheck(warning_condition,reason,__FILE__,__LINE__,__FUNCTION__)
#define RETURN_STRINGFY(parameter,str) if(parameter == str){return #str;}

//CONSTANTS
#define GAME_SIZE 5
#define MAX_EDGE 60
#define MAX_BOX 25
#define MAX_CHAIN 10

#define EMPTY_BOARD 0

#define RED_BOX 1
#define BLUE_BOX -1
#define EMPTY_BOX 0

namespace console
{
	//output msg.
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

	inline void Error(std::string reason)
	{
		std::cout << std::endl;
		Cprintf(">> ERROR: ", color::red);
		Cprintf(reason, color::white);
		std::cout << std::endl << std::endl;
	}
	inline void Message(std::string message, bool show_MSG = true)
	{
		std::cout << ">> ";
		if (show_MSG)
		{
			Cprintf("MSG: ", color::deep_green);
		}
		Cprintf(message, color::green);
		std::cout << std::endl << std::endl;
	}
	inline void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function)
	{
#ifdef WARNING
		if (condition)
		{
			Cprintf(">> WARNING: ", color::purple);
			Cprintf(reason, color::red);
			std::cout << std::endl;
			Cprintf("[File]: " + file, color::gray);
			std::cout << std::endl;
			Cprintf("[Line]: " + I2S(line), color::gray);
			std::cout << std::endl;
			Cprintf("[Func]: " + function, color::gray);
			std::cout << std::endl;
			system("pause");
		}
#endif
	}

	//Singleton
	template<class value_type>
	class Singleton final : public value_type
	{
	private:
		Singleton() = default;
		Singleton(const Singleton&) = delete;
		~Singleton() = default;
	public:

		static Singleton* instance()
		{
			static Singleton only_one;
			return &only_one;
		}
	};
}

namespace dots_and_boxes
{
	//type define.
	typedef unsigned char Edge;
	typedef short Margin;
	typedef long long BitBoard;

	//define Board and ActionVector.
	class Board
	{
	private:
		BitBoard _data;

	public:
		inline Board() :
			_data(0)
		{

		}
		inline Board(BitBoard board) :
			_data(board)
		{
		}

		//equal to the appointed value.
		inline void operator=(BitBoard board)
		{
			_data = board;
		}

		//return whether any bit is true.
		inline bool any() const
		{
			return _data != 0;
		}

		//return whether no any bit is true.
		inline bool none() const
		{
			return _data == 0;
		}

		//set appointed bit to true.
		inline void set(Edge index)
		{
			WARNING_CHECK(index >= MAX_EDGE, "out of range.");
			long long temp = 1;
			temp = temp << index;
			_data = _data | temp;
		}

		//reset appointed bit.
		inline void reset(Edge index)
		{
			WARNING_CHECK(index >= MAX_EDGE, "out of range.");
			long long temp = 1;
			temp = ~(temp << index);
			_data = _data & temp;
		}

		//reset all bits.
		inline void reset()
		{
			_data = 0;
		}

		//write value to appointed bit.
		inline void write(Edge index, int value)
		{
			WARNING_CHECK(index >= MAX_EDGE, "out of range.");
			if (value)
			{
				set(index);
			}
			else
			{
				reset(index);
			}
		}

		//get bit.
		inline bool operator[](Edge index) const
		{
			WARNING_CHECK(index >= MAX_EDGE, "out of range.");
			return ((_data >> index) & 0x1) == 1;
		}

		//get bit.
		inline bool get(Edge index) const
		{
			WARNING_CHECK(index >= MAX_EDGE, "out of range.");
			return ((_data >> index) & 0x1) == 1;
		}

		//get value
		inline BitBoard to_ullong() const
		{
			return _data;
		}

		//get string
		inline std::string to_string() const 
		{
			char c[MAX_EDGE];
			for (Edge i = MAX_EDGE - 1; i >= 0 && i <= MAX_EDGE; i--)
			{
				if (get(i))
				{
					c[MAX_EDGE - 1 - i] = '1';
				}
				else
				{
					c[MAX_EDGE - 1 - i] = '0';
				}
			}
			return std::string(c);
		}

		inline bool operator<(const Board& target)
		{
			return _data < target._data;
		}
		inline bool operator>(const Board& target)
		{
			return _data > target._data;
		}
		inline bool operator<=(const Board& target)
		{
			return _data <= target._data;
		}
		inline bool operator>=(const Board& target)
		{
			return _data >= target._data;
		}
		inline bool operator==(const Board& target)
		{
			return _data == target._data;
		}
	};
	typedef Board ActionVec;

	//state is used for show board mainly.
	class State
	{
	protected:
		Board _board;

	public:
		//create a empty state.
		State() = default;

		//create a state by bit board.
		State(Board board);

		inline Board board() 
		{
			return _board;
		}

		inline Board& board_ref()
		{
			return _board;
		}

		//show state.
		void Visualization(ActionVec action_vec = ActionVec(0)) const;

		//TODO
		void EdgeVisualization() const;

		//create a random state with appointed edge number.
		static State RandomState(size_t edge_num);
	};

	//namespace 'state' include some state function, such as index exchange of edge ,etc.
	namespace state
	{
		//得到某个横边左下的竖边的编号（限制为0~24）
		inline Edge UpperToLeftEdge(Edge hor_edge)
		{
			WARNING_CHECK(hor_edge > 24 || hor_edge < 0, "Wrong index");
			return (34 - (hor_edge / 5)) + 5 * (hor_edge % 5);
		}

		//得到某个竖边右上的横边的编号（限制为30~54）
		inline Edge LeftToUpperEdge(Edge vec_edge)
		{
			WARNING_CHECK(vec_edge > 54 || vec_edge < 30, "Wrong index");
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
		inline size_t GetLowerBoxEdgeNum(const Board& board, Edge hor_edge)
		{
			WARNING_CHECK(hor_edge > 24, "Wrong index");
			Edge lower_left_edge = UpperToLeftEdge(hor_edge);
			return board.get(hor_edge) + board.get(lower_left_edge) + board.get(lower_left_edge + 5) + board.get(hor_edge + 5);
		}

		//得到某个局面下第一个DEAD BOX(已经被占领了三条边)的空边的编号。没有的话则返回MAX_EDGE.
		Edge GetDeadBoxRemainEdgeIndex(const Board& board);

		//judge whether any dead box exists in a board.
		bool ExistDeadBox(const Board& board);

		//capture all possible box in this action.
		Board CaptureAllBoxes(Board& board);

		//return whether how many full-box as the edge as their part.
		inline Margin TheNumOfFullBoxWithTheEdge(const Board& board, Edge index)
		{

			if (!board.get(index))
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
						Edge lower_left_vec_edge = UpperToLeftEdge(index);
						Edge lower_right_vec_edge = lower_left_vec_edge + 5;
						if (board.get(lower_hor_edge) && board.get(lower_left_vec_edge) && board.get(lower_right_vec_edge))
						{
							num++;
						}

						//check upper box.
						Edge upper_hor_edge = index - 5;
						Edge upper_left_vec_edge = UpperToLeftEdge(upper_hor_edge);
						Edge upper_right_vec_edge = upper_left_vec_edge + 5;
						if (board.get(upper_hor_edge) && board.get(upper_left_vec_edge) && board.get(upper_right_vec_edge))
						{
							num++;
						}

					}
					else
					{
						//lower side hor edges

						//check upper box.
						Edge upper_hor_edge = index - 5;
						Edge upper_left_vec_edge = UpperToLeftEdge(upper_hor_edge);
						Edge upper_right_vec_edge = upper_left_vec_edge + 5;
						if (board.get(upper_hor_edge) && board.get(upper_left_vec_edge) && board.get(upper_right_vec_edge))
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
					Edge lower_left_vec_edge = UpperToLeftEdge(index);
					Edge lower_right_vec_edge = lower_left_vec_edge + 5;
					if (board.get(lower_hor_edge) && board.get(lower_left_vec_edge) && board.get(lower_right_vec_edge))
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
						Edge upper_right_hor_edge = LeftToUpperEdge(index);
						Edge lower_right_hor_edge = upper_right_hor_edge + 5;
						if (board.get(right_vec_edge) && board.get(upper_right_hor_edge) && board.get(lower_right_hor_edge))
						{
							num++;
						}

						//check left box.
						Edge left_vec_edge = index - 5;
						Edge upper_left_hor_edge = LeftToUpperEdge(left_vec_edge);
						Edge lower_left_hor_edge = upper_left_hor_edge + 5;
						if (board.get(left_vec_edge) && board.get(upper_left_hor_edge) && board.get(lower_left_hor_edge))
						{
							num++;
						}
					}
					else
					{
						//right side vec edges

						//check left box.
						Edge left_vec_edge = index - 5;
						Edge upper_left_hor_edge = LeftToUpperEdge(left_vec_edge);
						Edge lower_left_hor_edge = upper_left_hor_edge + 5;
						if (board.get(left_vec_edge) && board.get(upper_left_hor_edge) && board.get(lower_left_hor_edge))
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
					Edge upper_right_hor_edge = LeftToUpperEdge(index);
					Edge lower_right_hor_edge = upper_right_hor_edge + 5;
					if (board.get(right_vec_edge) && board.get(upper_right_hor_edge) && board.get(lower_right_hor_edge))
					{
						num++;
					}
				}
				return num;
			}
			return num;
		}

		//minimize state by change corner
		inline void ChangeBoxCorner(Board& board)
		{
			if ((board.to_ullong() & 0x400000001) == 0x400000000)
			{
				board.reset(34);
				board.set(0);
			}

			if ((board.to_ullong() & 0x800000000000010) == 0x800000000000000)
			{
				board.reset(59);
				board.set(4);
			}

			if ((board.to_ullong() & 0x42000000) == 0x40000000)
			{
				board.reset(30);
				board.set(25);
			}

			if ((board.to_ullong() & 0x80000020000000) == 0x80000000000000)
			{
				board.reset(55);
				board.set(29);
			}
		}

		//get the box that have been rotate for 90'.
		inline Board RotateBox(const Board& board)
		{
			long long temp = board.to_ullong() & 0x3FFFFFFF;
			for (Edge i = 30; i < 60; i++)
			{
				temp = temp << 1;
				if (board.get(i))
				{
					temp = temp | 1;
				}
			}
			return Board(temp);
		}

		//get the box that have been reversald.
		inline Board ReverseBox(const Board& board)
		{
			long long temp = 0;
			long long source = board.to_ullong();
			for (Edge i = 0; i < 6; i++)
			{
				Edge begin = 55 - (i * 5);
				for (Edge n = 0; n < 5; n++)
				{
					temp = temp << 1;
					if (board.get(begin + n))
					{
						temp = temp | 1;
					}
				}
			}

			for (Edge i = 0; i < 6; i++)
			{
				long long part = (source >> (i * 5)) & 0x1F;
				temp = (temp << 5) | part;
			}
			return Board(temp);
		}

		//get the minimal state of the box.
		Board MinimalForm(const Board& board);

		//check whether exist dead chain in a board.
		bool ExistDeadChain(const Board& board);

		//check whether exist free edge in a board.
		bool ExistFreeEdge(const Board& board);

		//get not reasonable state.
		bool IsReasonable(const Board& board);

		//judge whether a edge in a board is a free-edge.
		bool IsFreeEdge(const Board& board, Edge edge);

		//judge whether a edge is the upper edge of a box that is first box of a dead chain.
		bool IsUpperEdgeOfFirstBoxOfDeadChain(const Board& board, Edge edge);

		//judge whether two edge is upper edge of two neighbour box.
		bool IsUpperEdgeOfNeighbourBox(Edge a, Edge b);
	}

	//namespace 'game' include basic concept of game.
	//namespace game
	//{
	//	typedef std::vector<Edge> ActionDes;

	//	struct Player
	//	{
	//		size_t _score;
	//		ActionDes(*_func)(State&);
	//		std::string _des;
	//	};

	//	class GameState :public State
	//	{
	//	private:
	//		Player _fir_player;
	//		Player _sec_player;
	//		bool _auto_game;

	//		Edge _last_action;
	//		bool _last_player;

	//	public:
	//		GameState();
	//		GameState(Board board, Player fir_player, Player sec_player);

	//		//variable
	//		inline Margin get_margin()
	//		{
	//			return Margin(_fir_player._score - _sec_player._score);
	//		}
	//		inline const Player& player(bool is_sec_player)
	//		{
	//			if (is_sec_player)
	//			{
	//				return _sec_player;
	//			}
	//			else
	//			{
	//				return _fir_player;
	//			}
	//		}
	//		inline const Player& operator[](bool is_sec_player)
	//		{
	//			return player(is_sec_player);
	//		}
	//		inline Edge last_action()
	//		{
	//			return _last_action;
	//		}
	//		inline bool last_player()
	//		{
	//			return _last_player;
	//		}
	//		inline void set_player(bool is_sec_action, Player player)
	//		{
	//			if (is_sec_action)
	//			{
	//				_sec_player = player;
	//			}
	//			else
	//			{
	//				_fir_player = player;
	//			}
	//		}
	//		inline void set_auto_game(bool auto_game)
	//		{
	//			_auto_game = auto_game;
	//		}

	//		//func
	//		inline void GameEdgeRemove(bool is_sec_player, Edge edge)
	//		{
	//			if (is_sec_player)
	//			{
	//				_sec_player._score -= state::TheNumOfFullBoxWithTheEdge(*this, edge);
	//			}
	//			else
	//			{
	//				_fir_player._score -= state::TheNumOfFullBoxWithTheEdge(*this, edge);
	//			}
	//			reset(edge);
	//		}
	//		inline void GameEdgeSet(bool is_sec_player, Edge edge)
	//		{
	//			set(edge);
	//			_last_action = edge;
	//			_last_player = is_sec_player;
	//			if (is_sec_player)
	//			{
	//				_sec_player._score += state::TheNumOfFullBoxWithTheEdge(board::Create(*this), edge);
	//			}
	//			else
	//			{
	//				_fir_player._score += state::TheNumOfFullBoxWithTheEdge(board::Create(*this), edge);
	//			}
	//		}
	//		void DoActions(bool is_sec_player, ActionDes actions)
	//		{
	//			for (auto edge : actions)
	//			{
	//				GameEdgeSet(is_sec_player, edge);
	//				console::Message("Edge " + console::I2S(edge), false);
	//			}
	//		}
	//	};
	//}
}