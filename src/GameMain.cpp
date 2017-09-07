#include "GameDefine.h"
#include "StateAnalyst.h"
#include "Solver.h"
#include "../include/gameshell.h"

using namespace std;
using namespace gadt;

namespace dots_and_boxes
{
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
				console::Cprintf("input 'return' to stop sample. or other input to continue.", console::GRAY);
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
	namespace solver_shell
	{
		void Thread(solver::Solver& solver)
		{
			cout << ">> current thread num = " << solver.thread_num() << endl;
			console::Cprintf("input thread num", console::YELLOW);
			size_t num;
			cin >> num;
			if (num > 0 && num <= MAX_ALLOW_THREAD)
			{
				solver.set_thread_num(num);
				console::ShowMessage("set thread num success.");
			}
		}
		void FileCache(solver::Solver& solver)
		{
			cout << ">> use file cache = " << std::boolalpha << solver.file_cache() << std::noboolalpha << endl;
			console::Cprintf("input y to allow file cache.", console::YELLOW);
			string str;
			std::cin >> str;
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
		void Filter(solver::Solver& solver)
		{
			cout << ">> use filter = " << std::boolalpha << solver.use_filter() << std::noboolalpha << endl;
			console::Cprintf("input y to enable filter.", console::YELLOW);
			string str;
			std::cin >> str;
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
		void ChangeLayer(solver::Solver& solver)
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
		void Start(solver::Solver& solver)
		{
			//parameters check.
			console::ShowMessage("solver start");
			solver.Run();
		}
		void Set(solver::Solver& solver)
		{
			console::Cprintf("input solver aim('next' of layer number)", console::YELLOW);
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
				console::ShowMessage("aim layer = " + console::ToString(aim_layer));
				console::Cprintf("input allow thread number (max = " + console::ToString(MAX_ALLOW_THREAD) + " )", console::YELLOW);
				cin.getline(buffer, 50);
				size_t thread_num = atoi(buffer);
				if (thread_num <= MAX_ALLOW_THREAD && thread_num > 0)
				{
					console::ShowMessage("thread num = " + console::ToString(thread_num));
					bool use_file_cache;
					console::Cprintf("input 'y' to use file cache.", console::YELLOW);
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
					console::Cprintf("input 'y' to use filter.", console::YELLOW);
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
		void Show(solver::Solver& solver)
		{
			stringstream ss;
			ss << ">> " << "aim layer = " << solver.aim_layer() << endl;
			ss << ">> " << "current layer = " << solver.current_layer() << endl;
			ss << ">> " << "thread number = " << solver.thread_num() << endl;
			ss << ">> " << "use file cache = " << console::ToString(solver.file_cache()) << endl;
			ss << ">> " << "use filter = " << console::ToString(solver.use_filter()) << endl;

			cout << endl;
			cout << ss.str();
			cout << endl;
		}
		void Aim(solver::Solver& solver)
		{
			console::Cprintf("input solver aim('next' of layer number)", console::YELLOW);
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
				console::ShowMessage("aim layer = " + console::ToString(aim_layer));
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
		void RemoveEdge(State& state, const shell::ParamsList& list)
		{
			size_t num = atoi(list[0].c_str());
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
		void SetEdge(State& state, const shell::ParamsList& list)
		{
			size_t num = atoi(list[0].c_str());
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
		void RandomEdge(State& state, const shell::ParamsList& list)
		{
			size_t num = atoi(list[0].c_str());
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
				if (num != atoi(list[0].c_str()))
				{
					cout << ">> actual num = " << num << endl;
				}

				ActionAnalyst ma(state.board());
				ma.Visualization();
				//ActionVec temp = ma.ActionVec();
				cout << endl;
			}
		}
		void NextState(State& state, const shell::ParamsList& list)
		{
			size_t num = 1;
			if (list.size() != 0)
			{
				num = atoi(list[0].c_str());
				console::Cprintf("execute next " + list[0], console::YELLOW);
			}
			
			for (size_t i = 0; i < num; i++)
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
					break;
				}
			}
		}
		void SaveState(State& state)
		{
			ofstream ofs("free-edge.dat");
			ofs << state.board().to_ullong();
			console::ShowMessage("state have been saved successfully.");
		}
		void LoadState(State& state)
		{
			ifstream ifs("free-edge.dat");
			BoardValue bitboard;
			ifs >> bitboard;
			state = State(Board(bitboard));
			console::ShowMessage("state have been loaded successfully.");
			state.Visualization();
		}
		void ChangeSeed(State& state)
		{
			srand((unsigned)time(NULL));
			console::ShowMessage("set random seed success.");
		}
		void ShowState(State& state)
		{
			//MoveAnalyst ma(state, true);
			//state.Visualization();
			ActionAnalyst aa(state.board());
			aa.Visualization();
			console::Cprintf("state type = ", console::YELLOW);
			cout << state_type::ToString(aa.state_type()) << endl;
		}
		void ShowChain(State& state)
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
		void ShowAction(State& state)
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
		
		void Create(State& state)
		{
			console::Cprintf("Create Mode", console::YELLOW);
			cout << endl;
			Board temp = state.board();
			for (;;)
			{
				State(temp).EdgeVisualization();
				console::ShowMessage("use ''return' to finish create mode.");
				console::Cprintf("next edge", console::YELLOW);
				std::string input;
				std::cin >> input;
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
		void NewState(State& state)
		{
			state.board_ref() = Board();
		}
		
	}

	void ShellDefine()
	{
		//Create Shell and Pages
		shell::GameShell dab("Dots-And-Boxes");
		dab.SetDefaultInfoFunc([]() {
			console::Cprintf("=============================================\n", console::GRAY);
			console::Cprintf("    PARADOX Dots and Boxes\n", console::BLUE);
			console::Cprintf("    Copyright @ Junkai-Lu 2016\n", console::BLUE);
			console::Cprintf("=============================================\n\n", console::GRAY);
		});
		auto* root = dab.CreateShellPage("root");
		auto* solver = dab.CreateShellPage<solver::Solver>("solver");
		auto* state = dab.CreateShellPage<State>("state");

		//page root
		root->AddChildPage("solver", "start solver then set aim and parameters");
		root->AddChildPage("state", "start state-debug");
		root->AddFunction("sample", "get sample to check.", root_shell::Sample);
		root->AddFunction("debug", "debug mode.", root_shell::Debug);

		//page solver
		solver->AddFunction("thread", "change thread number", solver_shell::Thread);
		solver->AddFunction("cache", "set whether to use file cache.", solver_shell::FileCache);
		solver->AddFunction("filter", "set thether to use filter.", solver_shell::Filter);
		solver->AddFunction("layer", "change current layer", solver_shell::ChangeLayer);
		solver->AddFunction("start", "execute solver.", solver_shell::Start);
		solver->AddFunction("set", "change all solver set.", solver_shell::Set);
		solver->AddFunction("show", "show current solver parameters.", solver_shell::Show);

		auto IndexLimitCheck = [](const shell::ParamsList& list)->bool {
			if (list.size() != 1)
				return false;

			size_t num = atoi(list[0].c_str());
			if (num < 1 || num > MAX_EDGE)
			{
				return false;
			}
			return true;
		};
		auto ParamsLengthCheck = [](const shell::ParamsList& list)->bool {
			if (list.size() > 1)
				return false;
			return true;
		};

		//page state
		state->AddFunction("next", "set random edge in current state.", state_shell::NextState, ParamsLengthCheck);
		state->AddFunction("set", "set a edge in current state.", state_shell::SetEdge, IndexLimitCheck);
		state->AddFunction("remove", "remove a edge in current state.", state_shell::RemoveEdge, IndexLimitCheck);
		state->AddFunction("edge", "to create state with edges.", state_shell::RandomEdge, IndexLimitCheck);
		state->AddFunction("show", "show current state.", state_shell::ShowState);
		state->AddFunction("action", "get free-edges index.", state_shell::ShowAction);
		state->AddFunction("save", "save current state.", state_shell::SaveState);
		state->AddFunction("load", "load a state.", state_shell::LoadState);
		state->AddFunction("seed", "change random seed.", state_shell::ChangeSeed);
		state->AddFunction("chain", "show chain info in rear state", state_shell::ShowChain);
		state->AddFunction("create", "create a new state", state_shell::Create);
		state->AddFunction("reset", "reset state", state_shell::NewState);

		dab.StartFromPage("root");
	}
}

int main()
{
	dots_and_boxes::ShellDefine();
	return 0;
}

