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
	*/

	using DabPos = gadt::BasicUnsignedCoordinate<uint8_t>;

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabBoard
	{
	private:
		//static variables
		static const size_t _w0 = 0;//first index of horizon edges in the array.
		static const size_t _h0 = WIDTH * HEIGHT + WIDTH;//first index of vertical edges in the array.
		static const size_t _num_of_edges = 2 * WIDTH * HEIGHT + WIDTH + HEIGHT;//number of edges.
		static const size_t _num_of_boxes = WIDTH * HEIGHT;

	private:
		BoardType _edges;

	private:
		/*
		* edge operation functions.
		*/

		//return true if the 'edge' is the index of a edge.
		inline bool is_edge_index(size_t index) const
		{
			return index >= 0 && index < num_of_edges();
		}

		//return true if the 'edge' is he.
		inline bool is_he(EdgeIndex edge) const
		{
			return edge >= 0 && edge < _h0;
		}

		//return true if the 'edge' is ve.
		inline bool is_ve(EdgeIndex edge) const
		{
			return edge >= _h0 && edge < _num_of_edges;
		}

		//return true if the 'edge' is the.
		inline bool is_the(EdgeIndex edge) const
		{
			return edge >= 0 && edge < (_h0 - WIDTH);
		}

		//return true if the 'edge' is bhe.
		inline bool is_bhe(EdgeIndex edge) const
		{
			return edge >= WIDTH  && edge < _h0;
		}

		//return true if the 'edge' is lve.
		inline bool is_lve(EdgeIndex edge) const
		{
			return edge >= _h0 && edge < (_num_of_edges - HEIGHT);
		}

		//return true if the 'edge' is ve.
		inline bool is_rve(EdgeIndex edge) const
		{
			return edge >= (_h0 + HEIGHT) && edge < _num_of_edges;
		}

		//get the index by width and height
		inline EdgeIndex get_the(DabPos pos) const
		{ 
			return (pos.y * WIDTH) + pos.x;
		}

		//get lve index by width and height
		inline EdgeIndex get_lve(DabPos pos) const
		{
			return _h0 + (HEIGHT * pos.x) + pos.y;
		}
		
		//get bhe index by width and height
		inline EdgeIndex get_bhe(DabPos pos) const
		{
			return (pos.y * WIDTH) + pos.x + WIDTH;
		}

		//get rve index by width and height
		inline EdgeIndex get_rve(DabPos pos) const
		{
			return _h0 + (HEIGHT * pos.x) + pos.y + HEIGHT;
		}

		//get the position of 'edge'.
		inline DabPos get_the_pos(EdgeIndex edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_he(edge), " is not he");
			return { edge % width , edge / width };
		}

		//get the position of 'edge'.
		inline DabPos get_lve_pos(EdgeIndex edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_ve(edge), " is not ve");
			return { (edge - _h0) / HEIGHT , (edge - _h0) % HEIGHT };
		}

		//lve -> the of a box.
		inline EdgeIndex lve_to_the(EdgeIndex edge) const
		{
			DabPos pos = get_lve_pos(edge);
			return get_the(pos);
		}

		//the -> lve of a box. 
		inline EdgeIndex the_to_lve(EdgeIndex edge) const
		{
			DabPos pos = get_the_pos(edge);
			return get_lve(pos);
		}

		//the -> bhe of a box.
		inline EdgeIndex the_to_bhe(EdgeIndex edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_the(edge), "is not the");
			return edge + WIDTH;
		}

		//bhe -> the of a box.
		inline EdgeIndex bhe_to_the(EdgeIndex edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_bhe(edge), "is not bhe");
			return edge - WIDTH;
		}

		//lve -> rve of a box.
		inline EdgeIndex lve_to_rve(EdgeIndex edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_lve(edge), "is not lve");
			return edge + HEIGHT;
		}

		//rve -> lve of a box.
		inline EdgeIndex rve_to_lve(EdgeIndex edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_rve(edge), "is not rve");
			return edge - HEIGHT;
		}

		/*
		* box operation
		*/

		//return true if the 'index' is the index of boxes.
		inline bool is_box_index(size_t index) const
		{
			return index >= 0 && index < _num_of_boxes;
		}

		//get the position of the box by index.
		inline DabPos get_box_pos(size_t index) const
		{
			return get_the_pos(index);
		}

	public:
		
		DabBoard()
		{
		}

		DabBoard(BoardValueType bv):
			_edges(bv)
		{

		}

		//to int
		inline BoardValueType to_ullong() const
		{
			return _edges.to_ullong();
		}

		//get height
		inline size_t height() const 
		{
			return HEIGHT;
		}

		//get width
		inline size_t width() const
		{
			return WIDTH;
		}

		//get the number of edges.
		inline size_t num_of_edges() const
		{
			return _num_of_edges;
		}

		//get the number of boxes.
		inline size_t num_of_boxes() const
		{
			return _num_of_boxes;
		}

		//return true if the edge exist.
		inline bool edge_exist(size_t index) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_edge_index(index), "is not the index of edge");
			return _edges[index];
		}

		//return true if the box had been captured.
		inline bool box_exist(size_t index) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_box_index(index), "it is not box index");
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

		//set edge.
		inline void set_edge(size_t index)
		{
			_edges.set(index);
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
	};

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabState
	{
	private:
		DabBoard<WIDTH, HEIGHT> _board;
		bool _is_player_one;
		int _boxes_margin;

	public:

		DabState():
			_board(),
			_is_player_one(true),
			_boxes_margin(0)
		{
		}

		DabState(DabBoard<WIDTH, HEIGHT> board, int boxes_margin) :
			_board(board),
			_is_player_one(true),
			_boxes_margin(boxes_margin)
		{
		}

		DabState(BoardValueType bv, int boxes_margin):
			_board(bv),
			_is_player_one(true),
			_boxes_margin(boxes_margin)
		{
		}

		//Visualization
		void Visualization() const
		{
			const gadt::console::ConsoleColor edge_color = console::YELLOW;
			const gadt::console::ConsoleColor action_color = console::WHITE;
			const gadt::console::ConsoleColor dot_color = console::GRAY;
			const gadt::console::ConsoleColor box_color = console::GRAY;
			const std::string empty_he_str = "   ";
			const std::string empty_ve_str = " ";
			const std::string empty_box_str = "   ";
			const std::string he_str = "---";
			const std::string ve_str = "|";
			const std::string box_str = " X ";
			const std::string dot_str = "+";
			std::string prev_space = "     ";
			std::cout << std::endl;
			DabPos pos;

			//print title.
			std::cout << prev_space;
			gadt::console::Cprintf("[" + gadt::ToString(WIDTH) + " x " + gadt::ToString(HEIGHT) + "]", console::YELLOW);
			std::cout << std::endl << prev_space;
			gadt::console::Cprintf(_is_player_one? "Player: 1":"Player: 2", console::CYAN);
			std::cout << std::endl << prev_space;
			gadt::console::Cprintf("Boxes Margin: " + gadt::ToString(_boxes_margin), console::GREEN);
			std::cout << std::endl << std::endl;

			//print top horizon edges.
			std::cout << prev_space;
			gadt::console::Cprintf(dot_str, dot_color);
			for (uint8_t x = 0; x < WIDTH; x++)
			{
				pos = DabPos(x, 0);
				if (_board.the_exist(pos))
					gadt::console::Cprintf(he_str, edge_color);
				else
					gadt::console::Cprintf(empty_he_str, edge_color);
				gadt::console::Cprintf(dot_str, dot_color);
			}
			std::cout << std::endl;

			for (uint8_t y = 0; y < HEIGHT; y++)
			{
				//print vertical edges.
				std::cout << prev_space;
				for (uint8_t x = 0; x < WIDTH; x++)
				{
					pos = DabPos(x, y);
					//print lve
					if (_board.lve_exist(pos))
						gadt::console::Cprintf(ve_str, edge_color);
					else
						gadt::console::Cprintf(empty_ve_str, edge_color);

					//print box
					if (_board.rve_exist(pos))
						gadt::console::Cprintf(box_str, box_color);
					else
						gadt::console::Cprintf(empty_box_str, box_color);
				}
				pos = DabPos(WIDTH - 1, y);
				if (_board.rve_exist(pos))
					gadt::console::Cprintf(ve_str, edge_color);
				else
					gadt::console::Cprintf(empty_ve_str, edge_color);
				std::cout << std::endl;
				
				//print horizon edges.
				std::cout << prev_space;
				gadt::console::Cprintf(dot_str, dot_color);
				for (uint8_t x = 0; x < WIDTH; x++)
				{
					pos = DabPos(x, y);
					if (_board.bhe_exist(pos))
						gadt::console::Cprintf(he_str, edge_color);
					else
						gadt::console::Cprintf(empty_he_str, edge_color);
					gadt::console::Cprintf(dot_str, dot_color);
				}
				std::cout << std::endl;
			}
			std::cout << std::endl << std::endl;
		}

		const DabBoard<WIDTH, HEIGHT>& board() const
		{
			return _board;
		}

		void BeFull()
		{
			for (size_t i = 1; i < _board.num_of_edges(); i++)
			{
				_board.set_edge(i);
			}
		}

	};
}
