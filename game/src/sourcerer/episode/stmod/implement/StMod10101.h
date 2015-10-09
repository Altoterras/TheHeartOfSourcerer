/***********************************************************************//**
 *	StMod10101.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/14.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10101_H_
#define _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10101_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "StMod10000.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ストーリー 10101
 *
**//*---------------------------------------------------------------------*/
class StMod10101 : public StMod10000
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	bool notifyChangeMapBefore(s32* mapidNew, s32 mapidOld);
	void notifyChangedMap(s32 mapidNew, s32 mapidOld);
	void notifyPickupItem(const Unit* unitActor, const Unit* unitItem);
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10101_H_
