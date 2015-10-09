/***********************************************************************//**
 *	DebugMenuWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_MENU_IMPLEMENT_DEBUG_MENU_WINDOW_H_
#define _SRCR_GUI_WINDOW_MENU_IMPLEMENT_DEBUG_MENU_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MenuWindow.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	デバッグ メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class DebugMenuWindow : public MenuWindow
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
protected:
	void makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth);
	void drawBlock(const RenderCtx* rc, const Block* block, bool isFocus);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_MENU_IMPLEMENT_DEBUG_MENU_WINDOW_H_
