/***********************************************************************//**
 *	FocusMenuWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_MENU_IMPLEMENT_FOCUS_MENU_WINDOW_H_
#define _SRCR_GUI_WINDOW_MENU_IMPLEMENT_FOCUS_MENU_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MenuWindow.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class TiledRectDraw;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	フォーカス メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class FocusMenuWindow : public MenuWindow
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FocusMenuWindow();
	~FocusMenuWindow();
	bool create(TreeNode<MenuTreeNode>* tnodeSrc, s32 depth, Font* font, s32 fontsize, Texture* tex, void* paramShow, void* paramCreate);
	void destroy();

protected:
	void makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth);
	void drawBlock(const RenderCtx* rc, const Block* block, bool isFocus);

	//======================================================================
	// 変数
private:
	TiledRectDraw* _trdrawBgN;
	TiledRectDraw* _trdrawBgH;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_MENU_IMPLEMENT_FOCUS_MENU_WINDOW_H_
