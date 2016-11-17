#include "stdafx.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <unordered_map>
#include <list>
#include <io.h>
#include <direct.h>
#include <time.h>

#include "GameDefine.h"
#include "MoveAnalyst.h"
#include "Solver.h"

#define COUT_MUTEX_TIME 100

using namespace std;

namespace DAB
{
	//Constructor Function.
	Solver::Solver(size_t aim_depth, bool file_cache, bool use_filter, size_t thread_num) :
		_aim_depth(aim_depth),
		_file_cache(file_cache),
		_thread_num(thread_num),
		_use_filter(use_filter),
		_log("SolverLog.log",std::ios::app)
	{
		
		_current_depth = SOLVER::GetCurrentDepth();

		_log << "========== Solver Start ==========" << endl;
		_log << "start time = " << SOLVER::GetLogTimeStr() << endl;
		_log << "aim depth = " << _aim_depth << endl;
		_log << "current depth = " << _current_depth << endl;
		_log << "thread num = " << _thread_num << endl;
		_log << "file cache = " << _file_cache << endl;
		_log << "==================================" << endl << endl;

		/*if (_current_depth < _aim_depth)
		{
			std::cout << ">>>SYSTEM:[ your aim have been solved ]" << std::endl;
			system("pause");
			return;
		}*/
	}

	//Load data of last solved depth and return them as n parts;
	bool Solver::LoadStorages(std::vector< Storage >* storages, size_t target_depth)
	{
		std::ios_base::sync_with_stdio(false);
		clock_t start = clock();
		cout << endl << "========== Load Storage ========== " << endl;
		cout << "target = " << target_depth << endl;
		cout << "thread_num = " << thread_num() << endl;
		
		_log << endl << "========== Load Storage ========== " << endl;
		_log << "target = " << target_depth << endl;
		_log << "thread_num = " << thread_num() << endl;

		//push new lists
		stringstream path;
		path << "./data/layer_" << target_depth << "/layer_" << target_depth << "_final.dat";
		if (_access(path.str().c_str(), 0) != -1)
		{
			ifstream target_file(path.str());
			for (size_t i = 0; i < thread_num(); i++)
			{
				Storage new_storage;
				storages->push_back(new_storage);
			}

			//turn to load record
			size_t current_storage_index = 0;
			size_t count = 0;
			for (;;)
			{	
				BitBoard bit;
				Margin mar;
				target_file >> bit >> mar;
				if (target_file.eof())
				{
					break;
				}
				count++;
				storages->operator[](current_storage_index).push_back(SolverState(bit, mar));
				current_storage_index++;
				if (current_storage_index >= thread_num())
				{
					current_storage_index = 0;
				}
				if (count % DISPLAYED_TIME_INTERVAL == 0)
				{
					cout << ">> loading state = " << count << endl;
				}
			}
			double time_consuming = SOLVER::GetTimeConsuming(start);
			cout << "state num = " << count << endl;
			cout << "time cost = " << time_consuming << endl;
			cout << "================================== " << endl << endl;

			_log << "state num = " << thread_num() << endl;
			_log << "time cost = " << time_consuming << endl;
			_log << "================================== " << endl << endl;
			return true;
		}
		cout << ">> ERROR: [ can not find file " << path.str() << " ]" << endl;
		return false;
	}
	
	//Output the result of appointed layer
	size_t Solver::OutputResult(SolverStateMap& ss_map, size_t layer, bool filter)
	{
		std::ios_base::sync_with_stdio(false);
		clock_t start = clock();
		stringstream path;
		path << "./data/layer_" <<layer;
		if (_access(path.str().c_str(), 0) == -1)
		{
			_mkdir(path.str().c_str());
		}
		path << "/layer_" << layer;// <<;

		ofstream os((path.str() + "_final.dat").c_str());
		Message(("Output target = " + path.str() + "_final.dat"),false);
		size_t count = 0;

		if (filter)
		{
			for (auto ss : ss_map)
			{
				if (SOLVER::ValuableStateFilter(ss.first))
				{
					count++;
					os << ss.first << " " << ss.second << std::endl;
				}
			}
		}
		else
		{
			for (auto ss : ss_map)
			{
				count++;
				os << ss.first << " " << ss.second << std::endl;
			}
		}

		double time_consuming = SOLVER::GetTimeConsuming(start);

		_log << endl << "========== OUTPUT INFO ===========" << endl;
		_log << "time-consuming = " << time_consuming << endl;
		_log << "final size = " << count << endl;
		_log << "original size = " << ss_map.size() << endl;
		_log << "layer = " << layer << endl;
		_log << "==================================" << endl;

		cout << endl << "========== OUTPUT INFO ===========" << endl;
		cout << "time-consuming = " << time_consuming << endl;
		cout << "final size = " << count << endl;
		cout << "original size = " << ss_map.size() << endl;
		cout << "layer = " << layer << endl;
		cout << "==================================" << endl;
		return count;
	}

