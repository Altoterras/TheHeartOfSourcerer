/***********************************************************************//**
 *	MainMenuWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MainMenuWindow.h"

// Friends
#include "../../../../body/Game.h"
#include "../../../../gui/GameGui.h"

// External
#include "../../../../../tfw/lib/Rect.h"
#include "../../../../../tfw/lib/Size.h"
#include "../../../../../tfw/lib/Color.h"
#include "../../../../../tfw/g2d/StringDrawer.h"
#include "../../../../../tfw/gcmn/Renderer.h"
#include "../../../../../tfw/gcmn/RendererUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ
#define RECT_THIS					0, 48, Game::getGame()->getLogicalWidth(), Game::getGame()->getLogicalHeight()
#define UV_THIS						448.0f / W_TEX, 48.0f / H_TEX, 480.0f / W_TEX, 272.0f / H_TEX

#define X_MBLOCK					(184)
#define Y_MBLOCK					(64)
#define W_MBLOCK					(112)
#define H_MBLOCK					(40)
#define H_MBLOCK_PADDING			(20)
#define UV_MBLOCK_NRM				448 / W_TEX, 320 / H_TEX, 112 / W_TEX, 40 / H_TEX
#define UV_MBLOCK_HVR				560 / W_TEX, 320 / H_TEX, 112 / W_TEX, 40 / H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MainMenuWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MainMenuWindow::MainMenuWindow()
{
}

/*---------------------------------------------------------------------*//**
	作成通知
**//*---------------------------------------------------------------------*/
void MainMenuWindow::onCreatePanel()
{
	MenuWindow::onCreatePanel();

	// 自身の描画設定
	RectF32 rect(RECT_THIS);
	RectF32 uv(UV_THIS);
	setRectangle(&rect);
	setStateInfo(STATE_NORMAL, _texRef, &uv);

	// 動作設定
	setNoSelfCencel();	// 自己キャンセルを無効化
}

/*---------------------------------------------------------------------*//**
	要素の矩形を作成
**//*---------------------------------------------------------------------*/
void MainMenuWindow::makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth)
{
	f32 x = X_MBLOCK;
	f32 y = yBase;

	if(TFW_IS_FLAG(block->_typef, Block::TF_TITLE))
	{
		rectBlock->set(x, y, 0, 0);	// ノードの矩形サイズを設定（タイトル）
		rectText->set(x, y, 0, 0);	// テキストの矩形を設定（タイトル）
	}
	else
	{
		x = (Game::getGame()->getLogicalWidth() / 2) * (block->_index & 1) + ((Game::getGame()->getLogicalWidth() / 2) - W_MBLOCK) / 2;
		y = Y_MBLOCK + (f32)(block->_index >> 1) * (H_MBLOCK + H_MBLOCK_PADDING);

		rectBlock->set(x, y, W_MBLOCK, H_MBLOCK);	// ノードの矩形サイズを設定
		rectText->set(x + ((W_MBLOCK - sizeTxt->w()) / 2), y + ((H_MBLOCK - sizeTxt->h()) / 2), sizeTxt->w(), sizeTxt->h());	// テキストの矩形を設定
	}
}

/*---------------------------------------------------------------------*//**
	要素の描画
**//*---------------------------------------------------------------------*/
void MainMenuWindow::drawBlock(const RenderCtx* rc, const Block* block, bool isFocus)
{
	if(block->isHidden())	// 隠し属性
	{
		return;
	}
	if(!TFW_IS_FLAG(getFlags(), F_FRONT))		// 前面でないとき
	{	
		return;
	}

	bool isDisabled = block->isDisabled() || block->isUnused();

	Renderer* rdr = rc->getRenderer();

	// テクスチャセット
	rdr->bindTexture(_texRef);

	// 矩形描画
	RectF32 rectBlock((f32)(block->_rect->x() - getCurScrollPos()->x()), (f32)(block->_rect->y() - getCurScrollPos()->y()), (f32)block->_rect->w(), (f32)block->_rect->h());
	RectF32 uv;
	if(isDisabled)	// 無効
	{
		rdr->setSolidColor(127, 127, 127, 255);
	}
	else
	{
		rdr->setSolidColor(255, 255, 255, 255);
	}
	if(isFocus)		// フォーカス
	{
		RendererUtils::draw2dTextureRect(rdr, &rectBlock, RectF32::setout(&uv, UV_MBLOCK_HVR));
	}
	else
	{
		RendererUtils::draw2dTextureRect(rdr, &rectBlock, RectF32::setout(&uv, UV_MBLOCK_NRM));
	}

	// テキスト描画
	if(block->isDisabled())	// 無効
	{	
		rdr->setSolidColor((u8)(block->_color->r() * 127.0f), (u8)(block->_color->g() * 127.0f), (u8)(block->_color->b() * 127.0f), 255);
	}
	else	// 有効
	{	
		rdr->setSolidColor((u8)(block->_color->r() * 255.0f), (u8)(block->_color->g() * 255.0f), (u8)(block->_color->b() * 255.0f), 255);
	}
	block->_strdraw->draw(rc, - (f32)getCurScrollPos()->x(), - (f32)getCurScrollPos()->y());
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
