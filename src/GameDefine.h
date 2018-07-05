#include "../lib/gadt/src/gadtlib.h"
#include "../lib/gadt/src/bitboard.hpp"
#include "../lib/gadt/src/game_shell.h"

#ifdef GADT_GNUC
	#define DAB_UNIX
#endif

#pragma  once

//MACROS
#define RETURN_STRINGFY(parameter,str) if(parameter == str){return #str;}

namespace dots_and_boxes
{
	//CONSTANTS
	constexpr const bool DAB_WARNING = true;
	constexpr const size_t GAME_SIZE = 5;
	constexpr const size_t MAX_EDGE = 60;
	constexpr const size_t MAX_BOX = 25;
	constexpr const size_t MAX_CHAIN = 10;
	constexpr const size_t EMPTY_BOARD = 0;
	constexpr const size_t RED_BOX = 1;
	constexpr const size_t BLUE_BOX = -1;
	constexpr const size_t EMPTY_BOX = 0;

	//type define.
	using Edge = uint8_t;
	using Margin = uint16_t;
	using BoardValue = gadt::bitboard::gadt_int64;
	using Board = gadt::bitboard::BitBoard64;
	using ActionVec = gadt::bitboard::BitBoard64;

	constexpr const bool g_DAB_DEFINE_WARNING = true;

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
		void Print(ActionVec action_vec = ActionVec(0)) const;

		//TODO
		void EdgePrint() const;

		//create a random state with appointed edge number.
		static State RandomState(size_t edge_num);
	};

	//namespace 'state' include some state function, such as index exchange of edge ,etc.
	namespace state
	{
		//box type means boxes with different number of edges.
		namespace box_type
		{
			enum BoxType
			{
				FULL_BOX = 4,
				DEAD_BOX = 3,
				CHAIN_BOX = 2,
				FREE_BOX = 1
			};

			inline std::string ToString(BoxType bt)
			{
				RETURN_STRINGFY(bt, FULL_BOX)
				RETURN_STRINGFY(bt, DEAD_BOX)
				RETURN_STRINGFY(bt, CHAIN_BOX)
				RETURN_STRINGFY(bt, FREE_BOX)
				return "";
			}
		}

		//'box' is a class that include info about a box in the board.
		class Box
		{
		protected:
			Edge _index;
			box_type::BoxType _type;
			Edge _own_edge[4];
			Edge _neighbour_box[4];

		public:
			Box(const Board& board, Edge index);
			inline Edge index()
			{
				return _index;
			}
			inline box_type::BoxType type()
			{
				return _type;
			}
			inline Edge own_edge(size_t index)
			{
				return _own_edge[index];
			}
			inline Edge neighbour_box(size_t index)
			{
				return _neighbour_box[index];
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

		//得到某个横边左下的竖边的编号（限制为0~24）
		inline Edge UpperToLeftEdge(Edge hor_edge)
		{
			GADT_WARNING_IF(g_DAB_DEFINE_WARNING, hor_edge > 24 || hor_edge < 0, "Wrong index");
			return (34 - (hor_edge / 5)) + 5 * (hor_edge % 5);
		}

		//得到某个竖边右上的横边的编号（限制为30~54）
		inline Edge LeftToUpperEdge(Edge vec_edge)
		{
			GADT_WARNING_IF(g_DAB_DEFINE_WARNING, vec_edge > 54 || vec_edge < 30, "Wrong index");
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
			GADT_WARNING_IF(g_DAB_DEFINE_WARNING, hor_edge > 24, "Wrong index");
			Edge lower_left_edge = UpperToLeftEdge(hor_edge);
			return board.get(hor_edge) + board.get(lower_left_edge) + board.get(lower_left_edge + 5) + board.get(hor_edge + 5);
		}

		//return the number of full-box which contain the edge.
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

		//get the free edge of first dead-box.
		Edge GetFirstFreeEdgeInDeadBox(const Board& board);

		//get the first free edge in dead-chain.
		Edge GetFirstFreeEdgeInDeadChain(const Board& board);

		//judge whether any dead box exists in a board.
		bool ExistDeadBox(const Board& board);

		//check whether exist dead chain in a board.
		bool ExistDeadChain(const Board& board);

		//check whether exist free edge in a board.
		bool ExistFreeEdge(const Board& board);

		//get the minimal state of the box.
		Board MinimalForm(const Board& board);

		//get not reasonable state.
		bool IsReasonable(const Board& board);

		//judge whether a edge in a board is a free-edge.
		bool IsFreeEdge(const Board& board, Edge edge);

		//judge whether a edge is the upper edge of a box that is first box of a dead chain.
		bool IsUpperEdgeOfFirstBoxOfDeadChain(const Board& board, Edge edge);

		//judge whether two edge is upper edge of two neighbour box.
		bool IsUpperEdgeOfNeighbourBox(Edge a, Edge b);

	}
}