#include "GameDefine.h"
#include "StateAnalyst.h"
#include "Solver.h"
#include "StateDefine.hpp"
#include "LayerConatiner.hpp"
#include "DbController.hpp"
#include "RetrospectMapper.h"
#include "RetrospectReducer.h"
#include "Minimax.hpp"

namespace dots_and_boxes_solver
{
	//test codes.
	void DabTest()
	{
		DabState<5, 5> state;
		for (size_t i = 0; i < 60; i++)
		{
			state.SetRandomEdge();
			state.Visualization(state.board().GetFreeActionSet());
			gadt::console::SystemClear();
		}
		
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
			if (params.size() == 2)
			{
				size_t index = gadt::ToSizeT(params[1]);
				if (params[0] == "raw")
				{
					controller.PrintRaw(index);
					return true;
				}
				else if (params[0] == "part")
				{
					controller.PrintPartition(index);
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
		auto game = dab.CreateShellPage<DabState<WIDTH, HEIGHT>>("dab" + GameFeatureText<WIDTH, HEIGHT>());

		game->AddFunction("print", "print state/action", [](DabState<WIDTH, HEIGHT>& state, const gadt::shell::ParamsList& params)->bool {
			if (params.size() == 0)
			{
				state.Visualization();
				return true;
			}
			else if (params.size() == 1)
			{
				if (params[0] == "action")
					state.Visualization(state.board().GetFreeActionSet());
			}
			return false;
		});

		game->AddFunction("random", "set one or multi random edge", [](DabState<WIDTH, HEIGHT>& state, const gadt::shell::ParamsList& params)->bool {
			if (params.size() == 0)
			{
				state.Visualization();
				return true;
			}
			else if (params.size() == 1)
			{
				if (params[0] == "action")
					state.Visualization(state.board().GetFreeActionSet());
			}
			return false;
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
		InitGamePage<5, 5>(dab);			//create 5x5 game named 'dab55'
		InitDbControllerPage<5, 5>(dab);	//create db for 5x5 game.
		root->AddChildPage("dab55", "5x5 dots and boxes game");
		root->AddChildPage("db", "endgame database");
		root->AddFunction("test", "run test function", DabTest);
		db->AddChildPage("db55", "5x5 game");
		dab.StartFromPage("root", "./db/db55");
	}
}

int main()
{
	dots_and_boxes_solver::ShellInit();
	return 0;
}

