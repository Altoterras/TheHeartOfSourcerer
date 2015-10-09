/***********************************************************************//**
 *	SubMenuWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/03.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_MENU_IMPLEMENT_SUB_MENU_WINDOW_H_
#define _SRCR_GUI_WINDOW_MENU_IMPLEMENT_SUB_MENU_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MenuWindow.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Texture;
class TiledRectDraw;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	サブ メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class SubMenuWindow : public MenuWindow
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SubMenuWindow();

protected:
	void onCreatePanel();
	void onDestroyPanel();
	void makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);
	void drawBlock(const RenderCtx* rc, const Block* block, bool isFocus);

private:
	Texture* _texRef;
	TiledRectDraw* _trdraw;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_MENU_IMPLEMENT_SUB_MENU_WINDOW_H_
