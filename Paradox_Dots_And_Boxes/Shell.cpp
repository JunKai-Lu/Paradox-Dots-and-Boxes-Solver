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
		std::map < std::string, OrderInfo > order_list;
		
		void Info()
		{
			Cprintf("=============================================\n",8);
			Cprintf( "    PARADOX Dots and Boxes AI\n" ,14);
			Cprintf("    This software is under GPL lincense\n", 14);
			Cprintf("    Copyright @ Junkai-Lu 2016\n", 14);
			Cprintf("=============================================\n\n", 8);
		}
		void Layer()
		{
			size_t layer = SOLVER::GetCurrentDepth(false);
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
				cout << ">> input 'y' to confirm the action." << endl << ">>> ";
				cin.getline(buffer, 50);
				if (string(buffer) == "y")
				{
					SOLVER::WriteSolverData(layer, true);
					cout << ">> change layer success!" << endl << endl;
				}
				else
				{
					cout << ">> action cancel." << endl << endl;
				}
			}
			else
			{
				cout << ">> error: wrong layer" << endl << endl;
			}
		}
		void CleanScreen()
		{
			system("cls");
			Info();
			cout << ">> ";
			Cprintf("[ DAB Shell ]\n",2);
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
					cout << ">> no more states." << endl;
					cout << ">> sample finish." << endl; 
					break;
				}
				cout << endl << ">> input 'stop' to stop sample. or other input to continue." << endl<<">>> ";
				char buffer[50];
				cin.getline(buffer, 50);
				if (string(buffer) == "stop")
				{
					break;
				}
			}
			CleanScreen();
		}
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
				cout << ">> input thread num( max = "<<MAX_ALLOW_THREAD<<" )" << endl << ">>> ";
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
		void Help()
		{
			cout << ">> ";
			Cprintf("[ ORDER LIST ]\n\n",14);
			for (auto& order: order_list)
			{
				cout << "   '"; 
				Cprintf(order.first, 12);
				cout << "'" << string("          ").substr(0, 10 - order.first.length()) << order.second._descript << endl;
			}
			cout << endl;
		}
		void FreeEdge()
		{
			State state;
			for (;;)
			{
				cout << ">> input help to get more order." << endl << ">>> ";
				char buffer[50];
				cin.getline(buffer,50);
				string str(buffer);
				if (str == "exit")
				{
					CleanScreen();
					break;
				}
				else if (str == "help")
				{
					Cprintf("\n[ ORDER LIST ]\n\n", 14);

					cout << "   [0,60]      to create state with edges. "<< endl;
					cout << "   'next'      set random edge in current state." << endl;
					cout << "   'show'      show current state." << endl;
					cout << "   'action'    get free-edges index." << endl;
					cout << "   'save'      save current state." << endl;
					cout << "   'load'      load a state." << endl;
					cout << "   'exit'      finish." << endl << endl;
				}
				else if (str == "show")
				{
					
					MoveAnalyst ma(state, true);
					state.ActionVisualization(ma.ActionVec());
				}
				else if (str == "action")
				{
					MoveAnalyst ma(state, true);
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
				else if (str == "next")
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
						cout << ">> no more free-edge!" << endl; 
					}
				}
				else if (str == "save")
				{
					ofstream ofs("free-edge.dat");
					ofs << BOARD::Create(state);
					cout << ">> state have been saved successfully." << endl;
				}
				else if (str == "load")
				{
					ifstream ifs("free-edge.dat");
					BitBoard board;
					ifs >> board;
					state = State(board);
					cout << ">> state have been loaded successfully." << endl;
				}
				else
				{
					
					size_t num = atoi(buffer);
					if (num < 60)
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
						cout << ">> wrong input!" << endl << endl;
					}
				}	
			}
		}
		void Exit()
		{
			exit(0);
		}
		void Start()
		{
			AddOrder("info", Info, "get info about software.");
			AddOrder("layer", Layer, "show current layer that had been solved.");
			AddOrder("set", Set, "change the current layer data.");
			AddOrder("sample", Sample, "get sample of last layer.");
			AddOrder("solver", StartSolver, "start solver then set aim and parameters");
			AddOrder("help", Help, "get order list.");
			AddOrder("cls", CleanScreen, "clean screen.");
			AddOrder("edges", FreeEdge, "check those free edge in random or inputed state.");
			AddOrder("exit", Exit, "exit program.");

			CleanScreen();
			for (;;)
			{
				cout << ">>> ";
				char buffer[50];
				cin.getline(buffer, 50);
				string order = string(buffer);
				cout << endl;
				if (order_list.count(order))
				{
					order_list[order]._func();
				}
				else
				{
					cout << ">> error: wrong order in DAB Shell." << endl << endl;
				}
			}
			cout << ">> DabShell finish." << endl;
		}
	}
}