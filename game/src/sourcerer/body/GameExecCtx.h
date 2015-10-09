/***********************************************************************//**
 *	GameExecCtx.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BODY_GAME_EXEC_CTX_H_
#define _SRCR_BODY_GAME_EXEC_CTX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../etk/body/EtkExecCtx.h"

#include "Game.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Keyboard;
class TouchPanel;

TFW_END_NS

SRCR_BEGIN_NS

class Game;
class GameMode;

/*---------------------------------------------------------------------*//**
 *	ゲーム フレーム制御コンテキスト
 *
**//*---------------------------------------------------------------------*/
class GameExecCtx : public EtkExecCtx
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ゲームモードへの参照を得る
	inline GameMode*	getGameMode() const			{	return _gmodeRef;			}
	// イベント等による操作ロック中かを得る
	bool				isLockOperating() const;
	// ダウン防止中かどうかを得る
	bool				isPreventDown() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	GameExecCtx()
		: _gmodeRef(0L)
	{
	}

	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	GameExecCtx(f32 frameDelta, u32 eflags, TouchPanel* touchpRef, Keyboard* keybrdRef, u32 cntBody, f32 frameElapsed, bool isOpenedLockGui, bool isEventPause)
		: EtkExecCtx(frameDelta, eflags, touchpRef, keybrdRef, 0L, cntBody, frameElapsed, isOpenedLockGui, isEventPause)
		, _gmodeRef(Game::getGame()->getGameMode())
	{
	}

	/*-----------------------------------------------------------------*//**
		セット
	**//*-----------------------------------------------------------------*/
	void set(f32 frameDelta, u32 eflags, TouchPanel* touchpRef, Keyboard* keybrdRef, u32 cntBody, f32 frameElapsed, bool isOpenedLockGui, bool isEventPause)
	{
		EtkExecCtx::set(frameDelta, eflags, touchpRef, keybrdRef, 0L, cntBody, frameElapsed, isOpenedLockGui, isEventPause);
		_gmodeRef = Game::getGame()->getGameMode();
	}

	//======================================================================
	// 変数
private:
	GameMode*		_gmodeRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_BODY_GAME_EXEC_CTX_H_
