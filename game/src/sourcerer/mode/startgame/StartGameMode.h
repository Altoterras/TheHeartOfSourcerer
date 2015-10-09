/***********************************************************************//**
 *	StartGameMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_STARTGAME_START_GAME_MODE_H_
#define _SRCR_MODE_STARTGAME_START_GAME_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ゲーム開始 モード
 *
**//*---------------------------------------------------------------------*/
class StartGameMode : public GameModeBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
protected:
	bool notifyBegin(void* objParamRef);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_STARTGAME_START_GAME_MODE_H_
