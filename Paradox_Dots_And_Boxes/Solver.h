#include "stdafx.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <map>
#include <list>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>

#include "GameDefine.h"
#include "MoveAnalyst.h"
#include "StorageCache.h"

#pragma  once

#define DISPLAYED_TIME_INTERVAL 5000000
#define MAX_ALLOW_THREAD 8


namespace DAB
{
	

	/*
	naming rules
	[./data ]								is the folder to save all solver data.
	[./data/layer_x]						is the data of layer x.
	[./data/layer_x/layer_x_n.dat]			is the solver temp data of layer x with index n.
	[./data/layer_x/layer_x_final.dat]		is the final data of layer n.
	[./data/layer_x/layer_x_solver.dat]		is the info about solver of layer x.
	[./data/solver.dat]						is the info about solver.
	*/

	

	class Solver
	{
	private:
		size_t _aim_depth;
		bool _file_cache;
		size_t _thread_num;
		size_t _current_depth;
		bool _use_filter;
		bool _auto_execute;
		std::ofstream _log;

	public:

		/*
		* Constructor Function
		* parameters:
			* [aim_depth] is aim of this function, program would finish after we reach the aim.
			* [file_cache] means this solver would use files as cache to decrease memory requirement.
			* [thread_num] is the theard number that solver is able to use
			* [auto_execute] can control whether this solver would start automaticlly after it finish load datas.
		*/
		Solver(size_t aim_depth, bool file_cache, bool use_filter, size_t thread_num, bool auto_execute);

		/*
		* Start solver until reach the aim
		*/
		void Run();

		inline size_t thread_num()
		{
			return _thread_num;
		}
		inline size_t aim_depth()
		{
			return _aim_depth;
		}
		inline size_t current_depth()
		{
			return _current_depth;
		}
		inline bool use_filter()
		{
			return _use_filter;
		}

	private:
		
		/*
		* Load data of last solved depth and return them as n parts in a vector;
		* return value: whether this function load target layer data successfully.
		* parameter:
			* [lists] is the vetor contains multiple list,this function would fill in those lists with data of target layer
			* [target_layer] is the layer that this function would load from.
			* [list_num] is the number of list in the vector.
		*/
		bool LoadStorages(std::vector<Storage>* storages, size_t target_layer);

		/*
		* Output the result of appointed layer
		* return value: the size of solver state after filter.
		* parameters:
			* [ss_map] the solver state map that contain all solver state of this layer.
			* [layer] the layer index
			* [filter] whether filter would be execute during output.
		*/
		size_t OutputResult(SolverStateMap& ss_map, size_t layer, bool filter = true);

		/*
		* try to compute next layer to close solver aim.
		*/
		void ComputeNextLayer();

	};

	namespace SOLVER
	{
		extern std::timed_mutex cout_mtx;

		/*
		* get current time as string format;
		* return value: the string of current time.
		*/
		std::string GetLogTimeStr();

		/*
		* get the final file name of apponted layer;
		* return value: the string of current time.
		* prameter
		*	[layer] appointed layer;
		*/
		std::string GetFinalFileName(size_t layer);

		/*
		* get time cost;
		* return value: time cost.
		* parameter:
		[start] the clock of start time
		*/
		inline double GetTimeConsuming(clock_t& start)
		{
			clock_t finish = clock();
			return (double)(finish - start) / CLOCKS_PER_SEC;
		}

		/*
		* Load current record and return the last solved depth(1-60)
		* return value: the current layer depth.
		* paramerter
		*	[print_info] control the log output.
		*/
		size_t GetCurrentDepth(bool print_info = true);

		/*
		* Write last solved layer to the 'solver.dat'
		* parameter
		*	[last_layer] is the number we save in 'solver.dat'
		*	[print_info] control the log output.
		*/
		bool WriteSolverData(size_t last_layer, bool print_info = true);

		/*
		* Load all data in appointed layer to an hash_table;this can be using in minimax;
		* parameters:
		* [hash_table] is the hash_table that would be fill in datassss
		* [target_layer] is the target layer
		*/
		bool LoadHashTable(std::unordered_map<BitBoard, Margin>* hash_table, size_t target_layer);

		/*
		* Compute new storage then save it to map.
		* parameters:
		*	[original_storage] is the storage need to compute
		*	[ss_map] is the map of new layer.
		*	[allow_filter] control whether the filter function would be used during ToMap.
		*/
		void ComputeStorageToMap(Storage& original_storage, SolverStateMap& ss_map, bool use_filter, bool print_info);

		/*
		* this function is used for a thread.
		*/
		void ThreadComputeStorageToCache(Storage& original_storage, StorageCache& storage_cache, bool allow_filter, size_t thread_index);

		/*
		* Judge whether a state is valuable. some valuelss state would can be recognized by this function.
		* parameters:
		* [board] is the state that need be judge.
		*/
		bool ValuableStateFilter(BitBoard board);
	}
}