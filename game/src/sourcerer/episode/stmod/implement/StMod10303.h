/***********************************************************************//**
 *	StMod10303.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/18.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10303_H_
#define _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10303_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "StMod10300.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ストーリー 10303
 *
**//*---------------------------------------------------------------------*/
class StMod10303 : public StMod10300
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StMod10303();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	virtual void notifyChangedMap(s32 mapidNew, s32 mapidOld);

	virtual void onBlossomsWaterAscension(Unit* unitTree);

	//======================================================================
	// 変数
private:
	f32 _frameElapsedSuccessEvent;
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10303_H_
