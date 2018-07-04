#include "StateDefine.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	struct DabGameFuncPackage
	{
		using DabMoveList = std::vector<DabMove>;

		static void UpdateState(DabState<WIDTH, HEIGHT>& state, const DabMove& move)
		{
			state.Update(move);
		}

		static void MakeAction(const DabState<WIDTH, HEIGHT>& state, DabMoveList& move_list)
		{
			for (EdgeIndex i = 0; i < EdgeCount<WIDTH, HEIGHT>(); i++)
				if (state.board().edge_exist(i) == false)
					move_list.push_back({ i });
		}

		
		static gadt::AgentIndex DetermineWinner(const DabState<WIDTH, HEIGHT>& state)
		{
			return state.GetWinner();
		}

		static int EvaluateState(const DabState<WIDTH, HEIGHT>& state, const gadt::AgentIndex player_index)
		{
			if(state.is_fir_player())
				return state.boxes_margin();
			return -state.boxes_margin();
		}
	};
}

