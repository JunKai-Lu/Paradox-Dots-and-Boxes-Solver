#include "StateDefine.hpp"
#include "ChainAnalyst.hpp"

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
		Front,
		Rear,
		DeadBox,
		DeadChain,
		Finish
	};

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabActionGenerator
	{
	private:

		DabActionList _actions;

	private:

		static StateType GetStateType(const DabBoard<WIDTH, HEIGHT>& board)
		{
			if (board.edge_count() == EdgeCount<WIDTH, HEIGHT>())
			{
				return StateType::Finish;
			}
			if (board.ExistDeadBox())
			{
				return StateType::DeadBox;
			}
			if (board.ExistDeadChain())
			{
				return StateType::DeadChain;
			}
			if (board.ExistFreeEdge())//front state.
			{
				return StateType::Front;
			}
			return StateType::Rear;
		}

		//Get free actions.
		static DabActionList MakeFreeActions(const DabBoard<WIDTH, HEIGHT>& board)
		{
			DabActionList actions;

			for (EdgeIndex i = 0; i < EdgeCount<WIDTH, HEIGHT>(); i++)
			{
				if (board.IsFreeEdge(i))
				{
					actions.push_back(DabAction());
					actions.back().set(i);
				}
			}

			if (BoxCount<WIDTH, HEIGHT>() - board.ExistingBoxCount() < 8)
			{
				EdgeIndex inner_circle = board.GetInnerOpenCircleActions();
				if (inner_circle != EdgeCount<WIDTH, HEIGHT>())
				{
					actions.push_back(DabAction());
					actions.back().set(inner_circle);
				}
			}
			return actions;
		}

		//get the actions that appointed the edge can take the fir dead box.
		static DabActionList MakeDeadBoxAction(const DabBoard<WIDTH, HEIGHT>& board)
		{
			
			EdgeIndex fir = board.GetFirstFreeEdgeInDeadBox();
			GADT_WARNING_IF(DAB_WARNING, (fir == EdgeCount<WIDTH, HEIGHT>()), "no dead box exist");
			auto temp_board = board;
			temp_board.set_edge(fir);
			DabActionList actions = MakeAction(temp_board);
			if (actions.size() == 0)//finish state
			{
				actions.push_back(DabAction{ fir });
			}
			else
			{
				for (auto& action : actions)
					action.set(fir);
			}
			return actions;
		}

		static DabActionList MakeChainActions(const DabBoard<WIDTH, HEIGHT>& board)
		{
			ChainAnalyst<WIDTH, HEIGHT> ca(board);
			return ca.GetResult();
		}

		//get the possible actions for dead chain.
		static DabActionList MakeDeadChainAction(const DabBoard<WIDTH, HEIGHT>& board)
		{
			DabBoard<WIDTH, HEIGHT> temp_board = board;
			DabAction prev_action;
			DabAction sacrifice_action;
			for (;;)
			{
				EdgeIndex fir = temp_board.GetFirstFreeEdgeInDeadChain();
				temp_board.set_edge(fir);
				if (!temp_board.ExistDeadChain())
				{
					EdgeIndex sec = temp_board.GetAnotherEdgeThatOwnedBySameBox(fir);
					prev_action.set(sec);
					sacrifice_action = prev_action;
					prev_action.set(fir);
					temp_board.set_edge(sec);
					break;
				}
				else
				{
					prev_action.set(fir);
				}
			}
			
			//actions.push_back(DabAction{ fir });
			DabActionList actions = MakeAction(temp_board);
			if (actions.size() == 0)//finish state
			{
				actions.push_back(prev_action);
			}
			else
			{
				for (auto& action : actions)
					action |= prev_action;
			}
			actions.push_back(sacrifice_action);
			return actions;
		}

		//make all actions according to the type of state.
		static DabActionList MakeAction(const DabBoard<WIDTH, HEIGHT>& board)
		{
			StateType state_type = GetStateType(board);
			if (state_type == StateType::DeadBox)
			{
				return MakeDeadBoxAction(board);
			}
			else if (state_type == StateType::Front)
			{
				return MakeFreeActions(board);
			}
			else if (state_type == StateType::Rear)
			{
				return MakeChainActions(board);
			}
			else if (state_type == StateType::DeadChain)
			{
				return MakeDeadChainAction(board);;
			}
			return DabActionList();
		}

		void FileterActions(const DabBoard<WIDTH, HEIGHT>& board)
		{
			std::set<BoardValueType> existing_state;
			DabActionList actions;
			for (auto action : _actions)
			{
				DabBoard<WIDTH, HEIGHT> temp_board = board;
				temp_board.TakeActions(action);
				temp_board = temp_board.ToMinimalFormat();
				if (existing_state.count(temp_board.to_ullong()) == 0)
				{
					existing_state.insert(temp_board.to_ullong());
					actions.push_back(action);
				}
			}
			_actions = actions;
		}

	public:

		DabActionGenerator(const DabState<WIDTH, HEIGHT>& state):
			_actions(MakeAction(state.board()))
		{
			//FileterActions(state.board());
		}

		inline DabActionList actions() const
		{
			return _actions;
		}

		inline DabAction random_action() const
		{
			return gadt::func::GetRandomElement(_actions);
		}
	};
}
