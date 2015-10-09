/***********************************************************************//**
 *	TitleMenuWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/15.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_MENU_IMPLEMENT_TITLE_MENU_WINDOW_H_
#define _SRCR_GUI_WINDOW_MENU_IMPLEMENT_TITLE_MENU_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../MenuWindow.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Texture;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	タイトル メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class TitleMenuWindow : public MenuWindow
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TitleMenuWindow();
	bool create(TreeNode<MenuTreeNode>* tnodeSrc, s32 depth, Font* font, s32 fontsize, Texture* tex, void* paramShow, void* paramCreate);

protected:
	void onCreatePanel();
	void makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth);
	void drawBlock(const RenderCtx* rc, const Block* block, bool isFocus);

	//======================================================================
	// 変数
private:
	Texture* _texTitleRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_MENU_IMPLEMENT_TITLE_MENU_WINDOW_H_
