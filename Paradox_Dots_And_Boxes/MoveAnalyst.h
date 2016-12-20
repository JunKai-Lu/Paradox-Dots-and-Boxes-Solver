#include "stdafx.h"
#include "GameDefine.h"

#include <iostream>

#pragma  once

namespace dots_and_boxes
{
	class MoveAnalyst
	{
	private:
		bool _edge[MAX_EDGE];
		bool _action[MAX_EDGE];

	public:
		MoveAnalyst(State& state, bool filter = false);
		MoveAnalyst(BitBoard bit_group, bool filter = false);
		inline ActionVec ActionVec()
		{
			return action_vector::Create(_action);
		}
		inline bool* Actions()
		{
			return _action;
		}
		inline bool operator[](size_t index)
		{
			WARNING_CHECK(index >= 60,"Wrong index");
			return _action[index];
		}


	private:
		//�ж�ĳ����ʽ�Ƿ����
		inline bool EdgeExist(Edge index)
		{
			return _edge[index];
		}

		//�õ�ĳ��������µ����ߵı�ţ�����Ϊ0~24��
		inline Edge GetLowerLeftVecEdge(Edge hor_edge)
		{
			return state::GetLowerLeftVecEdge(hor_edge);
		}

		//�õ�ĳ���������ϵĺ�ߵı�ţ�����Ϊ30~54��
		inline Edge GetUpperRightHorEdge(Edge vec_edge)
		{
			return state::GetUpperRightHorEdge(vec_edge);
		}

		//�õ�ĳ������·��ĸ����Ѿ���ռ��ıߵ���Ŀ������Ϊ0~24��
		inline int GetLowerBoxEdgesNum(Edge hor_edge)
		{
#ifdef WARNING
			if (hor_edge > 24 || hor_edge < 0)
			{
				std::cout << ">> WARNING: Wrong hor_dege in GetLowerBoxEdgesNum" << std::endl;
			}
			if (EdgeExist(hor_edge))
			{
				std::cout << ">> WARNING: Edge hor_dege exist! GetLowerBoxEdgesNum" << std::endl;
			}
#endif

			Edge lower_hor_edge = hor_edge + 5;
			Edge lower_left_vec_edge = state::GetLowerLeftVecEdge(hor_edge);
			Edge lower_right_vec_edge = lower_left_vec_edge + 5;
			return EdgeExist(lower_hor_edge) + EdgeExist(lower_left_vec_edge) + EdgeExist(lower_right_vec_edge);

		}

		//�õ�ĳ������Ϸ��ĸ����Ѿ���ռ��ıߵ���Ŀ������Ϊ5~29��
		inline int GetUpperBoxEdgesNum(Edge hor_edge)
		{
#ifdef WARNING
			if (hor_edge > 29 || hor_edge < 5)
			{
				std::cout << ">> WARNING: Wrong hor_dege in GetUpperBoxEdgesNum" << std::endl;
			}
			if (EdgeExist(hor_edge))
			{
				std::cout << ">> WARNING: Edge hor_dege exist! GetUpperBoxEdgesNum" << std::endl;
			}
#endif
			Edge upper_hor_edge = hor_edge - 5;
			Edge upper_left_vec_edge = state::GetLowerLeftVecEdge(hor_edge - 5);
			Edge upper_right_vec_edge = upper_left_vec_edge + 5;
			return EdgeExist(upper_hor_edge) + EdgeExist(upper_left_vec_edge) + EdgeExist(upper_right_vec_edge);
		}

		//�õ�ĳ�������ұߵĸ����Ѿ���ռ��ıߵ���Ŀ������Ϊ30~54��
		inline int GetRightBoxEdgesNum(Edge vec_edge)
		{
#ifdef WARNING
			if (vec_edge > 54 || vec_edge < 30)
			{
				std::cout << ">> WARNING: Wrong vec_dege in GetRightBoxEdgesNum" << std::endl;
			}
			if (EdgeExist(vec_edge))
			{
				std::cout << ">> WARNING: Edge vec_dege exist! GetRightBoxEdgesNum" << std::endl;
			}
#endif

			Edge right_vec_edge = vec_edge + 5;
			Edge upper_right_hor_edge = state::GetUpperRightHorEdge(vec_edge);
			Edge lower_right_hor_edge = upper_right_hor_edge + 5;
			return EdgeExist(right_vec_edge) + EdgeExist(upper_right_hor_edge) + EdgeExist(lower_right_hor_edge);

		}

		//�õ�ĳ��������ߵĸ����Ѿ���ռ��ıߵ���Ŀ������Ϊ35~59��
		inline int GetLeftBoxEdgesNum(Edge vec_edge)
		{
#ifdef WARNING
			if (vec_edge > 59 || vec_edge < 35)
			{
				std::cout << ">> WARNING: Wrong vec_dege in GetLeftBoxEdgesNum" << std::endl;
			}
			if (EdgeExist(vec_edge))
			{
				std::cout << ">> WARNING: Edge vec_dege exist! GetLeftBoxEdgesNum" << std::endl;
			}
#endif

			Edge left_vec_edge = vec_edge - 5;
			Edge upper_left_hor_edge = state::GetUpperRightHorEdge(vec_edge - 5);
			Edge lower_left_hor_edge = upper_left_hor_edge + 5;
			return EdgeExist(left_vec_edge) + EdgeExist(upper_left_hor_edge) + EdgeExist(lower_left_hor_edge);

		}

		//�ж�ĳ�����Ƿ������Ϸ��ı�
		inline bool IsNotUpperSideHorEdge(Edge index)
		{
			return state::IsNotUpperSideHorEdge(index);
		}

		//�ж�ĳ�����Ƿ������·��ı�
		inline bool IsNotLowerSideHorEdge(Edge index)
		{
			return state::IsNotLowerSideHorEdge(index);
		}

		//�ж�ĳ�����Ƿ������󷽵ı�
		inline bool IsNotLeftSideVecEdge(Edge index)
		{
			return state::IsNotLeftSideVecEdge(index);
		}

		//�ж�ĳ�����Ƿ������ҷ��ı�
		inline bool IsNotRightSideVecEdge(Edge index)
		{
			return state::IsNotRightSideVecEdge(index);
		}

		//�ж�ĳ�����Ƿ��Ǻ��
		inline bool IsHorEdge(Edge index)
		{
			return state::IsHorEdge(index);
		}

		//�ж�ĳ�����Ƿ�������
		inline bool IsVecEdge(Edge index)
		{
			return state::IsVecEdge(index);
		}

		//�ж�ĳ����ʽ�Ƿ����
		bool IsPossibleAction(Edge index);

		//����ȫ��������ʽ
		void ComputeLegalMove();
	};
	
}