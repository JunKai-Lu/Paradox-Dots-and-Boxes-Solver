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

namespace DAB
{
	namespace SHELL
	{
		void Info()
		{
			Cprintf("=============================================\n", 8);
			Cprintf("    PARADOX Dots and Boxes AI\n", 14);
			Cprintf("    This software is under GPL lincense\n", 14);
			Cprintf("    Copyright @ Junkai-Lu 2016\n", 14);
			Cprintf("=============================================\n\n", 8);
		}
		void CleanScreen()
		{
			system("cls");
			Info();
			cout << ">> ";
			Cprintf("[ DAB Shell ]\n", 2);
			cout << ">> ";
			Cprintf("use 'help' to get more command\n\n", 2);
		}	
		void Exit()
		{
			exit(0);
		}
		void Sample()
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
				bool dead_chain = STATE::ExistDeadChain(board);
				bool free_edge = STATE::ExistFreeEdge(board);
				cout << ">> exist dead-chain = " << B2S(dead_chain) << endl;
				cout << ">> exist free-edge = " << B2S(free_edge) << endl;
				cout << ">> is reasonable = " << B2S(!(dead_chain && free_edge)) << endl;
				if (final_file.eof())
				{
					Error("no more states.");
					Message("sample finish.");
					break;
				}
				InputTip("input 'stop' to stop sample. or other input to continue.");
				char buffer[50];
				cin.getline(buffer, 50);
				if (string(buffer) == "stop")
				{
					break;
				}
			}
			CleanScreen();
		}
		void Debug()
		{
			State s(576460752303423487);
			s.Visualization();
			cout << B2S(STATE::IsReasonable(576460752303423487));
			system("pause");
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
				InputTip("input player [0/1].");
				bool player;
				cin >> player;
				InputTip("input type [0]HUMAN [1]MCTS");
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
				InputTip("input player [0/1]");
				bool player;
				cin >> player;
				GAME::ActionDes ac = GAME::MCTS::MctsRespond(game_state);
				game_state.DoActions(player, ac);
				Show(game_state);
			}
			void DoMan(GAME::GameState& game_state)
			{
				InputTip("input player and edge");
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
			void StartGame()
			{
				GAME::GameState game_state;
				CommandList<void(*)(GAME::GameState&)> commands;
				commands.Add("ai", DoAI, "ai action");
				commands.Add("man", DoMan, "human action");
				commands.Add("auto", StartAutoGame, "start auto game.");
				commands.Add("player", SetPlayer, "set game player");
				commands.Add("show", Show, "show game info.");
				commands.Add("cancel", Cancel, "cancel last action.");
				commands.AddDescript("return", "return to previous menu.");
				commands.AddDescript("help", "get command list");
				system("cls");
				cout << ">> ";
				Cprintf("[ Solver Shell ]\n", 2);
				cout << ">> ";
				Cprintf("use 'help' to get more command\n\n", 2);
				for (;;)
				{
					cout << ">>> ";
					string str = GetInput();
					if (str == "return")
					{
						CleanScreen();
						break;
					}
					else if (str == "help")
					{
						commands.ShowCommand();
					}
					else
					{
						if (commands.Exist(str))
						{
							commands.Func(str)(game_state);
						}
						else
						{
							Error("wrong input!");
						}
					}
				}
			}
		}
		namespace SOLVER_SHELL
		{
			void Thread(Solver& solver)
			{
				cout << ">> current thread num = " << solver.thread_num() << endl;
				InputTip("input thread num");
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
				InputTip("input y to allow file cache.");
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
				InputTip("input y to enable filter.");
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
				InputTip("input solver aim('next' of layer number)");
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
					InputTip("input allow thread number (max = "+I2S(MAX_ALLOW_THREAD)+" )");
					cin.getline(buffer, 50);
					size_t thread_num = atoi(buffer);
					if (thread_num <= MAX_ALLOW_THREAD && thread_num > 0)
					{
						Message("thread num = " + I2S(thread_num));
						bool use_file_cache;
						InputTip("input 'y' to use file cache.");
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
						InputTip("input 'y' to use filter.");
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
				InputTip("input solver aim('next' of layer number)");
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
			void StartSolver()
			{
				Solver solver(60, false, false, 1);
				CommandList<void(*)(Solver&)> commands;
				commands.Add("thread", Thread, "change thread number");
				commands.Add("cache", FileCache, "set whether to use file cache.");
				commands.Add("filter", Filter, "set thether to use filter.");
				commands.Add("layer", ChangeLayer, "change current layer");
				commands.Add("start", Start, "execute solver.");
				commands.Add("set", Set, "change all solver set.");
				commands.Add("show", Show, "show current solver parameters.");
				commands.AddDescript("return", "return to previous menu.");

				Set(solver);
				system("cls");
				cout << ">> ";
				Cprintf("[ Solver Shell ]\n", 2);
				cout << ">> ";
				Cprintf("use 'help' to get more command\n\n", 2);
				for (;;)
				{
					cout << ">>> ";
					string str = GetInput();
					if (str == "return")
					{
						CleanScreen();
						break;
					}
					else if (str == "help")
					{
						commands.ShowCommand();
					}
					else
					{
						if (commands.Exist(str))
						{
							commands.Func(str)(solver);
						}
						else
						{
							Error("wrong input!");
						}
					}
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
				ofs << BOARD::Create(state);
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
				InputTip("input the edge index that need remove.");
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
				BitBoard board = BOARD::Create(state);
				for (;;)
				{
					Edge next_edge = STATE::GetDeadBoxRemainEdgeIndex(board);
					if (next_edge != MAX_EDGE)
					{
						BOARD::EdgeSet(board, next_edge);
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
					if (STATE::IsFreeEdge(board, edge))
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
			void StartStateShell()
			{
				State state;
				CommandList<void(*)(State&)> commands;	
				commands.Add("next",NextState,"set random edge in current state.");
				commands.Add("show",ShowState,"show current state.");
				commands.Add("action",ShowAction,"get free-edges index.");
				commands.Add("save",SaveState,"save current state.");
				commands.Add("load",LoadState,"load a state.");
				commands.Add("remove",RemoveEdge,"remove a edge in current state.");
				commands.Add("seed",ChangeSeed,"change random seed.");
				commands.AddDescript("return", "return to previous menu.");
				commands.AddDescript("help", "get command list");
				commands.AddDescript("[0,60]", "to create state with edges.");
				system("cls");
				cout << ">> ";
				Cprintf("[ State Debug Shell ]\n", 2);
				cout << ">> ";
				Cprintf("use 'help' to get more command\n\n", 2);
				for (;;)
				{
					cout << ">>> ";
					char buffer[50];
					cin.getline(buffer, 50);
					string str(buffer);
					if (str == "return")
					{
						CleanScreen();
						break;
					}
					else if (str == "help")
					{
						commands.ShowCommand();
					}
					else
					{
						if (commands.Exist(str))
						{
							commands.Func(str)(state);
						}
						else
						{
							size_t num = atoi(buffer);
							if (num < 60 && num > 0)
							{
								cout << ">> create state, edges = " << num << endl;
								state = State::RandomState(num);
								MoveAnalyst ma(state, true);
								state.ActionVisualization(ma.ActionVec());
								ActionVec temp = ma.ActionVec();
								cout << endl;
							}
							else
							{
								Error("wrong input!");
							}
						}
					}
				}
			}
		}
		
		void ShellStart()
		{
			CommandList<void(*)()> commands;
			commands.Add("info", Info, "get info about software." );
			commands.Add("game", GAME_SHELL::StartGame, "start a new game.");
			commands.Add("solver", SOLVER_SHELL::StartSolver, "start solver then set aim and parameters");
			commands.Add("cls", CleanScreen, "clean screen.");
			commands.Add("state", STATE_SHELL::StartStateShell, "start state-debug");
			commands.Add("sample", Sample, "get sample to check.");
			commands.Add("exit", Exit, "exit program.");
			commands.Add("debug", Debug, "debug model.");
			commands.AddDescript("help", "get command list");
			CleanScreen();
			for (;;)
			{
				cout << ">>> ";
				string com = GetInput();
				if (commands.Exist(com))
				{
					commands.Func(com)();
				}
				else
				{
					if (com == "help")
					{
						commands.ShowCommand();
					}
					else
					{
						Error("wrong command");
						cout << endl;
					}
				}
			}
			Message("DabShell finish.");
		}
	}
	
}