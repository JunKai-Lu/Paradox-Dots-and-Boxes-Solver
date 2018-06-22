#include "LayerConatiner.hpp"

namespace dots_and_boxes_solver
{
	DabFileWriter::DabFileWriter(std::string file_path) :
		_ofs()
	{
		_ofs.open(file_path, std::ios::out);
	}

	DabFileLoader::DabFileLoader(std::string file_name) :
		_ifs()
	{
		_ifs.open(file_name, std::ios::in);
	}
}
