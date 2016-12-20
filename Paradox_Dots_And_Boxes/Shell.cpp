#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "Shell.h"
#include "GameDefine.h"
#include "Solver.h"
#include "MoveAnalyst.h"
#include "MCTS.h"

using namespace std;

#define REGISTER_SHELL(name,type) Shell<type>* name = new Shell<type>(#name)
#define REGISTER_SHELL_DATA(name,type,original_data) Shell<type>* name = new Shell<type>(#name, original_data)

//source code of shell
namespace dots_and_boxes
{
	namespace SHELL
	{
		//ShellBase
		ShellBase::ShellBase(str name) :
			_name(name),
			_parent_shell(nullptr)
		{
			ShellManager::RegisterShell(this);
		}
		ShellBase::~ShellBase()
		{
			ShellManager::set_current_shell(_parent_shell);
			ShellManager::RemoveShell(this);
		}

		//ShellManager
		std::map<str, ShellBase*> ShellManager::_shell_list;
		ShellBase* ShellManager::_current_shell;
		void ShellManager::CleanScreen(str name)
		{
			system("cls");
			if (name == "root")
			{
				Cprintf("=============================================\n", 8);
				Cprintf("    PARADOX Dots and Boxes AI\n", 9);
				Cprintf("    This software is under GPL lincense\n", 9);
				Cprintf("    Copyright @ Junkai-Lu 2016\n", 9);
				Cprintf("=============================================\n\n", 8);
			}
			cout << ">> ";
			Cprintf("[ Shell ", 6);
			Cprintf("<" + name + ">", 14);
			Cprintf(" ]\n", 6);

			cout << ">> ";
			Cprintf("use 'help' to get more command\n\n", 2);
		}

		void ShellManager::Run()
		{
			_init_func();
			WARNING_CHECK(!ShellExist("root"), "shell 'root' not found");
			GetShell("root")->Run();
		}
	}
}

//game extern.
namespace dots_and_boxes
{
	namespace SHELL
	{
		//initialize function
		std::function<void()> ShellManager::_init_func = ShellInit;
		void ShellInit()
		{
			REGISTER_SHELL(root, void*);
			REGISTER_SHELL(solver, Solver);
			REGISTER_SHELL(game, GAME::GameState);
			REGISTER_SHELL(state, State);

			//root
			root->AddFunction("sample", ROOT_SHELL::Sample, "get sample to check.");
			root->AddFunction("debug", ROOT_SHELL::Debug, "debug mode.");
			root->AddChildShell("solver", "start solver then set aim and parameters");
			root->AddChildShell("game", "start a new game.");
			root->AddChildShell("state", "start state-debug");

			//solver
			solver->AddFunction("thread", SOLVER_SHELL::Thread, "change thread number");
			solver->AddFunction("cache", SOLVER_SHELL::FileCache, "set whether to use file cache.");
			solver->AddFunction("filter", SOLVER_SHELL::Filter, "set thether to use filter.");
			solver->AddFunction("layer", SOLVER_SHELL::ChangeLayer, "change current layer");
			solver->AddFunction("start", SOLVER_SHELL::Start, "execute solver.");
			solver->AddFunction("set", SOLVER_SHELL::Set, "change all solver set.");
			solver->AddFunction("show", SOLVER_SHELL::Show, "show current solver parameters.");

			//state
			state->AddFunction("next", STATE_SHELL::NextState, "set random edge in current state.");
			state->AddFunction("show", STATE_SHELL::ShowState, "show current state.");
			state->AddFunction("action", STATE_SHELL::ShowAction, "get free-edges index.");
			state->AddFunction("save", STATE_SHELL::SaveState, "save current state.");
			state->AddFunction("load", STATE_SHELL::LoadState, "load a state.");
			state->AddFunction("remove", STATE_SHELL::RemoveEdge, "remove a edge in current state.");
			state->AddFunction("seed", STATE_SHELL::ChangeSeed, "change random seed.");
			state->AddDescript("(0,60]", "to create state with edges.");
			state->AddExtarCommand([](str command, State& state)->bool {
				size_t num = atoi(command.c_str());
				if (num < 60 && num > 0)
				{
					cout << ">> create state, edges = " << num << endl;
					state = State::RandomState(num);
					MoveAnalyst ma(state, true);
					state.ActionVisualization(ma.ActionVec());
					//ActionVec temp = ma.ActionVec();
					cout << endl;
					return true;
				}
				return false;
			});

			//game
			game->AddFunction("ai", GAME_SHELL::DoAI, "ai action");
			game->AddFunction("man", GAME_SHELL::DoMan, "human action");
			game->AddFunction("auto", GAME_SHELL::StartAutoGame, "start auto game.");
			game->AddFunction("player", GAME_SHELL::SetPlayer, "set game player");
			game->AddFunction("show", GAME_SHELL::Show, "show game info.");
			game->AddFunction("cancel", GAME_SHELL::Cancel, "cancel last action.");

		}

