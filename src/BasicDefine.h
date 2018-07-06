#include "../lib/gadt/src/gadtlib.h"
#include "../lib/gadt/src/bitboard.hpp"
#include "../lib/gadt/src/game_shell.h"
#include "../lib/gadt/src/parallel_mcts.hpp"
#include "../lib/gadt/src/minimax.hpp"
#include <unordered_map>

#pragma once

namespace dots_and_boxes_solver
{
	//CONSTANTS
	constexpr const bool DAB_WARNING = true;

	using BoardType = gadt::bitboard::BitBoard64;
	using BoardValueType = gadt::bitboard::gadt_int64;
	using EdgeIndex = uint8_t;
	using MarginType = int8_t;
	using DabPos = gadt::BasicUPoint<uint8_t>;
	using DabAction = gadt::bitboard::BitBoard64;
	using DabActionList = std::vector<DabAction>;

	constexpr const char* DAB_STATE_JSON_PATH = "./state.json";

	extern std::unordered_map<BoardValueType, MarginType> GLOBAL_HASH_TABLE;

	constexpr bool IsLegalGameSize(const size_t width, const size_t height)
	{
		return width > 0 && height > 0 && width >= height && (2 * width * height + width + height) <= 64;
	}

	template<size_t WIDTH, size_t HEIGHT>
	constexpr const size_t EdgeCount()
	{
		return 2 * WIDTH * HEIGHT + WIDTH + HEIGHT;
	}

	template<size_t WIDTH, size_t HEIGHT>
	constexpr const size_t BoxCount()
	{
		return WIDTH * HEIGHT;
	}

	size_t EdgeCount(size_t width, size_t height);

	size_t BoxCount(size_t width, size_t height);

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabBoard;

	enum class DabBoxType : EdgeIndex
	{
		FULL_BOX = 4,
		DEAD_BOX = 3,
		CHAIN_BOX = 2,
		FREE_BOX = 1,
		EMPTY_BOX = 0
	};

	//'box' is a class that include info about a box in the board.
	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabBox
	{
		static constexpr size_t _EDGE_COUNT = EdgeCount<WIDTH, HEIGHT>();	//count of edges.

	protected:

		DabPos		_pos;
		DabBoxType	_type;
		EdgeIndex	_own_edge[4];
		EdgeIndex	_neighbour_box[4];

	public:

		DabBox(const DabBoard<WIDTH, HEIGHT>& board, EdgeIndex index) :
			_pos(board.get_the_pos(index)),
			_type(static_cast<DabBoxType>(board.box_edges_count(board.get_the_pos(index)))),
			_own_edge{ index, board.the_to_lve(index), board.the_to_bhe(index), board.lve_to_rve(board.the_to_lve(index)) },
			_neighbour_box{ _EDGE_COUNT, _EDGE_COUNT, _EDGE_COUNT, _EDGE_COUNT }
		{

			//set neighbour boxes.
			if (board.is_bhe(the()))
				_neighbour_box[0] = index - WIDTH;
			if (board.is_rve(lve()))
				_neighbour_box[1] = index - 1;
			if (board.is_the(bhe()))
				_neighbour_box[2] = index + WIDTH;
			if (board.is_lve(rve()))
				_neighbour_box[3] = index + 1;
		}

		inline DabPos pos()
		{
			return _pos;
		}

		inline DabBoxType type()
		{
			return _type;
		}

		inline EdgeIndex own_edge(size_t index)
		{
			return _own_edge[index];
		}

		inline EdgeIndex neighbour_box(size_t index)
		{
			return _neighbour_box[index];
		}

		inline bool neighbour_exist(size_t index)
		{
			return _neighbour_box[index] != _EDGE_COUNT;
		}

		inline EdgeIndex index()
		{
			return the();
		}

		inline EdgeIndex the()
		{
			return _own_edge[0];
		}

		inline EdgeIndex lve()
		{
			return _own_edge[1];
		}

		inline EdgeIndex bhe()
		{
			return _own_edge[2];
		}

		inline EdgeIndex rve()
		{
			return _own_edge[3];
		}
	};
}

