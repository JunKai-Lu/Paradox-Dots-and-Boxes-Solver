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
		LayerPart& _source_layer;
		//RawLayer& _targer_layer;
		RetrospectFilterFunc _filter_func;

	public:

		RetrospectMaper(
			RawLayer& source_layer, 
			RetrospectFilterFunc filter_func = [](BoardValueType)->bool {return true; }
		);
		

	};

	//merger RawLayers into a Layer
	class RetrospectReducer
	{

	};
}
