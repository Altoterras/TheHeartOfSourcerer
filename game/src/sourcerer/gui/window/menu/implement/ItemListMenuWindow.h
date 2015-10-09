/***********************************************************************//**
 *	ItemListMenuWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_MENU_IMPLEMENT_ITEM_LIST_MENU_WINDOW_H_
#define _SRCR_GUI_WINDOW_MENU_IMPLEMENT_ITEM_LIST_MENU_WINDOW_H_

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
 *	アイテム リスト メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class ItemListMenuWindow : public ListMenuWindowBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
protected:
	void drawBlock(const RenderCtx* rc, const Block* block, bool isFocus);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_MENU_IMPLEMENT_ITEM_LIST_MENU_WINDOW_H_
