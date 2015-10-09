/***********************************************************************//**
 *	ListMenuWindowBase.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/12/29.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_MENU_LIST_MENU_WINDOW_BASE_H_
#define _SRCR_GUI_WINDOW_MENU_LIST_MENU_WINDOW_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "MenuWindow.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Size;
typedef Size<f32> SizeF32;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

class ListWindowBase;

/*---------------------------------------------------------------------*//**
 *	アイテム リスト メニュー ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class ListMenuWindowBase : public MenuWindow
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ListMenuWindowBase();

protected:
	void makeBlockRectDefault(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 x, f32 y, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth);

	virtual void onCreatePanel();
	virtual void makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth);
	virtual void remakeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, const SizeF32* sizeTxt);
	virtual void drawBlock(const RenderCtx* rc, const Block* block, bool isFocus);

protected:
	Texture* _texRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_MENU_LIST_MENU_WINDOW_BASE_H_
