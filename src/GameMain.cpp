#include "GameDefine.h"
#include "StateAnalyst.h"
#include "Solver.h"
#include "StateDefine.hpp"
#include "LayerConatiner.hpp"
#include "DbController.hpp"
#include "RetrospectMapper.h"
#include "RetrospectReducer.h"

namespace dots_and_boxes_solver
{
	//test codes.
	void DabTest()
	{
		DabState<5, 5> dab55;
		
		for (EdgeIndex edge = 0; edge < EdgeCount<5,5>()/2; edge++)
		{
			dab55.SetRandomEdge();
		}
		dab55.Visualization();
		auto dr = DabState<5,5>(dab55.board().DiagonalReverse(), 0);
		dr.Visualization();
		//auto rotate = DabState<5, 5>(dab55.board().Rotate(),0);
		//rotate.Visualization();
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
		db->AddFunction("info", "print info of all layers", [](Controller& controller)->void {
			controller.PrintInfo();
		});

		//cmd 'map'
		db->AddFunction("map", "start 'map' operation in current layer", [](Controller& controller)->void {
			if (controller.focus_layer_index() == EdgeCount<WIDTH, HEIGHT>())
			{
				gadt::console::PrintError("illegal layer index");
				return;
			}
			size_t thread_count = gadt::console::GetInput<size_t>("Input thread count >>");
			controller.RunMapProcess(thread_count);
		});

		//cmd 'reduce'
		db->AddFunction("reduce", "start 'reduce' operation in current layer", [](Controller& controller)->void {
			size_t thread_count = gadt::console::GetInput<size_t>("Input thread count >>");
			size_t partition_count = gadt::console::GetInput<size_t>("Input partition count >>");
			controller.RunReduceProcess(thread_count, partition_count);
		});

		//cmd 'checkout', allow user to change the layer they control
		db->AddFunction("checkout", "switch to another layer", [](Controller& controller, const gadt::shell::ParamsList& params)->bool {
			if (params.size() == 1)
			{
				size_t layer = gadt::ToSizeT(params[0]);
				if (layer >= 0 && layer <= EdgeCount<WIDTH, HEIGHT>() )
				{
					controller.checkout(layer);
					return true;
				}
			}
			gadt::console::PrintError("unexceted parameters");
			return false;
		});

		db->AddFunction("next", "switch to next layer", [](Controller& controller)->void {
			size_t layer = controller.focus_layer_index();
			if (layer > 0 && layer <= EdgeCount<WIDTH, HEIGHT>())
			{
				controller.checkout(layer - 1);
			}
			else
			{
				gadt::console::PrintError("unexceted layer index.");
			}
		});

		//cmd 'clear', clear db/
		db->AddFunction("clear", "clear game db", [](Controller& controller, const gadt::shell::ParamsList& params)->bool {
			if (params.size() == 1)
			{
				std::string str = gadt::console::GetInput("Please reinput target database to confirm your operation >>");
				if (str == params[0])
				{
					if (params[0] == "all")
					{
						controller.ClearDB();
						gadt::console::PrintMessage("Clear DB Success!");
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
					else if (params[0] == "partition")
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

