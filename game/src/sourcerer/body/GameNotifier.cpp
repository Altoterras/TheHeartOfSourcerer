/***********************************************************************//**
 *	GameNotifier.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/27.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// GameNotifier

// Self
#include "GameNotifier.h"

// Friends
#include "Game.h"
#include "../camera/GameCustomCamClass.h"
#include "../camera/WalkingCam.h"
#include "../confront/Confront.h"
#include "../body/Party.h"
#include "../gui/GameGui.h"
#include "../episode/StoryManager.h"
#include "../episode/data/FixedData.h"
#include "../event/EventSys.h"
#include "../mode/GameMode.h"
#include "../quest/QuestManager.h"
#include "../save/SaveSys.h"
#include "../unit/Unit.h"
#include "../unit/UnitArray.h"
#include "../unit/UnitManager.h"
#include "../unit/char/pc/PcUnit.h"

// External
#include "../../etk/sg/SorSgm.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GameNotifier メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	言語変更通知
	※システムが完全に構成されていないうちに呼び出される可能性がある点に注意
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyChangeLanguage()
{
	FixedData::notifyChangeLanguage();

	EtkNotifier::notifyChangeLanguage();
}

/*---------------------------------------------------------------------*//**
	画面リサイズ通知
	※現在はデバッグ用であり、不完全な実装である
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyResizeView()
{
	if (Game::getGame()->getGui() != 0L)
	{
		Game::getGame()->getGui()->notifyResizeView();
	}
}

/*---------------------------------------------------------------------*//**
	デシリアライズ（セーブデータ読み込み）通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyDeserialize()
{
	Game::getGame()->getGui()->notifyDeserialize();
}

/*---------------------------------------------------------------------*//**
	ゲーム開始通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyEnterGame()
{
	Game::getGame()->getGui()->notifyEnterGame();
}

/*---------------------------------------------------------------------*//**
	ゲームスタートモード開始通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyBeginStartGameMode()
{
#if defined(_ENLIGHT_PRPZL01)
	// パズルモードへ直接移行
	Game::getGame()->getGameMode()->changeEndMode(GameMode::MODE_GAME_PUREPUZZLE);
#else
	Game::getGame()->getStoryManager()->notifyBeginStartGameMode();
	Game::getGame()->getQuestManager()->notifyBeginStartGameMode();
#endif
}

/*---------------------------------------------------------------------*//**
	インゲームモード開始通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyBeginIngameMode()
{
	Game::getGame()->getStoryManager()->notifyBeginIngameMode();
	Game::getGame()->getQuestManager()->notifyBeginIngameMode();
}

/*---------------------------------------------------------------------*//**
	マップ変更前通知

	@param mapidNew 移動後のマップ ID （移動先を変更可）
	@param mapidOld	移動前のマップ ID
	@retval	true	そのままマップ変更を許可する
	@retval false	マップ変更を許可しない
**//*---------------------------------------------------------------------*/
bool GameNotifier::notifyChangeMapBefore(s32* mapidNew, s32 mapidOld)
{
	Game::getGame()->getGui()->notifyChangeMapBefore();
	Game::getGame()->getUnitManager()->notifyChangeMapBefore();

	if(!Game::getGame()->getStoryManager()->notifyChangeMapBefore(mapidNew, mapidOld))	// マップ番号強制変更の可能性あり
	{
		return false;	// 許可しない
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	マップ変更後通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyChangedMap(bool isDeserializeCase)
{
	Game::getGame()->getStoryManager()->notifyChangedMap(isDeserializeCase);
	Game::getGame()->getQuestManager()->notifyChangedMap(isDeserializeCase);
	Game::getGame()->getMyParty()->notifyChangedMap(isDeserializeCase);
	Game::getGame()->getUnitManager()->notifyChangedMap();
	Game::getGame()->getGui()->notifyChangedMap();
	Game::getGame()->getGameMode()->notifyChangedMap(isDeserializeCase);

	Camera* camVisible = Game::getGame()->getSceneGraph()->getVisibleCamera();
	if((camVisible != 0L) && (camVisible->getCameraClass() == GameCustomCamClass::CAMCLS_WALKING))
	{
		((WalkingCam*)camVisible)->notifyChangedMap();
	}
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	Game::getGame()->getEventSys()->notifyUnitInfluenceStat(unitRecv, scres, tstat);
	Game::getGame()->getStoryManager()->notifyUnitInfluenceStat(unitRecv, scres, tstat);
	Game::getGame()->getQuestManager()->notifyUnitInfluenceStat(unitRecv, scres, tstat);
	Game::getGame()->getGameMode()->notifyUnitInfluenceStat(unitRecv, scres, tstat);
}

/*---------------------------------------------------------------------*//**
	マジッククラスタの消滅通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyDisappearMagicCluster(MagicCluster* mc, u32 dcflags)
{
	Game::getGame()->getEventSys()->notifyDisappearMagicCluster(mc, dcflags);
}

/*---------------------------------------------------------------------*//**
	イベントモード変更通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyChangedEventMode(bool isEventMode)
{
	Game::getGame()->getUnitManager()->notifyChangedEventMode(isEventMode);
	Game::getGame()->getStoryManager()->notifyChangedEventMode(isEventMode);
}

/*---------------------------------------------------------------------*//**
	リアルタイムムービーの再生状態変更通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyChangedRtmvPlaying(bool isPlay, u16 evidRtmvMatter)
{
	Game::getGame()->getStoryManager()->notifyChangedRtmvPlaying(isPlay, evidRtmvMatter);
}

/*---------------------------------------------------------------------*//**
	対決状態変更通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, const Unit* unitCaller, bool isAddition)
{
	Party* party = Game::getGame()->getMyParty();

	// 対決参加者全員に開始通知を送る
	const UnitArray* uaTrgs = confront->getTargets();
	for(s32 iunit = 0; iunit < uaTrgs->getCount(); iunit++)
	{
		Unit* unit = uaTrgs->unit(iunit);
		if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_PC))	// プレイヤーキャラクタ
		{
			unit->notifyChangedConfront(isConfront, confront, unitTrg, unitTrg, isAddition);
		}
		else														// （おそらく）敵
		{
			unit->notifyChangedConfront(isConfront, confront, isConfront ? party->leaderPcUnit() : 0L, unitTrg, isAddition);
		}
	}
}

/*---------------------------------------------------------------------*//**
	無意識フォーカス検索通知
**//*---------------------------------------------------------------------*/
bool GameNotifier::notifyFindUnconsciousFocus(Unit** unitFind)
{
	if(Game::getGame()->getStoryManager()->notifyFindUnconsciousFocus(unitFind))
	{
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	ソウマ発射通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyDischargeSouma(Souma* souma, u16 mcid)
{
	Game::getGame()->getStoryManager()->notifyDischargeSouma(souma, mcid);
	Game::getGame()->getGameMode()->notifyDischargeSouma(souma, mcid);
}

/*---------------------------------------------------------------------*//**
	リーダー変更通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyChangeLeader()
{
	Game::getGame()->getGui()->notifyChangeLeader();
}

/*---------------------------------------------------------------------*//**
	アイテム拾い通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyPickupItem(const Unit* unitActor, const Unit* unitItem)
{
	Game::getGame()->getStoryManager()->notifyPickupItem(unitActor, unitItem);
}

/*---------------------------------------------------------------------*//**
	一連動作イベントマターの開始・終了通知
**//*---------------------------------------------------------------------*/
void GameNotifier::notifyEventActMatterTrigger(u16 evid, bool start, const Unit* unitTrg)
{
	Game::getGame()->getMyParty()->notifyEventActMatterTrigger(evid, start, unitTrg);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
GameNotifier::GameNotifier()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool GameNotifier::create()
{
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void GameNotifier::destroy()
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
