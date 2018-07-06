#include "BoardDefine.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabState
	{
	private:

		DabBoard<WIDTH, HEIGHT> _board;
		bool _is_fir_player;
		int _boxes_margin;

	private:

		

	public:

		//get the board.
		inline const DabBoard<WIDTH, HEIGHT>& board() const
		{
			return _board;
		}

		//get the board.
		inline DabBoard<WIDTH, HEIGHT>& board_ref()
		{
			return _board;
		}

		//return true if the next move player is first player.
		inline bool is_fir_player() const
		{
			return _is_fir_player;
		}

		//return the value that how many boxes that boxes captured by first player more than boxes that captured by second player.
		inline int boxes_margin() const
		{
			return _boxes_margin;
		}

		inline int is_finished() const
		{
			return _board.edge_count() == EdgeCount<WIDTH, HEIGHT>();
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
			_is_fir_player(true),
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

		//Print
		void Print(DabAction actions = DabAction()) const
		{
			using namespace gadt::console;
			constexpr ConsoleColor EDGE_COLOR = ConsoleColor::White;
			constexpr ConsoleColor ACTION_COLOR = ConsoleColor::Yellow;
			constexpr ConsoleColor DOT_COLOR = ConsoleColor::Gray;
			constexpr ConsoleColor BOX_COLOR = ConsoleColor::Gray;
			constexpr const char* EMPTY_HE_STR = "   ";
			constexpr const char* EMPTY_VE_STR = " ";
			constexpr const char* EMPTY_BOX_STR = "   ";
			constexpr const char* ACTION_HE_STR = " - ";
			constexpr const char* ACTION_VE_STR = "!";
			constexpr const char* HE_STR = "---";
			constexpr const char* VE_STR = "|";
			constexpr const char* BOX_STR = " X ";
			constexpr const char* DOT_STR = "+";
			constexpr const char* PREV_SPACE = "     ";

			PrintEndLine();
			DabPos pos;

			//print title.
			std::cout << PREV_SPACE;
			Cprintf("[" + gadt::ToString(WIDTH) + " x " + gadt::ToString(HEIGHT) + "]  ", ConsoleColor::Yellow);
			//std::cout << std::endl << PREV_SPACE;
			Cprintf((_is_fir_player? "P1":"P2"), ConsoleColor::Cyan);
			//std::cout << std::endl << PREV_SPACE;
			Cprintf("  Margin: " + gadt::ToString(_boxes_margin), ConsoleColor::Green);
			//Cprintf(std::string("  Reasonable: ") + (board().IsReasonable() ? "Y" : "N"), ConsoleColor::Brown);
			PrintEndLine<2>();

			//print top horizon edges.
			std::cout << PREV_SPACE;
			Cprintf(DOT_STR, DOT_COLOR);
			for (uint8_t x = 0; x < WIDTH; x++)
			{
				pos = DabPos(x, 0);
				if (actions[_board.get_the(pos)] == true)
					Cprintf(ACTION_HE_STR, ACTION_COLOR);
				else if (_board.the_exist(pos))
					Cprintf(HE_STR, EDGE_COLOR);
				else
					Cprintf(EMPTY_HE_STR, EDGE_COLOR);
					
				Cprintf(DOT_STR, DOT_COLOR);
			}
			PrintEndLine();

			for (uint8_t y = 0; y < HEIGHT; y++)
			{
				//print vertical edges.
				std::cout << PREV_SPACE;
				for (uint8_t x = 0; x < WIDTH; x++)
				{
					pos = DabPos(x, y);
					//print lve
					if (actions[_board.get_lve(pos)] == true)
						Cprintf(ACTION_VE_STR, ACTION_COLOR);
					else if (_board.lve_exist(pos))
						Cprintf(VE_STR, EDGE_COLOR);
					else
						Cprintf(EMPTY_VE_STR, EDGE_COLOR);

					//print box
					if (_board.box_exist(pos))
						Cprintf(BOX_STR, BOX_COLOR);
					else
						Cprintf(EMPTY_BOX_STR, BOX_COLOR);
				}
				pos = DabPos(WIDTH - 1, y);
				if (actions[_board.get_rve(pos)] == true)
					Cprintf(ACTION_VE_STR, ACTION_COLOR);
				else if (_board.rve_exist(pos))
					Cprintf(VE_STR, EDGE_COLOR);
				else
					Cprintf(EMPTY_VE_STR, EDGE_COLOR);
				PrintEndLine();
				
				//print horizon edges.
				std::cout << PREV_SPACE;
				Cprintf(DOT_STR, DOT_COLOR);
				for (uint8_t x = 0; x < WIDTH; x++)
				{
					pos = DabPos(x, y);
					if (actions[_board.get_bhe(pos)] == true)
						Cprintf(ACTION_HE_STR, ACTION_COLOR);
					else if (_board.bhe_exist(pos))
						Cprintf(HE_STR, EDGE_COLOR);
					else
						Cprintf(EMPTY_HE_STR, EDGE_COLOR);
					Cprintf(DOT_STR, DOT_COLOR);
				}
				PrintEndLine();
			}
			PrintEndLine<2>();
		}

		gadt::AgentIndex GetWinner() const
		{
			if (board().edge_count() == EdgeCount<WIDTH, HEIGHT>())
			{
				if (boxes_margin() > 0)
					return 0x1;
				if (boxes_margin() < 0)
					return 0x2;
				else
					return 0x3;
			}
			return 0x0;
		}

		void ChangePlayer()
		{
			_is_fir_player = !_is_fir_player;
		}

		void SetMargin(MarginType margin)
		{
			_boxes_margin = margin;
		}

		//set the state to full
		void BeFull()
		{
			_board.set_all_edges();
		}
 
		//set random edge.
		void SetRandomEdge()
		{
			gadt::bitboard::ValueVector<EdgeCount<WIDTH, HEIGHT>()> moves;
			for (EdgeIndex i = 0; i < EdgeCount<WIDTH, HEIGHT>(); i++)
				if (!_board.edge_exist(i))
					moves.push(i);
			if (moves.length() > 0)
			{
				size_t edge = moves.draw_value();
				_board.set_edge((EdgeIndex)edge);
			}
		}

		//
		void Update(const DabAction& action)
		{
			size_t prev_box_count = _board.ExistingBoxCount();
			_board.TakeActions(action);
			auto margin = MarginType(_board.ExistingBoxCount() - prev_box_count);
			if (is_fir_player())
				_boxes_margin += margin;
			else
				_boxes_margin -= margin;
			ChangePlayer();
		}
	};
}
