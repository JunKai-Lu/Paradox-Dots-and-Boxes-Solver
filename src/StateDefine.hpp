#include "GameDefine.h"

#pragma once

namespace dots_and_boxes_solver
{
	/*
	* EdgeSet is a class that contains all the edges of a state. Each edge was represened by a bool variable.
	* he = horizon edges.
	* ve = vertical edges.
	* the = top horizon edge of a box.
	* bhe = bottom horizon edge of a box.
	* lve = left vertical edge of a box.
	* rve = left vertical edge of a box.

	* all the index of horizon edges were ordered by left to right, then up to down.
	* all the index of vertical edges were ordered by up to down, then left to right.
	* the index of horizon edges is in the front of the indexs of vertical edges.  
	* all the inndex of boxes were orderd by left to right, then up to down, which is same with the index of its top horizon edge(the).
	*/

	using DabPos = gadt::BasicUPoint<uint8_t>;

	struct DabMove
	{
		EdgeIndex edge;
	};

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabBoard
	{
	private:
		//static variables
		static constexpr size_t _EDGE_COUNT = EdgeCount<WIDTH, HEIGHT>();	//count of edges.
		static constexpr size_t _BOX_COUNT = BoxCount<WIDTH, HEIGHT>();		//count of boxes.

		static constexpr size_t _H0 = 0;						//first index of horizon edges in the array.
		static constexpr size_t _V0 = WIDTH * HEIGHT + WIDTH;	//first index of vertical edges in the array.

		static constexpr size_t _LTB_HE = _H0;					//index of outside horizon edge of left top box;
		static constexpr size_t _LTB_VE = _V0;					//index of outside vertical edge of left top box;
		static constexpr size_t _RTB_HE = _H0 + WIDTH - 1;		//index of outside horizon edge of right top box;
		static constexpr size_t _RTB_VE = _EDGE_COUNT - HEIGHT;	//index of outside vertical edge of right top box;
		static constexpr size_t _LBB_HE = _V0 - WIDTH;			//index of outside horizon edge of left bottom box;
		static constexpr size_t _LBB_VE = _V0 + HEIGHT - 1;		//index of outside vertical edge of left bottom box;
		static constexpr size_t _RBB_HE = _V0 -1;				//index of outside horizon edge of right bottom box;
		static constexpr size_t _RBB_VE = _EDGE_COUNT - 1;		//index of outside vertical edge of right bottom box;

	private:

		BoardType _edges;

	private:
		/*
		* edge operation functions.
		*/

		//return true if the 'edge' is the index of a edge.
		inline constexpr bool is_edge_index(size_t index) const
		{
			return index >= 0 && index < num_of_edges();
		}

		//return true if the 'edge' is he.
		inline constexpr bool is_he(EdgeIndex edge) const
		{
			return edge >= 0 && edge < _V0;
		}

		//return true if the 'edge' is ve.
		inline constexpr bool is_ve(EdgeIndex edge) const
		{
			return edge >= _V0 && edge < num_of_edges();
		}

		//return true if the 'edge' is the.
		inline constexpr bool is_the(EdgeIndex edge) const
		{
			return edge >= 0 && edge < (_V0 - WIDTH);
		}

		//return true if the 'edge' is bhe.
		inline constexpr bool is_bhe(EdgeIndex edge) const
		{
			return edge >= WIDTH  && edge < _V0;
		}

		//return true if the 'edge' is lve.
		inline constexpr bool is_lve(EdgeIndex edge) const
		{
			return edge >= _V0 && edge < (num_of_edges() - HEIGHT);
		}

		//return true if the 'edge' is ve.
		inline constexpr bool is_rve(EdgeIndex edge) const
		{
			return edge >= (_V0 + HEIGHT) && edge < num_of_edges();
		}

		//get the index by width and height
		inline constexpr EdgeIndex get_the(DabPos pos) const
		{ 
			return (pos.y * WIDTH) + pos.x;
		}

		//get lve index by width and height
		inline constexpr EdgeIndex get_lve(DabPos pos) const
		{
			return _V0 + (HEIGHT * pos.x) + pos.y;
		}
		
		//get bhe index by width and height
		inline constexpr EdgeIndex get_bhe(DabPos pos) const
		{
			return (pos.y * WIDTH) + pos.x + WIDTH;
		}

		//get rve index by width and height
		inline constexpr EdgeIndex get_rve(DabPos pos) const
		{
			return _V0 + (HEIGHT * pos.x) + pos.y + HEIGHT;
		}

		//get the position of 'edge'.
		inline constexpr DabPos get_the_pos(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_he(edge), " is not he");
			return { edge % WIDTH , edge / WIDTH };
		}

