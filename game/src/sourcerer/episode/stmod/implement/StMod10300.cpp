/***********************************************************************//**
 *	StMod10300.cpp
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
#include "StMod10300.h"

// Friends
#include "../../MapIdDef.h"
#include "../../StDepEvDef.h"
#include "../../StoryManager.h"
#include "../../party/MyParty.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
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

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StMod10300 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StMod10300::StMod10300()
	: _ebstatTreeAscend(EBSTAT_NULL)
	, _ebstatReikaWarmed(EBSTAT_NULL)
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StMod10300::exec(ExecRes* res, const ExecCtx* ec)
{
	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);

	if(unitmng->checkEnableEnemy())	// 有効な敵がいるかどうか確認する
	{
		// いる場合はイベントに移行できない
		if(_ebstatTreeAscend == EBSTAT_DELAY)		// 桜の水元素アセンション成功後の演出待ち
		{
			_ebstatTreeAscend = EBSTAT_NULL;	// キャンセル
		}
		else if(_ebstatReikaWarmed == EBSTAT_DELAY)	// レイカを暖めることに成功後の演出待ち
		{
			_ebstatReikaWarmed = EBSTAT_NULL;	// キャンセル
		}
	}
	else
	{
		// マップに敵がいないときのみイベントに移行できる
		if(_ebstatTreeAscend == EBSTAT_DELAY)		// 桜の水元素アセンション成功後の演出待ち
		{
			Unit* unitTree = unitmng->findUnitFromCtrlEvid(StDepEvDef::CTRLEVID_EP01_CERJA_TREE);
			if(unitTree != 0L)
			{
				// 表示上の火元素が 25% を超えたか
				const EleneStat* eestatTree = unitTree->getEleneStat();
				s32 ratioFi = eestatTree->getDfv()->getFire() * 100 / eestatTree->getDfv()->getAethel();
				if(ratioFi >= 25)	// 25% を超えた
				{
					// 成功
					onBlossomsWaterAscension(unitTree);
					_ebstatTreeAscend = EBSTAT_NULL;
				}
			}
		}
		else if(_ebstatReikaWarmed == EBSTAT_DELAY)	// レイカを暖めることに成功後の演出待ち
		{
			Unit* unitReika = unitmng->findUnitFromCtrlEvid(StDepEvDef::CTRLEVID_EP01_REIKA);
			if(unitReika != 0L)
			{
				onReikaWarmed(unitReika);
				_ebstatReikaWarmed = EBSTAT_NULL;
			}
		}
	}

	StMod10000::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void StMod10300::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	if(unitRecv->getCtrlEvid() == StDepEvDef::CTRLEVID_EP01_CERJA_TREE)			// 桜の木である
	{
		// 水単元素アセンション検知
		if((tstat->getFuncFlags() & EleneStat::FF_MASK_OP) == EleneStat::FF_OP_ASCEN_S)
		{
			// 火元素が空元素の 25% を超えた場合
			s32 ratioFi = tstat->getEleneStat()->getCql()->getFire() * 100 / tstat->getEleneStat()->getCql()->getAethel();
			if(ratioFi >= 25)	// 25% を超えた
			{
				// 成功
				if(_ebstatTreeAscend == EBSTAT_NULL)
				{
					_ebstatTreeAscend = EBSTAT_DELAY;	// 演出待ちへ
				}
			}
		}
	}
	else if(unitRecv->getCtrlEvid() == StDepEvDef::CTRLEVID_EP01_REIKA)			// レイカである
	{
		// 火元素エフェクト検知
		u32 op = tstat->getFuncFlags() & EleneStat::FF_MASK_OP;
		u32 eff = tstat->getFuncFlags() & EleneStat::FF_MASK_EFF;
		u32 atrbf = tstat->getFuncFlags() & EleneStat::FF_MASK_ATRB;
		// エフェクトの火系
		if(	(op == EleneStat::FF_OP_NOP)
			&& (	
				(eff == EleneStat::FF_EFF_SMOKE_H)
				|| (eff == EleneStat::FF_EFF_SAND_H)
				|| (eff ==EleneStat:: FF_EFF_MIST_H)
				|| (eff == EleneStat::FF_EFF_FOG_H)
				|| (eff == EleneStat::FF_EFF_WGRAIN_H)
				|| (eff == EleneStat::FF_EFF_SPRAY_H)
			) )
		{
			if(_ebstatReikaWarmed == EBSTAT_NULL)
			{
				_ebstatReikaWarmed = EBSTAT_DELAY;	// 演出待ちへ
			}
		}
		// もしくは、火の結界
		if(	((op == EleneStat::FF_OP_WRAP) || (op == EleneStat::FF_OP_BARRIER)) && TFW_IS_FLAG(atrbf, EleneStat::FF_ATRB_FIRE) )
		{
			if(_ebstatReikaWarmed == EBSTAT_NULL)
			{
				_ebstatReikaWarmed = EBSTAT_DELAY;	// 演出待ちへ
			}
		}
	}

	StMod10000::notifyUnitInfluenceStat(unitRecv, scres, tstat);
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void StMod10300::notifyChangedMap(s32 mapidNew, s32 mapidOld)
{
	// 状態リセット
	_ebstatTreeAscend = EBSTAT_NULL;
	_ebstatReikaWarmed = EBSTAT_NULL;

	StMod10000::notifyChangedMap(mapidNew, mapidOld);
}

/*---------------------------------------------------------------------*//**
	次のイベントに安全に移行できるかチェックする
**//*---------------------------------------------------------------------*/
bool StMod10300::isSafeNextEventConditionNow()
{
	// 通常歩行時以外（ソウマ詠唱中や対決中など）は失敗
	GameMode::ModeKind kindEnd = Game::getGame()->getGameMode()->getChildEndModeKind();
	if(kindEnd != GameMode::MODE_GAME_INGAME_WALKING_NORMAL)	{	return false;	}

	Party* party = Game::getGame()->getMyParty();
	// アサダウン中は失敗
	if(party->getPcUnit(MyParty::PC_MY)->getCharStat()->getStat() != CharStat::STAT_ALIVE)	{	return false;	}
	// コルポダウン中は失敗
	if(party->getPcUnit(MyParty::PC_PTN)->getCharStat()->getStat() != CharStat::STAT_ALIVE)	{	return false;	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
