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

	//�ж�ĳ����ʽ�Ƿ����
	bool MoveAnalyst::IsPossibleAction(Edge index)
	{
		if (EdgeExist(index))//����������Ѿ���ռ�죬���ؼ١�
		{
			return false;
		}
		if (IsHorEdge(index))//���
		{
			if (IsNotUpperSideHorEdge(index))//����������ĺ�ߣ�����Ҫ����Ϸ��ĸ���
			{
				Edge upper_hor_edge = index - 5;								//�Ϸ����ӵĶ���
				Edge upper_left_vec_edge = GetLowerLeftVecEdge(index - 5);	//�Ϸ����ӵ����
				Edge upper_right_vec_edge = upper_left_vec_edge + 5;			//�Ϸ����ӵ��ұ�

				//����Ѿ�������������
				if ( ( EdgeExist(upper_hor_edge) + EdgeExist(upper_left_vec_edge) + EdgeExist(upper_right_vec_edge) ) >= 2)
				{
					if (!EdgeExist(upper_hor_edge))//�Ϸ��ı��ǿյ�
					{
						if (IsNotUpperSideHorEdge(upper_hor_edge))//������ϵıߣ������Ϸ������Ƿ�յġ�Ҫ����ϱߣ����ÿ��ǡ�
						{
							if (GetUpperBoxEdgesNum(upper_hor_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(upper_left_vec_edge))//���Ϸ��ı��ǿյ�
					{
						if (IsNotLeftSideVecEdge(upper_left_vec_edge))//��������ߵı�
						{
							if (GetLeftBoxEdgesNum(upper_left_vec_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(upper_right_vec_edge))//���Ϸ��ı��ǿյ�
					{
						if (IsNotRightSideVecEdge(upper_right_vec_edge))//�������ұߵı�
						{
							if (GetRightBoxEdgesNum(upper_right_vec_edge) == 2)
							{
								return false;
							}
						}
					}
				}
			}
			if (IsNotLowerSideHorEdge(index))//����������ĺ�ߣ�����Ҫ����·��ĸ���
			{
				Edge lower_hor_edge = index + 5;
				Edge lower_left_vec_edge = GetLowerLeftVecEdge(index);
				Edge lower_right_vec_edge = lower_left_vec_edge + 5;

				if ((EdgeExist(lower_hor_edge) + EdgeExist(lower_left_vec_edge) + EdgeExist(lower_right_vec_edge)) >= 2)
				{
					if (!EdgeExist(lower_hor_edge))//�·��ı��ǿյ�
					{
						if (IsNotLowerSideHorEdge(lower_hor_edge))//�������±ߵı�
						{
							if (GetLowerBoxEdgesNum(lower_hor_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(lower_left_vec_edge))//���·��ı��ǿյ�
					{
						if (IsNotLeftSideVecEdge(lower_left_vec_edge))//��������ߵı�
						{
							if (GetLeftBoxEdgesNum(lower_left_vec_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(lower_right_vec_edge))//���·��ı��ǿյ�
					{
						if (IsNotRightSideVecEdge( lower_right_vec_edge ))//�������ұߵı�
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
		else if (IsVecEdge(index))//����
		{
			if (IsNotLeftSideVecEdge(index))//��������ߵıߣ�����Ҫ����󷽵ĸ���
			{
				Edge left_vec_edge = index - 5;
				Edge upper_left_hor_edge = GetUpperRightHorEdge(index - 5);
				Edge lower_left_hor_edge = upper_left_hor_edge + 5;

				if ((EdgeExist(left_vec_edge) + EdgeExist(upper_left_hor_edge) + EdgeExist(lower_left_hor_edge)) >= 2)
				{
					if (!EdgeExist(left_vec_edge))//�󷽵ı��ǿյ�
					{
						if (IsNotLeftSideVecEdge(left_vec_edge))//��������ߵıߣ������󷽸����Ƿ�յġ�
						{
							if (GetLeftBoxEdgesNum(left_vec_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(upper_left_hor_edge))//���Ϸ��ı��ǿյ�
					{
						if (IsNotUpperSideHorEdge(upper_left_hor_edge))//�������ϱߵı�
						{
							if (GetUpperBoxEdgesNum(upper_left_hor_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(lower_left_hor_edge))//���·��ı��ǿյ�
					{
						if (IsNotLowerSideHorEdge(lower_left_hor_edge))//�������±ߵı�
						{
							if (GetLowerBoxEdgesNum(lower_left_hor_edge) == 2)
							{
								return false;
							}
						}
					}
				}
			}

			if (IsNotRightSideVecEdge(index))//�������ұߵıߣ���Ҫ����ҷ��ĸ���
			{
				Edge right_vec_edge = index + 5;
				Edge upper_right_hor_edge = GetUpperRightHorEdge(index);
				Edge lower_right_hor_edge = upper_right_hor_edge + 5;
				if ((EdgeExist(right_vec_edge) + EdgeExist(upper_right_hor_edge) + EdgeExist(lower_right_hor_edge)) >= 2)
				{
					if (!EdgeExist(right_vec_edge))//�ҷ��ı��ǿյ�
					{
						if (IsNotRightSideVecEdge(right_vec_edge))//�������ұߵıߣ������ҷ������Ƿ�յġ�
						{
							if (GetRightBoxEdgesNum(right_vec_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(upper_right_hor_edge))//���Ϸ��ı��ǿյ�
					{
						if (IsNotUpperSideHorEdge(upper_right_hor_edge))//�������ϱߵı�
						{
							if (GetUpperBoxEdgesNum(upper_right_hor_edge) == 2)
							{
								return false;
							}
						}
					}
					else if (!EdgeExist(lower_right_hor_edge))//���·��ı��ǿյ�
					{
						if (IsNotLowerSideHorEdge(lower_right_hor_edge))//�������±ߵı�
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


