#include "GameDefine.h"
#include "StateAnalyst.h"
#include "Solver.h"
#include "StateDefine.hpp"
#include "LayerConatiner.hpp"
#include "DbController.hpp"
#include "RetrospectMapper.h"
#include "RetrospectReducer.h"
#include "Minimax.hpp"
#include "ActionGenerator.hpp"

namespace dots_and_boxes_solver
{
	//test codes.
	void DabTest()
	{
		DabState<5, 5> state;
		state.BeFull();
		auto temp = state.board();
		temp.reset_edge(0);
		state = DabState<5, 5>(temp, 0);
		std::cout << (int)EvalMinimax<5, 5>(state) << std::endl;

	}

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	std::string GameFeatureText()
	{
		return gadt::ToString(WIDTH) + gadt::ToString(HEIGHT);
	}

	//generate a game with defined width and height.
	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	void InitDbControllerPage(gadt::shell::GameShell& dab)
	{
		using Controller = DbController<WIDTH, HEIGHT>;
		auto db = dab.CreateShellPage<Controller>("db" + GameFeatureText<WIDTH, HEIGHT>());

		//cmd 'info'， prin info of all layers.
		db->AddFunction("list", "print info of all layers", [](Controller& controller)->void {
			controller.PrintInfo();
		});

		//cmd 'map'
		db->AddFunction("map", "start 'map' operation in current layer", [](Controller& controller, const gadt::shell::ParamsList& params)->bool {
			size_t thread_count = 1;
			if (controller.focus_layer_index() == EdgeCount<WIDTH, HEIGHT>())
			{
				gadt::console::PrintError("illegal layer index");
				return false;
			}
			if (params.size() == 0)
				thread_count = gadt::console::GetInput<size_t>("Input thread count >>");
			else if (params.size() == 1)
			{
				thread_count = gadt::ToSizeT(params[0]);
				if (thread_count == 0 || thread_count > 8)
					return false;
			}
			else
			{
				return false;
			}
			controller.RunMapProcess(thread_count);
			return true;
		});

		//cmd 'reduce'
		db->AddFunction("reduce", "start 'reduce' operation in current layer", [](Controller& controller, const gadt::shell::ParamsList& params)->bool {
			size_t thread_count = 1;
			size_t partition_count = 1;
			if (params.size() == 0)
			{
				thread_count = gadt::console::GetInput<size_t>("Input thread count >>");
				partition_count = gadt::console::GetInput<size_t>("Input partition count >>");
			}
			else if (params.size() == 2)
			{
				thread_count = gadt::ToSizeT(params[0]);
				partition_count = gadt::ToSizeT(params[1]);
				if (thread_count == 0 || thread_count > 8)
					return false;
				if (partition_count == 0 || partition_count > RETROSPCECT_MAX_PARTITION_COUNT)
					return false;
			}
			else
			{
				return false;
			}
			controller.RunReduceProcess(thread_count, partition_count);
			return true;
		});

		//cmd 'checkout', allow user to change the layer they control
		db->AddFunction("checkout", "switch to another layer", [](Controller& controller, const gadt::shell::ParamsList& params)->bool {
			if (params.size() == 1)
			{
				size_t layer = gadt::ToSizeT(params[0]);
				if (layer >= 0 && layer <= EdgeCount<WIDTH, HEIGHT>() )
				{
					controller.checkout(layer);
					gadt::console::PrintMessage("checkout to layer " + params[0]);
					return true;
				}
			}
			gadt::console::PrintError("unexceted parameters");
			return false;
		});

		//cmd 'next', checkout to next layer.
		db->AddFunction("next", "switch to next layer", [](Controller& controller)->void {
			size_t layer = controller.focus_layer_index();
			if (layer > 0 && layer <= EdgeCount<WIDTH, HEIGHT>())
			{
				controller.checkout(layer - 1);
				gadt::console::PrintMessage("checkout to layer " + gadt::ToString(layer - 1));
			}
			else
			{
				gadt::console::PrintError("unexceted layer index.");
			}
		});

		//cmd 'print', print states of raw/partitions.
		db->AddFunction("print", "print items in file.", [](Controller& controller, const gadt::shell::ParamsList& params)->bool {
			if (params.size() >= 2)
			{
				bool minimax = false;
				if (params.size() == 3)
					if (params[2] == "minimax")
						minimax = true;
				size_t index = gadt::ToSizeT(params[1]);
				if (params[0] == "raw")
				{
					controller.PrintRaw(index, minimax);
					return true;
				}
				else if (params[0] == "part")
				{
					controller.PrintPartition(index, minimax);
					return true;
				}
			}
			return false;
		});

		//cmd 'clear', clear db/
		db->AddFunction("cleardb", "clear game db", [](Controller& controller, const gadt::shell::ParamsList& params)->bool {
			if (params.size() == 1)
			{
				std::string str = gadt::console::GetInput("Please reinput target database to confirm your operation >>");
				if (str == params[0])
				{
					if (params[0] == "all")
					{
						controller.ClearDB();
						gadt::console::PrintMessage("Clear DB Success!");
						gadt::console::PrintMessage("checkout to layer " + gadt::ToString(EdgeCount<WIDTH, HEIGHT>()));
						return true;
					}
					else if (params[0] == "layer")
					{
						controller.ClearLayer();
						gadt::console::PrintMessage("Clear Layer Files Success!");
						return true;
					}
					else if (params[0] == "raw")
					{
						controller.ClearRawFiles();
						gadt::console::PrintMessage("Clear Raw Files Success!");
						return true;
					}
					else if (params[0] == "part")
					{
						controller.ClearPartFiles();
						gadt::console::PrintMessage("Clear Partition Files Success!");
						return true;
					}
				}
				else
				{
					gadt::console::PrintMessage("Operation Canceled.");
					return true;
				}
			}
			return false;
		});
	}

