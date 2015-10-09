/***********************************************************************//**
 *	EventMode.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EventMode.h"

// Friends
#include "../../../event/EventData.h"
#include "../../../event/EventSys.h"
#include "../../../event/matter/EventMatterType.h"
#include "../../../event/matter/implement/ActMatter.h"
#include "../../../body/Game.h"
#include "../../../body/GameNotifier.h"
#include "../../../gui/EventMovieFront.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/MoveCursor.h"

// External
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EventMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EventMode::EventMode()
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EventMode::~EventMode()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EventMode::create()
{
	// 自身の作成
	if(!GameModeBase::create(GameMode::MODE_TEMPLATE__START_, GameMode::MODE_TEMPLATE__END_, 0L))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EventMode::destroy()
{
	GameModeBase::destroy();
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool EventMode::notifyBegin(void* objParamRef)
{
	// プレイヤーキャラクタへイベントモード変更を通知
	Game::getGame()->getGameNotifier()->notifyChangedEventMode(true);

	// イベントムービーフロントを表示
	EventMovieFront* evmfr = Game::getGame()->getGui()->getEventMovieFront(); ASSERT(evmfr != 0L);
	evmfr->setEnable(true);

	// 移動カーソルをリセットする
	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
	mvcsr->reset();

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void EventMode::notifyEnd(void* objParamRef)
{
	// プレイヤーキャラクタのイベントモード終了を通知
	Game::getGame()->getGameNotifier()->notifyChangedEventMode(false);

	// イベントムービーフロントを隠す
	EventMovieFront* evmfr = Game::getGame()->getGui()->getEventMovieFront(); ASSERT(evmfr != 0L);
	evmfr->setEnable(false);
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void EventMode::exec(ExecRes* res, const ExecCtx* ec)
{
	GameModeBase::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void EventMode::draw(const RenderCtx* rc)
{
	GameModeBase::draw(rc);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
