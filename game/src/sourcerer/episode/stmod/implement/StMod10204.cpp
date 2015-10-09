/***********************************************************************//**
 *	StMod10204.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StMod10204.h"

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
#include "../../../status/CharStat.h"
#include "../../../status/TransStat.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define CTRLEVID_CF_UNBOSHI		(51004)		// 雲星岩の制御用イベント ID

#define FRAMECNT_WAIT_PERFM		(60.0f)		// 発光演出待ち

#define EVID_SUCCESS_LUMINES	(10004)		// 発光成功時イベント
#
////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StMod10204 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StMod10204::StMod10204()
	: _framecntHitLightToUnboshi(-1.0f)
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StMod10204::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	if(_framecntHitLightToUnboshi >= 0.0f)	// 発光に成功している
	{
		_framecntHitLightToUnboshi += gec->getDeltaFrame();
		if(_framecntHitLightToUnboshi >= FRAMECNT_WAIT_PERFM)	// 演出待ち後
		{
			_framecntHitLightToUnboshi = -1.0f;	// リセット

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
				evsys->begin(EVID_SUCCESS_LUMINES, 0, &evcause);
			}
			while(false);
		}
	}

	StMod10000::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void StMod10204::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	if(Game::getGame()->getMoveMap()->getCurMapId() == MapIdDef::MAPID_EP01_FOREST4_ROCK_POND)	// シネンの森、雲月がいる池
	{
		if(unitRecv->getCtrlEvid() == CTRLEVID_CF_UNBOSHI)	// 雲星岩である
		{
			if((tstat->getFuncFlags() & EleneStat::FF_MASK_EFF) == EleneStat::FF_EFF_SHINE)
			{
				// OK
				_framecntHitLightToUnboshi = 0.0f;
			}
		}
	}

	StMod10000::notifyUnitInfluenceStat(unitRecv, scres, tstat);
}

/*---------------------------------------------------------------------*//**
	無意識フォーカス検索通知
**//*---------------------------------------------------------------------*/
bool StMod10204::notifyFindUnconsciousFocus(Unit** unitFind)
{
	if(Game::getGame()->getMoveMap()->getCurMapId() == MapIdDef::MAPID_EP01_FOREST4_ROCK_POND)	// シネンの森、雲月がいる池
	{
		UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
		Unit* unit = unitmng->findUnitFromCtrlEvid(CTRLEVID_CF_UNBOSHI);
		if(unit != 0L)
		{
			*unitFind = unit;
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