	//generate a game with defined width and height.
	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	void InitGamePage(gadt::shell::GameShell& dab)
	{
		auto game = dab.CreateShellPage<DabState<WIDTH, HEIGHT>>("game" + GameFeatureText<WIDTH, HEIGHT>());

		auto IsEdgeIndex = [](const gadt::shell::ParamsList& params)->bool {
			if (params.size() == 1)
				if (gadt::ToSizeT(params[0]) < EdgeCount<WIDTH, HEIGHT>())
					return true;
			return false;
		};

		game->AddFunction("print", "print state/action", [](DabState<WIDTH, HEIGHT>& state, const gadt::shell::ParamsList& params)->bool {
			if (params.size() == 0)
			{
				state.Print();
				return true;
			}
			else if (params.size() == 1)
			{
				if (params[0] == "action")
					state.Print(state.board().GetFreeActionSet());
			}
			return false;
		});

		game->AddFunction("random", "set one or multi random edge", [](DabState<WIDTH, HEIGHT>& state, const gadt::shell::ParamsList& params)->bool {
			
			if (params.size() == 0)
			{
				DabActionGenerator<WIDTH, HEIGHT> ag(state);
				auto action = ag.random_action();
				DabActionSet as;
				as.set(action.edge);
				state.Update(action);
				state.Print(as);
				return true;
			}
			else if (params.size() == 1)
			{
				auto v = gadt::ToSizeT(params[0]);
				if (v <= EdgeCount<WIDTH,HEIGHT>())
				{
					for (size_t i = 0; i < v; i++)
					{
						if (state.is_finished())
							break;
						DabActionGenerator<WIDTH, HEIGHT> ag(state);
						state.Update(ag.random_action());
					}
					state.Print();
					return true;
				}
			}
			return false;
		});

		game->AddFunction("set", "set edge", [](DabState<WIDTH, HEIGHT>& state, const gadt::shell::ParamsList& params)->bool {
			state.Update(DabMove{ gadt::ToUInt8(params[0]) });
			return true;
		}, IsEdgeIndex);

		game->AddFunction("actions", "get actions of current state", [](DabState<WIDTH, HEIGHT>& state)->void {
			DabActionGenerator<WIDTH, HEIGHT> ag(state);
			state.Print(ag.actions());
		});

		game->AddFunction("eval", "get evaluation of current state", [](DabState<WIDTH, HEIGHT>& state)->void {
			if (state.board().edge_count() < 35)
			{
				gadt::console::PrintMessage("unable to do this");
				return;
			}
			std::cout << "eval = " << (int)EvalMinimax<WIDTH, HEIGHT>(state) << std::endl;
		});
	}

	//init shell
	void ShellInit()
	{
		gadt::shell::GameShell dab("Dots-And-Boxes");
		dab.SetDefaultInfoFunc([]() {
			using namespace gadt::console;
			Cprintf("=============================================\n", ConsoleColor::Gray);
			Cprintf("    PARADOX Dots and Boxes\n", ConsoleColor::Blue);
			Cprintf("    Copyright @ Junkai-Lu 2018\n", ConsoleColor::Blue);
			Cprintf("=============================================\n\n", ConsoleColor::Gray);
		});
		auto* root = dab.CreateShellPage("root");
		auto* db = dab.CreateShellPage("db");
		auto* game = dab.CreateShellPage("game");
		InitGamePage<5, 5>(dab);			//create 5x5 game named 'dab55'
		InitDbControllerPage<5, 5>(dab);	//create db for 5x5 game.
		root->AddChildPage("db", "endgame database");
		root->AddChildPage("game", "endgame database");
		root->AddFunction("test", "run test function", DabTest);

		db->AddChildPage("db55", "5x5 game");

		game->AddChildPage("game55", "5x5 game");

		dab.StartFromPage("root", "./game/game55");
	}
}

int main()
{
	dots_and_boxes_solver::ShellInit();
	return 0;
}

