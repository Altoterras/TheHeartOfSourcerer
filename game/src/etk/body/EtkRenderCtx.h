/***********************************************************************//**
 *	EtkRenderCtx.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2012/05/29.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_BODY_ETK_RENDER_CTX_H_
#define _ETK_BODY_ETK_RENDER_CTX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/RenderCtx.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ツールキット描画コンテキスト
 *
**//*---------------------------------------------------------------------*/
class EtkRenderCtx : public RenderCtx
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ゲームカウンタを取得する
	inline u32 getBodyExecCount() const { return _cntBodyExec; }	// このカウンタは可変フレームに関係なくインクリメントされる値であることに注意
	// ゲームのフレームカウンタを取得する
	inline f32 getBodyElapsedFrame() const { return _frameBodyElapsed; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	EtkRenderCtx()
		: _cntBodyExec(0)
		, _frameBodyElapsed(0.0f)
	{
	}

	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	EtkRenderCtx(Renderer* rdrRef, u32 cntBodyExec, f32 frameBodyElapsed)
		: RenderCtx(rdrRef)
		, _cntBodyExec(cntBodyExec)
		, _frameBodyElapsed(frameBodyElapsed)
	{
	}

	/*-----------------------------------------------------------------*//**
		セット
	**//*-----------------------------------------------------------------*/
	void set(Renderer* rdrRef, u32 cntBodyExec, f32 frameBodyElapsed)
	{
		RenderCtx::set(rdrRef);
		_cntBodyExec = cntBodyExec;
		_frameBodyElapsed = frameBodyElapsed;
	}
	
	//======================================================================
	// メンバ変数
private:
	u32 _cntBodyExec;
	f32 _frameBodyElapsed;
};

////////////////////////////////////////////////////////////////////////////

ETK_END_NS

#endif	// _ETK_BODY_ETK_RENDER_CTX_H_
