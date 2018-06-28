#include "GameStreamDefine.h"
#include "../lib/gadt/src/minimax.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	void RunMinimax(const DabState<WIDTH, HEIGHT>& state)
	{
		gadt::minimax::MinimaxSearch<DabState<WIDTH, HEIGHT>, DabMove> mm(
			DabGameFuncPackage<WIDTH, HEIGHT>::UpdateState,
			DabGameFuncPackage<WIDTH, HEIGHT>::MakeAction,
			DabGameFuncPackage<WIDTH, HEIGHT>::DetermineWinner,
			DabGameFuncPackage<WIDTH, HEIGHT>::EvalForParent
		);

	}
}
