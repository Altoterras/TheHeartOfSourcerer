/***********************************************************************//**
 *	StMod10303.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/18.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StMod10303.h"

// Friends
#include "../../MapIdDef.h"
#include "../../StoryManager.h"
#include "../../party/MyParty.h"
#include "../../../event/EventSys.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/MoveMap.h"
#include "../../../mode/GameMode.h"
#include "../../../unit/char/pc/PcUnit.h"
#include "../../../unit/UnitManager.h"
#include "../../../save/SaveData.h"
#include "../../../save/SaveSys.h"
#include "../../../status/CharStat.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define EVID_TREE_WATER_ASCEN			(10001)		// 桜水元素シフトイベント ID
#define FRAMECNT_DELAY_TREE_WATER_ASCEN	(1.0f * FRAMERATE)		// 桜水元素シフトイベント起動までの遅延フレーム

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StMod10303 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StMod10303::StMod10303()
	: _frameElapsedSuccessEvent(-1.0f)
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StMod10303::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	if(Game::getGame()->getMoveMap()->getCurMapId() == MapIdDef::MAPID_EP01_FOREST4_BLOSSOMS)	// セルジャの桜マップ（念のため）
	{
		if(_frameElapsedSuccessEvent >= 0.0f)	// イベント起動準備中
		{
			_frameElapsedSuccessEvent += gec->getDeltaFrame();	// カウント更新

			if(_frameElapsedSuccessEvent >= FRAMECNT_DELAY_TREE_WATER_ASCEN)	// イベント起動 OK
			{
				// 次のイベント起動確認
				do
				{
					// 通常歩行時以外（ソウマ詠唱中や対決中など）は失敗
					GameMode::ModeKind kindEnd = Game::getGame()->getGameMode()->getChildEndModeKind();
					if(kindEnd != GameMode::MODE_GAME_INGAME_WALKING_NORMAL)	{	break;	}

					Party* party = Game::getGame()->getMyParty();
					// アサダウン中は失敗
					if(party->getPcUnit(MyParty::PC_MY)->getCharStat()->getStat() != CharStat::STAT_ALIVE)	{	break;	}
					// コルポダウン中は失敗
					if(party->getPcUnit(MyParty::PC_PTN)->getCharStat()->getStat() != CharStat::STAT_ALIVE)	{	break;	}

					// イベント起動
					EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
					EvCause evcause(EventCause::CID_NULL);
					evsys->begin(EVID_TREE_WATER_ASCEN, 0, &evcause);
					_frameElapsedSuccessEvent = -1.0f;
				}
				while(false);
			}
		}
	}

	StMod10300::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void StMod10303::notifyChangedMap(s32 mapidNew, s32 mapidOld)
{
	// イベント起動カウントリセット
	_frameElapsedSuccessEvent = -1.0f;

	StMod10300::notifyChangedMap(mapidNew, mapidOld);
}

/*---------------------------------------------------------------------*//**
	桜の水元素アセンション通知
**//*---------------------------------------------------------------------*/
void StMod10303::onBlossomsWaterAscension(Unit* unitTree)
{
	// イベント起動カウント開始
	_frameElapsedSuccessEvent = 0.0f;

	StMod10300::onBlossomsWaterAscension(unitTree);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
