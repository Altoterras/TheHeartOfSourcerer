/***********************************************************************//**
 *	SubTestBase.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_DEBUG_SUB_TEST_BASE_H_
#define _SRCR_DEBUG_SUB_TEST_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	デバッグ クラス
 *
**//*---------------------------------------------------------------------*/
class SubTestBase
{
public:
	SubTestBase() {}
	virtual ~SubTestBase() {}
	virtual bool create() { return true; }
	virtual void destroy() {}
	virtual void exec(ExecRes* res, const ExecCtx* ec) {}	// フレーム制御
	virtual void render(const RenderCtx* rc) {}			// 3D 描画
	virtual void draw(const RenderCtx* rc) {}			// 2D 描画
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_DEBUG_SUB_TEST_BASE_H_
