/***********************************************************************//**
 *	StMod.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STMOD_ST_MOD_H_
#define _SRCR_EPISODE_STMOD_ST_MOD_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

#include "../StoryIdDef.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class StatusChangeResult;
class Game;
class Souma;
class StoryManager;
class TransStat;
class Unit;

/*---------------------------------------------------------------------*//**
 *	ストーリー毎の専用処理モジュール
 *		※タイトル依存の処理を行う
 *		　原則的には、可能な限りイベントで実装すべきだが、
 *		　イベントのみで実装が困難な場合はこのモジュールを実装する
 *			実装方法の検討は以下の順で考えるべきである
 *				(1) 既存の EventMatter のみで実装する
 *				　　↓
 *				(2) ストーリー専用の StEmp で実装する
 *				　　↓
 *				(3) 最終手段としてこのモジュールを派生して実装する
 *
**//*---------------------------------------------------------------------*/
class StMod : public FrameExec
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StMod();
	virtual ~StMod();
	virtual bool create(StoryManager* strymngRef);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec) {}	// フレーム制御

	virtual bool begin();
	virtual bool end();

	virtual bool notifyChangeMapBefore(s32* mapidNew, s32 mapidOld) { return true; }
	virtual void notifyChangedMap(s32 mapidNew, s32 mapidOld) {}
	virtual void notifyChangedEventMode(bool isEventMode) {}
	virtual void notifyChangedRtmvPlaying(bool isPlay, u16 evidRtmvMatter) {}
	virtual void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat) {}
	virtual bool notifyFindUnconsciousFocus(Unit** unitFind) { return false; }
	virtual void notifyDischargeSouma(Souma* souma, u16 mcid) {}
	virtual void notifyPickupItem(const Unit* unitActor, const Unit* unitItem) {}

	virtual void onDecisionParty(s32 mapidNew, s32 mapidOld) {}
	virtual void onRestAtHome() {}

	//======================================================================
	// 変数
protected:
	StoryManager* _strymngRef;
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_ST_MOD_H_
