#include "DbInfo.h"

namespace dots_and_boxes_solver
{
	//get folder of root directory
	std::string LayerInfo::GetRootDirectory() const
	{
		if (!gadt::filesystem::exist_directory(DAB_DB_ROOT_PATH))
			gadt::filesystem::create_directory(DAB_DB_ROOT_PATH);
		return DAB_DB_ROOT_PATH;
	}

	//get folder of root directory
	std::string LayerInfo::GetLayerDirectory() const
	{
		std::string path = GetRootDirectory() + gadt::ToString(_layer) + "/";
		if (!gadt::filesystem::exist_directory(path))
			gadt::filesystem::create_directory(path);
		return path;
	}

	//get folder of raw files and ensure its existence.
	std::string LayerInfo::GetRawDirectory() const
	{
		std::string path = GetLayerDirectory() + DAB_DB_RAW_PATH;
		if (!gadt::filesystem::exist_directory(path))
			gadt::filesystem::create_directory(path);
		return path;
	}

	//get folder of partition files and ensure its existence.
	std::string LayerInfo::GetPartitionDirectory() const
	{
		std::string path = GetLayerDirectory() + DAB_DB_PARTITION_PATH;
		if (!gadt::filesystem::exist_directory(path))
			gadt::filesystem::create_directory(path);
		return path;
	}

	//get the path of file by layer index.
	std::string LayerInfo::GetLayerInfoFilePath() const
	{
		std::string path = GetLayerDirectory() + DAB_DB_LAYER_INFO_NAME;
		return path;
	}

	//get the path of raw file.
	std::string LayerInfo::GetRawFilePath(size_t index) const
	{
		std::string path = GetRawDirectory() + index_to_raw_file_name(index);
		return path;
	}

	//get the path of partition file.
	std::string LayerInfo::GetPartitionFilePath(size_t index) const
	{
		std::string path = GetRawDirectory() + index_to_part_file_name(index);
		return path;
	}

	//load json file.
	void LayerInfo::LoadLayerInfoFile()
	{
		using json11::Json;
		std::string json_str = gadt::filesystem::load_file_as_string(GetLayerInfoFilePath());
		std::string err;
		Json json = json11::Json::parse(json_str, err);
		gadt::log::JsonLoader loader;
		size_t width = loader.JsonToUInt(json[DAB_DB_JSON_WIDTH_KEY]);
		size_t height = loader.JsonToUInt(json[DAB_DB_JSON_HEIGHT_KEY]);
		size_t layer = loader.JsonToUInt(json[DAB_DB_JSON_LAYER_KEY]);
		if (width == _width && height == _height && layer == _layer)
		{
			_raw_files = loader.JsonToUIntVector(json[DAB_DB_JSON_RAW_FILES_KEY]);
			_part_files = loader.JsonToUIntVector(json[DAB_DB_JSON_PART_FILES_KEY]);
			if (loader.no_error())
				_is_init = true;
		}
	}

	//load layer info from file path.
	LayerInfo::LayerInfo(size_t width, size_t height, size_t layer):
		_is_init(false),
		_width(width),
		_height(height),
		_layer(layer),
		_raw_files(),
		_part_files()
	{
		LoadLayerInfoFile();
	}

	//save to file.
	void LayerInfo::SaveToFile() const
	{
		std::string path = GetLayerInfoFilePath();
		std::vector<json11::Json> raw_files_json;
		std::vector<json11::Json> part_files_json;
		for (auto value : _raw_files)
			raw_files_json.push_back(json11::Json((int)value));
		for (auto value : _part_files)
			part_files_json.push_back(json11::Json((int)value));
		json11::Json info_json = json11::Json::object{
			{ DAB_DB_JSON_WIDTH_KEY, (int)_width },
			{ DAB_DB_JSON_HEIGHT_KEY, (int)_height },
			{ DAB_DB_JSON_LAYER_KEY, (int)_layer },
			{ DAB_DB_JSON_RAW_FILES_KEY, raw_files_json },
			{ DAB_DB_JSON_PART_FILES_KEY, part_files_json }
		};
		gadt::filesystem::save_string_as_file(info_json.dump(), path);
	}

	//print info of this layer.
	void LayerInfo::PrintInfo() const
	{
		using namespace gadt::console;
		Table tb(3, 3);
		tb.set_width({ 3, 5, 12 });
		std::string title = "Game: " + gadt::ToString(_width) + " x " + gadt::ToString(_height) + " , layer = " + gadt::ToString(_layer);
		tb.enable_title({ title, COLOR_GRAY, TABLE_ALIGN_MIDDLE });
		tb.set_cell_in_column(0, { { "Type"}, {"Raw"}, {"Part"} });
		tb.set_cell_in_row(0, { { "Type" }, { "STATE" }, { "FILES" } });
		std::stringstream raw_files_ss("[ ");
		std::stringstream part_files_ss("[ ");
		for (auto value : _raw_files)
			raw_files_ss << value << ", ";
		for (auto value : _part_files)
			part_files_ss << value << ", ";
		raw_files_ss << "]";
		part_files_ss << "]";
		tb.set_cell({ gadt::ToString(_raw_files.size()) + " files" }, 1, 1);
		tb.set_cell({ gadt::ToString(_part_files.size()) + " files" }, 2, 1);
		tb.set_cell({ raw_files_ss.str() }, 1, 2);
		tb.set_cell({ part_files_ss.str() }, 2, 2);
		tb.Print();
	}

	//deconstructor, this class would be save to file before it was destroy.
	LayerInfo::~LayerInfo()
	{
		SaveToFile();
	}
}
