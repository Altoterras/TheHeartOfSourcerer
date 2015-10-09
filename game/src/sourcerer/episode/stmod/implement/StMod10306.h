/***********************************************************************//**
 *	StMod10306.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/20.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10306_H_
#define _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10306_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "StMod10300.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ストーリー 10305
 *
**//*---------------------------------------------------------------------*/
class StMod10306 : public StMod10300
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StMod10306();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	void notifyChangedMap(s32 mapidNew, s32 mapidOld);

	void onBlossomsWaterAscension(Unit* unitTree);
	void onReikaWarmed(Unit* unitReika);

private:

	//======================================================================
	// 変数
private:
	f32 _frameElapsedSuccessEvent;
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10306_H_
