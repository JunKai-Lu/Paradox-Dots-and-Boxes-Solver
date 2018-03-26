#include "LayerConatiner.hpp"
#include "StateRepresentation.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	struct FileInfo
	{
		std::string folder;
		std::string name;
	};

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class LayerController
	{
		friend class GameController<WIDTH, HEIGHT>;

	private:

		size_t _layer_depth;
		size_t _part_file_count;

		std::vector<FileInfo> raw_files;
		std::vector<FileInfo> part_files;

		const LayerController* _prev_layer;
		LayerController* _next_layer;

	private:

		bool JsonFileExist()
		{
			std::fstream fs;
			fs.open(get_json_path(), ios::in);
			return fs;//return true if open successfully.
		}

		bool FolderExist()
		{
			return gadt::file::DirExist(get_layer_path());
		}

		inline std::string get_game_path() const
		{
			return std::string(DAB_DB_PATH) + gadt::ToString(WIDTH) + "_" + gadt::ToString(HEIGHT) + "/";
		}

		inline std::string get_layer_path() const
		{
			return  get_game_path() + gadt::ToString(_layer_depth) + "/";
		}

		inline std::string get_json_path() const
		{
			return get_layer_path() + "LayerInfo.json";
		}

	protected:

		//creating new layer by prev layer.
		LayerController(size_t depth, size_t part_file_count):
			_layer_depth(depth),
			_part_file_count(part_file_count)
		{

		}

	public:

		//creating end-layer of the game.
		LayerController()
		{

		}

		void ToJson() const
		{
			if (!gadt::file::DirExist(get_layer_path()))
			{
				gadt::file::MakeDir(get_layer_path());
				if (!gadt::file::DirExist(get_layer_path()))
				{
					GADT_CHECK_WARNING(true, true, "SAVE JSON FAILED, UNABLE TO FIND GAME");
					return;
				}
			}
		}

		void AddRawFile(size_t raw_file_index)
		{

		}
	};

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class GameController
	{
	private:

		std::vector<LayerController<WIDTH, HEIGHT>> _layers;

		inline std::string get_game_path() const
		{
			return std::string(DAB_DB_PATH) + gadt::ToString(WIDTH) + "_" + gadt::ToString(HEIGHT) + "/";
		}



		void ensure_dir_exist()
		{
			if (!gadt::file::DirExist(DAB_DB_PATH))
				gadt::file::MakeDir(DAB_DB_PATH);
			if (!gadt::file::DirExist(get_game_path()))
				gadt::file::MakeDir(get_game_path());
		}

	public:

		GameController():
			_layers()
		{
			ensure_dir_exist();
		}
	};
}
