/***********************************************************************//**
 *	SubTest_MessageWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_DEBUG_SUB_MESSAGE_WINDOW_H_
#define _SRCR_DEBUG_SUB_MESSAGE_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "SubTestBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ETK_BEGIN_NS

class MessageWindow;

ETK_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	デバッグ クラス
 *
**//*---------------------------------------------------------------------*/
class SubTest_MessageWindow : public SubTestBase
{
public:
	SubTest_MessageWindow();
	~SubTest_MessageWindow();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);			// 3D 描画
	void draw(const RenderCtx* rc);				// 2D 描画

private:
	MessageWindow* _wndMsg;
	s32 _cntFrame;	
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_DEBUG_SUB_MESSAGE_WINDOW_H_
