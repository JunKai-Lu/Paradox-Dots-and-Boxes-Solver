#include "GameDefine.h"

#pragma once

namespace dots_and_boxes_solver
{
	/*
	* EdgeSet is a class that contains all the edges of a state. Each edge was represened by a bool variable.
	* he = horizon edges.
	* ve = vertical edges.
	* the = top horizon edge of a box.
	* bhe = bottom horizon edge of a box.
	* lve = left vertical edge of a box.
	* rve = left vertical edge of a box.

	* all the index of horizon edges were ordered by left to right, then up to down.
	* all the index of vertical edges were ordered by up to down, then left to right.
	* the index of horizon edges is in the front of the indexs of vertical edges.  
	* all the inndex of boxes were orderd by left to right, then up to down, which is same with the index of its top horizon edge(the).
	*/

	using DabPos = gadt::BasicUPoint<uint8_t>;

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabBoard
	{
	private:
		//static variables
		static constexpr size_t _EDGE_COUNT = EdgeCount<WIDTH, HEIGHT>();	//count of edges.
		static constexpr size_t _BOX_COUNT = BoxCount<WIDTH, HEIGHT>();		//count of boxes.

		static constexpr size_t _H0 = 0;						//first index of horizon edges in the array.
		static constexpr size_t _V0 = WIDTH * HEIGHT + WIDTH;	//first index of vertical edges in the array.

		static constexpr size_t _LTB_HE = _H0;					//index of outside horizon edge of left top box;
		static constexpr size_t _LTB_VE = _V0;					//index of outside vertical edge of left top box;
		static constexpr size_t _RTB_HE = _H0 + WIDTH - 1;		//index of outside horizon edge of right top box;
		static constexpr size_t _RTB_VE = _EDGE_COUNT - HEIGHT;	//index of outside vertical edge of right top box;
		static constexpr size_t _LBB_HE = _V0 - WIDTH;			//index of outside horizon edge of left bottom box;
		static constexpr size_t _LBB_VE = _V0 + HEIGHT - 1;		//index of outside vertical edge of left bottom box;
		static constexpr size_t _RBB_HE = _V0 -1;				//index of outside horizon edge of right bottom box;
		static constexpr size_t _RBB_VE = _EDGE_COUNT - 1;		//index of outside vertical edge of right bottom box;

	private:
		BoardType _edges;

	private:
		/*
		* edge operation functions.
		*/

		//return true if the 'edge' is the index of a edge.
		inline constexpr bool is_edge_index(size_t index) const
		{
			return index >= 0 && index < num_of_edges();
		}

		//return true if the 'edge' is he.
		inline constexpr bool is_he(EdgeIndex edge) const
		{
			return edge >= 0 && edge < _V0;
		}

		//return true if the 'edge' is ve.
		inline constexpr bool is_ve(EdgeIndex edge) const
		{
			return edge >= _V0 && edge < num_of_edges();
		}

		//return true if the 'edge' is the.
		inline constexpr bool is_the(EdgeIndex edge) const
		{
			return edge >= 0 && edge < (_V0 - WIDTH);
		}

		//return true if the 'edge' is bhe.
		inline constexpr bool is_bhe(EdgeIndex edge) const
		{
			return edge >= WIDTH  && edge < _V0;
		}

		//return true if the 'edge' is lve.
		inline constexpr bool is_lve(EdgeIndex edge) const
		{
			return edge >= _V0 && edge < (num_of_edges() - HEIGHT);
		}

		//return true if the 'edge' is ve.
		inline constexpr bool is_rve(EdgeIndex edge) const
		{
			return edge >= (_V0 + HEIGHT) && edge < num_of_edges();
		}

		//get the index by width and height
		inline constexpr EdgeIndex get_the(DabPos pos) const
		{ 
			return (pos.y * WIDTH) + pos.x;
		}

		//get lve index by width and height
		inline constexpr EdgeIndex get_lve(DabPos pos) const
		{
			return _V0 + (HEIGHT * pos.x) + pos.y;
		}
		
		//get bhe index by width and height
		inline constexpr EdgeIndex get_bhe(DabPos pos) const
		{
			return (pos.y * WIDTH) + pos.x + WIDTH;
		}

		//get rve index by width and height
		inline constexpr EdgeIndex get_rve(DabPos pos) const
		{
			return _V0 + (HEIGHT * pos.x) + pos.y + HEIGHT;
		}

		//get the position of 'edge'.
		inline constexpr DabPos get_the_pos(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_he(edge), " is not he");
			return { edge % WIDTH , edge / WIDTH };
		}

