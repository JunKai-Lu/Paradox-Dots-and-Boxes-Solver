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
		void Layer()
		{
			size_t layer = DAB::SOLVER::GetCurrentDepth(false);
			cout << ">> current layer = " << layer << endl;
		}
		void Set()
		{
			//cout << ">> set current layer = ";
			cout << ">> input new layer, current layer = " << SOLVER::GetCurrentDepth(false) << endl << ">>> ";
			char buffer[50];
			cin.getline(buffer, 50);
			size_t layer = atoi(buffer);
			if (layer <= 60 && layer >= 1)
			{
				Message("input 'y' to confirm the action.", false);
				InputTip();
				cin.getline(buffer, 50);
				if (string(buffer) == "y")
				{
					SOLVER::WriteSolverData(layer, true);
					Message("change layer success!");
					cout << endl;
				}
				else
				{
					Message("action cancel.");
					cout << endl;
				}
			}
			else
			{
				Error("wrong layer");
				cout << endl;
			}
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
		void Sample()
		{
			size_t layer = SOLVER::GetCurrentDepth();
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
				Message("input 'stop' to stop sample. or other input to continue.", false);
				InputTip();
				char buffer[50];
				cin.getline(buffer, 50);
				if (string(buffer) == "stop")
				{
					break;
				}
			}
			CleanScreen();
		}
		void Exit()
		{
			exit(0);
		}
		
		namespace GAME
		{
			
		}
		namespace SOLVERSHELL
		{
			void StartSolver()
			{
				char buffer[50];
				cout << ">> solver begin, input solver aim('next' of layer number)" << endl << ">>> ";
				cin.getline(buffer, 50);
				size_t aim_layer;
				if (string(buffer) == "next")
				{
					aim_layer = SOLVER::GetCurrentDepth(false) - 1;
				}
				else
				{
					aim_layer = atoi(buffer);
				}
				size_t current = SOLVER::GetCurrentDepth(false);
				if (aim_layer < current && aim_layer >0)
				{
					cout << ">> input thread num( max = " << MAX_ALLOW_THREAD << " )" << endl << ">>> ";
					cin.getline(buffer, 50);
					size_t thread_num = atoi(buffer);
					if (thread_num <= MAX_ALLOW_THREAD && thread_num > 0)
					{
						bool file_cache;
						cout << ">> input 'y' to use file cache." << endl << ">>> ";
						cin.getline(buffer, 50);
						if (string(buffer) == "y")
						{
							file_cache = true;
						}
						else
						{
							file_cache = false;
						}

						cout << ">> Solver is ready." << endl;
						cout << ">> aim layer = " << aim_layer << endl;
						cout << ">> thread num = " << thread_num << endl;
						cout << ">> file cache = ";
						if (file_cache)
							cout << "yes" << endl;
						else
							cout << "no" << endl;

						cout << ">> input 'y' to start." << endl << ">>> ";
						cin.getline(buffer, 50);
						if (string(buffer) == "y")
						{
							Solver(aim_layer, file_cache, true, thread_num, true);
							return;
						}
						else
						{
							cout << ">> action cancel." << endl << endl;
							return;
						}
					}
				}

				cout << ">> error: wrong input, action cancel." << endl << endl;

			}
		}
		namespace STATESHELL
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
			}
			void RemoveEdge(State& state)
			{
				Message("input the edge index that need remove.", false);
				InputTip();
				Edge edge;
				cin >> edge;
				if (edge < 60)
				{
					if (state.EdgeExist(edge))
					{
						state.EdgeRemove(edge);
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
				MoveAnalyst ma(state, true);
				state.ActionVisualization(ma.ActionVec());
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
				commands.AddDescript("exit", "finish.");
				commands.AddDescript("[0,60]", "to create state with edges.");

				system("cls");
				cout << ">> ";
				Cprintf("[ State Debug Shell ]\n", 2);
				cout << ">> ";
				Cprintf("use 'help' to get more command\n\n", 2);
				commands.ShowCommand();

				for (;;)
				{
					InputTip();
					char buffer[50];
					cin.getline(buffer, 50);
					string str(buffer);
					if (str == "exit")
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
			commands.Add("info", Info, "get info about software.");
			commands.Add("layer", Layer, "show current layer that had been solved.");
			commands.Add("set", Set, "change the current layer data.");
			commands.Add("sample", Sample, "get sample of last layer.");
			commands.Add("solver", SOLVERSHELL::StartSolver, "start solver then set aim and parameters");
			commands.Add("cls", CleanScreen, "clean screen.");
			commands.Add("state", STATESHELL::StartStateShell, "start state-debug");
			commands.Add("exit", Exit, "exit program.");
			commands.AddDescript("help", "get command list");

			CleanScreen();
			commands.ShowCommand();
			for (;;)
			{
				InputTip();
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