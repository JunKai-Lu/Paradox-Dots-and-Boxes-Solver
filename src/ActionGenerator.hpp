#include "StateDefine.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	/*
	free-edge	dead-box		dead-chain

	FrontState						Yes			No			No
	FrontStateWithDeadBox			Yes			Yes			No
	RearState						No			No			No
	RearStateWithDeadBox			No			Yes			No
	RearStateWithDeadChain			No			No			Yes

	*/

	//state type means different period of state.
	enum class StateType
	{
		FRONT,
		REAR,
		DEAD_BOX,
		DEAD_CHAIN,
		FINISH
	};

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabActionGenerator
	{
	private:

		const DabState<WIDTH, HEIGHT>&	_state;
		StateType						_state_type;
		std::vector<EdgeIndex>			_actions;

	private:

		StateType GetStateType(const DabState<WIDTH, HEIGHT>& state)
		{

		}

	public:

		DabActionGenerator()
		{

		}

		inline StateType state_type() const
		{
			return _state_type;
		}

		inline std::vector<EdgeIndex> actions() const
		{
			return _actions;
		}
	};
}
