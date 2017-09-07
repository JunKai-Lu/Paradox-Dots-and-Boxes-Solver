#include "GameDefine.h"
#include "StateAnalyst.h"
#include "Solver.h"
#include "./include/gameshell.h"

using namespace std;
using namespace gadt;
using namespace dots_and_boxes;

using std::cout;
using std::endl;
using std::cin;

namespace root_shell
{
	void Sample(int x)
	{
		size_t layer = solver::GetCurrentLayer();
		ifstream final_file(solver::GetFinalFileName(layer));
		for (;;)
		{
			BoardValue boardvalue;
			Margin margin;
			final_file >> boardvalue >> margin;
			State game_state = State(Board(boardvalue));
			cout << ">> state = " << endl;
			game_state.Visualization();
			cout << ">> margin = " << margin << endl;
			cout << ">> is reasonable = " << std::boolalpha << state::IsReasonable(Board(boardvalue)) << std::noboolalpha << endl;
			if (final_file.eof())
			{
				console::ShowError("no more states.");
				console::ShowMessage("sample finish.");
				break;
			}
			GameShell::InputTip("input 'return' to stop sample. or other input to continue.");
			char buffer[50];
			cin.getline(buffer, 50);
			if (string(buffer) == "return")
			{
				break;
			}
		}

	}
	void Debug(int x)
	{
	}
}
namespace game_shell
{
	/*void Show(game::GameState& game_state)
	{
	game_state.EdgeVisualization();
	cout << ">> Player 0: score = " << game_state[0]._score << " type = " << game_state[0]._des << endl;
	cout << ">> Player 1: score = " << game_state[1]._score << " type = " << game_state[1]._des << endl << endl;
	}
	void SetPlayer(game::GameState& game_state)
	{
	GameShell::InputTip("input player [0/1].");
	bool player;
	cin >> player;
	GameShell::InputTip("input type [0]HUMAN [1]MCTS");
	size_t num;
	cin >> num;
	if (num == 0)
	{
	game_state.set_player(player, { game_state[player]._score, dots_and_boxes::game::human::HumanRespond ,"Human" });
	}
	else
	{
	game_state.set_player(player, { game_state[player]._score, dots_and_boxes::game::mcts::MctsRespond ,"MCTS" });
	}
	}
	void StartAutoGame(game::GameState& game_state)
	{

	}
	void DoAI(game::GameState& game_state)
	{
	GameShell::InputTip("input player [0/1]");
	bool player;
	cin >> player;
	game::ActionDes ac = game::mcts::MctsRespond(game_state);
	game_state.DoActions(player, ac);
	Show(game_state);
	}
	void DoMan(game::GameState& game_state)
	{
	GameShell::InputTip("input player and edge");
	bool player;
	size_t edge;
	cin >> player >> edge;
	game_state.GameEdgeSet(player, (Edge)edge);
	Show(game_state);
	}
	void Cancel(game::GameState& game_state)
	{
	if (game_state.EdgeExist(game_state.last_action()))
	{
	game_state.GameEdgeRemove(game_state.last_player(), game_state.last_action());
	console::ShowMessage("cancel success!");
	}
	else
	{
	console::ShowError("cancel failed, edge do not exist!");
	}
	}*/
}
namespace solver_shell
{
	void Thread	        (solver::Solver& solver)
	{
		cout << ">> current thread num = " << solver.thread_num() << endl;
		GameShell::InputTip("input thread num");
		size_t num;
		cin >> num;
		if (num > 0 && num <= MAX_ALLOW_THREAD)
		{
			solver.set_thread_num(num);
			console::ShowMessage("set thread num success.");
		}
	}
	void FileCache      (solver::Solver& solver)
	{
		cout << ">> use file cache = " << std::boolalpha << solver.file_cache() << std::noboolalpha << endl;
		GameShell::InputTip("input y to allow file cache.");
		string str = GameShell::GetInput();
		if (str == "y")
		{
			solver.set_use_file_cache(true);
			console::ShowMessage("enable file cache.");
		}
		else
		{
			solver.set_use_file_cache(false);
			console::ShowMessage("disable file cache.");
		}
	}
	void Filter         (solver::Solver& solver)
	{
		cout << ">> use filter = " << std::boolalpha << solver.use_filter() << std::noboolalpha << endl;
		GameShell::InputTip("input y to enable filter.");
		string str = GameShell::GetInput();
		if (str == "y")
		{
			solver.set_use_filter(true);
			console::ShowMessage("enable filter.");
		}
		else
		{
			solver.set_use_filter(false);
			console::ShowMessage("disable filter.");
		}
	}
	void ChangeLayer    (solver::Solver& solver)
	{
		//cout << ">> set current layer = ";
		cout << ">> input new layer, current layer = " << solver::GetCurrentLayer() << endl << ">>> ";
		char buffer[50];
		cin.getline(buffer, 50);
		size_t layer = atoi(buffer);
		if (layer <= 60 && layer >= 1)
		{
			solver::WriteSolverData(layer, true);
			console::ShowMessage("change layer success!");
			cout << endl;
		}
		else
		{
			console::ShowError("wrong layer");
			cout << endl;
		}
	}
	void Start          (solver::Solver& solver)
	{
		//parameters check.
		console::ShowMessage("solver start");
		solver.Run();
	}
	void Set            (solver::Solver& solver)
	{
		GameShell::InputTip("input solver aim('next' of layer number)");
		char buffer[50];
		cin.getline(buffer, 50);
		size_t aim_layer;
		if (string(buffer) == "next")
		{
			aim_layer = solver::GetCurrentLayer() - 1;
		}
		else
		{
			aim_layer = atoi(buffer);
		}
		size_t current = solver::GetCurrentLayer();
		if (aim_layer < current && aim_layer >0)
		{
			console::ShowMessage("aim layer = " + console::I2S(aim_layer));
			GameShell::InputTip("input allow thread number (max = " + console::I2S(MAX_ALLOW_THREAD) + " )");
			cin.getline(buffer, 50);
			size_t thread_num = atoi(buffer);
			if (thread_num <= MAX_ALLOW_THREAD && thread_num > 0)
			{
				console::ShowMessage("thread num = " + console::I2S(thread_num));
				bool use_file_cache;
				GameShell::InputTip("input 'y' to use file cache.");
				cin.getline(buffer, 50);
				if (string(buffer) == "y")
				{
					use_file_cache = true;
					console::ShowMessage("use file cache = true");
				}
				else
				{
					use_file_cache = false;
					console::ShowMessage("use file cache = true");
				}

				bool use_filter;
				GameShell::InputTip("input 'y' to use filter.");
				cin.getline(buffer, 50);
				if (string(buffer) == "y")
				{
					use_filter = true;
					console::ShowMessage("use filter = true");
				}
				else
				{
					use_filter = false;
					console::ShowMessage("use filter = true");
				}
				solver.set_aim_layer(aim_layer);
				solver.set_thread_num(thread_num);
				solver.set_use_file_cache(use_file_cache);
				solver.set_use_filter(use_filter);
				console::ShowMessage("set solver succeess.");
			}
			else
			{
				console::ShowError("wrong input.");
				return;
			}
		}
		else
		{
			console::ShowError("wrong input.");
			return;
		}
	}
	void Show		    (solver::Solver& solver)
	{
		stringstream ss;
		ss << ">> " << "aim layer = " << solver.aim_layer() << endl;
		ss << ">> " << "current layer = " << solver.current_layer() << endl;
		ss << ">> " << "thread number = " << solver.thread_num() << endl;
		ss << ">> " << "use file cache = " << console::B2S(solver.file_cache()) << endl;
		ss << ">> " << "use filter = " << console::B2S(solver.use_filter()) << endl;

		cout << endl;
		cout << ss.str();
		cout << endl;
	}
	void Aim		    (solver::Solver& solver)
	{
		GameShell::InputTip("input solver aim('next' of layer number)");
		char buffer[50];
		cin.getline(buffer, 50);
		size_t aim_layer;
		if (string(buffer) == "next")
		{
			aim_layer = solver::GetCurrentLayer() - 1;
		}
		else
		{
			aim_layer = atoi(buffer);
		}
		size_t current = solver::GetCurrentLayer();
		if (aim_layer < current && aim_layer >0)
		{
			solver.set_aim_layer(aim_layer);
			console::ShowMessage("aim layer = " + console::I2S(aim_layer));
			Show(solver);
		}
		else
		{
			console::ShowError("wrong input!");
		}
	}
}
namespace state_shell
{
	void SaveState      (State& state)
	{
		ofstream ofs("free-edge.dat");
		ofs << state.board().to_ullong();
		console::ShowMessage("state have been saved successfully.");
	}
	void LoadState      (State& state)
	{
		ifstream ifs("free-edge.dat");
		BoardValue bitboard;
		ifs >> bitboard;
		state = State(Board(bitboard));
		console::ShowMessage("state have been loaded successfully.");
		state.Visualization();
	}
	void RemoveEdge     (State& state)
	{
		GameShell::InputTip("remove edge");
		size_t num;
		cin >> num;
		Edge edge = (Edge)num;
		if (edge < 60)
		{
			state.board_ref().reset(edge);
			cout << ">> edge = " << edge << endl;
			console::ShowMessage("edge remove successfully.");
		}
		else
		{
			console::ShowError("wrong index.");
		}
	}
	void SetEdge        (State& state)
	{
		GameShell::InputTip("set edge");
		size_t num;
		cin >> num;
		Edge edge = (Edge)num;
		if (edge < 60)
		{
			state.board_ref().set(edge);
			cout << ">> edge = " << num << endl;
			console::ShowMessage("edge set successfully.");
		}
		else
		{
			console::ShowError("wrong index.");
		}
	}
	void ChangeSeed     (State& state)
	{
		srand((unsigned)time(NULL));
		console::ShowMessage("set random seed success.");
	}
	void ShowState      (State& state)
	{
		//MoveAnalyst ma(state, true);
		//state.Visualization();
		ActionAnalyst aa(state.board());
		aa.Visualization();
		console::Cprintf("state type = ", console::YELLOW);
		cout << state_type::ToString(aa.state_type()) << endl;
	}
	void ShowChain      (State& state)
	{
		ActionAnalyst ana(state.board());
		if (ana.state_type() == state_type::REAR)
		{
			state.Visualization();
			rear_state::ChainAnalyst chains(state.board());
			chains.ShowChainInfo();
		}
		else
		{
			console::ShowError("state is not rear state");
		}
	}
	void ShowAction     (State& state)
	{
		ActionAnalyst ma(state.board());
		ma.Visualization();

		cout << ">> state type = " << state_type::ToString(ma.state_type()) << endl;
		cout << ">> action = {" << endl << "  ";
		for (Edge i = 0; i < MAX_EDGE; i++)
		{
			if (ma[i])
			{
				cout << static_cast<size_t>(i) << " ";
			}
		}
		cout << endl << "}" << endl << endl;
	}
	void NextState      (State& state)
	{
		ActionAnalyst ana(state.board());
		if (ana.state_type() != state_type::FINISH)
		{
			Edge e = ana.RandomAction();
			state.board_ref().set(e);
			cout << ">> take edge = " << size_t(e) << endl;
			ActionAnalyst(state.board()).Visualization(e);
		}
		else
		{
			console::ShowError("state have finished.");
		}
	}
	void Create         (State& state)
	{
		console::Cprintf("Create Mode", console::YELLOW);
		cout << endl;
		Board temp = state.board();
		for (;;)
		{
			State(temp).EdgeVisualization();
			console::ShowMessage("use ''return' to finish create mode.", false);
			GameShell::InputTip("next edge");
			std::string input = GameShell::GetInput();
			if (input == "return")
			{
				state.board_ref() = temp;
				return;
			}
			Edge num = atoi(input.c_str());
			if (num >= 0 && num < MAX_EDGE)
			{
				temp.set(num);
			}
		}
	}
	void NewState       (State& state)
	{
		state.board_ref() = Board();
	}
}

