/***********************************************************************//**
 *	GameRenderCtx.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BODY_GAME_RENDER_CTX_H_
#define _SRCR_BODY_GAME_RENDER_CTX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../etk/body/EtkRenderCtx.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class GameFontSet;
class Game;

/*---------------------------------------------------------------------*//**
 *	ゲーム描画コンテキスト
 *
**//*---------------------------------------------------------------------*/
class GameRenderCtx : public EtkRenderCtx
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// フォントセットを得る
	inline GameFontSet*		getFontSet()				{	return _fontsetRef;			}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	GameRenderCtx()
		: _fontsetRef(0L)
	{
	}

	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	GameRenderCtx(GameFontSet* fontsetRef, Renderer* rdrRef, u32 cntBodyExec, f32 frameCnt)
		: EtkRenderCtx(rdrRef, cntBodyExec, frameCnt)
		, _fontsetRef(fontsetRef)
	{
	}

	/*-----------------------------------------------------------------*//**
		セット
	**//*-----------------------------------------------------------------*/
	void set(GameFontSet* fontsetRef, Renderer* rdrRef, u32 cntBodyExec, f32 frameCnt)
	{
		EtkRenderCtx::set(rdrRef, cntBodyExec, frameCnt);
		_fontsetRef = fontsetRef;
	}

	//======================================================================
	// メンバ変数
private:
	GameFontSet*		_fontsetRef;
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_BODY_GAME_RENDER_CTX_H_
