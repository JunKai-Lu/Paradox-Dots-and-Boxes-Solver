#include "StateDefine.hpp"
#include "GameStreamDefine.h"

#pragma once

namespace dots_and_boxes_solver
{
	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	DabAction DabMcts(const DabState<WIDTH, HEIGHT>& state)
	{
		gadt::mcts::LockFreeMCTS<DabState<WIDTH, HEIGHT>, DabAction, gadt::AgentIndex, false> mcts(
			DabGameFuncPackage<WIDTH, HEIGHT>::UpdateState,
			DabGameFuncPackage<WIDTH, HEIGHT>::MakeAction,
			DabGameFuncPackage<WIDTH, HEIGHT>::DetermineWinner,
			DabGameFuncPackage<WIDTH, HEIGHT>::StateToResult,
			DabGameFuncPackage<WIDTH, HEIGHT>::AllowUpdateValue
		);
		gadt::mcts::MctsSetting setting;
		setting.max_node_per_thread = 1000000;
		setting.max_iteration_per_thread = 1000000;
		setting.max_thread = 4;
		setting.no_winner_index = 0x0;
		setting.simulation_warning_length = 60;
		setting.timeout = 30;
		mcts.InitLog(
			DabGameFuncPackage<WIDTH, HEIGHT>::StateToString,
			DabGameFuncPackage<WIDTH, HEIGHT>::ActionToString,
			DabGameFuncPackage<WIDTH, HEIGHT>::ResultToString
		);
		mcts.EnableLog();
		return mcts.Run(state, setting);
	}
}
