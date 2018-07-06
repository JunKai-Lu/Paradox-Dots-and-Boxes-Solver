#include "BasicDefine.h"

namespace dots_and_boxes_solver
{
	std::unordered_map<BoardValueType, MarginType> GLOBAL_HASH_TABLE;

	size_t EdgeCount(size_t width, size_t height)
	{
		return 2 * width * height + width + height;
	}
	size_t BoxCount(size_t width, size_t height)
	{
		return width * height;
	}
}