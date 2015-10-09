/***********************************************************************//**
 *	ListMenuWindowBase.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/12/29.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ListMenuWindowBase.h"

// Friends
#include "../../../body/Game.h"
#include "../../../gui/GameGui.h"

// External
#include "../../../../tfw/collection/Tree.h"
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/g2d/StringDrawer.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/smenu/MenuTreeNode.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define W_TEX						(1024.0f)					// テクスチャ全体の幅
#define H_TEX						(1024.0f)					// テクスチャ全体の高さ

#define X_MBLOCK					(128)
#define W_MBLOCK					(Game::getGame()->getLogicalWidth() - 256)
#define H_MBLOCK					(32)
#define UV_MBLOCK_NRM				448 / W_TEX, 360 / H_TEX, 224 / W_TEX, 32 / H_TEX
#define UV_MBLOCK_HVR				448 / W_TEX, 392 / H_TEX, 224 / W_TEX, 32 / H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ListMenuWindowBase メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ListMenuWindowBase::ListMenuWindowBase()
	: _texRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	要素の標準矩形を作成
**//*---------------------------------------------------------------------*/
void ListMenuWindowBase::makeBlockRectDefault(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 x, f32 y, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth)
{
	if(TFW_IS_FLAG(block->_typef, Block::TF_TITLE))
	{
		rectBlock->set(x, y, 0, 0);	// ノードの矩形サイズを設定（タイトル）
		rectText->set(x, y, 0, 0);	// テキストの矩形を設定（タイトル）
	}
	else
	{
		rectBlock->set(x, y, W_MBLOCK, H_MBLOCK);	// ノードの矩形サイズを設定
		rectText->set(x + ((W_MBLOCK - sizeTxt->w()) / 2), y + ((H_MBLOCK - sizeTxt->h()) / 2), sizeTxt->w(), sizeTxt->h());	// テキストの矩形を設定
	}

	if(block->_tnode->object()->isHidden())
	{
		rectBlock->h() = 0;
	}
}

/*---------------------------------------------------------------------*//**
	作成通知
**//*---------------------------------------------------------------------*/
void ListMenuWindowBase::onCreatePanel()
{
	MenuWindow::onCreatePanel();

	// 動作設定
	setNoSelfCencel();	// 自己キャンセルを無効化
	setReverseDraw();	// 逆順描画に設定（複製アニメーションのため）

	// テクスチャを得る
	_texRef = Game::getGame()->getGui()->getGuiTexture();

	// 要素（ブロック）設定
	for(int i = 0; i < getElementNum(); i++)
	{
		Block* block = getBlock(i);
		if((block != 0L) && !TFW_IS_FLAG(block->_typef, Block::TF_TITLE))
		{
			block->enableOffsetAnim(2.5f, 0.95f);	// 要素（ブロック）をオフセットアニメ有効に
		}
	}
}

/*---------------------------------------------------------------------*//**
	要素の矩形を作成
**//*---------------------------------------------------------------------*/
void ListMenuWindowBase::makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth)
{
	makeBlockRectDefault(rectBlock, rectText, block, X_MBLOCK, yBase, sizeTxt, sizeAllTxtMax, depth);
}

/*---------------------------------------------------------------------*//**
	要素の矩形を再作成
**//*---------------------------------------------------------------------*/
void ListMenuWindowBase::remakeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, const SizeF32* sizeTxt)
{
	rectBlock->copy(block->_rect);
	rectText->set(block->_rect->x() + (block->_rect->w() - sizeTxt->w()) / 2, block->_rect->y() + (block->_rect->h() - sizeTxt->h()) / 2, sizeTxt->w(), sizeTxt->h());
}

/*---------------------------------------------------------------------*//**
	要素の描画
**//*---------------------------------------------------------------------*/
void ListMenuWindowBase::drawBlock(const RenderCtx* rc, const Block* block, bool isFocus)
{
	if(block->isHidden() || TFW_IS_FLAG(block->_typef, Block::TF_TITLE))	{	return;	}	// 隠し属性やタイトルは除外
	bool isDisabled = block->isDisabled() || block->isUnused();

	Renderer* rdr = rc->getRenderer();
	rdr->bindTexture(_texRef);	// テクスチャセット

	// 矩形描画
	RectF32 rectBlock(	(f32)(block->_rect->x() + block->_rectOfsAnim->x() - getCurScrollPos()->x()),
						(f32)(block->_rect->y() + block->_rectOfsAnim->y() - getCurScrollPos()->y()),
						(f32)block->_rect->w() + block->_rectOfsAnim->w(),
						(f32)block->_rect->h() + block->_rectOfsAnim->h()	);
	RectF32 uv;
	if(isDisabled)		{	rdr->setSolidColor(127, 127, 127, 255);	}	// 無効
	else				{	rdr->setSolidColor(255, 255, 255, 255);	}
	if(isFocus)	{	RendererUtils::draw2dTextureRect(rdr, &rectBlock, RectF32::setout(&uv, UV_MBLOCK_HVR));	}	// フォーカス
	else		{	RendererUtils::draw2dTextureRect(rdr, &rectBlock, RectF32::setout(&uv, UV_MBLOCK_NRM));	}

	// テキスト描画
	if(isDisabled)		{	rdr->setSolidColor((u8)(block->_color->r() * 127.0f), (u8)(block->_color->g() * 127.0f), (u8)(block->_color->b() * 127.0f), 255);	}	// 無効
	else if(isFocus)	{	rdr->setSolidColor((u8)(block->_color->r() * 255.0f), (u8)(block->_color->g() * 231.0f), (u8)(block->_color->b() * 127.0f), 255);	}	// フォーカス
	else				{	rdr->setSolidColor((u8)(block->_color->r() * 255.0f), (u8)(block->_color->g() * 255.0f), (u8)(block->_color->b() * 255.0f), 255);	}
	block->_strdraw->draw(rc, block->_rectOfsAnim->x() - (f32)getCurScrollPos()->x(), block->_rectOfsAnim->y() - (f32)getCurScrollPos()->y());
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
