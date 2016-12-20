#include "stdafx.h"

#include "GameDefine.h"

#pragma  once

namespace dots_and_boxes
{
	namespace game
	{
		namespace mcts
		{
			ActionDes MctsRespond(State& state);
		}

		namespace human
		{
			ActionDes HumanRespond(State& state);
		}

		namespace random
		{
			ActionDes RandomRespond(State& state);
		}
	}
}