/***********************************************************************//**
 *	SourceListMenuWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_MENU_IMPLEMENT_SOURCE_LIST_MENU_WINDOW_H_
#define _SRCR_GUI_WINDOW_MENU_IMPLEMENT_SOURCE_LIST_MENU_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../ListMenuWindowBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Texture;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ソース リスト メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class SourceListMenuWindow : public ListMenuWindowBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
protected:
	void onDoubleTapRelease(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptTouch, Block* block);
	void drawBlock(const RenderCtx* rc, const Block* block, bool isFocus);
	bool adjustBlockString(VcString* str, RectF32* rectText, StringDrawer* strdraw, const RectF32* rectBlock);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_MENU_IMPLEMENT_SOURCE_LIST_MENU_WINDOW_H_