		//get the position of 'edge'.
		inline constexpr DabPos get_lve_pos(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_ve(edge), " is not ve");
			return { (edge - _V0) / HEIGHT , (edge - _V0) % HEIGHT };
		}

		//lve -> the of a box.
		inline constexpr EdgeIndex lve_to_the(EdgeIndex edge) const
		{
			DabPos pos = get_lve_pos(edge);
			return get_the(pos);
		}

		//the -> lve of a box. 
		inline constexpr EdgeIndex the_to_lve(EdgeIndex edge) const
		{
			DabPos pos = get_the_pos(edge);
			return get_lve(pos);
		}

		//the -> bhe of a box.
		inline constexpr EdgeIndex the_to_bhe(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_the(edge), "is not the");
			return edge + WIDTH;
		}

		//bhe -> the of a box.
		inline constexpr EdgeIndex bhe_to_the(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_bhe(edge), "is not bhe");
			return edge - WIDTH;
		}

		//lve -> rve of a box.
		inline constexpr EdgeIndex lve_to_rve(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_lve(edge), "is not lve");
			return edge + HEIGHT;
		}

		//rve -> lve of a box.
		inline constexpr EdgeIndex rve_to_lve(EdgeIndex edge) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_rve(edge), "is not rve");
			return edge - HEIGHT;
		}

		/*
		* box operation
		*/

		//return true if the 'index' is the index of a box.
		inline constexpr bool is_box_index(size_t index) const
		{
			return index >= 0 && index < num_of_boxes();
		}

		//return true if the 'pos' is the point of a box.
		inline constexpr bool is_box_pos(DabPos pos) const
		{
			return pos.x < WIDTH && pos.y < HEIGHT;
		}

		//get the position of the box by index.
		inline constexpr DabPos get_box_pos(EdgeIndex index) const
		{
			return get_the_pos(index);
		}

		

	public:
		
		//default constructor.
		constexpr inline DabBoard():
			_edges(0)
		{
		}

		//construct by value.
		constexpr inline DabBoard(BoardType b):
			_edges(b)
		{

		}

		//construct by value.
		constexpr inline DabBoard(BoardValueType bv):
			_edges(bv)
		{

		}

		//to int
		inline constexpr BoardValueType to_ullong() const
		{
			return _edges.to_ullong();
		}

		//get height
		inline constexpr size_t height() const 
		{
			return HEIGHT;
		}

		//get width
		inline constexpr size_t width() const
		{
			return WIDTH;
		}

		//get the number of edges.
		inline constexpr size_t num_of_edges() const
		{
			return EdgeCount<WIDTH, HEIGHT>();
		}

		//get the number of boxes.
		inline constexpr size_t num_of_boxes() const
		{
			return BoxCount<WIDTH, HEIGHT>();
		}

		inline size_t edge_count() const
		{
			return _edges.total();
		}

		//return true if the edge exist.
		inline bool edge_exist(size_t index) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_edge_index(index), "is not the index of edge");
			return _edges[index];
		}

		//return true if the box had been captured.
		inline bool box_exist(DabPos pos) const
		{
			GADT_WARNING_IF(DAB_WARNING, !is_box_pos(pos), "it is not box index");
			return box_edges_count(pos) == 4;
		}

		//return the number of exist that had exist for a box.
		inline size_t box_edges_count(DabPos pos) const
		{
			return 
				(size_t) the_exist(pos) +
				(size_t) bhe_exist(pos) +
				(size_t) lve_exist(pos) +
				(size_t) rve_exist(pos);
		}

		//get the count of boxes that owns this edge, which means this edge is a part of the box(es), the result may be { 0, 1, 2}.
		inline size_t count_of_boxes_that_owns_edge(EdgeIndex edge) const
		{
			size_t count = 0;
			if (edge_exist(edge))
			{
				if (is_he(edge))
				{
					if (is_the(edge))
					{
						EdgeIndex lve = the_to_lve(edge);
						EdgeIndex rve = lve_to_rve(lve);
						EdgeIndex bhe = the_to_bhe(edge);
						if (edge_exist(lve) && edge_exist(rve) && edge_exist(bhe))
							count += 1;
					}
					if (is_bhe(edge))
					{
						EdgeIndex the = bhe_to_the(edge);
						EdgeIndex lve = the_to_lve(the);
						EdgeIndex rve = lve_to_rve(lve);
						if (edge_exist(lve) && edge_exist(rve) && edge_exist(the))
							count += 1;
					}
				}
				else if (is_ve(edge))
				{
					if (is_lve(edge))
					{
						EdgeIndex rve = lve_to_rve(edge);
						EdgeIndex the = lve_to_the(edge);
						EdgeIndex bhe = the_to_bhe(the);
						if (edge_exist(the) && edge_exist(bhe) && edge_exist(rve))
							count += 1;
					}
					if (is_rve(edge))
					{
						EdgeIndex lve = rve_to_lve(edge);
						EdgeIndex the = lve_to_the(lve);
						EdgeIndex bhe = the_to_bhe(the);
						if (edge_exist(the) && edge_exist(bhe) && edge_exist(lve))
							count += 1;
					}
				}
			}
			return count;
		}

		//set edge.
		inline void set_edge(EdgeIndex index)
		{
			_edges.set(index);
		}

		//reset a edge to empty
		inline void reset_edge(EdgeIndex index)
		{
			_edges.reset(index);
		}

		//return true if the edge of box exist.
		inline bool the_exist(DabPos box_pos) const
		{
			EdgeIndex edge = get_the(box_pos);
			return edge_exist(edge);
		}

		//return true if bhe edge of box exist.
		inline bool bhe_exist(DabPos box_pos) const
		{
			EdgeIndex edge = get_bhe(box_pos);
			return edge_exist(edge);
		}

		//return true if lve edge of box exist.
		inline bool lve_exist(DabPos box_pos) const
		{
			EdgeIndex edge = get_lve(box_pos);
			return edge_exist(edge);
		}

		//return true if rve edge of box exist.
		inline bool rve_exist(DabPos box_pos) const
		{
			EdgeIndex edge = get_rve(box_pos);
			return edge_exist(edge);
		}

		//fill in all the edges.
		inline void set_all_edges()
		{
			for (EdgeIndex i = 0; i < num_of_edges(); i++)
				set_edge(i);
		}

		//reset all the edges.
		inline void reset_all_edges()
		{
			_edges = 0;
		}

		//minimize corner if possible, which may make the value smaller. 
		void ChangeCorner()
		{
			if (_edges[_LTB_VE] == true && _edges[_LTB_HE] == false)
			{
				_edges.set(_LTB_HE);
				_edges.reset(_LTB_VE);
			}
			if (_edges[_RTB_VE] == true && _edges[_RTB_HE] == false)
			{
				_edges.set(_RTB_HE);
				_edges.reset(_RTB_VE);
			}
			if (_edges[_LBB_VE] == true && _edges[_LBB_HE] == false)
			{
				_edges.set(_LBB_HE);
				_edges.reset(_LBB_VE);
			}
			if (_edges[_RBB_VE] == true && _edges[_RBB_HE] == false)
			{
				_edges.set(_RBB_HE);
				_edges.reset(_RBB_VE);
			}
		}

		//rotate the board with a 180 angle.
		DabBoard Rotate() const
		{
			BoardType new_edges;
			for (size_t i = _H0; i < _V0; i++)
				if (_edges[_V0 - 1 - i] == true)
					new_edges.set(i);
			for (size_t i = _V0; i < _EDGE_COUNT; i++)
				if (_edges[(_EDGE_COUNT - 1) - (i - _V0)] == true)
					new_edges.set(i);
			return DabBoard(new_edges);
		}

		//reverse the board(left-right)
		DabBoard Reverse() const
		{
			BoardValueType temp = 0;
			BoardValueType source = _edges.to_ullong();

			//assemble the part of vectical edges
			for (size_t ve_column = 0; ve_column < WIDTH + 1; ve_column++)
			{
				BoardValueType feature = (UINT64_MAX >> (64 - HEIGHT));//the value would be 0x1F if HEIGHT = 5
				size_t v = (_V0 - HEIGHT + (ve_column * HEIGHT));
				BoardValueType part = (source >> (_V0 + (ve_column * HEIGHT))) & feature;
				temp = (temp << HEIGHT) | part;
			}

			//assemble the part of horizon edges.
			for (size_t he_row = 0; he_row < HEIGHT + 1; he_row++)
			{
				size_t he_begin = (_V0 - WIDTH) - (he_row * WIDTH);//begin of each he row, from bottom to top.
				for (size_t he_index = 0; he_index < WIDTH; he_index++)
				{
					temp = temp << 1;
					if (_edges.get(he_begin + he_index) == true)
						temp = temp | 0x1;
				}
			}
			return DabBoard(temp);
		}

		//diagonally reverse the board
		DabBoard DiagonalReverse() const
		{
			constexpr bool is_square = (WIDTH == HEIGHT) ? true : false;
			if (is_square)
			{
				BoardValueType temp = 0;
				for (size_t i = 0; i < _EDGE_COUNT; i++)
				{
					temp = temp << 1;
					if (_edges[i] == true)
						temp |= 0x1;
				}
				return DabBoard(temp);
			}
			return *this;
		}

		//get the minimal format of this board by rotate/reverse and change corner.
		DabBoard ToMinimalFormat() const
		{
			//4 form for all board: prototype, rotate, reverse, rotate + reverse
			//4 additional form for square board: diagonally reverse all above 4 form.
			constexpr bool is_square = (WIDTH == HEIGHT) ? true : false;
			constexpr size_t count = is_square ? 8 : 4;

			DabBoard boards[count];
			boards[0] = *this;//prototype
			boards[1] = this->Rotate();
			boards[2] = this->Reverse();
			boards[3] = boards[1].Reverse();
			if (is_square)
			{
				boards[4] = boards[0].DiagonalReverse();
				boards[5] = boards[1].DiagonalReverse();
				boards[6] = boards[2].DiagonalReverse();
				boards[7] = boards[3].DiagonalReverse();
			}
			boards[0].ChangeCorner();
			DabBoard minimal = boards[0];
			for (size_t i = 1; i < count; i++)
			{
				boards[i].ChangeCorner();
				if (boards[i]._edges.to_ullong() < minimal._edges.to_ullong())
					minimal = boards[i];
			}
			return minimal;
		}

		//return ture if the two box are neighbour
		bool IsNeighbourBox(DabPos a, DabPos b) const
		{
			if (a.x == b.x)//same column
				if (a.y - b.y == 1 || b.y - a.y == 1)
					return true;
			if (a.y == b.y)//same row
				if (a.x - b.x == 1 || b.x - a.x == 1)
					return true;
			return false;
		}

		//judge whether a edge is a free-edge, which means the fillup of it would not cause a dead-chain. 
		bool IsFreeEdge(EdgeIndex edge) const
		{
			if (is_he(edge))
			{
				if (_edges.get(edge) == false)
				{ 
					bool affect_upper_box = false;
					bool affect_lower_box = false;
					//check the box below the edge.
					if (is_the(edge))
					{
						EdgeIndex fir_box_bhe = the_to_bhe(edge);
						EdgeIndex fir_box_lve = the_to_lve(edge);
						EdgeIndex fir_box_rve = lve_to_rve(fir_box_lve);
						size_t fir_box_edge_num = (size_t)_edges.get(fir_box_bhe) + (size_t)_edges.get(fir_box_lve) + (size_t)_edges.get(fir_box_rve);
						if (fir_box_edge_num == 2)
						{
							//check the lower box.
							if (_edges.get(fir_box_bhe) == false)
							{
								if (is_the(fir_box_bhe))
								{
									EdgeIndex sec_box_the = fir_box_bhe;
									EdgeIndex sec_box_bhe = the_to_bhe(sec_box_the);
									EdgeIndex sec_box_lve = the_to_lve(sec_box_the);
									EdgeIndex sec_box_rve = lve_to_rve(sec_box_lve);
									if ((size_t)_edges.get(sec_box_lve) + (size_t)_edges.get(sec_box_rve) + (size_t)_edges.get(sec_box_bhe) == 2)
									{
										affect_lower_box = true;
									}
								}
							}
							//check the right box.
							else if (_edges.get(fir_box_lve) == false)
							{
								if (is_rve(fir_box_lve))
								{
									EdgeIndex sec_box_rve = fir_box_lve;
									EdgeIndex sec_box_lve = rve_to_lve(sec_box_rve);
									EdgeIndex sec_box_the = lve_to_the(sec_box_lve);
									EdgeIndex sec_box_bhe = the_to_bhe(sec_box_the);
									if ((size_t)_edges.get(sec_box_the) + (size_t)_edges.get(sec_box_bhe) + _edges.get(sec_box_lve) == 2)
									{
										affect_lower_box = true;
									}
								}
							}
							//check the left box.
							else if (_edges.get(fir_box_rve) == false)
							{
								if (is_lve(fir_box_rve))
								{
									EdgeIndex sec_box_lve = fir_box_rve;
									EdgeIndex sec_box_rve = lve_to_rve(sec_box_lve);
									EdgeIndex sec_box_the = lve_to_the(sec_box_lve);
									EdgeIndex sec_box_bhe = the_to_bhe(sec_box_the);
									if ((size_t)_edges.get(sec_box_the) + (size_t)_edges.get(sec_box_bhe) + _edges.get(sec_box_rve) == 2)
									{
										affect_lower_box = true;
									}
								}
							}
						}
						else if (fir_box_edge_num == 3)
						{
							//dead box.
							return false;
						}
					}

					//check the box above the edge
					if (is_bhe(edge))
					{
						EdgeIndex fir_box_bhe = edge;
						EdgeIndex fir_box_the = bhe_to_the(fir_box_bhe);
						EdgeIndex fir_box_lve = the_to_lve(fir_box_the);
						EdgeIndex fir_box_rve = lve_to_rve(fir_box_lve);
						size_t fir_box_edge_num = (size_t)_edges.get(fir_box_the) + (size_t)_edges.get(fir_box_lve) + (size_t)_edges.get(fir_box_rve);
						if (fir_box_edge_num == 2)
						{
							//check upper box.
							if (_edges.get(fir_box_the) == false)
							{
								if (is_bhe(fir_box_the))
								{
									EdgeIndex sec_box_bhe = fir_box_the;
									EdgeIndex sec_box_the = bhe_to_the(sec_box_bhe);
									EdgeIndex sec_box_lve = the_to_lve(sec_box_the);
									EdgeIndex sec_box_rve = lve_to_rve(sec_box_lve);
									if ((size_t)_edges.get(sec_box_lve) + (size_t)_edges.get(sec_box_rve) + (size_t)_edges.get(sec_box_the) == 2)
									{
										affect_upper_box = true;
									}
								}
							}
							//check left box.
							else if (_edges.get(fir_box_lve) == false)
							{
								if (is_rve(fir_box_lve))
								{
									EdgeIndex sec_box_rve = fir_box_lve;
									EdgeIndex sec_box_lve = rve_to_lve(sec_box_rve);
									EdgeIndex sec_box_the = lve_to_the(sec_box_lve);
									EdgeIndex sec_box_bhe = the_to_bhe(sec_box_the);
									if ((size_t)_edges.get(sec_box_the) + (size_t)_edges.get(sec_box_bhe) + (size_t)_edges.get(sec_box_lve) == 2)
									{
										affect_upper_box = true;
									}
								}
							}
							//check right box.
							else if (_edges.get(fir_box_rve) == false)
							{
								if (is_lve(fir_box_rve))
								{
									EdgeIndex sec_box_lve = fir_box_rve;
									EdgeIndex sec_box_rve = lve_to_rve(sec_box_lve);
									EdgeIndex sec_box_the = lve_to_the(sec_box_lve);
									EdgeIndex sec_box_bhe = the_to_bhe(sec_box_the);
									if ((size_t)_edges.get(sec_box_the) + (size_t)_edges.get(sec_box_bhe) + (size_t)_edges.get(sec_box_rve) == 2)
									{
										affect_upper_box = true;
									}
								}
							}
						}
						else if (fir_box_edge_num == 3)
						{
							//dead box.
							return false;
						}
					}

					if (affect_upper_box || affect_lower_box)
						return false;
					return true;
				}
			}
			else if (is_ve(edge))
			{
				if (_edges.get(edge) == false)
				{
					//check the box on the left of the edge.
					bool affect_left_box = false;
					bool affect_right_box = false;

					//check the box on the left of the edge.
					if (is_rve(edge))
					{
						EdgeIndex fir_box_rve = edge;
						EdgeIndex fir_box_lve = rve_to_lve(fir_box_rve);
						EdgeIndex fir_box_the = lve_to_the(fir_box_lve);
						EdgeIndex fir_box_bhe = the_to_bhe(fir_box_the);
						size_t fir_box_edge_num = (size_t)_edges.get(fir_box_bhe) + (size_t)_edges.get(fir_box_the) + (size_t)_edges.get(fir_box_lve);
						if (fir_box_edge_num == 2)
						{
							//check the left box.
							if (_edges.get(fir_box_lve) == false)
							{
								if (is_rve(fir_box_lve))
								{
									EdgeIndex sec_box_rve = fir_box_lve;
									EdgeIndex sec_box_lve = rve_to_lve(sec_box_rve);
									EdgeIndex sec_box_the = lve_to_the(sec_box_lve);
									EdgeIndex sec_box_bhe = the_to_bhe(sec_box_the);
									if ((size_t)_edges.get(sec_box_the) + (size_t)_edges.get(sec_box_bhe) + (size_t)_edges.get(sec_box_lve) == 2)
									{
										affect_left_box = true;
									}
								}
							}
							//check the upper box.
							else if (_edges.get(fir_box_the) == false)
							{
								if (is_bhe(fir_box_the))
								{
									EdgeIndex sec_box_bhe = fir_box_the;
									EdgeIndex sec_box_the = bhe_to_the(sec_box_bhe);
									EdgeIndex sec_box_lve = the_to_lve(sec_box_the);
									EdgeIndex sec_box_rve = lve_to_rve(sec_box_lve);
									if ((size_t)_edges.get(sec_box_the) + (size_t)_edges.get(sec_box_lve) + (size_t)_edges.get(sec_box_rve) == 2)
									{
										affect_left_box = true;
									}
								}
							}
							//check the lower box.
							else if (_edges.get(fir_box_bhe) == false)
							{
								if (is_the(fir_box_bhe))
								{
									EdgeIndex sec_box_the = fir_box_bhe;
									EdgeIndex sec_box_bhe = the_to_bhe(sec_box_the);
									EdgeIndex sec_box_lve = the_to_lve(sec_box_the);
									EdgeIndex sec_box_rve = lve_to_rve(sec_box_lve);
									if ((size_t)_edges.get(sec_box_bhe) + (size_t)_edges.get(sec_box_lve) + (size_t)_edges.get(sec_box_rve) == 2)
									{
										affect_left_box = true;
									}
								}
							}
						}
						else if (fir_box_edge_num == 3)
						{
							//dead box.
							return false;
						}
					}

					//check the box on the right of the edge.
					if (is_lve(edge))
					{
						EdgeIndex fir_box_lve = edge;
						EdgeIndex fir_box_rve = lve_to_rve(fir_box_lve);
						EdgeIndex fir_box_the = lve_to_the(fir_box_lve);
						EdgeIndex fir_box_bhe = the_to_bhe(fir_box_the);
						size_t fir_box_edge_num = (size_t)_edges.get(fir_box_bhe) + (size_t)_edges.get(fir_box_the) + (size_t)_edges.get(fir_box_rve);
						if (fir_box_edge_num == 2)
						{
							//check the right box.
							if (_edges.get(fir_box_rve) == false)
							{
								if (is_lve(fir_box_rve))
								{
									EdgeIndex sec_box_lve = fir_box_rve;
									EdgeIndex sec_box_rve = lve_to_rve(sec_box_lve);
									EdgeIndex sec_box_the = lve_to_the(sec_box_lve);
									EdgeIndex sec_box_bhe = the_to_bhe(sec_box_the);
									if ((size_t)_edges.get(sec_box_rve) + (size_t)_edges.get(sec_box_the) + (size_t)_edges.get(sec_box_bhe) == 2)
									{
										affect_right_box = true;
									}
								}
							}
							//check the upper box.
							else if (_edges.get(fir_box_the) == false)
							{
								if (is_bhe(fir_box_the))
								{
									EdgeIndex sec_box_bhe = fir_box_the;
									EdgeIndex sec_box_the = bhe_to_the(sec_box_bhe);
									EdgeIndex sec_box_lve = the_to_lve(sec_box_the);
									EdgeIndex sec_box_rve = lve_to_rve(sec_box_lve);
									if ((size_t)_edges.get(sec_box_the) + (size_t)_edges.get(sec_box_lve) + (size_t)_edges.get(sec_box_rve) == 2)
									{
										affect_right_box = true;
									}
								}
							}
							//check the lower box.
							else if (_edges.get(fir_box_bhe) == false)
							{
								if (is_the(fir_box_bhe))
								{
									EdgeIndex sec_box_the = fir_box_bhe;
									EdgeIndex sec_box_bhe = the_to_bhe(sec_box_the);
									EdgeIndex sec_box_lve = the_to_lve(sec_box_the);
									EdgeIndex sec_box_rve = lve_to_rve(sec_box_lve);
									if ((size_t)_edges.get(sec_box_bhe) + (size_t)_edges.get(sec_box_lve) + (size_t)_edges.get(sec_box_rve) == 2)
									{
										affect_right_box = true;
									}
								}
							}
						}
						else if (fir_box_edge_num == 3)
						{
							//dead box.
							return false;
						}
					}

					if (affect_left_box || affect_right_box)
						return false;
					return true;
				}
			}
			return false;
		}

		//return true if there is any free-edge exist.
		bool ExistFreeEdge() const
		{
			for (EdgeIndex edge = 0; edge < _EDGE_COUNT; edge++)
			{
				if (IsFreeEdge(edge))
					return true;
			}
			return false;
		}

		//get not reasonable state
		bool IsReasonable() const
		{
			size_t dead_box = 0;
			size_t dead_chain = 0;
			EdgeIndex fir_dead_box_the = _EDGE_COUNT;
			for (EdgeIndex fir_box_the = 0; fir_box_the < _BOX_COUNT; fir_box_the++)
			{
				EdgeIndex fir_box_bhe = the_to_bhe(fir_box_the);
				EdgeIndex fir_box_lve = the_to_lve(fir_box_the);
				EdgeIndex fir_box_rve = lve_to_rve(fir_box_lve);
				if ((size_t)_edges.get(fir_box_the) + (size_t)_edges.get(fir_box_bhe) + (size_t)_edges.get(fir_box_lve) + (size_t)_edges.get(fir_box_rve) == 3)
				{
					size_t sec_box_edge_count = 0;

					//check upper box.
					if (_edges.get(fir_box_the) == false)
					{
						if (is_bhe(fir_box_the))
						{
							DabPos box_pos = get_box_pos(fir_box_the);
							sec_box_edge_count = box_edges_count(box_pos - DabPos{ 0,1 });
						}
					}
					//check lower box
					else if (_edges.get(fir_box_bhe) == false)
					{
						if (is_the(fir_box_bhe))
						{
							DabPos box_pos = get_box_pos(fir_box_the);
							sec_box_edge_count = box_edges_count(box_pos + DabPos{ 0, 1 });
						}
							
					}
					//check left box.
					else if (_edges.get(fir_box_lve) == false)
					{
						if (is_rve(fir_box_lve))
						{
							DabPos box_pos = get_box_pos(fir_box_the);
							sec_box_edge_count = box_edges_count(box_pos - DabPos{ 1, 0 });
						}
					}
					//check right box.
					else if (_edges.get(fir_box_rve) == false)
					{
						if (is_lve(fir_box_rve))
						{
							DabPos box_pos = get_box_pos(fir_box_the);
							sec_box_edge_count = box_edges_count(box_pos + DabPos{ 1, 0 });
						}
					}

					if (sec_box_edge_count == 2)//is dead chain
					{
						dead_chain++;
						if (dead_chain > 2)
							return false;
					}
					else//is dead box.
					{
						dead_box++;
						if (dead_box == 1)
						{
							fir_dead_box_the = fir_box_the;
						}
						else if (dead_box == 2)
						{
							//if those two dead box is not neighbour ,this is a unreasonable state.
							if (IsNeighbourBox(get_box_pos(fir_box_the), get_box_pos(fir_dead_box_the)) == false)
								return false;
						}
						else
						{
							return false;
						}
					}
				}
			}

			//if dead chain and dead box coexist, this is a unreasonable state.
			if (dead_chain > 0 && dead_box > 0)
			{
				return false;
			}

			//if any dead chain exist, it must be a rear state, which means there is no free-edge exist in this state.
			if (dead_chain > 0 && ExistFreeEdge())
			{
				return false;
			}

			return true;
		}
	};

	template<size_t WIDTH, size_t HEIGHT, typename std::enable_if< IsLegalGameSize(WIDTH, HEIGHT), int>::type = 0>
	class DabState
	{
	private:

		DabBoard<WIDTH, HEIGHT> _board;
		bool _is_fir_player;
		int _boxes_margin;

	public:

		//get the board.
		inline const DabBoard<WIDTH, HEIGHT>& board() const
		{
			return _board;
		}

		//return true if the next move player is first player.
		inline bool is_fir_player() const
		{
			return _is_fir_player;
		}

		//return the value that how many boxes that boxes captured by first player more than boxes that captured by second player.
		inline int boxes_margin() const
		{
			return _boxes_margin;
		}

	public:

		//default constructor
		constexpr DabState():
			_board(),
			_is_fir_player(true),
			_boxes_margin(0)
		{
		}

		//create specified state.
		constexpr DabState(DabBoard<WIDTH, HEIGHT> board, int boxes_margin) :
			_board(board),
			_is_fir_player(true),
			_boxes_margin(boxes_margin)
		{
		}

		//create specified state.
		constexpr DabState(BoardValueType bv, int boxes_margin):
			_board(bv),
			_is_player_one(true),
			_boxes_margin(boxes_margin)
		{
		}

		//Visualization
		void Visualization() const
		{
			using namespace gadt::console;
			constexpr ConsoleColor edge_color = ConsoleColor::Yellow;
			constexpr ConsoleColor action_color = ConsoleColor::White;
			constexpr ConsoleColor dot_color = ConsoleColor::Gray;
			constexpr ConsoleColor box_color = ConsoleColor::Gray;
			const std::string empty_he_str = "   ";
			const std::string empty_ve_str = " ";
			const std::string empty_box_str = "   ";
			const std::string he_str = "---";
			const std::string ve_str = "|";
			const std::string box_str = " X ";
			const std::string dot_str = "+";
			std::string prev_space = "     ";

			PrintEndLine();
			DabPos pos;

			//print title.
			std::cout << prev_space;
			Cprintf("[" + gadt::ToString(WIDTH) + " x " + gadt::ToString(HEIGHT) + "]  ", ConsoleColor::Yellow);
			//std::cout << std::endl << prev_space;
			Cprintf(_is_fir_player? "P0":"P1", ConsoleColor::Cyan);
			//std::cout << std::endl << prev_space;
			Cprintf("  Margin: " + gadt::ToString(_boxes_margin), ConsoleColor::Green);
			Cprintf(std::string("  Reasonable: ") + (board().IsReasonable() ? "Y" : "N"), ConsoleColor::Brown);
			PrintEndLine<2>();

			//print top horizon edges.
			std::cout << prev_space;
			Cprintf(dot_str, dot_color);
			for (uint8_t x = 0; x < WIDTH; x++)
			{
				pos = DabPos(x, 0);
				if (_board.the_exist(pos))
					Cprintf(he_str, edge_color);
				else
					Cprintf(empty_he_str, edge_color);
				Cprintf(dot_str, dot_color);
			}
			PrintEndLine();

			for (uint8_t y = 0; y < HEIGHT; y++)
			{
				//print vertical edges.
				std::cout << prev_space;
				for (uint8_t x = 0; x < WIDTH; x++)
				{
					pos = DabPos(x, y);
					//print lve
					if (_board.lve_exist(pos))
						Cprintf(ve_str, edge_color);
					else
						Cprintf(empty_ve_str, edge_color);

					//print box
					if (_board.box_exist(pos))
						Cprintf(box_str, box_color);
					else
						Cprintf(empty_box_str, box_color);
				}
				pos = DabPos(WIDTH - 1, y);
				if (_board.rve_exist(pos))
					Cprintf(ve_str, edge_color);
				else
					Cprintf(empty_ve_str, edge_color);
				PrintEndLine();
				
				//print horizon edges.
				std::cout << prev_space;
				Cprintf(dot_str, dot_color);
				for (uint8_t x = 0; x < WIDTH; x++)
				{
					pos = DabPos(x, y);
					if (_board.bhe_exist(pos))
						Cprintf(he_str, edge_color);
					else
						Cprintf(empty_he_str, edge_color);
					Cprintf(dot_str, dot_color);
				}
				PrintEndLine();
			}
			PrintEndLine<2>();
		}

		gadt::AgentIndex GetWinner() const
		{
			if (board().edge_count() == EdgeCount<WIDTH, HEIGHT>())
			{
				if (boxes_margin() > 0)
					return 0x1;
				if (boxes_margin() < 0)
					return 0x2;
				else
					return 0x3;
			}
			return 0x0;
		}

		//set the state to full
		void BeFull()
		{
			_board.set_all_edges();
		}
 
		//set random edge.
		void SetRandomEdge()
		{
			gadt::bitboard::ValueVector<EdgeCount<WIDTH, HEIGHT>()> moves;
			for (EdgeIndex i = 0; i < EdgeCount<WIDTH, HEIGHT>(); i++)
				if (!_board.edge_exist(i))
					moves.push(i);
			if (moves.length() > 0)
			{
				size_t edge = moves.draw_value();
				_board.set_edge((EdgeIndex)edge);
			}
		}

		//
		void Update(const DabMove& move)
		{
			GADT_WARNING_IF(DAB_WARNING, _board.edge_exist(move.edge), "edge exist!");
			_board.set_edge(move.edge);
			int new_box = (int)_board.count_of_boxes_that_owns_edge(move.edge);
			if (is_fir_player())
			{
				if (new_box == 0)
					_is_fir_player = false;//change player
				else
					_boxes_margin += new_box;//change margin.
			}
			else
			{
				if (new_box == 0)
					_is_fir_player = true;//change player
				else
					_boxes_margin -= new_box;//change margin.
			}
		}
	};

	template<size_t WIDTH, size_t HEIGHT>
	DabBoard<WIDTH, HEIGHT> GetTerminalBoard()
	{
		DabBoard<WIDTH, HEIGHT> board;
		
	}
}
