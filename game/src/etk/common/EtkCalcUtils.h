/***********************************************************************//**
 *	EtkCalcUtils.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2013/08/22.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_COMMON_ETK_CALC_UTILS_H_
#define _ETK_COMMON_ETK_CALC_UTILS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF;
template<class TYPE> class Vector2;
typedef Vector2<f32> Vector2F;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	よく使う計算
 *
**//*---------------------------------------------------------------------*/
class EtkCalcUtils
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
	
	static void calcFullScreenMultiResolTexCoord(RectF* vtx, RectF* uv);
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_COMMON_ETK_CALC_UTILS_H_
