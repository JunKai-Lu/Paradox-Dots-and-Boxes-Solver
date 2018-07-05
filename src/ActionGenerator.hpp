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

		const DabState<WIDTH, HEIGHT>&	_state;
		StateType						_state_type;
		DabActionList					_actions;

	private:

		StateType GetStateType(const DabState<WIDTH, HEIGHT>& state)
		{
			if (state.board().edge_count() == EdgeCount<WIDTH, HEIGHT>())
			{
				return StateType::Finish;
			}
			if (state.board().ExistDeadBox())
			{
				return StateType::DeadBox;
			}
			if (state.board().ExistDeadChain())
			{
				return StateType::DeadChain;
			}
			if (state.board().ExistFreeEdge())//front state.
			{
				return StateType::Front;
			}
			return StateType::Rear;
		}

		//Get free actions.
		static DabActionList MakeFreeActions(const DabState<WIDTH, HEIGHT>& state)
		{
			DabActionList actions;
			for (EdgeIndex i = 0; i < EdgeCount<WIDTH, HEIGHT>(); i++)
			{
				if (state.board().IsFreeEdge(i))
				{
					actions.push_back(DabAction());
					actions.back().set(i);
				}
			}
			return actions;
		}

		//get the actions that appointed the edge can take the fir dead box.
		static void MakeFirDeadBoxAction(const DabState<WIDTH, HEIGHT>& state)
		{
			DabActionList actions;
			EdgeIndex fir = state.board().GetFirstFreeEdgeInDeadBox();
			GADT_WARNING_IF(DAB_WARNING, (fir == EdgeCount<WIDTH, HEIGHT>()), "no dead box exist");
			actions.push_back(DabAction{ fir });
			return actions;
		}

		static void MakeChainActions(const DabState<WIDTH, HEIGHT>& state)
		{
			ChainAnalyst<WIDTH, HEIGHT> ca(state.board());
			return ca.GetResult();
		}

		//get the possible actions for dead chain.
		static void MakeDeadChainAction(const DabState<WIDTH, HEIGHT>& state)
		{
			DabBoard<WIDTH, HEIGHT> temp_board = _state.board();
			EdgeIndex fir = _state.board().GetFirstFreeEdgeInDeadChain();
			_actions.push_back({ fir });
			temp_board.set_edge(fir);
			if (!temp_board.ExistDeadChain())
			{
				//find next edge.
				if (temp_board.is_he(fir))
				{
					//horizon edge.
					if (temp_board.is_bhe(fir))
					{
						//check upper box.
						DabBox<WIDTH, HEIGHT> box(temp_board, fir - WIDTH);
						if (box.type() == DabBoxType::DEAD_BOX)
						{
							for (size_t i = 0; i < 4; i++)
							{
								if (!temp_board.edge_exist(box.own_edge(i)))
								{
									_actions.push_back({ box.own_edge(i) });
									return;
								}
							}
						}
					}
					if (temp_board.is_the(fir))
					{
						//check lower box.
						DabBox<WIDTH, HEIGHT> box(temp_board, fir);
						if (box.type() == DabBoxType::DEAD_BOX)
						{
							for (size_t i = 0; i < 4; i++)
							{
								if (!temp_board.edge_exist(box.own_edge(i)))
								{
									_actions.push_back({ box.own_edge(i) });
									return;
								}
							}
						}
					}
				}
				else if(temp_board.is_ve(fir))
				{
					//vecitical edge.
					if (temp_board.is_lve(fir))
					{
						//check right box.
						DabBox<WIDTH, HEIGHT> box(temp_board, temp_board.lve_to_the(fir));
						if (box.type() == DabBoxType::DEAD_BOX)
						{
							for (size_t i = 0; i < 4; i++)
							{
								if (!temp_board.edge_exist(box.own_edge(i)))
								{
									_actions.push_back({ box.own_edge(i) });
									return;
								}
							}
						}

					}
					if (temp_board.is_rve(fir))
					{
						//check left box.
						DabBox<WIDTH, HEIGHT> box(temp_board, temp_board.lve_to_the(temp_board.rve_to_lve(fir)));
						if (box.type() == DabBoxType::DEAD_BOX)
						{
							for (size_t i = 0; i < 4; i++)
							{
								if (!temp_board.edge_exist(box.own_edge(i)))
								{
									_actions.push_back({ box.own_edge(i) });
									return;
								}
							}
						}
					}
				}
			}
		}

		//make all actions according to the type of state.
		void MakeAction()
		{
			if (_state_type == StateType::DeadBox)
			{
				MakeFirDeadBoxAction();
			}
			else if (_state_type == StateType::Front)
			{
				MakeFreeActions();
			}
			else if (_state_type == StateType::Rear)
			{
				MakeChainActions();
			}
			else if (_state_type == StateType::DeadChain)
			{
				MakeDeadChainAction();;
			}
		}

	public:

		DabActionGenerator(const DabState<WIDTH, HEIGHT> state):
			_state(state),
			_state_type(GetStateType(state)),
			_actions()
		{
			MakeAction();
		}

		inline StateType state_type() const
		{
			return _state_type;
		}

		inline std::vector<DabMove> actions() const
		{
			return _actions;
		}

		inline DabMove random_action() const
		{
			return gadt::func::GetRandomElement(_actions);
		}
	};
}
