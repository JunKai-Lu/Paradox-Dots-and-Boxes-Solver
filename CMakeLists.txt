cmake_minimum_required(VERSION 3.2)
project(General-AI-Development-Toolkit VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(SRC_LIST
        ./include/gadtlib.h
        ./include/gadtlib.cpp
        ./include/bitboard.hpp
        ./include/gameshell.h 
        ./include/gameshell.cpp 
        ./include/mcts.hpp 
        ./include/mcts_new.hpp
        ./include/visual_tree.h
        ./include/visual_tree.cpp
		./src/GameMain.cpp
		./src/GameDefine.h
        ./src/GameDefine.cpp
        ./src/StateAnalyst.h
		./src/StateAnalyst.cpp
		./src/StorageCache.h
		./src/StorageCache.cpp
		./src/Solver.h
		./src/Solver.cpp
        )
 
ADD_EXECUTABLE(Paradox_Dots-And-Boxes_Solver ${SRC_LIST})