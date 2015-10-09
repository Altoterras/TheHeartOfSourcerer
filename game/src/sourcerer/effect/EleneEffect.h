/***********************************************************************//**
 *	EleneEffect.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/25.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EFFECT_ELENE_EFFECT_H_
#define _SRCR_EFFECT_ELENE_EFFECT_H_

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

class EleneStat;
class Game;
class TransStat;

/*---------------------------------------------------------------------*//**
 *	元素エフェクト
 *
**//*---------------------------------------------------------------------*/
class EleneEffect
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	static void on(const TransStat* tstat, const Vector3F* pos);

private:
	static void appearEffect(const EleneStat* eestat, u32 eeffFuncFlags, const Vector3F* pos);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EFFECT_ELENE_EFFECT_H_
