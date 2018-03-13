#include "GameDefine.h"

#pragma once

namespace dots_and_boxes
{
	/*
	* EdgeSet is a class that contains all the edges of a state. Each edge was represened by a bool variable.
	* HE = horizon edges.
	* VE = vertical edges.
	* THE = top horizon edge of a box.
	* BHE = bottom horizon edge of a box.
	* LVE = left vertical edge of a box.
	* RVE = left vertical edge of a box.

	* all the index of horizon edges were ordered by left to right, then up to down.
	* all the index of vertical edges were ordered by up to down, then left to right.
	* the index of horizon edges is in the front of the indexs of vertical edges.  
	*/

	template<size_t WIDTH, size_t HEIGHT, std::enable_if< 2 * WIDTH * HEIGHT + WIDTH + HEIGHT <= 64, int> = 0>
	class EdgeSet
	{
	private:
		//static variables
		static size_t w0 = 0;//first index of horizon edges in the array.
		static size_t h0 = WIDTH * HEIGHT + WIDTH;//first index of vertical edges in the array.
		static size_t num_of_edges = 2 * WIDTH * HEIGHT + WIDTH + HEIGHT;//number of edges.
		static size_t num_of_boxes = WIDTH * HEIGHT;

	private:
		Board _edges;

	private:
		/*
		* edge operation functions.
		*/

		//return true if the 'edge' is HE.
		inline bool is_HE(Edge edge) const
		{
			return edge >= 0 && edge < h0;
		}

		//return true if the 'edge' is VE.
		inline bool is_VE(Edge edge) const
		{
			return edge >= h0 && edge < num_of_edges;
		}

		//return true if the 'edge' is THE.
		inline bool is_THE(Edge edge) const
		{
			return edge >= 0 && edge < (h0 - WIDTH);
		}

		//return true if the 'edge' is BHE.
		inline bool is_BHE(Edge edge) const
		{
			return edge >= WIDTH  && edge < h0;
		}

		//return true if the 'edge' is LVE.
		inline bool is_LVE(Edge edge) const
		{
			return edge >= h0 && edge < (num_of_edges - HEIGHT);
		}

		//return true if the 'edge' is VE.
		inline bool is_RVE(Edge edge) const
		{
			return edge >= (h0 + HEIGHT) && edge < num_of_edges;
		}

		//get THE index by width and height
		inline Edge get_THE(Edge w, Edge h)
		{
			return (h * WIDTH) + w;
		}

		//get LVE index by width and height
		inline Edge get_LVE(Edge w, Edge h)
		{
			return h0 + (HEIGHT * w) + h;
		}
		
		//get BHE index by width and height
		inline Edge get_BHE(Edge w, Edge h)
		{
			return (h * WIDTH) + w + WIDTH;
		}

		//get RVE index by width and height
		inline Edge get_RVE(Edge w, Edge h)
		{
			return h0 + (HEIGHT * w) + h + HEIGHT;
		}

		//return the height of the THE 'edge'. 
		inline Edge get_THE_height(Edge edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_HE(edge), " is not HE");
			return edge / width;
		}

		//return the width of the THE 'edge'.
		inline Edge get_THE_width(Edge edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_HE(edge), " is not VE");
			return edge % width;
		}

		//return the height of the LVE 'edge'. 
		inline Edge get_LVE_height(Edge edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_VE(edge), " is not VE");
			return (edge - h0) % HEIGHT;
		}

		//return the width of the LVE 'edge'.
		inline Edge get_LVE_width(Edge edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_VE(edge), " is not HE");
			return (edge - h0) / HEIGHT;
		}

		//LVE -> THE of a box.
		inline Edge LVE_to_THE(Edge edge) const
		{
			Edge w = get_LVE_width(edge);
			Edge h = get_LVE_height(edge);
			return get_THE(w, h);
		}

		//THE -> LVE of a box. 
		inline Edge THE_to_LVE(Edge edge) const
		{
			Edge w = get_THE_width(edge);
			Edge h = get_THE_height(edge);
			return get_LVE(w, h);
		}

		//THE -> BHE of a box.
		inline Edge THE_to_BHE(Edge edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_THE(edge), "is not THE");
			return edge + WIDTH;
		}

		//BHE -> THE of a box.
		inline Edge BHE_to_THE(Edge edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_BHE(edge), "is not BHE");
			return edge - WIDTH;
		}

		//LVE -> RVE of a box.
		inline Edge LVE_to_RVE(Edge edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_LVE(edge), "is not LVE");
			return edge + HEIGHT;
		}

		//RVE -> LVE of a box.
		inline Edge RVE_to_LVE(Edge edge) const
		{
			GADT_CHECK_WARNING(DAB_WARNING, !is_RVE(edge), "is not RVE");
			return edge - HEIGHT;
		}

		/*
		* box operation
		*/

		//is box index
		inline bool is_box_index(size_t index) const
		{
			return index >= 0 && index < num_of_boxes;
		}

		//get box width
		inline Edge get_box_width(size_t index) const
		{
			return get_THE_width(index);
		}

		//get box height
		inline Edge get_box_height(size_t index) const
		{
			return get_THE_height(index);
		}



	public:
		
		//get height
		inline size_t height() const 
		{
			return HEIGHT;
		}

		//get width
		inline size_t width() const
		{
			return WIDTH;
		}

		//get num of edges.
		inline size_t num_of_edges() const
		{
			return num_of_edges;
		}
	};
}
