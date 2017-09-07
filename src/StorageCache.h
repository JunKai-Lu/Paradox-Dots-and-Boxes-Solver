#include "GameDefine.h"

#pragma  once

namespace dots_and_boxes
{
	typedef std::pair<BoardValue, Margin> SolverState;
	typedef std::vector<SolverState> Storage;
	typedef std::map<BoardValue, Margin> SolverStateMap;

	/*
	* StorageCache include memory of save file method. 
	*/
	class StorageCache
	{
	private:
		bool _memory_method;
		Storage* _storage;
		std::string _cache_file_path;
		std::ofstream _os;
		size_t _count;

	public:

		/*
		* Default construct function would use memory as cache.
		*/
		StorageCache();

		/*
		* If the construct function be given a cache file name it would use file system as cache.
		* parameters
			* [cache_file_name] the file path of this cache.
		*/
		StorageCache(std::string cache_file_path);
		
		/*
		* Copy construct function. to avoid ofstream copy.
		* parameters
		* [sc] source StorageCache.
		*/
		StorageCache(StorageCache& sc);

		/*
		* Default destruct function.
		*/
		~StorageCache();

		/*
		* Push a solver state to cache.
		* parameter:
			* [board] board of the solver state that need be push to cache.
			* [margin] margin of the solver state that need be push to cache.
		*/
		inline void Push(BoardValue board,Margin margin)
		{
			_count++;
			if (_memory_method)
			{
				PushToStorage(board, margin);
			}
			else
			{
				PushToFile(board, margin);
			}
		}

		/*
		* Load all solver state in this cache to map.
		* parameter:
			* [ss_map] the target map.
		*/
		inline void ToMap(SolverStateMap& ss_map)
		{
			if (_memory_method)
			{
				MemoryCacheToMap(ss_map);
			}
			else
			{
				FileCacheToMap(ss_map);
			}
		}

		/*
		* Get current number of solver state in this storage cache;
		*/
		inline size_t count()
		{
			return _count;
		}

	private:
		inline void PushToStorage(BoardValue board, Margin margin)
		{
			_storage->push_back(SolverState(board,margin));
		}
		inline void PushToFile(BoardValue board, Margin margin)
		{
			_os << board << " " << margin << std::endl;
		}

		/*
		* Load all solver state in cache file to target map.
		* parameter:
			* [ss_map] the target map.
		*/
		void FileCacheToMap(SolverStateMap& ss_map);

		/*
		* Load all solver state in cache memory to target map.
		* parameter:
			* [ss_map] the target map.
		*/
		void MemoryCacheToMap(SolverStateMap& ss_map);
	};
}