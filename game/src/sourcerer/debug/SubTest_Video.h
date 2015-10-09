/***********************************************************************//**
 *	SubTest_Video.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_DEBUG_SUB_TEST_VIDEO_H_
#define _SRCR_DEBUG_SUB_TEST_VIDEO_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "SubTestBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Texture;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	デバッグ クラス
 *
**//*---------------------------------------------------------------------*/
class SubTest_Video : public SubTestBase
{
public:
	SubTest_Video();
	~SubTest_Video();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);			// 3D 描画
	void draw(const RenderCtx* rc);			// 2D 描画

private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_DEBUG_SUB_TEST_VIDEO_H_
