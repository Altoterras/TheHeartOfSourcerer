/***********************************************************************//**
 *	TestViewTerminatorMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/11/26.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TestViewTerminatorMode.h"

// Friends
#include "../../body/Game.h"
#include "../../mode/GameMode.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TestViewTerminatorMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool TestViewTerminatorMode::notifyBegin(void* objParamRef)
{
	///this->getParentMode()->reserveMode(GameMode::MODE_GAME_MODELVW);	// 最初のデバッグモードへ
	Game::getGame()->getGameMode()->changeEndMode(GameMode::MODE_GAME_MODELVW);	// 最初のデバッグモードへ

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
