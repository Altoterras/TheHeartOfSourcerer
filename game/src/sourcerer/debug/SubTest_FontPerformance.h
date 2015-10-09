/***********************************************************************//**
 *	SubTest_FontPerformance.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_DEBUG_SUB_TEST_FONT_PERFORMANCE_H_
#define _SRCR_DEBUG_SUB_TEST_FONT_PERFORMANCE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "SubTestBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class StringDrawer;
class VcString;
class Font;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	デバッグ クラス
 *
**//*---------------------------------------------------------------------*/
class SubTest_FontPerformance : public SubTestBase
{
public:
	SubTest_FontPerformance();
	~SubTest_FontPerformance();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);			// 3D 描画
	void draw(const RenderCtx* rc);			// 2D 描画

private:
	StringDrawer*				_strdraw;
	Font*						_fontRef;
	VcString*					_str;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_DEBUG_SUB_TEST_FONT_PERFORMANCE_H_
