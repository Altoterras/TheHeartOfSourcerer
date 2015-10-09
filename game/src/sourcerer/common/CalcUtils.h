/***********************************************************************//**
 *	CalcUtils.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/31.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_COMMON_CALC_UTILS_H_
#define _SRCR_COMMON_CALC_UTILS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
template<class TYPE> class Vector2;
typedef Vector2<f32> Vector2F;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

SRCR_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	よく使う計算
 *
**//*---------------------------------------------------------------------*/
class CalcUtils
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static f32 randF();
	static f32 randPlusMinusOneF();

	static bool calc3dPosToScreenPos(Vector2F* posScr, const Vector3F* pos3d);
	static bool calc3dPosToScreenPos(Vector2F* arrposScr, const Vector3F* arrpos3d, s32 numArr);
	static bool calcScreenPosTo3dPos(Vector3F* pos3d, const Vector2F* posScr, const f32 zScrPos);
	static bool calcScreenPosTo3dPos(Vector3F* arrpos3d, const Vector2F* arrposScr, const f32* arrzScrPos, s32 numArr);
	static bool calcScreenPosAnd3dZTo3dPos(Vector3F* pos3d, const Vector2F* posScr, const f32 z3dPos);
	static bool calcScreenPosAnd3dZTo3dPos(Vector3F* arrpos3d, const Vector2F* arrposScr, const f32* arrz3dPos, s32 numArr);

	static void collectUnitFanRange(List<Unit*>* listUnits, const Vector3F* center, f32 angleDirY, u16 unitflagsTrg, f32 radiusRangeSq, f32 cosDotRange);
	static void collectUnitRoundRange(List<Unit*>* listUnits, const Vector3F* center, u16 unitflagsTrg, f32 radiusRangeSq);

	static void sortUnitArrayByDistanceAscending(List<Unit*>* listUnits, const Vector3F* center);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_COMMON_CALC_UTILS_H_
