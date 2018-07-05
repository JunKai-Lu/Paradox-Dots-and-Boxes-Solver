#include "StateDefine.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	struct DabGameFuncPackage
	{
		static void UpdateState(DabState<WIDTH, HEIGHT>& state, const DabAction& action)
		{
			state.Update(action);
		}

		static void MakeAction(const DabState<WIDTH, HEIGHT>& state, DabActionList& action_list)
		{
			DabActionGenerator<WIDTH, HEIGHT> ag(state);
			action_list = ag.actions();
		}

		static gadt::AgentIndex DetermineWinner(const DabState<WIDTH, HEIGHT>& state)
		{
			return state.GetWinner();
		}

		static gadt::AgentIndex StateToResult(const DabState<WIDTH, HEIGHT>& state, gadt::AgentIndex winner)
		{
			return winner;
		}

		static bool AllowUpdateValue(const DabState<WIDTH, HEIGHT>& state, const gadt::AgentIndex& result)
		{
			if (result == 0x1 && state.is_fir_player() == true)
				return true;
			else if (result == 0x2 && state.is_fir_player() == false)
				return true;
			return false;
		}

		static int EvaluateState(const DabState<WIDTH, HEIGHT>& state, const gadt::AgentIndex player_index)
		{
			if(state.is_fir_player())
				return state.boxes_margin();
			return -state.boxes_margin();
		}

		static std::string StateToString(const DabState<WIDTH, HEIGHT>& state)
		{
			return gadt::ToString(state.board().to_ullong());
		}

		static std::string ActionToString(const DabAction& action)
		{
			std::stringstream ss;
			ss << "{ ";
			for (size_t i = 0; i < action.upper_bound(); i++)
			{
				if (action[i])
					ss << i << ", ";
			}
			ss << "}";
			return ss.str();
		}

		static std::string ResultToString(const gadt::AgentIndex& winner)
		{
			if (winner == 0x0)
				return "no winner";
			else if (winner == 0x1)
				return "player 0 win";
			else if (winner == 0x2)
				return "player 1 win";
			return "draw";
		}
	};
}