int main()
{
	//Create Shell and Pages
	GameShell dab("Dots&Boxes");
	ShellPage<int>* root = CreateShellPage<int>(dab,"root", 0);
	ShellPage<solver::Solver>* solver = CreateShellPage<solver::Solver>(dab, "solver");
	ShellPage<State>* state = CreateShellPage<State>(dab, "state");
	//ShellPage<game::GameState>* game = dab.CreatePage<game::GameState>("game");

	//page root
	//root->  AddChildPage("game", "start a new game.");
	root->  AddChildPage("solver", "start solver then set aim and parameters");
	root->  AddChildPage("state", "start state-debug");
	root->  AddInfoFunc([]() {
		console::Cprintf("=============================================\n", console::GRAY);
		console::Cprintf("    PARADOX Dots and Boxes\n", console::BLUE);
		console::Cprintf("    Copyright @ Junkai-Lu 2016\n", console::BLUE);
		console::Cprintf("=============================================\n\n", console::GRAY);
	});
	root->  AddDescript("return", "exit");
	root->  AddFunction("sample",   root_shell::Sample,         "get sample to check."              );
	root->  AddFunction("debug",    root_shell::Debug,          "debug mode."                       );

	//page solver
	solver->AddFunction("thread",   solver_shell::Thread,	    "change thread number"              );
	solver->AddFunction("cache",    solver_shell::FileCache,    "set whether to use file cache."    );
	solver->AddFunction("filter",   solver_shell::Filter,       "set thether to use filter."        );
	solver->AddFunction("layer",    solver_shell::ChangeLayer,  "change current layer"              );
	solver->AddFunction("start",    solver_shell::Start,        "execute solver."                   );
	solver->AddFunction("set",      solver_shell::Set,          "change all solver set."            );
	solver->AddFunction("show",     solver_shell::Show,         "show current solver parameters."   );

	//page state
	state->	AddFunction("next",     state_shell::NextState,     "set random edge in current state." );
	state-> AddFunction("show",     state_shell::ShowState,     "show current state."               );
	state-> AddFunction("action",   state_shell::ShowAction,    "get free-edges index."             );
	state-> AddFunction("save",     state_shell::SaveState,     "save current state."               );
	state-> AddFunction("load",     state_shell::LoadState,     "load a state."                     );
	state-> AddFunction("remove",   state_shell::RemoveEdge,    "remove a edge in current state."   );
	state-> AddFunction("seed",     state_shell::ChangeSeed,    "change random seed."               );
	state-> AddFunction("set",      state_shell::SetEdge,       "set a edge in current state."      );
	state-> AddFunction("chain",    state_shell::ShowChain,     "show chain info in rear state"     );
	state-> AddFunction("create",   state_shell::Create,        "create a new state"                );
	state-> AddFunction("new",      state_shell::NewState,      "reset state"                       );

	state-> AddExtarCommand([](std::string command, State& state)->bool {
		size_t num = atoi(command.c_str());
		if (num < 60 && num > 0)
		{
			cout << ">> create state, edges = " << num << endl;
			Board temp;
			for (size_t i = 0; i < num; i++)
			{
				ActionAnalyst ana(temp);
				if (ana.result().any())
				{
					temp.set(ana.RandomAction());
				}
				else
				{
					num = i;
					break;
				}
			}
			state.board_ref() = temp;
			if (num != atoi(command.c_str()))
			{
				cout << ">> actual num = " << num << endl;
			}

			ActionAnalyst ma(state.board());
			ma.Visualization();
			//ActionVec temp = ma.ActionVec();
			cout << endl;
			return true;
		}
		return false;
	}, "(0, 60]", "to create state with edges.");

	//page game
	/*game->AddFunction("ai", game_shell::DoAI, "ai action");
	game->AddFunction("man", game_shell::DoMan, "human action");
	game->AddFunction("auto", game_shell::StartAutoGame, "start auto game.");
	game->AddFunction("player", game_shell::SetPlayer, "set game player");
	game->AddFunction("show", game_shell::Show, "show game info.");
	game->AddFunction("cancel", game_shell::Cancel, "cancel last action.");*/

	dab.RunPage("root");
	return 0;
}

