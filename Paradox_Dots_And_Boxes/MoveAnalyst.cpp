#include "stdafx.h"

#include "GameDefine.h"
#include "MoveAnalyst.h"

namespace dots_and_boxes
{
	MoveAnalyst::MoveAnalyst(State& state, bool filter)
	{
		for (Edge i = 0; i < MAX_EDGE; i++)
		{
			if (state.EdgeExist(i))
			{
				_edge[i] = true;
			}
			else
			{
				_edge[i] = false;
			}
		}
		BitBoard board = boardCreate(state);
		for (Edge i = 0; i < MAX_EDGE; i++)
		{
			if (filter)
			{
				_action[i] = state::IsFreeEdge(board, i);
			}
			else
			{
				_action[i] = !_edge[i];
			}
		}
	}
	MoveAnalyst::MoveAnalyst(BitBoard bit_group, bool filter)
	{
		for (Edge i = 0; i < MAX_EDGE; i++)
		{
			if (boardEdgeExist(bit_group, i))
			{
				_edge[i] = true;
			}
			else
			{
				_edge[i] = false;
			}
		}
		for (Edge i = 0; i < MAX_EDGE; i++)
		{
			if (filter)
			{
				_action[i] = state::IsFreeEdge(bit_group,i);
			}
			else
			{
				_action[i] = !_edge[i];
			}
		}
	}

	//判断某个招式是否可行
	bool MoveAnalyst::IsPossibleAction(Edge index)
	{
		if (EdgeExist(index))//如果这条边已经被占领，返回假。
		{
			return false;
		}
		if (IsHorEdge(index))//横边
		{
			if (IsNotUpperSideHorEdge(index))//不是最上面的横边，则需要检查上方的格子
			{
				Edge upper_hor_edge = index - 5;								//上方格子的顶边
				Edge upper_left_vec_edge = GetLowerLeftVecEdge(index - 5);	//上方格子的左边
				Edge upper_right_vec_edge = upper_left_vec_edge + 5;			//上方格子的右边

				//如果已经存在了两条边
				if ( ( EdgeExist(upper_hor_edge) + EdgeExist(upper_left_vec_edge) + EdgeExist(upper_right_vec_edge) ) >= 2)
				{
					if (!EdgeExist(upper_hor_edge))//上方的边是空的
					{
						if (IsNotUpperSideHorEdge(upper_hor_edge))//不是最顶上的边，则考虑上方格子是否空的。要是最顶上边，则不用考虑。
						{
							if (GetUpperBoxEdgesNum(upper_hor_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(upper_left_vec_edge))//左上方的边是空的
					{
						if (IsNotLeftSideVecEdge(upper_left_vec_edge))//不是最左边的边
						{
							if (GetLeftBoxEdgesNum(upper_left_vec_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(upper_right_vec_edge))//右上方的边是空的
					{
						if (IsNotRightSideVecEdge(upper_right_vec_edge))//不是最右边的边
						{
							if (GetRightBoxEdgesNum(upper_right_vec_edge) == 2)
							{
								return false;
							}
						}
					}
				}
			}
			if (IsNotLowerSideHorEdge(index))//不是最下面的横边，则需要检查下方的格子
			{
				Edge lower_hor_edge = index + 5;
				Edge lower_left_vec_edge = GetLowerLeftVecEdge(index);
				Edge lower_right_vec_edge = lower_left_vec_edge + 5;

				if ((EdgeExist(lower_hor_edge) + EdgeExist(lower_left_vec_edge) + EdgeExist(lower_right_vec_edge)) >= 2)
				{
					if (!EdgeExist(lower_hor_edge))//下方的边是空的
					{
						if (IsNotLowerSideHorEdge(lower_hor_edge))//不是最下边的边
						{
							if (GetLowerBoxEdgesNum(lower_hor_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(lower_left_vec_edge))//左下方的边是空的
					{
						if (IsNotLeftSideVecEdge(lower_left_vec_edge))//不是最左边的边
						{
							if (GetLeftBoxEdgesNum(lower_left_vec_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(lower_right_vec_edge))//右下方的边是空的
					{
						if (IsNotRightSideVecEdge( lower_right_vec_edge ))//不是最右边的边
						{
							if (GetRightBoxEdgesNum(lower_right_vec_edge) == 2)
							{
								return false;
							}
						}
					}
				}
			}
		}
		else if (IsVecEdge(index))//竖边
		{
			if (IsNotLeftSideVecEdge(index))//不是最左边的边，则需要检查左方的格子
			{
				Edge left_vec_edge = index - 5;
				Edge upper_left_hor_edge = GetUpperRightHorEdge(index - 5);
				Edge lower_left_hor_edge = upper_left_hor_edge + 5;

				if ((EdgeExist(left_vec_edge) + EdgeExist(upper_left_hor_edge) + EdgeExist(lower_left_hor_edge)) >= 2)
				{
					if (!EdgeExist(left_vec_edge))//左方的边是空的
					{
						if (IsNotLeftSideVecEdge(left_vec_edge))//不是最左边的边，则考虑左方格子是否空的。
						{
							if (GetLeftBoxEdgesNum(left_vec_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(upper_left_hor_edge))//左上方的边是空的
					{
						if (IsNotUpperSideHorEdge(upper_left_hor_edge))//不是最上边的边
						{
							if (GetUpperBoxEdgesNum(upper_left_hor_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(lower_left_hor_edge))//左下方的边是空的
					{
						if (IsNotLowerSideHorEdge(lower_left_hor_edge))//不是最下边的边
						{
							if (GetLowerBoxEdgesNum(lower_left_hor_edge) == 2)
							{
								return false;
							}
						}
					}
				}
			}

			if (IsNotRightSideVecEdge(index))//不是最右边的边，需要检查右方的格子
			{
				Edge right_vec_edge = index + 5;
				Edge upper_right_hor_edge = GetUpperRightHorEdge(index);
				Edge lower_right_hor_edge = upper_right_hor_edge + 5;
				if ((EdgeExist(right_vec_edge) + EdgeExist(upper_right_hor_edge) + EdgeExist(lower_right_hor_edge)) >= 2)
				{
					if (!EdgeExist(right_vec_edge))//右方的边是空的
					{
						if (IsNotRightSideVecEdge(right_vec_edge))//不是最右边的边，则考虑右方格子是否空的。
						{
							if (GetRightBoxEdgesNum(right_vec_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(upper_right_hor_edge))//右上方的边是空的
					{
						if (IsNotUpperSideHorEdge(upper_right_hor_edge))//不是最上边的边
						{
							if (GetUpperBoxEdgesNum(upper_right_hor_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(lower_right_hor_edge))//右下方的边是空的
					{
						if (IsNotLowerSideHorEdge(lower_right_hor_edge))//不是最下边的边
						{
							if (GetLowerBoxEdgesNum(lower_right_hor_edge) == 2)
							{
								return false;
							}
						}
					}
				}
			}
		}
		return true;
	}

	//
}