	//try to compute next layer to close solver aim.
	void Solver::ComputeNextLayer()
	{
		cout << ">> Solver [" << _current_depth << " -> " << _current_depth - 1 << "] start!" << endl << endl;
		_log << ">> Solver [" << _current_depth << " -> " << _current_depth - 1 << "] start!" << endl << endl;

		vector < Storage > storages(thread_num());		
		if (!LoadStorages(&storages, _current_depth))
		{
			system("pause");
			return;
		}

		vector<StorageCache> caches;
		if (! ( thread_num() == 1 && !_file_cache ))
		{
			if (_file_cache)//using file cache.
			{
				for (size_t i = 0; i < thread_num(); i++)
				{
					//create cache folder if it do not exist.
					if (_access("./cache", 0) == -1)
					{
						_mkdir("./cache");
					}

					stringstream ss;
					ss << "./cache/cache_" << i << ".dat";
					caches.push_back(StorageCache(ss.str()));
				}

				cout << ">> Create file cache success!" << endl;
				_log << ">> Create file cache success!" << endl;
			}
			else//using memory cache.
			{
				for (size_t i = 0; i < thread_num(); i++)
				{
					caches.push_back(StorageCache());
				}
				cout << ">> Create memory cache success!" << endl;
				_log << ">> Create memory cache success!" << endl;
			}
		}
		
		SolverStateMap ss_map;
		if (thread_num() == 1 && !_file_cache)
		{
			cout << ">> Compute storage start[ToMap]..." << endl;
			SOLVER::ComputeStorageToMap(storages[0], ss_map, use_filter(), true);
			cout << ">> Compute storage, method = [ToMap], size = " << storages[0].size() << endl;
			_log << ">> Compute storage, method = [ToMap], size = " << storages[0].size() << endl;
			storages.clear();
		}
		else
		{
			/*if (thread_num() ==1)
			{
				cout << ">> Compute storage start[ToCache]..." << endl;
				ComputeStorageToCache(storages[0], caches[0], true);
				cout << ">> Compute storage finish, method = [ToCache], cache size = " << caches[0].count() << endl;
				_log << ">> Compute storage finish, method = [ToCache], cache size = " << caches[0].count() << endl;
				storages.clear();
			}
			else
			{
				//multi thread, to do.

				storages.clear();	
			}*/
			vector<thread> threads(thread_num());
			std::timed_mutex cout_mutex;
			for (size_t i = 0; i < thread_num(); i++)
			{
				threads[i] = thread(SOLVER::ThreadComputeStorageToCache, storages[i], caches[i], use_filter(), i);
			}

			for (auto& thd : threads)
			{
				thd.join();
			}
			cout << ">> all threads finish." << endl;

			//execute ToMap.
			for (size_t i = 0; i < caches.size(); i++)
			{
				cout << ">> ToMap [ " << i + 1 << "/" << thread_num() << " ] executing..." << endl;
				_log << ">> ToMap [ " << i + 1 << "/" << thread_num() << " ] executing..." << endl;
				caches[i].ToMap(ss_map);
			}
		}
		cout << endl << ">> Start outputing... " << endl;
		_log << endl << ">> Start outputing... " << endl;
		
		_current_depth--;
		size_t output_size = OutputResult(ss_map, _current_depth, false);
		
		SOLVER::WriteSolverData(_current_depth);
		cout << endl << ">> Solver ["<< _current_depth + 1 << " -> " << _current_depth <<"] finish!"<< endl << endl;
		_log << endl << ">> Solver [" << _current_depth + 1 << " -> " << _current_depth << "] finish!" << endl << endl;
	}

	//start solver
	void Solver::Run()
	{	
		for (;;)
		{
			if (_current_depth <= _aim_depth)
			{
				break;
			}
			ComputeNextLayer();
		}
	}

	namespace SOLVER
	{
		std::timed_mutex cout_mtx;

		//get time as string format
		std::string GetLogTimeStr()
		{
			time_t t;
			tm local;
			char buf[128] = { 0 };
			t = time(NULL);
			localtime_s(&local, &t);
			strftime(buf, 64, "%Y.%m.%d-%H:%M:%S", &local);
			return std::string(buf);
		}

		//get the final file name of apponted layer;
		std::string GetFinalFileName(size_t layer)
		{
			stringstream path;
			path << "./data/layer_" << layer << "/layer_" << layer << "_final.dat";// <<;
			return path.str();
		}

