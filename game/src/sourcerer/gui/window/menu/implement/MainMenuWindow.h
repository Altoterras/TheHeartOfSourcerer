/***********************************************************************//**
 *	MainMenuWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_MENU_IMPLEMENT_MAIN_MENU_WINDOW_H_
#define _SRCR_GUI_WINDOW_MENU_IMPLEMENT_MAIN_MENU_WINDOW_H_

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
 *	メイン メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class MainMenuWindow : public MenuWindow
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MainMenuWindow();

protected:
	virtual void	onCreatePanel();
	virtual void	makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth);
	virtual void	drawBlock(const RenderCtx* rc, const Block* block, bool isFocus);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_MENU_IMPLEMENT_MAIN_MENU_WINDOW_H_
