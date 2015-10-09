/***********************************************************************//**
 *	SubMenuWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/03.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SubMenuWindow.h"

// Friends
#include "../../../../body/Game.h"
#include "../../../../gui/GameGui.h"

// External
#include "../../../../../tfw/collection/Tree.h"
#include "../../../../../tfw/lib/Rect.h"
#include "../../../../../tfw/lib/Size.h"
#include "../../../../../tfw/lib/Color.h"
#include "../../../../../tfw/g2d/StringDrawer.h"
#include "../../../../../tfw/g2d/TiledRectDraw.h"
#include "../../../../../tfw/gcmn/Gcalc.h"
#include "../../../../../tfw/gcmn/Renderer.h"
#include "../../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../../tfw/smenu/MenuTreeNode.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define W_TEX						(1024.0f)					// テクスチャ全体の幅
#define H_TEX						(1024.0f)					// テクスチャ全体の高さ

// 背景タイル
// U1 U2   U3 U4
// +--+----+--+ V1
// |  |    |  |
// +--+----+--+ V2
// |  |    |  |
// |  |    |  |
// +--+----+--+ V3
// |  |    |  |
// +--+----+--+ V4
#define UV_BG_U1		(656.0f)
#define UV_BG_U2		(704.0f)
#define UV_BG_U3		(768.0f)
#define UV_BG_V1		(696.0f)
#define UV_BG_V2		(720.0f)
#define UV_BG_V3		(768.0f)
#define UV_BG_U12_W		(32.0f)
#define UV_BG_U23_W		(64.0f)
#define UV_BG_U34_W		(32.0f)
#define UV_BG_V12_H		(24.0f)
#define UV_BG_V23_H		(48.0f)
#define UV_BG_V34_H		(24.0f)
#define UV_BG_LT_U		UV_BG_U1
#define UV_BG_LT_V		UV_BG_V1
#define UV_BG_LT_W		UV_BG_U12_W
#define UV_BG_LT_H		UV_BG_V12_H
#define UV_BG_CT_U		UV_BG_U2
#define UV_BG_CT_V		UV_BG_V1
#define UV_BG_CT_W		UV_BG_U23_W
#define UV_BG_CT_H		UV_BG_V12_H
#define UV_BG_RT_U		UV_BG_U3
#define UV_BG_RT_V		UV_BG_V1
#define UV_BG_RT_W		UV_BG_U34_W
#define UV_BG_RT_H		UV_BG_V12_H
#define UV_BG_LM_U		UV_BG_U1
#define UV_BG_LM_V		UV_BG_V2
#define UV_BG_LM_W		UV_BG_U12_W
#define UV_BG_LM_H		UV_BG_V23_H
#define UV_BG_CM_U		UV_BG_U2
#define UV_BG_CM_V		UV_BG_V2
#define UV_BG_CM_W		UV_BG_U23_W
#define UV_BG_CM_H		UV_BG_V23_H
#define UV_BG_RM_U		UV_BG_U3
#define UV_BG_RM_V		UV_BG_V2
#define UV_BG_RM_W		UV_BG_U34_W
#define UV_BG_RM_H		UV_BG_V23_H
#define UV_BG_LB_U		UV_BG_U1
#define UV_BG_LB_V		UV_BG_V3
#define UV_BG_LB_W		UV_BG_U12_W
#define UV_BG_LB_H		UV_BG_V34_H
#define UV_BG_CB_U		UV_BG_U2
#define UV_BG_CB_V		UV_BG_V3
#define UV_BG_CB_W		UV_BG_U23_W
#define UV_BG_CB_H		UV_BG_V34_H
#define UV_BG_RB_U		UV_BG_U3
#define UV_BG_RB_V		UV_BG_V3
#define UV_BG_RB_W		UV_BG_U34_W
#define UV_BG_RB_H		UV_BG_V34_H

// メニューブロック
#define UV_MBLOCK_NRM				752.0f,	352.0f, 96.0f, 32.0f, W_TEX, H_TEX		// メニューウィンドウボタン
#define UV_MBLOCK_HVR				752.0f,	384.0f, 96.0f, 32.0f, W_TEX, H_TEX		// メニューウィンドウボタン押
#define W_MBLOCK					(96)
#define H_MBLOCK					(32)
#define Y_MBLOCK_EDGE_PADDING		(14)
#define X_MBLOCK_PADDING			(15)
#define Y_MBLOCK_PADDING			(4)
#define X_TXT_PADDING				(15)
#define Y_TXT_PADDING				(0)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SubMenuWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SubMenuWindow::SubMenuWindow()
	: _texRef(0L)
	, _trdraw(0L)
{

}

/*---------------------------------------------------------------------*//**
	作成通知
**//*---------------------------------------------------------------------*/
void SubMenuWindow::onCreatePanel()
{
	MenuWindow::onCreatePanel();

	// 動作設定
	//setNoSelfCencel();	// 自己キャンセルを無効化

	// テクスチャを得る
	_texRef = Game::getGame()->getGui()->getGuiTexture();

	// タイル描画を作成
	_trdraw = new TiledRectDraw();
	if(_trdraw->create(_texRef))
	{
		RectF32 uv;
		_trdraw->setTile(TiledRectDraw::TP_LT, Gcalc::calcTexUv(&uv, UV_BG_LT_U, UV_BG_LT_V, UV_BG_LT_W, UV_BG_LT_H, W_TEX, H_TEX, Env_get2drRate()), UV_BG_LT_W, UV_BG_LT_H, TFW_DISPF_RANG_0);
		_trdraw->setTile(TiledRectDraw::TP_CT, Gcalc::calcTexUv(&uv, UV_BG_CT_U, UV_BG_CT_V, UV_BG_CT_W, UV_BG_CT_H, W_TEX, H_TEX, Env_get2drRate()), UV_BG_CT_W, UV_BG_CT_H, TFW_DISPF_RANG_0);
		_trdraw->setTile(TiledRectDraw::TP_RT, Gcalc::calcTexUv(&uv, UV_BG_RT_U, UV_BG_RT_V, UV_BG_RT_W, UV_BG_RT_H, W_TEX, H_TEX, Env_get2drRate()), UV_BG_RT_W, UV_BG_RT_H, TFW_DISPF_RANG_0);
		_trdraw->setTile(TiledRectDraw::TP_LM, Gcalc::calcTexUv(&uv, UV_BG_LM_U, UV_BG_LM_V, UV_BG_LM_W, UV_BG_LM_H, W_TEX, H_TEX, Env_get2drRate()), UV_BG_LM_W, UV_BG_LM_H, TFW_DISPF_RANG_0);
		_trdraw->setTile(TiledRectDraw::TP_CM, Gcalc::calcTexUv(&uv, UV_BG_CM_U, UV_BG_CM_V, UV_BG_CM_W, UV_BG_CM_H, W_TEX, H_TEX, Env_get2drRate()), UV_BG_CM_W, UV_BG_CM_H, TFW_DISPF_RANG_0);
		_trdraw->setTile(TiledRectDraw::TP_RM, Gcalc::calcTexUv(&uv, UV_BG_RM_U, UV_BG_RM_V, UV_BG_RM_W, UV_BG_RM_H, W_TEX, H_TEX, Env_get2drRate()), UV_BG_RM_W, UV_BG_RM_H, TFW_DISPF_RANG_0);
		_trdraw->setTile(TiledRectDraw::TP_LB, Gcalc::calcTexUv(&uv, UV_BG_LB_U, UV_BG_LB_V, UV_BG_LB_W, UV_BG_LB_H, W_TEX, H_TEX, Env_get2drRate()), UV_BG_LB_W, UV_BG_LB_H, TFW_DISPF_RANG_0);
		_trdraw->setTile(TiledRectDraw::TP_CB, Gcalc::calcTexUv(&uv, UV_BG_CB_U, UV_BG_CB_V, UV_BG_CB_W, UV_BG_CB_H, W_TEX, H_TEX, Env_get2drRate()), UV_BG_CB_W, UV_BG_CB_H, TFW_DISPF_RANG_0);
		_trdraw->setTile(TiledRectDraw::TP_RB, Gcalc::calcTexUv(&uv, UV_BG_RB_U, UV_BG_RB_V, UV_BG_RB_W, UV_BG_RB_H, W_TEX, H_TEX, Env_get2drRate()), UV_BG_RB_W, UV_BG_RB_H, TFW_DISPF_RANG_0);
	}
	else
	{
		delete _trdraw;
		_trdraw = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	作成通知
**//*---------------------------------------------------------------------*/
void SubMenuWindow::onDestroyPanel()
{
	// タイル描画を削除する
	if(_trdraw != 0L)
	{
		_trdraw->destroy();
		delete _trdraw;
		_trdraw = 0L;
	}

	MenuWindow::onDestroyPanel();
}

/*---------------------------------------------------------------------*//**
	要素の矩形を作成
**//*---------------------------------------------------------------------*/
void SubMenuWindow::makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth)
{
	rectBlock->set(
		xBase,
		yBase,
		TFW_MAX(W_MBLOCK, sizeAllTxtMax->w()) + (X_TXT_PADDING * 2) + (X_MBLOCK_PADDING * 2),
		TFW_MAX(H_MBLOCK, sizeAllTxtMax->h()) + (Y_TXT_PADDING * 2) + (Y_MBLOCK_PADDING * 2) + ( (TFW_IS_FLAG(block->_typef, Block::TF_TAIL) || (block->_index == 0)) ? Y_MBLOCK_EDGE_PADDING : 0 )	);
	if(rectBlock->left() < 0)
	{
		rectBlock->left() = 0;
	}
	if(rectBlock->right() > Game::getGame()->getLogicalWidth())
	{
		rectBlock->left() -= rectBlock->right() - Game::getGame()->getLogicalWidth();
	}

	if(TFW_IS_FLAG(block->_typef, Block::TF_TITLE))
	{
		rectBlock->height() = 0;	// ノードの矩形サイズを設定（タイトル）
		rectText->height() = 0;		// テキストの矩形を設定（タイトル）
	}
	else
	{
		RectF32 rectButton(
			rectBlock->x() + X_MBLOCK_PADDING,
			rectBlock->y() + Y_MBLOCK_PADDING + ( (block->_index == 0) ? Y_MBLOCK_EDGE_PADDING : 0 ),
			rectBlock->w() - (X_MBLOCK_PADDING * 2),
			rectBlock->h() - (Y_MBLOCK_PADDING * 2) - ( (TFW_IS_FLAG(block->_typef, Block::TF_TAIL) || (block->_index == 0)) ? Y_MBLOCK_EDGE_PADDING : 0 )	);
		rectText->set(rectButton.x() + ((rectButton.w() - sizeTxt->w()) / 2), rectButton.y() + ((rectButton.h() - sizeTxt->h()) / 2), sizeTxt->w(), sizeTxt->h());	// テキストの矩形を設定
	}

	if(block->_tnode->object()->isHidden())
	{
		rectBlock->h() = 0;
	}
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void SubMenuWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	Renderer* rdr = rc->getRenderer();

	// 背景描画
	if(_trdraw != 0L)
	{
		_trdraw->draw(rc, getRectangle());
	}

	MenuWindow::drawContents(rc, rectBase);
}

/*---------------------------------------------------------------------*//**
	要素の描画
**//*---------------------------------------------------------------------*/
void SubMenuWindow::drawBlock(const RenderCtx* rc, const Block* block, bool isFocus)
{
	if(block->isHidden() || TFW_IS_FLAG(block->_typef, Block::TF_TITLE))	// 隠し属性やタイトルは除外
	{
		return;
	}

	bool isDisabled = block->isDisabled() || block->isUnused();

	Renderer* rdr = rc->getRenderer();
	RectF32 uv;

	// テクスチャセット
	rdr->bindTexture(_texRef);

	// 矩形描画
	RectF32 rectButton(
		(f32)(block->_rect->x() - getCurScrollPos()->x() + X_MBLOCK_PADDING),
		(f32)(block->_rect->y() - getCurScrollPos()->y() + Y_MBLOCK_PADDING + ( (block->_index == 0) ? Y_MBLOCK_EDGE_PADDING : 0 )),
		(f32)(block->_rect->w() - (X_MBLOCK_PADDING * 2)),
		(f32)(block->_rect->h() - (Y_MBLOCK_PADDING * 2) - ( (TFW_IS_FLAG(block->_typef, Block::TF_TAIL) || (block->_index == 0)) ? Y_MBLOCK_EDGE_PADDING : 0 ))	);
	// 色選択
	if(isDisabled)	// 無効
	{
		rdr->setSolidColor(127, 127, 127, 255);
	}
	else
	{
		rdr->setSolidColor(255, 255, 255, 255);
	}
	// ボタン描画
	if(isFocus && !isDisabled)	// フォーカス
	{
		RendererUtils::draw2dTextureRect(rdr, &rectButton, Gcalc::calcTexUv(&uv, UV_MBLOCK_HVR, 1.0f));
	}
	else
	{
		RendererUtils::draw2dTextureRect(rdr, &rectButton, Gcalc::calcTexUv(&uv, UV_MBLOCK_NRM, 1.0f));
	}

	// テキスト描画
	if(isDisabled)	// 無効
	{	
		rdr->setSolidColor((u8)(block->_color->r() * 127.0f), (u8)(block->_color->g() * 127.0f), (u8)(block->_color->b() * 127.0f), 255);
	}
	else			// 有効
	{	
		if(isFocus)	// フォーカス
		{
			rdr->setSolidColor(255 - (u8)(block->_color->r() * 255.0f), 255 - (u8)(block->_color->g() * 255.0f), 255 - (u8)(block->_color->b() * 255.0f), 255);
		}
		else
		{
			rdr->setSolidColor((u8)(block->_color->r() * 255.0f), (u8)(block->_color->g() * 255.0f), (u8)(block->_color->b() * 255.0f), 255);
		}
	}
	block->_strdraw->draw(rc, - (f32)getCurScrollPos()->x(), - (f32)getCurScrollPos()->y());

	///rdr->setSolidColor(255, 0, 0, 255);
	///RendererUtils::draw2dRectLine(rdr, block->_rect);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
