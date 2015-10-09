/***********************************************************************//**
 *	StMod10000.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10000_H_
#define _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10000_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../StMod.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class WorldMapStateUpdaterVol01;

/*---------------------------------------------------------------------*//**
 *	Vol.1 ストーリー ベースクラス
 *
**//*---------------------------------------------------------------------*/
class StMod10000 : public StMod
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StMod10000();
	virtual ~StMod10000();
	virtual bool create(StoryManager* strymngRef);
	virtual void destroy();
	virtual void onDecisionParty(s32 mapidNew, s32 mapidOld);
	virtual void notifyChangedRtmvPlaying(bool isPlay, u16 evidRtmvMatter);
	virtual void notifyChangedMap(s32 mapidNew, s32 mapidOld);
	virtual void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);

	//======================================================================
	// 変数
private:
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_STMOD_IMPLEMENT_ST_MOD_10000_H_
