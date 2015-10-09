/***********************************************************************//**
 *	GameNotifier.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/27.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BODY_NOTIFIER_H_
#define _SRCR_BODY_NOTIFIER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../etk/body/EtkNotifier.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class StatusChangeResult;
class Confront;
class Game;
class MagicCluster;
class Souma;
class TransStat;
class Unit;
class UnitArray;

/*---------------------------------------------------------------------*//**
 *	ゲーム通知
 *		※　ゲーム的要素の通知が主．
 *			ハードウェアやシステム的要素の通知はあまり扱わない．
 *
**//*---------------------------------------------------------------------*/
class GameNotifier : public EtkNotifier
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void notifyChangeLanguage();
	void notifyResizeView();
	void notifyDeserialize();
	void notifyEnterGame();
	void notifyBeginStartGameMode();
	void notifyBeginIngameMode();
	bool notifyChangeMapBefore(s32* mapidNew, s32 mapidOld);
	void notifyChangedMap(bool isDeserializeCase);
	void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);
	void notifyDisappearMagicCluster(MagicCluster* mc, u32 dcflags);
	void notifyChangedEventMode(bool isEventMode);
	void notifyChangedRtmvPlaying(bool isPlay, u16 evidRtmvMatter);
	void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, const Unit* unitCaller, bool isAddition);
	bool notifyFindUnconsciousFocus(Unit** unitFind);
	void notifyDischargeSouma(Souma* souma, u16 mcid);
	void notifyChangeLeader();
	void notifyPickupItem(const Unit* unitActor, const Unit* unitItem);
	void notifyEventActMatterTrigger(u16 evid, bool start, const Unit* unitTrg);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	GameNotifier();
	bool create();
	void destroy();

	//======================================================================
	// 変数
private:
};

SRCR_END_NS

	////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_BODY_NOTIFIER_H_
