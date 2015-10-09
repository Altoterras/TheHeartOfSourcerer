/***********************************************************************//**
 *	VelocityInterpolation.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2015/01/22.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "VelocityInterpolation.h"

// Friends

// External
#include "../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// VelocityInterpolation メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	補間
**//*---------------------------------------------------------------------*/
void VelocityInterpolation::interpolation(f32* cur, f32* vel, const f32 trg, const f32 frameDelta)
{
	f32 d = trg - *cur;

	// 目標値に近い場合は補間無し
	if(d == 0.0f)
	{
		*vel = 0.0f;
		return;
	}
	if(TFW_IS_NEAR(d, 0.0f, 0.001f))
	{
		*cur = trg;
		*vel = 0.0f;
		return;
	}

	// 目標の速さ
	f32 vt = d;
	if(vt > _speedMax)
	{
		vt = _speedMax;
	}
	else if(vt < -_speedMax)
	{
		vt = -_speedMax;
	}

	// 目標に近づけるために加速
	f32 acc = vt - *vel;
	if(acc > _accMax)
	{
		acc = _accMax;
	}
	else if(acc < -_accMax)
	{
		acc = -_accMax;
	}
//	*vel += acc * frameDelta;
	*vel += acc;

	// 現在値の更新
//	f32 nc = *cur + (*vel * frameDelta);
	f32 nc = *cur + *vel;
	if(*cur < trg)
	{
		*cur = (nc > trg) ? trg : nc;
	}
	else
	{
		*cur = (nc < trg) ? trg : nc;
	}
}

/*---------------------------------------------------------------------*//**
	ベクトル補間
**//*---------------------------------------------------------------------*/
void VelocityInterpolation::interpolation(Vector3F* cur, Vector3F* vel, const Vector3F* trg, const f32 frameDelta)
{
	for(int i = 0; i < 3; i++)
	{
		interpolation(&cur->_v[i], &vel->_v[i], trg->_v[i], frameDelta);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
VelocityInterpolation::VelocityInterpolation(f32 accMax, f32 speedMax)
	: _accMax(accMax)
	, _speedMax(speedMax)
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
