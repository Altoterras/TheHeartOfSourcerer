/***********************************************************************//**
 *  FrameExecDraw.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FRAMEMOD_FRAME_EXEC_DRAW_H_
#define _TFW_FRAMEMOD_FRAME_EXEC_DRAW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "FrameExec.h"
#include "FrameDraw.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	フレーム制御・レンダーモジュールの基底クラス
 *
**//*---------------------------------------------------------------------*/
TFW_INTERFACE FrameExecDraw : public FrameExec, public FrameDraw
{
public:
	// フレーム制御
	virtual void exec(ExecRes* res, const ExecCtx* ec) = 0;
	// 3D 描画
	virtual void render(const RenderCtx* rc) {}

	// 2D 描画
	virtual void draw(const RenderCtx* rc) {}
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_FRAMEMOD_FRAME_EXEC_DRAW_H_
