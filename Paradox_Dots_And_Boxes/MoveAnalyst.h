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
		//判断某个招式是否存在
		inline bool EdgeExist(Edge index)
		{
			return _edge[index];
		}

		//得到某个横边左下的竖边的编号（限制为0~24）
		inline Edge GetLowerLeftVecEdge(Edge hor_edge)
		{
			return state::GetLowerLeftVecEdge(hor_edge);
		}

		//得到某个竖边右上的横边的编号（限制为30~54）
		inline Edge GetUpperRightHorEdge(Edge vec_edge)
		{
			return state::GetUpperRightHorEdge(vec_edge);
		}

		//得到某个横边下方的格子已经被占领的边的数目（限制为0~24）
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

		//得到某个横边上方的格子已经被占领的边的数目（限制为5~29）
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

		//得到某个竖边右边的格子已经被占领的边的数目（限制为30~54）
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

		//得到某个竖边左边的格子已经被占领的边的数目（限制为35~59）
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

		//判断某个边是否不是最上方的边
		inline bool IsNotUpperSideHorEdge(Edge index)
		{
			return state::IsNotUpperSideHorEdge(index);
		}

		//判断某个边是否不是最下方的边
		inline bool IsNotLowerSideHorEdge(Edge index)
		{
			return state::IsNotLowerSideHorEdge(index);
		}

		//判断某个边是否不是最左方的边
		inline bool IsNotLeftSideVecEdge(Edge index)
		{
			return state::IsNotLeftSideVecEdge(index);
		}

		//判断某个边是否不是最右方的边
		inline bool IsNotRightSideVecEdge(Edge index)
		{
			return state::IsNotRightSideVecEdge(index);
		}

		//判断某个边是否是横边
		inline bool IsHorEdge(Edge index)
		{
			return state::IsHorEdge(index);
		}

		//判断某个边是否是竖边
		inline bool IsVecEdge(Edge index)
		{
			return state::IsVecEdge(index);
		}

		//判断某个招式是否可行
		bool IsPossibleAction(Edge index);

		//计算全部可行招式
		void ComputeLegalMove();
	};
	
}