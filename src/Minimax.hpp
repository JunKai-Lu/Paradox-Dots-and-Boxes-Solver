#include "GameStreamDefine.h"
#include "../lib/gadt/src/minimax.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabMinimaxNode
	{
	public:
		using State = DabState<WIDTH, HEIGHT>;
		using pointer = DabMinimaxNode*;
		using reference = DabMinimaxNode&;
		using Node = DabMinimaxNode;
		using ActionList = DabActionList;
		using FuncPackage = DabGameFuncPackage<WIDTH, HEIGHT>;

	private:
		const State			  _state;		//game state
		const size_t		  _depth;		//depth of the node
		ActionList			  _action_list;	//action set
		gadt::AgentIndex	  _winner;		//winner of the node.

	private:

		//node initialize
		inline void NodeInit()
		{
			_winner = FuncPackage::DetermineWinner(_state);
			FuncPackage::MakeAction(_state, _action_list);
		}

	public:
		//constructor
		DabMinimaxNode(const State& new_state, size_t depth) :
			_state(new_state),
			_depth(depth),
			_action_list(),
			_winner()
		{
			NodeInit();
		}

		//return the count of actions
		inline size_t action_count() const
		{
			return _action_list.size();
		}

		inline const DabAction& action(size_t index) const
		{
			return _action_list[index];
		}

		//get state of the node.
		inline const State& state() const
		{
			return _state;
		}

		//get depth of current node.
		inline size_t depth() const
		{
			return _depth;
		}

		//get the action set of the node.
		inline const ActionList& action_list() const
		{
			return _action_list;
		}

		//get winner of state.
		inline gadt::AgentIndex winner() const
		{
			return _winner;
		}

		//return true if the node is terminal.
		inline bool is_terminal_state() const
		{
			return _winner != 0x0;
		}
	};

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabMinimaxSearch
	{
	public:

		using Node = DabMinimaxNode<WIDTH, HEIGHT>;
		using State = typename Node::State;
		using FuncPackage = typename Node::FuncPackage;
		using ActionList = typename Node::ActionList;

	private:

		size_t _max_depth;

	private:

		//get the highest eval for parent of node.
		template<bool ALPHABETA_ENABLED>
		MarginType NegamaxEvaluateStates(const Node& node, MarginType alpha, MarginType beta, size_t& leaf_node_count)
		{
			const size_t original_leaf_node_count = leaf_node_count;

			if (node.depth() == 0 || node.is_terminal_state())
			{
				//get the eavl of the parent node.
				leaf_node_count++;
				MarginType eval = FuncPackage::EvaluateState(node.state(), node.winner());
				return eval;
			}
			//pick up best value in child nodes.
			MarginType best_value = alpha;
			for (size_t i = 0; i < node.action_list().size(); i++)
			{
				State child_state = node.state();
				FuncPackage::UpdateState(child_state, node.action_list()[i]);
				Node child(child_state, node.depth() - 1);
				MarginType child_value = -NegamaxEvaluateStates<ALPHABETA_ENABLED>(child, -beta, -alpha, leaf_node_count);
				if (ALPHABETA_ENABLED && child_value >= beta)
					return beta;//prune!
				if (child_value >= best_value) { best_value = child_value; }
			}
			return best_value;
		}

		//start a negamax search.
		template<bool ALPHABETA_ENABLED>
		std::pair<DabAction, MarginType> StartNegamaxIteration(const State& state)
		{
			Node root(state, _max_depth);
			std::vector<MarginType> eval_set(root.action_list().size(), MarginType());
			size_t leaf_node_count = 0;

			MarginType best_value = (INT8_MIN + 1);
			size_t best_action_index = 0;
			for (size_t i = 0; i < root.action_list().size(); i++)
			{
				//create new node.
				State child_state = state;
				FuncPackage::UpdateState(child_state, root.action_list()[i]);
				Node child(child_state, root.depth() - 1);
				MarginType child_beta = (ALPHABETA_ENABLED ? -best_value : (INT8_MAX -1 ));
				eval_set[i] = -NegamaxEvaluateStates<ALPHABETA_ENABLED>(child, (INT8_MIN + 1), child_beta, leaf_node_count);
				if (eval_set[i] > best_value)
				{
					best_action_index = i;
					best_value = eval_set[i];
				}
			}
			//return best action.
			return { root.action_list()[best_action_index], best_value };
		}

	public:
		//constructor func.
		DabMinimaxSearch(size_t max_depth) :
			_max_depth(max_depth)
		{
		}

		//excute nega minimax search
		DabAction RunNegamax(const State& state)
		{
			return StartNegamaxIteration<false>(state).first;
		}

		//excute alpha-beta search.
		DabAction RunAlphabeta(const State& state)
		{
			return StartNegamaxIteration<true>(state).first;
		}

		MarginType GetEvalValue(const State& state)
		{
			return StartNegamaxIteration<true>(state).second;
		}
	};

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	MarginType EvalMinimax(const DabState<WIDTH, HEIGHT>& state)
	{
		DabMinimaxSearch<WIDTH, HEIGHT> mm(EdgeCount<WIDTH, HEIGHT>());
		return mm.GetEvalValue(state);
	}
}
