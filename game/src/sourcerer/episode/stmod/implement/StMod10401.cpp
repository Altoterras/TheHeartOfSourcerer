/***********************************************************************//**
 *	StMod10401.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/21.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StMod10401.h"

// Friends
#include "../../MapIdDef.h"
#include "../../StoryManager.h"
#include "../../party/MyParty.h"
#include "../../save/SaveIdDef.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../body/MoveMap.h"
#include "../../../mode/GameMode.h"
#include "../../../unit/char/pc/PcUnit.h"
#include "../../../unit/UnitManager.h"
#include "../../../save/SaveData.h"
#include "../../../save/SaveSys.h"
#include "../../../status/CharStat.h"

// External
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define EVID_KOTSUME_MEET_AGAIN	(10002)		// コツメ再会イベント

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StMod10401 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StMod10401::StMod10401()
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StMod10401::exec(ExecRes* res, const ExecCtx* ec)
{
	if(Game::getGame()->getMoveMap()->getCurMapId() == MapIdDef::MAPID_EP01_FOREST4_EXIT_SQUARE)	// シネンの森出口広場マップ
	{
		SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
		if(	svd->getSaveF(SaveIdDef::SFID_ST10401_DONE_YASAN_THANK) &&	// ヤサンイベントが終わっている
			!svd->getSaveF(SaveIdDef::SFID_ST10204_MEET_KOTSUME) )		// コツメが合流していない
		{
			UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
			if(!unitmng->checkEnableEnemy())	// 有効な敵がいない場合のみ
			{
				// キャラクタの位置によってイベント起動
				const PcUnit* unitLeader = Game::getGame()->getMyParty()->getLeaderPcUnit();
				if(unitLeader != 0L)
				{
					const Vector3F* posLeader = unitLeader->getPosition();
					if(posLeader->x() <= 2700.0f)	// 大猿に近づいた
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
							evsys->begin(EVID_KOTSUME_MEET_AGAIN, 0, &evcause);
						}
						while(false);
					}
				}
			}
		}
	}

	StMod10000::exec(res, ec);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
