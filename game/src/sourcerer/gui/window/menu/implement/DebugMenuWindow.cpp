/***********************************************************************//**
 *	DebugMenuWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "DebugMenuWindow.h"

// Friends

// External
#include "../../../../../tfw/collection/Tree.h"
#include "../../../../../tfw/lib/Rect.h"
#include "../../../../../tfw/lib/Size.h"
#include "../../../../../tfw/lib/Color.h"
#include "../../../../../tfw/g2d/StringDrawer.h"
#include "../../../../../tfw/g2d/font/Font.h"
#include "../../../../../tfw/gcmn/Renderer.h"
#include "../../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../../tfw/smenu/MenuTreeNode.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define X_PADDING		8
#define Y_PADDING		8

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// DebugMenuWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	要素の矩形を作成
**//*---------------------------------------------------------------------*/
void DebugMenuWindow::makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth)
{
	f32 x = xBase + (depth * 20);
	f32 y = yBase + (depth * 20);

	if(TFW_IS_FLAG(block->_typef, Block::TF_TITLE))
	{
		#if 0
			rectNode.set(xBase + X_PADDING, yBase, sizeTxt.w() + (X_PADDING * 2), sizeTxt.h() + Y_PADDING);		// ノードの矩形サイズを設定（タイトル）
			rectText.set(rectNode.x() + X_PADDING, rectNode.y() + Y_PADDING / 2, sizeTxt.w(), sizeTxt.h());		// テキストの矩形を設定（タイトル）
		#else
			rectBlock->set(x, y, 0, 0);	// ノードの矩形サイズを設定（タイトル）
			rectText->set(x, y, 0, 0);	// テキストの矩形を設定（タイトル）
		#endif
	}
	else
	{
		rectBlock->set(x, y, sizeAllTxtMax->w() + (X_PADDING * 2), sizeTxt->h() + (Y_PADDING * 2));	// ノードの矩形サイズを設定
		rectText->set(x + X_PADDING + (sizeAllTxtMax->w() - sizeTxt->w()) / 2, y + Y_PADDING, sizeTxt->w(), sizeTxt->h());		// テキストの矩形を設定
	}
}

/*---------------------------------------------------------------------*//**
	要素の描画
**//*---------------------------------------------------------------------*/
void DebugMenuWindow::drawBlock(const RenderCtx* rc, const Block* block, bool isFocus)
{
	Renderer* rdr = rc->getRenderer();

	if(!block->isHidden())	// 隠し属性でなければ
	{
		const MenuTreeNode* mtnode = block->_tnode->getObject();	ASSERT(mtnode != 0L);

		ColorU8 col1, col2, col3, col4;

		// 矩形描画
		RectF32 rectBlock((f32)(block->_rect->x() - getCurScrollPos()->x()), (f32)(block->_rect->y() - getCurScrollPos()->y()), (f32)block->_rect->w(), (f32)block->_rect->h());
		if(isFocus)	// フォーカス
		{
			RendererUtils::draw2dColorRect(rdr, &rectBlock, ColorU8::setout(&col1, 0, 0, 0, 191), ColorU8::setout(&col2, 127, 127, 127, 191), ColorU8::setout(&col3, 63, 63, 63, 191), ColorU8::setout(&col4, 191, 191, 191, 191) );
		}
		else
		{
			RendererUtils::draw2dColorRect(rdr, &rectBlock, ColorU8::setout(&col1, 0, 0, 0, 127), ColorU8::setout(&col2, 127, 127, 127, 127), ColorU8::setout(&col3, 63, 63, 63, 127), ColorU8::setout(&col4, 191, 191, 191, 127) );
		}

		// テキスト描画
		if(block->isDisabled())	{	rdr->setSolidColor((u8)(block->_color->r() * 127.0f), (u8)(block->_color->g() * 127.0f), (u8)(block->_color->b() * 127.0f), 255);	}	// 無効色
		else					{	rdr->setSolidColor((u8)(block->_color->r() * 255.0f), (u8)(block->_color->g() * 255.0f), (u8)(block->_color->b() * 255.0f), 255);	}	// 有効色
		block->_strdraw->draw(rc, - (f32)getCurScrollPos()->x(), - (f32)getCurScrollPos()->y());

		// チェック時
		if(mtnode->isChecked())
		{
			block->_strdraw->getFont()->draw('>', rectBlock.x() + 1, rectBlock.y() + 12, 10, rc);
		}

		// 非前面時は薄幕をかける
		if(!TFW_IS_FLAG(getFlags(), F_FRONT))
		{
			RendererUtils::draw2dColorRect(rdr, &rectBlock, ColorU8::setout(&col1, 0, 0, 0, 127));
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
