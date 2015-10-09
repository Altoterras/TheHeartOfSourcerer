/***********************************************************************//**
 *  GamePauseScreen.h
 *  Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2012/09/04.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BODY_GAME_PAUSE_SCREEN_H_
#define _SRCR_BODY_GAME_PAUSE_SCREEN_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../etk/body/PauseScreen.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ポーズ スクリーン
 *
**//*---------------------------------------------------------------------*/
class GamePauseScreen : public PauseScreen
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	void exec(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);

protected:
	bool loadTexture();
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_BODY_GAME_PAUSE_SCREEN_H_