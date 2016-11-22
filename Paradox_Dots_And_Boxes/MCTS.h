#include "stdafx.h"

#include "GameDefine.h"

#pragma  once

namespace DAB
{
	namespace GAME
	{
		namespace MCTS
		{
			ActionDes MctsRespond(State& state);
		}

		namespace HUMAN
		{
			ActionDes HumanRespond(State& state);
		}

		namespace RANDOM
		{
			ActionDes RandomRespond(State& state);
		}
	}
}