		namespace ROOT_SHELL
		{
			void Sample(void* v)
			{
				size_t layer = SOLVER::GetCurrentLayer();
				ifstream final_file(SOLVER::GetFinalFileName(layer));
				for (;;)
				{
					BitBoard board;
					Margin margin;
					final_file >> board >> margin;
					State state(board);
					cout << ">> state = " << endl;
					state.Visualization();
					cout << ">> margin = " << margin << endl;
					//bool dead_chain = state::ExistDeadChain(board);
					//bool free_edge = state::ExistFreeEdge(board);
					//cout << ">> exist dead-chain = " << B2S(dead_chain) << endl;
					//cout << ">> exist free-edge = " << B2S(free_edge) << endl;
					cout << ">> is reasonable = " << B2S(state::IsReasonable(board)) << endl;
					if (final_file.eof())
					{
						Error("no more states.");
						Message("sample finish.");
						break;
					}
					ShellManager::InputTip("input 'return' to stop sample. or other input to continue.");
					char buffer[50];
					cin.getline(buffer, 50);
					if (string(buffer) == "return")
					{
						break;
					}
				}

			}
			void Debug(void* v)
			{
				State s(576460752303423487);
				s.Visualization();
				cout << B2S(state::IsReasonable(576460752303423487));
				system("pause");
			}
		}
		namespace GAME_SHELL
		{
			void Show(GAME::GameState& game_state)
			{
				game_state.EdgeVisualization();
				cout << ">> Player 0: score = " << game_state[0]._score << " type = " << game_state[0]._des << endl;
				cout << ">> Player 1: score = " << game_state[1]._score << " type = " << game_state[1]._des << endl << endl;
			}
			void SetPlayer(GAME::GameState& game_state)
			{
				ShellManager::InputTip("input player [0/1].");
				bool player;
				cin >> player;
				ShellManager::InputTip("input type [0]HUMAN [1]MCTS");
				size_t num;
				cin >> num;
				if (num == 0)
				{
					game_state.set_player(player, { game_state[player]._score, DAB::GAME::HUMAN::HumanRespond ,"Human" });
				}
				else
				{
					game_state.set_player(player, { game_state[player]._score, DAB::GAME::MCTS::MctsRespond ,"MCTS" });
				}
			}
			void StartAutoGame(GAME::GameState& game_state)
			{

			}
			void DoAI(GAME::GameState& game_state)
			{
				ShellManager::InputTip("input player [0/1]");
				bool player;
				cin >> player;
				GAME::ActionDes ac = GAME::MCTS::MctsRespond(game_state);
				game_state.DoActions(player, ac);
				Show(game_state);
			}
			void DoMan(GAME::GameState& game_state)
			{
				ShellManager::InputTip("input player and edge");
				bool player;
				size_t edge;
				cin >> player >> edge;
				game_state.GameEdgeSet(player, (Edge)edge);
				Show(game_state);
			}
			void Cancel(GAME::GameState& game_state)
			{
				if (game_state.EdgeExist(game_state.last_action()))
				{
					game_state.GameEdgeRemove(game_state.last_player(), game_state.last_action());
					Message("cancel success!");
				}
				else
				{
					Error("cancel failed, edge do not exist!");
				}
			}
		}
		namespace SOLVER_SHELL
		{
			void Thread(Solver& solver)
			{
				cout << ">> current thread num = " << solver.thread_num() << endl;
				ShellManager::InputTip("input thread num");
				size_t num;
				cin >> num;
				if (num > 0 && num <= MAX_ALLOW_THREAD)
				{
					solver.set_thread_num(num);
					Message("set thread num success.");
				}
			}
			void FileCache(Solver& solver)
			{
				cout << ">> use file cache = " << B2S(solver.file_cache()) << endl;
				ShellManager::InputTip("input y to allow file cache.");
				string str = GetInput();
				if (str == "y")
				{
					solver.set_use_file_cache(true);
					Message("enable file cache.");
				}
				else
				{
					solver.set_use_file_cache(false);
					Message("disable file cache.");
				}
			}
			void Filter(Solver& solver)
			{
				cout << ">> use filter = " << B2S(solver.use_filter()) << endl;
				ShellManager::InputTip("input y to enable filter.");
				string str = GetInput();
				if (str == "y")
				{
					solver.set_use_filter(true);
					Message("enable filter.");
				}
				else
				{
					solver.set_use_filter(false);
					Message("disable filter.");
				}
			}
			void ChangeLayer(Solver& solver)
			{
				//cout << ">> set current layer = ";
				cout << ">> input new layer, current layer = " << SOLVER::GetCurrentLayer() << endl << ">>> ";
				char buffer[50];
				cin.getline(buffer, 50);
				size_t layer = atoi(buffer);
				if (layer <= 60 && layer >= 1)
				{
					SOLVER::WriteSolverData(layer, true);
					Message("change layer success!");
					cout << endl;
				}
				else
				{
					Error("wrong layer");
					cout << endl;
				}
			}
			void Start(Solver& solver)
			{
				//parameters check.
				Message("solver start");
				solver.Run();
			}
			void Set(Solver& solver)
			{
				ShellManager::InputTip("input solver aim('next' of layer number)");
				char buffer[50];
				cin.getline(buffer, 50);
				size_t aim_layer;
				if (string(buffer) == "next")
				{
					aim_layer = SOLVER::GetCurrentLayer() - 1;
				}
				else
				{
					aim_layer = atoi(buffer);
				}
				size_t current = SOLVER::GetCurrentLayer();
				if (aim_layer < current && aim_layer >0)
				{
					Message("aim layer = " + I2S(aim_layer));
					ShellManager::InputTip("input allow thread number (max = " + I2S(MAX_ALLOW_THREAD) + " )");
					cin.getline(buffer, 50);
					size_t thread_num = atoi(buffer);
					if (thread_num <= MAX_ALLOW_THREAD && thread_num > 0)
					{
						Message("thread num = " + I2S(thread_num));
						bool use_file_cache;
						ShellManager::InputTip("input 'y' to use file cache.");
						cin.getline(buffer, 50);
						if (string(buffer) == "y")
						{
							use_file_cache = true;
							Message("use file cache = true");
						}
						else
						{
							use_file_cache = false;
							Message("use file cache = true");
						}

						bool use_filter;
						ShellManager::InputTip("input 'y' to use filter.");
						cin.getline(buffer, 50);
						if (string(buffer) == "y")
						{
							use_filter = true;
							Message("use filter = true");
						}
						else
						{
							use_filter = false;
							Message("use filter = true");
						}
						solver.set_aim_layer(aim_layer);
						solver.set_thread_num(thread_num);
						solver.set_use_file_cache(use_file_cache);
						solver.set_use_filter(use_filter);
						Message("set solver succeess.");
					}
					else
					{
						Error("wrong input.");
						return;
					}
				}
				else
				{
					Error("wrong input.");
					return;
				}
			}
			void Show(Solver& solver)
			{
				stringstream ss;
				ss << ">> " << "aim layer = " << solver.aim_layer() << endl;
				ss << ">> " << "current layer = " << solver.current_layer() << endl;
				ss << ">> " << "thread number = " << solver.thread_num() << endl;
				ss << ">> " << "use file cache = " << B2S(solver.file_cache()) << endl;
				ss << ">> " << "use filter = " << B2S(solver.use_filter()) << endl;

				cout << endl;
				cout << ss.str();
				cout << endl;
			}
			void Aim(Solver& solver)
			{
				ShellManager::InputTip("input solver aim('next' of layer number)");
				char buffer[50];
				cin.getline(buffer, 50);
				size_t aim_layer;
				if (string(buffer) == "next")
				{
					aim_layer = SOLVER::GetCurrentLayer() - 1;
				}
				else
				{
					aim_layer = atoi(buffer);
				}
				size_t current = SOLVER::GetCurrentLayer();
				if (aim_layer < current && aim_layer >0)
				{
					solver.set_aim_layer(aim_layer);
					Message("aim layer = " + I2S(aim_layer));
					Show(solver);
				}
				else
				{
					Error("wrong input!");
				}
			}
		}
		namespace STATE_SHELL
		{
			void ShowAction(State& state)
			{
				MoveAnalyst ma(state, true);
				state.Visualization(ma.ActionVec());
				cout << ">> action = {" << endl << "  ";
				for (size_t i = 0; i < MAX_EDGE; i++)
				{
					if (ma[i])
					{
						cout << i << " ";
					}
				}
				cout << "}" << endl << endl;
			}
			void SaveState(State& state)
			{
				ofstream ofs("free-edge.dat");
				ofs << boardCreate(state);
				Message("state have been saved successfully.");
			}
			void LoadState(State& state)
			{
				ifstream ifs("free-edge.dat");
				BitBoard board;
				ifs >> board;
				state = State(board);
				Message("state have been loaded successfully.");
				state.Visualization();
			}
			void RemoveEdge(State& state)
			{
				ShellManager::InputTip("input the edge index that need remove.");
				size_t num;
				cin >> num;
				Edge edge = (Edge)num;
				if (edge < 60)
				{
					if (state.EdgeExist(edge))
					{
						state.EdgeRemove(edge);
						cout << ">> edge = " << edge << endl;
						Message("edge have removed");
					}
					else
					{
						Error("edge do not exist.");
					}
				}
				else
				{
					Error("edge do not exist.");
				}
			}
			void ChangeSeed(State& state)
			{
				srand((unsigned)time(NULL));
				Message("set random seed success.");
			}
			void ShowState(State& state)
			{
				//MoveAnalyst ma(state, true);
				state.Visualization();
			}
			void NextState(State& state)
			{
				BitBoard board = boardCreate(state);
				for (;;)
				{
					Edge next_edge = state::GetDeadBoxRemainEdgeIndex(board);
					if (next_edge != MAX_EDGE)
					{
						boardEdgeSet(board, next_edge);
						state.EdgeSet(next_edge);
						cout << ">> take edge = " << size_t(next_edge) << endl;
					}
					else
					{
						break;
					}
				}
				vector<Edge> actions;
				for (Edge edge = 0; edge < MAX_EDGE; edge++)
				{
					if (state::IsFreeEdge(board, edge))
					{
						actions.push_back(edge);
					}
				}
				if (actions.size() != 0)
				{
					size_t rnd = rand() % actions.size();
					state.EdgeSet(actions[rnd]);
					cout << ">> take edge = " << size_t(actions[rnd]) << endl;
					MoveAnalyst ma(state, true);
					state.ActionVisualization(ma.ActionVec());
				}
				else
				{
					Error("no more free-edge!");
				}
			}
		}
	}
}
