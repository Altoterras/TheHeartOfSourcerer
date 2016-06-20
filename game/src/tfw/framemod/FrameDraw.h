/***********************************************************************//**
 *  FrameDraw.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FRAMEMOD_FRAME_DRAW_H_
#define _TFW_FRAMEMOD_FRAME_DRAW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

// # 前方参照で定義しても良いが、各所で面倒なのでインクルードしておく
#include "RenderCtx.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	レンダーモジュールの基底クラス
 *
**//*---------------------------------------------------------------------*/
TFW_INTERFACE FrameDraw
{
public:
	// 3D 描画
#if defined(_IOS)
	virtual void render(const RenderCtx* rc) {}
#else
	virtual void render(const RenderCtx* rc) { *((int*)0) = 0; }	// 【仮処理】実行すると強制終了
#endif
	// 2D 描画
#if defined(_IOS)
	virtual void draw(const RenderCtx* rc) {}
#else
	virtual void draw(const RenderCtx* rc) { *((int*)0) = 0; }		// 【仮処理】実行すると強制終了
#endif
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_FRAMEMOD_FRAME_DRAW_H_
