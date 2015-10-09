/***********************************************************************//**
 *	VelocityInterpolation.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2015/01/22.
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

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	速度補間
 *
 *		山型のスロープを描くような速度変化を起こす補間
 *
 *		|v	  　------
 *		|   　／　　  ＼
 *		|   ／          ＼
 *		| ／              ＼  t
 *		+----------------------
**//*---------------------------------------------------------------------*/
class VelocityInterpolation
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void interpolation(f32* cur, f32* vel, const f32 trg, const f32 frameDelta);
	void interpolation(Vector3F* cur, Vector3F* vel, const Vector3F* trg, const f32 frameDelta);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部サービス
public:
	VelocityInterpolation(f32 accMax, f32 speedMax);

	//======================================================================
	// 変数
private:
	f32 _accMax;			// 最大加速
	f32 _speedMax;			// 最大速さ
//	f32 _frameBrakeRemain;	// 制動のかかる残りフレーム数
	f32 _distBrakeRemain;	// 制動のかかる残り距離
	f32 _accBrake;			// 制動加速（速度/フレーム）
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_COMMON_CALC_UTILS_H_
