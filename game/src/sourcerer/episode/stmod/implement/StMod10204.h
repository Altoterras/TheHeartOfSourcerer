/***********************************************************************//**
 *	StMod10204.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10204_H_
#define _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10204_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "StMod10000.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ストーリー 10204
 *
**//*---------------------------------------------------------------------*/
class StMod10204 : public StMod10000
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StMod10204();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);
	bool notifyFindUnconsciousFocus(Unit** unitFind);

	//======================================================================
	// 変数
private:
	f32 _framecntHitLightToUnboshi;
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10204_H_
