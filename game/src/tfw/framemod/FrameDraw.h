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
	virtual void render(const RenderCtx* rc) {}

	// 2D 描画
	virtual void draw(const RenderCtx* rc) {}
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_FRAMEMOD_FRAME_DRAW_H_
