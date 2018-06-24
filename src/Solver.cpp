#include "GameDefine.h"
#include "StateAnalyst.h"
#include "Solver.h"

#define COUT_MUTEX_TIME 100

using namespace std;
using namespace gadt;

namespace dots_and_boxes
{
	namespace solver
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

		//Load current record and return the last solved layer(1-60) 
		size_t GetCurrentLayer()
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
				Board current_board = Board(ss.first);
				Margin current_margin = ss.second;
				for (Edge i = 0; i <MAX_EDGE; i++)
				{
					//if edge 'i' exist
					if (current_board.get(i))
					{
						Board new_board = current_board;
						new_board.reset(i);	//get a new board without edge i
						new_board = state::MinimalForm(new_board);
						Margin new_margin;
						Margin num = state::TheNumOfFullBoxWithTheEdge(current_board, i);
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

						if (ss_map.count(new_board.to_ullong()))
						{
							//exist 
							if (new_margin > ss_map[new_board.to_ullong()])
							{
								ss_map[new_board.to_ullong()] = new_margin;	//refresh value if the margin is better.
							}
						}
						else
						{
							//no exist
							ss_map[new_board.to_ullong()] = new_margin;
						}
					}
				}
			}
		}

		//this function is used for a thread.
		void ThreadComputeStorageToCache(Storage& original_storage, StorageCache& storage_cache, bool use_filter, size_t thread_index)
		{
			if (original_storage.size() == 0)
			{
				return;
			}
			if (cout_mtx.try_lock_for(std::chrono::milliseconds(COUT_MUTEX_TIME)))
			{
				cout << ">> Thread " << thread_index << " : compute storage start[ToCache]..." << endl;
				cout_mtx.unlock();
			}

			for (SolverState& ss : original_storage)
			{
				Board current_board = Board(ss.first);
				Margin current_margin = ss.second;
				for (Edge i = 0; i <MAX_EDGE; i++)
				{
					//if edge 'i' exist
					if (current_board.get(i))
					{
						Board new_board = current_board;
						new_board.reset(i);	//get a new board without edge i
						new_board = state::MinimalForm(new_board);

						if (use_filter)
						{
							if (!ReasonableStateFilter(new_board))
							{
								continue;
							}
						}

						//if (!use_filter || ReasonableStateFilter(new_board))
						//{
						Margin new_margin;
						size_t num = state::TheNumOfFullBoxWithTheEdge(current_board, i);
						if (num != 0)
						{
							new_margin = current_margin + (Margin)num;	//break a box, the margin increase 1.
						}
						else
						{
							new_margin = -current_margin;	//do not break the box, change player and reverse margin.
						}
						storage_cache.Push(new_board.to_ullong(), new_margin);

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

			if (cout_mtx.try_lock_for(std::chrono::milliseconds(COUT_MUTEX_TIME)))
			{
				cout << ">> Thread " << thread_index << " : Compute storage finish, cache size = " << storage_cache.count() << endl;
				cout_mtx.unlock();
			}
		}

		//Constructor Function.
		Solver::Solver() :
			_aim_layer(60),
			_file_cache(false),
			_thread_num(1),
			_use_filter(false),
			_log("SolverLog.log", std::ios::app)
		{
		}
		Solver::Solver(const Solver& solver) :
			_aim_layer(solver._aim_layer),
			_file_cache(solver._file_cache),
			_thread_num(solver._thread_num),
			_use_filter(solver._use_filter),
			_log("SolverLog.log", std::ios::app)
		{
		}
		Solver::Solver(size_t aim_layer, bool file_cache, bool use_filter, size_t thread_num) :
			_aim_layer(aim_layer),
			_file_cache(file_cache),
			_thread_num(thread_num),
			_use_filter(use_filter),
			_log("SolverLog.log", std::ios::app)
		{
		}
		
		//Load data of last solved layer and return them as n parts;
		bool Solver::LoadStorages(std::vector< Storage >& storage_vec, size_t target_layer)
		{
			std::ios_base::sync_with_stdio(false);
			clock_t start = clock();
			cout << endl << "========== Load Storage ========== " << endl;
			cout << "target = " << target_layer << endl;
			cout << "thread_num = " << thread_num() << endl;

			_log << endl << "========== Load Storage ========== " << endl;
			_log << "target = " << target_layer << endl;
			_log << "thread_num = " << thread_num() << endl;

			//push new lists
			stringstream path;
			path << "./data/layer_" << target_layer << "/layer_" << target_layer << "_final.dat";
			if (_access(path.str().c_str(), 0) != -1)
			{
				ifstream target_file(path.str());
				//turn to load record
				size_t current_storage_index = 0;
				size_t count = 0;
				for (;;)
				{
					BoardValue bit;
					Margin mar;
					target_file >> bit >> mar;
					if (target_file.eof())
					{
						break;
					}
					count++;
					storage_vec[current_storage_index].push_back(SolverState(bit, mar));
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
				double time_consuming = solver::GetTimeConsuming(start);
				cout << "state num = " << count << endl;
				cout << "time cost = " << time_consuming << endl;
				cout << "================================== " << endl << endl;

				_log << "state num = " << thread_num() << endl;
				_log << "time cost = " << time_consuming << endl;
				_log << "================================== " << endl << endl;
				return true;
			}
			console::PrintError("can not find file " + path.str());
			return false;
		}

		//Output the result of appointed layer
		size_t Solver::OutputResult(SolverStateMap& ss_map, size_t layer)
		{
			std::ios_base::sync_with_stdio(false);
			clock_t start = clock();
			stringstream path;
			path << "./data/layer_" << layer;
			if (_access(path.str().c_str(), 0) == -1)
			{
				_mkdir(path.str().c_str());
			}
			path << "/layer_" << layer;// <<;

			ofstream os((path.str() + "_final.dat").c_str());
			console::PrintMessage(("Output target = " + path.str() + "_final.dat"));
			size_t count = 0;

			for (auto ss : ss_map)
			{
				count++;
				os << ss.first << " " << ss.second << std::endl;
			}

			double time_consuming = solver::GetTimeConsuming(start);

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
			size_t current_layer = solver::GetCurrentLayer();
			cout << ">> Solver [" << current_layer << " -> " << current_layer - 1 << "] start!" << endl << endl;
			_log << ">> Solver [" << current_layer << " -> " << current_layer - 1 << "] start!" << endl << endl;

			vector < Storage > storage_vec(thread_num());
			if (!LoadStorages(storage_vec, current_layer))
			{
				console::SystemPause();
				return;
			}

			vector<StorageCache> cache_vec;
			if (!(thread_num() == 1 && !_file_cache))
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
						cache_vec.push_back(StorageCache(ss.str()));
					}

					cout << ">> Create file cache success!" << endl;
					_log << ">> Create file cache success!" << endl;
				}
				else//using memory cache.
				{
					for (size_t i = 0; i < thread_num(); i++)
					{
						cache_vec.push_back(StorageCache());
					}
					cout << ">> Create memory cache success!" << endl;
					_log << ">> Create memory cache success!" << endl;
				}
			}

			SolverStateMap ss_map;
			if (thread_num() == 1 && !_file_cache)
			{
				cout << ">> Compute storage start[ToMap]..." << endl;
				solver::ComputeStorageToMap(storage_vec[0], ss_map, use_filter(), true);
				cout << ">> Compute storage, method = [ToMap], size = " << storage_vec[0].size() << endl;
				_log << ">> Compute storage, method = [ToMap], size = " << storage_vec[0].size() << endl;
			}
			else
			{
				vector<thread> threads(thread_num());
				std::timed_mutex cout_mutex;

				for (size_t i = 0; i < thread_num(); i++)
				{
					threads[i] = thread(solver::ThreadComputeStorageToCache, storage_vec[i], cache_vec[i], use_filter(), i);
				}

				for (auto& thd : threads)
				{
					thd.join();
				}
				cout << ">> all threads finish." << endl;

				//execute ToMap.
				for (size_t i = 0; i < cache_vec.size(); i++)
				{
					cout << ">> ToMap [ " << i + 1 << "/" << thread_num() << " ] executing..." << endl;
					_log << ">> ToMap [ " << i + 1 << "/" << thread_num() << " ] executing..." << endl;
					cache_vec[i].ToMap(ss_map);
				}
			}

			cout << endl << ">> Start outputing... " << endl;
			_log << endl << ">> Start outputing... " << endl;

			current_layer--;
			size_t output_size = OutputResult(ss_map, current_layer);

			solver::WriteSolverData(current_layer);
			cout << endl << ">> Solver [" << current_layer + 1 << " -> " << current_layer << "] finish!" << endl << endl;
			_log << endl << ">> Solver [" << current_layer + 1 << " -> " << current_layer << "] finish!" << endl << endl;
		}

		//start solver
		void Solver::Run()
		{
			for (;;)
			{
				if (solver::GetCurrentLayer() <= _aim_layer)
				{
					break;
				}
				ComputeNextLayer();
			}
		}
	}

	namespace minimax
	{
		Edge edge_queue[MAX_EDGE] = {
			12,47,17,42,
			43,48,13,18,46,41,16,11,
			6,7,8,53,52,51,23,22,21,36,37,38,
			39,44,49,54,9,14,19,24,50,45,40,35,20,15,10,5,
			0,1,2,3,4,59,58,57,56,55,29,28,27,26,25,30,31,32,33,34
		};

		Margin Minimax(Board board, Margin margin, size_t edge_num, size_t aim_edge_num, SolverHash& solver_hash)
		{
			/*for (;;)
			{
				Edge edge = state::GetDeadBoxRemainEdgeIndex(board);
				if (edge != MAX_EDGE)
				{
					board::EdgeSet(board, edge);
					margin += state::TheNumOfFullBoxWithTheEdge(board, edge);
					edge_num++;
					if (edge_num == aim_edge_num)
					{
#ifdef WARNING
						if (solver_hash.count(board))
						{
							return solver_hash[board];
						}
						else
						{
							stringstream ss;
							ss << "can not find state: " << board;
							WARNING_CHECK(ss.str(),"Minimax");
						}
#else
						return solver_hash[board];
#endif
					}
				}

				//standard state.
				Margin best_margin = -MAX_EDGE;
				for (size_t i = 0; i < MAX_EDGE; i++)
				{
					if (state::IsFreeEdge(board, EdgeQueue(i)))
					{
						Board new_board = board;
						board::EdgeSet(new_board, EdgeQueue(i));
						Margin child_margin = Minimax(new_board, -margin, edge_num, aim_edge_num, solver_hash);
						if (child_margin > best_margin)
						{
							best_margin = child_margin;
						}
					}
				}

				//judge
			}*/

			return 0;
		}
	}

}