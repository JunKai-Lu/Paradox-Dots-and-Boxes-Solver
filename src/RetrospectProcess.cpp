#include "RetrospectProcess.h"

namespace dots_and_boxes_solver
{
	RetrospectMaper::RetrospectMaper(RawLayer& source_layer, RetrospectFilterFunc filter_func) :
		_source_layer(source_layer),
		_filter_func(filter_func)
	{
	}
}