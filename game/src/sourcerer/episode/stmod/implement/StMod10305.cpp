/***********************************************************************//**
 *	StMod10305.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/19.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StMod10305.h"

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

#define EVID_TREE_ASCEND				(10002)		// レイカ出現イベント ID
#define EVID_REIKA_WARMED				(10003)		// レイカ暖めイベント ID
#define EVID_TREE_ASCEND_AGAIN			(10004)		// レイカ再出現イベント ID

#define FRAMECNT_DELAY_TREE_ASCEND		(30.0f)		// 桜水元素シフトイベント起動までの遅延フレーム
#define FRAMECNT_DELAY_REIKA_WARMED		(30.0f)		// レイカ暖めイベント起動までの遅延フレーム

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StMod10305 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StMod10305::StMod10305()
	: _frameElapsedSuccessEvent(-1.0f)
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StMod10305::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	if(Game::getGame()->getMoveMap()->getCurMapId() == MapIdDef::MAPID_EP01_FOREST4_BLOSSOMS)	// セルジャの桜マップ（念のため）
	{
		Step step = getCurStep();
		switch(step)
		{
		case STEP_WAIT_TREE_ASCEND_1:
		case STEP_WAIT_TREE_ASCEND_2:
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

						// イベント起動
						u16 evid = (step == STEP_WAIT_TREE_ASCEND_1) ? EVID_TREE_ASCEND : EVID_TREE_ASCEND_AGAIN;
						EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
						EvCause evcause(EventCause::CID_NULL);
						evsys->begin(evid, 0, &evcause);

						_frameElapsedSuccessEvent = -1.0f;	// カウンタリセット
					}
					while(false);
				}
			}
			break;
		case STEP_WAIT_REIKA_WARM:
			if(_frameElapsedSuccessEvent >= 0.0f)	// イベント起動準備中
			{
				_frameElapsedSuccessEvent += gec->getDeltaFrame();	// カウント更新
				if(_frameElapsedSuccessEvent >= FRAMECNT_DELAY_REIKA_WARMED)	// イベント起動 OK
				{
					// 次のイベント起動
					do
					{
						// イベントを安全に実行できるか？
						if(!isSafeNextEventConditionNow())		{	break;	}

						// 桜ユニットをフォーカス可にする
						UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
						Unit* unitTree = unitmng->findUnitFromCtrlEvid(StDepEvDef::CTRLEVID_EP01_CERJA_TREE);
						if((unitTree != 0L) && (!unitTree->isEnableFocus()))
						{	
							unitTree->setEnableFocus(true);	// 桜をフォーカス可に
							Game::getGame()->getGui()->getFocusCursor()->setFocus(0, 0L);	// 現在のフォーカスを外す
						}

						// イベント起動
						EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
						EvCause evcause(EventCause::CID_NULL);
						evsys->begin(EVID_REIKA_WARMED, 0, &evcause);

						_frameElapsedSuccessEvent = -1.0f;	// カウンタリセット
					}
					while(false);
				}
			}
			break;
		}
	}

	StMod10300::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void StMod10305::notifyChangedMap(s32 mapidNew, s32 mapidOld)
{
	// 状態リセット
	_frameElapsedSuccessEvent = -1.0f;

	StMod10300::notifyChangedMap(mapidNew, mapidOld);
}

/*---------------------------------------------------------------------*//**
	桜の水元素アセンション通知
**//*---------------------------------------------------------------------*/
void StMod10305::onBlossomsWaterAscension(Unit* unitTree)
{
	// イベント起動カウント開始
	Step step = getCurStep();
	if((step == STEP_WAIT_TREE_ASCEND_1) || (step == STEP_WAIT_TREE_ASCEND_2))
	{
		_frameElapsedSuccessEvent = 0.0f;
	}

	StMod10300::onBlossomsWaterAscension(unitTree);
}

/*---------------------------------------------------------------------*//**
	レイカ暖め通知
**//*---------------------------------------------------------------------*/
void StMod10305::onReikaWarmed(Unit* unitReika)
{
	// イベント起動カウント開始
	if(getCurStep() == STEP_WAIT_REIKA_WARM)
	{
		_frameElapsedSuccessEvent = 0.0f;
	}

	StMod10300::onReikaWarmed(unitReika);
}

/*---------------------------------------------------------------------*//**
	現在のステップを得る
**//*---------------------------------------------------------------------*/
StMod10305::Step StMod10305::getCurStep() const
{
	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
	Unit* unitTree = unitmng->findUnitFromCtrlEvid(StDepEvDef::CTRLEVID_EP01_CERJA_TREE);
	if((unitTree != 0L) && unitTree->isEnableFocus())
	{
		SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
		if(svd->getSaveF(SaveIdDef::SFID_ST10305_ONCE_DONE_REIKA_APPEAR))	// 一度レイカを出現させている
		{
			return STEP_WAIT_TREE_ASCEND_2;
		}
		else
		{
			return STEP_WAIT_TREE_ASCEND_1;
		}
	}
	else
	{
		return STEP_WAIT_REIKA_WARM;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
