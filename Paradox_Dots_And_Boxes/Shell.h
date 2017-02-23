#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <functional>

#include "GameDefine.h"
#include "Solver.h"
#include "GeneralGameShell.h"

#pragma  once

//game extern.
namespace dots_and_boxes
{
	namespace shell
	{
		void ShellInit();

		namespace root_shell
		{
			void Sample(int x);
			void Debug(int x);
		}
		namespace game_shell
		{
			/*void Show(game::GameState& game_state);
			void SetPlayer(game::GameState& game_state);
			void StartAutoGame(game::GameState& game_state);
			void DoAI(game::GameState& game_state);
			void DoMan(game::GameState& game_state);
			void Cancel(game::GameState& game_state);*/
		}
		namespace solver_shell
		{
			void Thread(solver::Solver& solver);
			void FileCache(solver::Solver& solver);
			void Filter(solver::Solver& solver);
			void ChangeLayer(solver::Solver& solver);
			void Start(solver::Solver& solver);
			void Set(solver::Solver& solver);
			void Show(solver::Solver& solver);
			void Aim(solver::Solver& solver);
		}
		namespace state_shell
		{
			void ShowAction(State& state);
			void SaveState(State& state);
			void LoadState(State& state);
			void RemoveEdge(State& state);
			void SetEdge(State& state);
			void ChangeSeed(State& state);
			void ShowState(State& state);
			void ShowChain(State& state);
			void NextState(State& state);
			void Create(State& state);
			void NewState(State& state);
		}
	}
}