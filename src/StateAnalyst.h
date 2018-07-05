#include "GameDefine.h"

#pragma once

namespace dots_and_boxes
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
	namespace state_type
	{
		enum StateType
		{
			FRONT,
			REAR,
			DEAD_BOX,
			DEAD_CHAIN,
			FINISH
		};

		inline std::string ToString(StateType st)
		{
			RETURN_STRINGFY(st, FRONT)
			RETURN_STRINGFY(st, REAR)
			RETURN_STRINGFY(st, DEAD_BOX)
			RETURN_STRINGFY(st, DEAD_CHAIN)
			RETURN_STRINGFY(st, FINISH)
			return "";
		}
	}

	//chain type is used to difine the prop of chain.
	namespace chain_type
	{
		enum ChainType
		{
			UNDEFINED = 0,
			CHAIN = 1,
			CIRCLE = 2,
			OPEN_CHAIN = 3,
			OPEN_CIRCLE = 4,
			DEAD_CHAIN = 5,
			DEAD_CIRCLE = 6
		};

		inline std::string ToString(ChainType ct)
		{
			RETURN_STRINGFY(ct, UNDEFINED)
			RETURN_STRINGFY(ct, CHAIN)
			RETURN_STRINGFY(ct, CIRCLE)
			RETURN_STRINGFY(ct, OPEN_CHAIN)
			RETURN_STRINGFY(ct, OPEN_CIRCLE)
			RETURN_STRINGFY(ct, DEAD_CHAIN)
			RETURN_STRINGFY(ct, DEAD_CIRCLE)
			return "";
		}
	}

	//front state include some functin that is used for front state.
	namespace action_analyst
	{
		//Get free actions.
		ActionVec GetFreeActions(const Board& board);

		//get the actions that appointed the edge can take the fir dead box.
		ActionVec GetFirDeadBoxAction(const Board& board);

		//get the possible actions for dead chain.
		ActionVec GetDeadChainAction(const Board& board);
	}

	//rear state include some functin that is used for rear state.
	namespace rear_state
	{
		//box info that are included in chain analyst.
		class BoxInfo:public state::Box
		{
		private:
			size_t _belonging_chain;

		public:
			BoxInfo(const Board& board, Edge index);
			
			inline size_t belonging_chain()
			{
				return _belonging_chain;
			}
			inline void set_belonging_chain(size_t index)
			{
				_belonging_chain = index;
			}
			inline bool NoBelongingChain()
			{
				return _belonging_chain == MAX_CHAIN;
			}
			

		};

		//chain info that are included in chain analyst.
		class ChainInfo
		{
		private:
			chain_type::ChainType _type;
			size_t _boxes_num;

		public:
			inline ChainInfo() :
				_type(chain_type::UNDEFINED),
				_boxes_num(0)
			{

			}
			inline chain_type::ChainType type()
			{
				return _type;
			}
			inline size_t boxes_num()
			{
				return _boxes_num;
			}
			inline void set_type(chain_type::ChainType type)
			{
				_type = type;
			}
			inline void add_box_num(size_t num = 1)
			{
				_boxes_num += num;
			}
			inline void Clear()
			{
				_type = chain_type::UNDEFINED;
				_boxes_num = 0;
			}
		};

		//an class that is used for analysing chain in a rear-state.
		class ChainAnalyst
		{
		public:
			ChainAnalyst(Board board);

		private:
			Board _board;
			BoxInfo _boxes[MAX_BOX];
			ChainInfo _chains[MAX_CHAIN];

			//analysis chains in this state.
			void AnalysisChains();

			//get first undefined chain index.
			inline Edge GetFirUndefinedChainIndex()
			{
				for (Edge i = 0; i < MAX_CHAIN; i++)
				{
					if (_chains[i].type() == chain_type::UNDEFINED)
					{
						return i;
					}
				}
				gadt::console::PrintError("chain list is full!");
				return 0;
			}

			//add box to the chain.
			inline void AddBoxToChain(size_t box, size_t chain)
			{
				_boxes[box].set_belonging_chain(chain);
				_chains[chain].add_box_num();
			}

			//register a chain from a box.
			void RegisterChainFromBox(Edge start_box, Edge fir_box, Edge ignore_edge);

			//register a circle from a box.
			void RegisterCircleFromBox(Edge start_box, Edge fir_box, Edge ignore_edge);

			//merge two chains into a single chain.
			void MergeChain(size_t fir_chain, size_t sec_chain, chain_type::ChainType new_chain_type);

			//combine two chains from a source box they shared. the box must be the source of 3 or more chains.
			void CombineChainsFromSourceBox(Edge box_index);

			//get the first edge of appointed chain.
			Edge GetFirstEdgeOfChain(size_t chain_index);

		public:

			ActionVec result();

			//show boxes info.
			inline void ShowBoxInfo()
			{
				for (auto& box : _boxes)
				{
					if (box.index() % 5 == 0)
					{
						std::cout << std::endl;
					}
					std::cout << box.type() << " ";
				}
			}

			//show chains info.
			void ShowChainInfo();
		};
	}

	//class Analyst is basic class of Action Analyst and Retro Analyst.
	class Analyst
	{
	protected:
		const Board _board;		//the board of this analyst.
		ActionVec _result;		//the result of analyst.
		state_type::StateType _state_type;	//state type of the board.

		Analyst(Board board);

	public:
		//determind the type of this state.
		static state_type::StateType DetermindStateType(const Board& board);

		//get the result of analyst.
		inline ActionVec result() const
		{
			return _result;
		}

		//get the board of this analyst.
		inline Board board() const
		{
			return _board;
		}

		//get the state type of the board in this analyst.
		inline state_type::StateType state_type() const
		{
			return _state_type;
		}

		//get whether appointed edge is legal move.
		inline bool operator[](Edge edge) const
		{
			return _result[edge];
		}
	};

	//a action analyst is used for generate reasonable actions with filter.
	class ActionAnalyst :public Analyst
	{
	public:
		ActionAnalyst(Board board);

		//return random legal action.
		Edge RandomAction();

		//visualization
		void Print(Edge tag_edge = MAX_EDGE) const;
	};

	//class retro analyst offers possible actions of retro analysis.
	class RetroAnalyst :public Analyst
	{
	public:
		RetroAnalyst(Board board);
	};
}