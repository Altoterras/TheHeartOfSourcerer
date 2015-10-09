/***********************************************************************//**
 *	StMod10300.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/18.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10300_H_
#define _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10300_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "StMod10000.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Game;

/*---------------------------------------------------------------------*//**
 *	ストーリー 10301
 *
**//*---------------------------------------------------------------------*/
class StMod10300 : public StMod10000
{
	//======================================================================
	// 定数
private:
	enum Ebstat
	{
		EBSTAT_NULL,	// 初期（成功していない）
		EBSTAT_DELAY,	// 成功後の演出待ち
		EBSTAT_BEGUN,	// イベント実行後
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StMod10300();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	virtual void notifyChangedMap(s32 mapidNew, s32 mapidOld);
	virtual void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);

	virtual void onBlossomsWaterAscension(Unit* unitTree) {}
	virtual void onReikaWarmed(Unit* unitReika) {}
	
protected:
	bool isSafeNextEventConditionNow();

	//======================================================================
	// 変数
private:
	Ebstat _ebstatTreeAscend;		// 桜の水元素アセンションに成功した
	Ebstat _ebstatReikaWarmed;		// レイカを暖めることに成功した
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10300_H_