		//get the position of 'edge'.
		inline constexpr DabPos get_lve_pos(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_ve(edge), " is not ve");
			return { (edge - _V0) / HEIGHT , (edge - _V0) % HEIGHT };
		}

		//lve -> the of a box.
		inline constexpr EdgeIndex lve_to_the(EdgeIndex edge) const
		{
			DabPos pos = get_lve_pos(edge);
			return get_the(pos);
		}

		//the -> lve of a box. 
		inline constexpr EdgeIndex the_to_lve(EdgeIndex edge) const
		{
			DabPos pos = get_the_pos(edge);
			return get_lve(pos);
		}

		//the -> bhe of a box.
		inline constexpr EdgeIndex the_to_bhe(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_the(edge), "is not the");
			return edge + WIDTH;
		}

		//bhe -> the of a box.
		inline constexpr EdgeIndex bhe_to_the(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_bhe(edge), "is not bhe");
			return edge - WIDTH;
		}

		//lve -> rve of a box.
		inline constexpr EdgeIndex lve_to_rve(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_lve(edge), "is not lve");
			return edge + HEIGHT;
		}

		//rve -> lve of a box.
		inline constexpr EdgeIndex rve_to_lve(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_rve(edge), "is not rve");
			return edge - HEIGHT;
		}

		/*
		* box operation
		*/

		//return true if the 'index' is the index of boxes.
		inline constexpr bool is_box_index(size_t index) const
		{
			return index >= 0 && index < num_of_boxes();
		}

		//get the position of the box by index.
		inline constexpr DabPos get_box_pos(size_t index) const
		{
			return get_the_pos(index);
		}

	public:
		
		//default constructor.
		constexpr inline DabBoard():
			_edges(0)
		{
		}

		//construct by value.
		constexpr inline DabBoard(BoardValueType bv):
			_edges(bv)
		{

		}

		//to int
		inline constexpr BoardValueType to_ullong() const
		{
			return _edges.to_ullong();
		}

		//get height
		inline constexpr size_t height() const 
		{
			return HEIGHT;
		}

		//get width
		inline constexpr size_t width() const
		{
			return WIDTH;
		}

		//get the number of edges.
		inline constexpr size_t num_of_edges() const
		{
			return EdgeCount<WIDTH, HEIGHT>();
		}

		//get the number of boxes.
		inline constexpr size_t num_of_boxes() const
		{
			return BoxCount<WIDTH, HEIGHT>();
		}

		//return true if the edge exist.
		inline bool edge_exist(size_t index) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_edge_index(index), "is not the index of edge");
			return _edges[index];
		}

		//return true if the box had been captured.
		inline bool box_exist(size_t index) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_box_index(index), "it is not box index");
			return the_of_box_exist(index) && bhe_of_box_exist(index) && lve_of_box_exist(index) && rve_of_box_exist(index);
		}

		//return the number of exist that had exist for a box.
		inline size_t box_edges_count(size_t index) const
		{
			return 
				(size_t) the_of_box_exist(index) +
				(size_t) bhe_of_box_exist(index) +
				(size_t) lve_of_box_exist(index) +
				(size_t) rve_of_box_exist(index);
		}

		//get the count of boxes that owns this edge, which means this edge is a part of the box(es), the result may be { 0, 1, 2}.
		inline size_t count_of_boxes_that_owns_edge(EdgeIndex edge) const
		{
			size_t count = 0;
			if (edge_exist(edge))
			{
				if (is_he(edge))
				{
					if (is_the(edge))
					{
						EdgeIndex lve = the_to_lve(edge);
						EdgeIndex rve = lve_to_rve(lve);
						EdgeIndex bhe = the_to_bhe(edge);
						if (edge_exist(lve) && edge_exist(rve) && edge_exist(bhe))
							count += 1;
					}
					if (is_bhe(edge))
					{
						EdgeIndex the = bhe_to_the(edge);
						EdgeIndex lve = the_to_lve(the);
						EdgeIndex rve = lve_to_rve(lve);
						if (edge_exist(lve) && edge_exist(rve) && edge_exist(the))
							count += 1;
					}
				}
				else if (is_ve(edge))
				{
					if (is_lve(edge))
					{
						EdgeIndex rve = lve_to_rve(edge);
						EdgeIndex the = lve_to_the(edge);
						EdgeIndex bhe = the_to_bhe(the);
						if (edge_exist(the) && edge_exist(bhe) && edge_exist(rve))
							count += 1;
					}
					if (is_rve(edge))
					{
						EdgeIndex lve = rve_to_lve(edge);
						EdgeIndex the = lve_to_the(lve);
						EdgeIndex bhe = the_to_bhe(the);
						if (edge_exist(the) && edge_exist(bhe) && edge_exist(lve))
							count += 1;
					}
				}
			}
			return count;
		}

		//set edge.
		inline void set_edge(EdgeIndex index)
		{
			_edges.set(index);
		}

		//reset a edge to empty
		inline void reset_edge(EdgeIndex index)
		{
			_edges.reset(index);
		}

		//return true if the edge of box exist.
		inline bool the_exist(DabPos box_pos) const
		{
			EdgeIndex edge = get_the(box_pos);
			return edge_exist(edge);
		}

		//return true if bhe edge of box exist.
		inline bool bhe_exist(DabPos box_pos) const
		{
			EdgeIndex edge = get_bhe(box_pos);
			return edge_exist(edge);
		}

		//return true if lve edge of box exist.
		inline bool lve_exist(DabPos box_pos) const
		{
			EdgeIndex edge = get_lve(box_pos);
			return edge_exist(edge);
		}

		//return true if rve edge of box exist.
		inline bool rve_exist(DabPos box_pos) const
		{
			EdgeIndex edge = get_rve(box_pos);
			return edge_exist(edge);
		}

		//fill in all the edges.
		inline void set_all_edges()
		{
			for (EdgeIndex i = 0; i < num_of_edges(); i++)
				set_edge(i);
		}

		//reset all the edges.
		inline void reset_all_edges()
		{
			_edges = 0;
		}

		//minimize corner if possible, which may make the value smaller. 
		void ChangeCorner()
		{
			if (_edges[_LTB_VE] == true && _edges[_LTB_HE] = false)
			{
				_edges.set(_LTB_HE);
				_edges.reset(_LTB_VE);
			}
			if (_edges[_RTB_VE] == true && _edges[_RTB_HE] = false)
			{
				_edges.set(_RTB_HE);
				_edges.reset(_RTB_VE);
			}
			if (_edges[_LBB_VE] == true && _edges[_LBB_HE] = false)
			{
				_edges.set(_LBB_HE);
				_edges.reset(_LBB_VE);
			}
			if (_edges[_RBB_VE] == true && _edges[_RBB_HE] = false)
			{
				_edges.set(_RBB_HE);
				_edges.reset(_RBB_VE);
			}
		}

		void Rotate()
		{
			BoardType new_edges;

		}
	};

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabState
	{
	private:

		DabBoard<WIDTH, HEIGHT> _board;
		bool _is_fir_player;
		int _boxes_margin;

	public:

		//get the board.
		const DabBoard<WIDTH, HEIGHT>& board() const
		{
			return _board;
		}

		//return true if the next move player is first player.
		bool is_fir_player() const
		{
			return _is_fir_player;
		}

		//return the value that how many boxes that boxes captured by first player more than boxes that captured by second player.
		int boxes_margin() const
		{
			return _boxes_margin;
		}

	public:

		//default constructor
		constexpr DabState():
			_board(),
			_is_fir_player(true),
			_boxes_margin(0)
		{
		}

		//create specified state.
		constexpr DabState(DabBoard<WIDTH, HEIGHT> board, int boxes_margin) :
			_board(board),
			_is_player_one(true),
			_boxes_margin(boxes_margin)
		{
		}

		//create specified state.
		constexpr DabState(BoardValueType bv, int boxes_margin):
			_board(bv),
			_is_player_one(true),
			_boxes_margin(boxes_margin)
		{
		}

		//Visualization
		void Visualization() const
		{
			using namespace gadt::console;
			constexpr ConsoleColor edge_color = ConsoleColor::Yellow;
			constexpr ConsoleColor action_color = ConsoleColor::White;
			constexpr ConsoleColor dot_color = ConsoleColor::Gray;
			constexpr ConsoleColor box_color = ConsoleColor::Gray;
			const std::string empty_he_str = "   ";
			const std::string empty_ve_str = " ";
			const std::string empty_box_str = "   ";
			const std::string he_str = "---";
			const std::string ve_str = "|";
			const std::string box_str = " X ";
			const std::string dot_str = "+";
			std::string prev_space = "     ";

			PrintEndLine();
			DabPos pos;

			//print title.
			std::cout << prev_space;
			Cprintf("[" + gadt::ToString(WIDTH) + " x " + gadt::ToString(HEIGHT) + "]", ConsoleColor::Yellow);
			std::cout << std::endl << prev_space;
			Cprintf(_is_player_one? "Player: 1":"Player: 2", ConsoleColor::Cyan);
			std::cout << std::endl << prev_space;
			Cprintf("Boxes Margin: " + gadt::ToString(_boxes_margin), ConsoleColor::Green);
			PrintEndLine<2>();

			//print top horizon edges.
			std::cout << prev_space;
			Cprintf(dot_str, dot_color);
			for (uint8_t x = 0; x < WIDTH; x++)
			{
				pos = DabPos(x, 0);
				if (_board.the_exist(pos))
					Cprintf(he_str, edge_color);
				else
					Cprintf(empty_he_str, edge_color);
				Cprintf(dot_str, dot_color);
			}
			PrintEndLine();

			for (uint8_t y = 0; y < HEIGHT; y++)
			{
				//print vertical edges.
				std::cout << prev_space;
				for (uint8_t x = 0; x < WIDTH; x++)
				{
					pos = DabPos(x, y);
					//print lve
					if (_board.lve_exist(pos))
						Cprintf(ve_str, edge_color);
					else
						Cprintf(empty_ve_str, edge_color);

					//print box
					if (_board.rve_exist(pos))
						Cprintf(box_str, box_color);
					else
						Cprintf(empty_box_str, box_color);
				}
				pos = DabPos(WIDTH - 1, y);
				if (_board.rve_exist(pos))
					Cprintf(ve_str, edge_color);
				else
					Cprintf(empty_ve_str, edge_color);
				PrintEndLine();
				
				//print horizon edges.
				std::cout << prev_space;
				Cprintf(dot_str, dot_color);
				for (uint8_t x = 0; x < WIDTH; x++)
				{
					pos = DabPos(x, y);
					if (_board.bhe_exist(pos))
						Cprintf(he_str, edge_color);
					else
						Cprintf(empty_he_str, edge_color);
					Cprintf(dot_str, dot_color);
				}
				PrintEndLine();
			}
			PrintEndLine<2>();
		}

		//set the state to full
		void BeFull()
		{
			_board.set_all_edges();
		}
 
	};

	template<size_t WIDTH, size_t HEIGHT>
	DabBoard<WIDTH, HEIGHT> GetTerminalBoard()
	{
		DabBoard<WIDTH, HEIGHT> board;
		
	}
}