		//Load current record and return the last solved depth(1-60) 
		size_t GetCurrentDepth(bool print_info)
		{
			//check whether the folder exist
			stringstream path;
			path << "./data";
			if (_access(path.str().c_str(), 0) == -1)
			{
				//if floder "./data" not exist, create it and layer 60;
				WriteSolverData(60);
				path << "/layer_60";
				_mkdir(path.str().c_str());
				path << "/layer_60_final.dat";
				ofstream fl(path.str(), std::ios::trunc);
				fl << 0x0FFFFFFFFFFFFFFF << " " << 0 << endl;
				return 60;
			}
			size_t last_layer;
			ifstream file("./data/solver.dat");
			file >> last_layer;

			if (print_info)
			{
				cout << ">> Load last layer success!, last layer = " << last_layer << endl << endl;
			}
			return last_layer;
		}

		//Write last solved layer to the 'solver.dat'
		bool WriteSolverData(size_t last_layer, bool print_info)
		{
			if (_access("./data", 0) == -1)
			{
				cout << ">> INFO:[can not find 'data' folder]" << endl;
				cout << ">> INFO:[folder 'data' would be created]" << endl;
				_mkdir("./data");
				if (_access("./data", 0) == -1)
				{
					return false;
				}
			}
			ofstream log("./data/solver.dat", std::ios::trunc);
			log << last_layer;
			//cout << ">> INFO: [write 'solver.dat' success!]" << endl;
			return true;
		}

		// Compute new storage then save it to map.
		void ComputeStorageToMap(Storage& original_storage, SolverStateMap& ss_map, bool use_filter, bool print_info)
		{
			size_t count = 0;
			//for all solver state in original storage.
			for (SolverState& ss : original_storage)
			{
				BitBoard current_board = ss.first;
				Margin current_margin = ss.second;
				for (Edge i = 0; i <MAX_EDGE; i++)
				{
					//if edge 'i' exist
					if (BOARD::EdgeExist(current_board, i))
					{
						BitBoard new_board = current_board;
						BOARD::EdgeRemove(new_board, i);	//get a new board without edge i
						new_board = STATE::MinimalForm(new_board);
						Margin new_margin;
						Margin num = STATE::TheNumOfFullBoxWithTheEdge(current_board, i);
						if (num != 0)
						{
							new_margin = current_margin + num;	//break one or more box, the margin increase.
						}
						else
						{
							new_margin = -current_margin;	//do not break the box, change player and reverse margin.
						}

						count++;
						if (count % DISPLAYED_TIME_INTERVAL == 0 && print_info)
						{
							cout << ">> Computing storage[ToMap], state num = " << count << endl;
						}

						if (ss_map.count(new_board))
						{
							//exist 
							if (new_margin > ss_map[new_board])
							{
								ss_map[new_board] = new_margin;	//refresh value if the margin is better.
							}
						}
						else
						{
							//no exist
							ss_map[new_board] = new_margin;
						}
					}
				}
			}
		}

		//this function is used for a thread.
		void ThreadComputeStorageToCache(Storage& original_storage, StorageCache& storage_cache, bool use_filter, size_t thread_index)
		{
			if (cout_mtx.try_lock_for(std::chrono::milliseconds(COUT_MUTEX_TIME)))
			{
				cout << ">> Thread " << thread_index << " : compute storage start[ToCache]..." << endl;
				cout_mtx.unlock();
			}

			for (SolverState& ss : original_storage)
			{
				BitBoard current_board = ss.first;
				Margin current_margin = ss.second;
				for (Edge i = 0; i <MAX_EDGE; i++)
				{
					//if edge 'i' exist
					if (BOARD::EdgeExist(current_board, i))
					{
						BitBoard new_board = current_board;
						BOARD::EdgeRemove(new_board, i);	//get a new board without edge i
						new_board = STATE::MinimalForm(new_board);
						if (!use_filter || ValuableStateFilter(new_board))
						{
							Margin new_margin;
							size_t num = STATE::TheNumOfFullBoxWithTheEdge(current_board, i);
							if (num != 0)
							{
								new_margin = current_margin + (Margin)num;	//break a box, the margin increase 1.
							}
							else
							{
								new_margin = -current_margin;	//do not break the box, change player and reverse margin.
							}
							storage_cache.Push(new_board, new_margin);

							if (storage_cache.count() % DISPLAYED_TIME_INTERVAL == 0)
							{
								if (cout_mtx.try_lock_for(std::chrono::milliseconds(COUT_MUTEX_TIME)))
								{
									cout << ">> Thread " << thread_index << ": Computed storage state num = " << storage_cache.count() << endl;
									cout_mtx.unlock();
								}
							}
						}
					}
				}
			}

			if (cout_mtx.try_lock_for(std::chrono::milliseconds(COUT_MUTEX_TIME)))
			{
				cout << ">> Thread " << thread_index << " : Compute storage finish, cache size = " << storage_cache.count() << endl;
				cout_mtx.unlock();
			}
		}
	}
}