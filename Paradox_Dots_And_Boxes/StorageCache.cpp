#include "StorageCache.h"
#include "GameDefine.h"

#include <fstream>

using namespace std;

namespace dots_and_boxes
{
	
	//Default construct function would use memory as cache.
	StorageCache::StorageCache() :
		_memory_method(true),
		_cache_file_path("./cache/cache.dat"),
		_count(0)
	{
		_storage = new Storage;
	}

	//If the construct function be given a cache file name it would use file system as cache.	
	StorageCache::StorageCache(std::string cache_file_path) :
		_memory_method(false),
		_storage(nullptr),
		_cache_file_path(cache_file_path),
		_os(_cache_file_path),
		_count(0)
	{
	}

	//Copy construct function. to avoid ofstream copy.
	StorageCache::StorageCache(StorageCache& sc):
		_memory_method(sc._memory_method),
		_cache_file_path(sc._cache_file_path),
		_os(sc._cache_file_path),
		_count(sc._count)
	{
		if (sc._memory_method)
		{
			_storage = new Storage;
			for (size_t i = 0; i < sc._storage->size(); i++)
			{
				_storage->push_back(sc._storage->operator[](i));
			}
		}
		else
		{
			_storage = nullptr;	
		}
	}

	//Default destruct function.
	StorageCache::~StorageCache()
	{
		if (_memory_method)
		{
			delete _storage;
		}
		
	}

	//Load all solver state in cache file to target map.
	void StorageCache::FileCacheToMap(SolverStateMap& ss_map)
	{
		if (!_memory_method)
		{
			ifstream ifs(_cache_file_path);
			for (;;)
			{
				long long board;
				Margin margin;
				ifs >> board >> margin;
				if (!ifs.eof())
				{
					if (ss_map.count(board))
					{
						//exist 
						if (margin > ss_map[board])
						{
							ss_map[board] = margin;	//refresh value if the margin is better.
						}
					}
					else
					{
						//no exist
						ss_map[board] = margin;
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	//Load all solver state in cache memory to target map.
	void StorageCache::MemoryCacheToMap(SolverStateMap& ss_map)
	{
		if (_memory_method)
		{
			for (SolverState& ss : *_storage)
			{
				if (ss_map.count(ss.first))
				{
					//exist 
					if (ss.second > ss_map[ss.first])
					{
						ss_map[ss.first] = ss.second;	//refresh value if the margin is better.
					}
				}
				else
				{
					//no exist
					ss_map.insert(ss);
				}
			}
		}
	}

}
