/***********************************************************************//**
 *	StatusChangeEffect.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/25.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EFFECT_STATUS_CHANGE_EFFECT_H_
#define _SRCR_EFFECT_STATUS_CHANGE_EFFECT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

ETK_BEGIN_NS

class Particle;

ETK_END_NS

SRCR_BEGIN_NS

class StatusChangeResult;
class Game;

/*---------------------------------------------------------------------*//**
 *	ステータス変更エフェクト
 *
**//*---------------------------------------------------------------------*/
class StatusChangeEffect
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	static void on(const StatusChangeResult* scres);

private:
	static Particle* appearParticle(u16 ptcldid, const Vector3F* pos, u32 flags);
	static void moveVectorRandom(Vector3F* v);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EFFECT_STATUS_CHANGE_EFFECT_H_
