#include "StateRepresentation.hpp"
#include "LayerConatiner.hpp"

#pragma once

namespace dots_and_boxes_solver
{
	//Filter Function Type of the Retrospector.
	using RetrospectFilterFunc = std::function<bool(BoardValueType)>;

	//LayerPart map to multi RawLayer
	class RetrospectMaper
	{
	private:
		//DabFileLoader& _source_file;
		//DabFileWriter& _target_file;
		RetrospectFilterFunc _filter_func;

	public:
	};

	//merger RawLayers into a Layer
	class RetrospectReducer
	{

	};
}
