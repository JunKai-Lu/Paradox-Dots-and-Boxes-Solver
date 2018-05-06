#include "LayerConatiner.hpp"
#include "StateRepresentation.hpp"
#include "../lib/gadt/lib/json11/json11.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	/*
	* Json file structure:
		{
			game_width:5,
			game_height:5,
			layer_depth:60,
			raw_count:1,
			partition_count:1
			raw_files:[1,2,3,5,6...],
			partition_files:[4,5,6,7...]
		}
	*/
	constexpr const char* DAB_DB_ROOT_PATH = "./db/";
	constexpr const char* DAB_DB_RAW_PATH = "raw/";
	constexpr const char* DAB_DB_PARTITION_PATH = "partition/";
	constexpr const char* DAB_DB_RAW_SUFFIX = ".DabRawLayer";
	constexpr const char* DAB_DB_PARTITION_SUFFIX = ".DabLayerPartition";

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class LayerController
	{
	private:

		const size_t _layer_depth;
		const LayerController<WIDTH, HEIGHT>* _prev_layer;

		size_t _raw_count;
		size_t _partition_count;
		std::vector<bool> _raw_state;
		std::vector<bool> _partition_state;

	private:

		//creating new layer by prev layer.
		LayerController(size_t depth, const LayerController<WIDTH, HEIGHT>* prev_layer) :
			_layer_depth(depth),
			_prev_layer(prev_layer),
			_raw_count(prev_layer->_partition_count),
			_partition_count(1),
			_raw_state(prev_layer->_partition_count, false),
			_partition_state(_partition_count, false)
		{
			EnsureFolderExist();
			if (JsonFileExist())
			{
				LoadJson();
				gadt::console::ShowMessage("Load Layer " + gadt::ToString(depth) + "successfully!");
			}
			else
			{
				UpdateJson();
				gadt::console::ShowMessage("Create Layer " + gadt::ToString(depth) + "successfully!");
			}
		}

		//Create Folder
		void EnsureFolderExist() const
		{
			if (!gadt::file::DirExist(get_layer_path()))
			{
				gadt::file::MakeDir(get_layer_path());
				if (!gadt::file::DirExist(get_layer_path()))
					gadt::console::ShowError("Create folder failed.");
			}

			if (!gadt::file::DirExist(get_raw_path()))
			{
				gadt::file::MakeDir(get_raw_path());
				if (!gadt::file::DirExist(get_raw_path()))
					gadt::console::ShowError("Create folder failed.");
			}

			if (!gadt::file::DirExist(get_partition_path()))
			{
				gadt::file::MakeDir(get_partition_path());
				if (!gadt::file::DirExist(get_partition_path()))
					gadt::console::ShowError("Create folder failed.");
			}
		}

		//return true if the json file exist.
		bool JsonFileExist()
		{
			std::fstream fs;
			fs.open(get_json_path(), ios::in);
			return fs.is_open();//return true if open successfully.
		}

		//load json file.
		void LoadJson()
		{
			std::string err;
			json11::Json json = json11::Json::parse(get_json_path(), err);
			if (
				json["game_width"].number_value() == (int)WIDTH &&
				json["game_height"].number_value() == (int)HEIGHT &&
				json["layer_depth"].number_value() == (int)_layer_depth &&
				json["raw_count"].is_number() &&
				json["partition_count"].is_number() &&
				json["raw_files"].is_array() &&
				json["partition_files"].is_array()
				)
			{
				_raw_count = json["raw_count"].int_value();
				_partition_count = json["partition_count"].int_value();
				_raw_state = std::vector<bool>(_raw_count, false);
				_partition_state = std::vector<bool>(_partition_count, false);

				for (auto raw_file : json["raw_files"].array_items())
					_raw_state[raw_file.int_value()] = true;

				for (auto partition_file : json["partition_files"].array_items())
					_raw_state[partition_file.int_value()] = true;
			}
		}


		//get game path.
		inline std::string get_game_path() const
		{
			return std::string(DAB_DB_ROOT_PATH) + gadt::ToString(WIDTH) + "_" + gadt::ToString(HEIGHT) + "/";
		}

		//get layer path.
		inline std::string get_layer_path() const
		{
			return  get_game_path() + gadt::ToString(_layer_depth) + "/";
		}

		//get raw path.
		inline std::string get_raw_path() const
		{
			return get_layer_path() + DAB_DB_RAW_PATH;
		}

		//get partition path.
		inline std::string get_partition_path() const
		{
			return get_layer_path() + DAB_DB_PARTITION_PATH;
		}

		//get json path.
		inline std::string get_json_path() const
		{
			return get_layer_path() + "LayerInfo.json";
		}

		//get raw file path.
		inline std::string get_raw_file_path(std::string folder, size_t index)
		{
			return folder + gadt::ToString(index) + DAB_DB_RAW_SUFFIX;
		}

		//get partition file path.
		inline std::string get_partition_file_path(std::string folder, size_t index)
		{
			return folder + gadt::ToString(index) + DAB_DB_PARTITION_SUFFIX;
		}

	public:

		//creating end-layer of the game.
		LayerController():
			_layer_depth(EdgeCount<WIDTH, HEIGHT>()),
			_prev_layer(nullptr),
			_raw_count(0),
			_partition_count(1),
			_raw_state(),
			_partition_state(1)
		{
			EnsureFolderExist();
			DabFileWriter writer(get_partition_file_path(get_partition_path(), 0));
			DabState<WIDTH, HEIGHT> state;
			state.BeFull();
			writer.SaveItem(state.board().to_ullong(), 0);
			AddPartitionFile(0);
			UpdateJson();
		}

		//print info of files in this layer.
		void PrintInfo() const
		{
			gadt::log::ConsoleTable tb(4, max(_raw_state.size(), _partition_count) + 1);
			tb.enable_title({ "LAYER: " + gadt::ToString(_layer_depth), gadt::console::YELLOW, gadt::log::ALIGN_MIDDLE });
			tb.set_width({ 8,4,8,4 });
			tb.set_cell({ "RAW FILE", gadt::console::YELLOW, gadt::log::ALIGN_MIDDLE }, {0,0});
			tb.set_cell({ "PARTITION FILE", gadt::console::YELLOW, gadt::log::ALIGN_MIDDLE }, { 2,0 });
			for (size_t i = 0; i < _raw_state.size(); i++)
			{
				tb.set_cell({ gadt::ToString(i), gadt::console::BROWN , gadt::log::ALIGN_MIDDLE }, 0, i + 1);
				if(_raw_state[i])
					tb.set_cell({ "EXIST", gadt::console::GREEN }, 1, i + 1 );
				else
					tb.set_cell({ "NONE", gadt::console::RED }, 1, i + 1 );
			}
			for (size_t i = 0; i < _partition_state.size(); i++)
			{
				tb.set_cell({ gadt::ToString(i), gadt::console::BROWN, gadt::log::ALIGN_MIDDLE }, 2, i + 1 );
				if (_partition_state[i])
					tb.set_cell({ "EXIST", gadt::console::GREEN }, 3, i + 1 );
				else
					tb.set_cell({ "NONE", gadt::console::RED }, 3, i + 1 );
			}
			tb.print();
		}

		//add raw file.
		void AddRawFile(size_t index)
		{
			GADT_CHECK_WARNING(DAB_WARNING, index >= _raw_state.size(), "wrong index");
			_raw_state[index] = true;			
		}

		//add partition file.
		void AddPartitionFile(size_t index)
		{
			GADT_CHECK_WARNING(DAB_WARNING, index >= _partition_state.size(), "wrong index");
			_partition_state[index] = true;
		}

		//clear raw files
		void ClearRawFiles() const
		{
			gadt::file::RemoveDir(get_raw_path());
			gadt::file::MakeDir(get_raw_path());
		}

		//clear partition files.
		void ClearPartitionFiles() const
		{
			gadt::file::RemoveDir(get_partition_path());
			gadt::file::MakeDir(get_partition_path());
		}

		//get next layer
		LayerController<WIDTH, HEIGHT> GetNext() const
		{
			return LayerController<WIDTH,HEIGHT>((_layer_depth) - 1, this);
		}

		//update json file.
		void UpdateJson() const
		{
			EnsureFolderExist();
			std::vector<json11::Json> raw;
			std::vector<json11::Json> partition;
			for (int i = 0; i < _raw_state.size(); i++)
				if (_raw_state[i])
					raw.push_back(json11::Json(i));
			for (int i = 0; i < _partition_state.size(); i++)
				if (_partition_state[i])
					partition.push_back(json11::Json{ i });

			json11::Json json = json11::Json::object{
				{"game_width", (int)WIDTH},
				{"game_height", (int)HEIGHT},
				{"layer_depth", (int)_layer_depth},
				{"raw_count", (int)_raw_count},
				{"partition_count", (int)_partition_count},
				{"raw_files", json11::Json::array(raw)},
				{"partition_files", json11::Json::array(partition)}
			};
			std::string json_str = json.dump();
			ofstream ofs(get_json_path(), std::ios::out);
			ofs << json_str;
		}

		//return true if all the partitions had been done.
		bool IsFinished() const
		{
			for (auto b : _partition_state)
				if (!b)
					return false;
			return true;
		}
	};

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class GameController
	{
	private:

		std::vector<LayerController<WIDTH, HEIGHT>> _layers;
		size_t _focus_layer_index;

	private:

		void EnsureFolderExist()
		{
			if (!gadt::file::DirExist(DAB_DB_ROOT_PATH))
				gadt::file::MakeDir(DAB_DB_ROOT_PATH);
			if (!gadt::file::DirExist(get_game_path()))
				gadt::file::MakeDir(get_game_path());
		}

		inline std::string get_game_path() const
		{
			return std::string(DAB_DB_ROOT_PATH) + gadt::ToString(WIDTH) + "_" + gadt::ToString(HEIGHT) + "/";
		}

	public:

		//default constructor.
		GameController() :
			_layers(),
			_focus_layer_index(EdgeCount<WIDTH, HEIGHT>())
		{
			LoadLayerControllers();
		}

		//get current layer.
		inline size_t focus_layer_index() const
		{
			return _focus_layer_index;
		}

		//set current layer.
		inline void set_focus_layer_index(size_t layer)
		{
			_focus_layer_index = layer;
		}

		//get the reference of current layer.
		inline LayerController<WIDTH, HEIGHT>& current_layer()
		{
			return _layers[EdgeCount<WIDTH,HEIGHT>() - _focus_layer_index];
		}

		//return the count of existing layers
		inline size_t existing_layers_count() const
		{
			return _layers.size();
		}

		//print info of all existing layers in this db. 
		void PrintInfo() const
		{
			std::cout << "Game Folder: ";
			gadt::console::Cprintf(get_game_path(), gadt::console::YELLOW);
			std::cout << std::endl;
			std::cout << "Layer Number: " << _layers.size() << std::endl;
		}

		//create new layer behind the last existing layer.
		void CreateNextLayer()
		{
			if (_layers.size() < EdgeCount<WIDTH, HEIGHT>())
				if(_layers.back().IsFinished())
					_layers.push_back(_layers.back().GetNext());
				else
					gadt::console::ShowMessage("the last layer is still not finished");
			else
				gadt::console::ShowMessage("No more layer is allowed!");
		}

		//clear db.
		void ClearDB()
		{
			gadt::file::RemoveDir(DAB_DB_ROOT_PATH);
			LoadLayerControllers();
		}

		//load layer controllers from json files.
		void LoadLayerControllers()
		{
			EnsureFolderExist();
			_layers.clear();
			_layers.push_back(LayerController<WIDTH, HEIGHT>());
			while (_layers.back().IsFinished() && _layers.size() < EdgeCount<WIDTH, HEIGHT>())
				CreateNextLayer();
		}
	};
}
