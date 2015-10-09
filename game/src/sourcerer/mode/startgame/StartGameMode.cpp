/***********************************************************************//**
 *	StartGameMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StartGameMode.h"

// Friends
#include "../../body/Game.h"
#include "../../body/GameNotifier.h"
#include "../../event/EventSys.h"
#include "../../episode/StoryManager.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StartGameMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool StartGameMode::notifyBegin(void* objParamRef)
{
	// ゲームスタートモード開始通知
	GameNotifier* gnotif = Game::getGame()->getGameNotifier(); ASSERT(gnotif != 0L);
	gnotif->notifyBeginStartGameMode();

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
