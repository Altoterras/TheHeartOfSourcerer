/***********************************************************************//**
 *	StMod10306.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/20.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StMod10306.h"

// Friends
#include "../../MapIdDef.h"
#include "../../StDepEvDef.h"
#include "../../StoryManager.h"
#include "../../party/MyParty.h"
#include "../../save/SaveIdDef.h"
#include "../../../event/EventSys.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/MoveMap.h"
#include "../../../gui/FocusCursor.h"
#include "../../../gui/GameGui.h"
#include "../../../mode/GameMode.h"
#include "../../../unit/char/CharUnit.h"
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

#define EVID_CERJA_AND_REIKA			(10003)		// セルジャとレイカのイベント ID

#define FRAMECNT_DELAY_TREE_ASCEND		(30.0f)		// 桜水元素シフトイベント起動までの遅延フレーム

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StMod10306 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StMod10306::StMod10306()
	: _frameElapsedSuccessEvent(-1.0f)
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StMod10306::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	if(Game::getGame()->getMoveMap()->getCurMapId() == MapIdDef::MAPID_EP01_FOREST4_BLOSSOMS)	// セルジャの桜マップ（念のため）
	{
		if(_frameElapsedSuccessEvent >= 0.0f)	// イベント起動準備中
		{
			_frameElapsedSuccessEvent += gec->getDeltaFrame();	// カウント更新
			if(_frameElapsedSuccessEvent >= FRAMECNT_DELAY_TREE_ASCEND)		// イベント起動 OK
			{
				// 次のイベント起動
				do
				{
					// イベントを安全に実行できるか？
					if(!isSafeNextEventConditionNow())		{	break;	}

					// 桜ユニットをフォーカス不可にする
					UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
					Unit* unitTree = unitmng->findUnitFromCtrlEvid(StDepEvDef::CTRLEVID_EP01_CERJA_TREE);
					if((unitTree != 0L) && (unitTree->isEnableFocus()))
					{	
						unitTree->setEnableFocus(false);	// 桜をフォーカス不可に
						Game::getGame()->getGui()->getFocusCursor()->setFocus(0, 0L);	// 現在のフォーカスを外す
					}

					// セルジャの手紙は得ているか確認
					SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
					if(!svd->getSaveF(SaveIdDef::SFID_ST10306_TOOK_CERJA_LETTER))
					{
						_frameElapsedSuccessEvent = -1.0f;	// カウンタリセット
						break;
					}

					// イベント起動
					EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
					EvCause evcause(EventCause::CID_NULL);
					evsys->begin(EVID_CERJA_AND_REIKA, 0, &evcause);

					_frameElapsedSuccessEvent = -1.0f;	// カウンタリセット
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
void StMod10306::notifyChangedMap(s32 mapidNew, s32 mapidOld)
{
	// 状態リセット
	_frameElapsedSuccessEvent = -1.0f;

	StMod10300::notifyChangedMap(mapidNew, mapidOld);
}

/*---------------------------------------------------------------------*//**
	桜の水元素アセンション通知
**//*---------------------------------------------------------------------*/
void StMod10306::onBlossomsWaterAscension(Unit* unitTree)
{
	// イベント起動カウント開始
	_frameElapsedSuccessEvent = 0.0f;

	StMod10300::onBlossomsWaterAscension(unitTree);
}

/*---------------------------------------------------------------------*//**
	レイカ暖め通知
**//*---------------------------------------------------------------------*/
void StMod10306::onReikaWarmed(Unit* unitReika)
{
	// 特になし

	StMod10300::onReikaWarmed(unitReika);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
