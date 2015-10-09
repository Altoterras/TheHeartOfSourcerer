/***********************************************************************//**
 *	FocusMenuWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FocusMenuWindow.h"

// Friends

// External
#include "../../../../../tfw/lib/Rect.h"
#include "../../../../../tfw/lib/Size.h"
#include "../../../../../tfw/lib/Color.h"
#include "../../../../../tfw/g2d/StringDrawer.h"
#include "../../../../../tfw/g2d/TiledRectDraw.h"
#include "../../../../../tfw/gcmn/Gcalc.h"
#include "../../../../../tfw/gcmn/Renderer.h"
#include "../../../../../tfw/gcmn/RendererUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 色
#define COLOR_RECT_N_LT		 91,   0,   0, 255
#define COLOR_RECT_N_RT		 91,  31,  31, 255
#define COLOR_RECT_N_LB		 91,  47,  47, 255
#define COLOR_RECT_N_RB		 91,  63,  63, 255
#define COLOR_RECT_H_LT		245, 221, 211, 255
#define COLOR_RECT_H_RT		245, 221, 221, 255
#define COLOR_RECT_H_LB		255, 231, 221, 255
#define COLOR_RECT_H_RB		255, 231, 221, 255

// コンテンツのパディング
#define X_PADDING			8
#define Y_PADDING			8

// テクスチャ
#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ
#define W_BG_EDGE_L					 16.0f
#define H_BG_EDGE_L					 32.0f
#define W_BG_LOOP					 16.0f
#define H_BG_LOOP					 32.0f
#define W_BG_EDGE_R					 16.0f
#define H_BG_EDGE_R					 32.0f
#define UV_BG_EDGE_L_N				 96.0f, 896.0f, 16.0f, 32.0f, W_TEX, H_TEX
#define UV_BG_LOOP_N				112.0f, 896.0f, 16.0f, 32.0f, W_TEX, H_TEX
#define UV_BG_EDGE_R_N				128.0f, 896.0f, 16.0f, 32.0f, W_TEX, H_TEX
#define UV_BG_EDGE_L_H				 96.0f, 928.0f, 16.0f, 32.0f, W_TEX, H_TEX
#define UV_BG_LOOP_H				112.0f, 928.0f, 16.0f, 32.0f, W_TEX, H_TEX
#define UV_BG_EDGE_R_H				128.0f, 928.0f, 16.0f, 32.0f, W_TEX, H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// FocusMenuWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FocusMenuWindow::FocusMenuWindow()
	: _trdrawBgN(0L)
	, _trdrawBgH(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FocusMenuWindow::~FocusMenuWindow()
{
	ASSERT(_trdrawBgN == 0L);
	ASSERT(_trdrawBgH == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool FocusMenuWindow::create(TreeNode<MenuTreeNode>* tnodeSrc, s32 depth, Font* font, s32 fontsize, Texture* tex, void* paramShow, void* paramCreate)
{
	if(!MenuWindow::create(tnodeSrc, depth, font, fontsize, tex, paramShow, paramCreate))
	{
		return false;
	}

	// 背景タイルの作成
	RectF32 uv;
	_trdrawBgN = new TiledRectDraw();
	if(!_trdrawBgN->create(tex))
	{
		delete _trdrawBgN;
		_trdrawBgN = 0L;
		ASSERT(false);
		return false;
	}
	_trdrawBgN->setTile(TiledRectDraw::TP_LM, Gcalc::calcTexUv(&uv, UV_BG_EDGE_L_N, 1.0f), W_BG_EDGE_L, H_BG_EDGE_L, TFW_DISPF_RANG_0);
	_trdrawBgN->setTile(TiledRectDraw::TP_CM, Gcalc::calcTexUv(&uv, UV_BG_LOOP_N, 1.0f),   W_BG_LOOP,   H_BG_LOOP,   TFW_DISPF_RANG_0);
	_trdrawBgN->setTile(TiledRectDraw::TP_RM, Gcalc::calcTexUv(&uv, UV_BG_EDGE_R_N, 1.0f), W_BG_EDGE_R, H_BG_EDGE_R, TFW_DISPF_RANG_0);
	_trdrawBgH = new TiledRectDraw();
	if(!_trdrawBgH->create(tex))
	{
		delete _trdrawBgH;
		_trdrawBgH = 0L;
		ASSERT(false);
		return false;
	}
	_trdrawBgH->setTile(TiledRectDraw::TP_LM, Gcalc::calcTexUv(&uv, UV_BG_EDGE_L_H, 1.0f), W_BG_EDGE_L, H_BG_EDGE_L, TFW_DISPF_RANG_0);
	_trdrawBgH->setTile(TiledRectDraw::TP_CM, Gcalc::calcTexUv(&uv, UV_BG_LOOP_H, 1.0f),   W_BG_LOOP,   H_BG_LOOP,   TFW_DISPF_RANG_0);
	_trdrawBgH->setTile(TiledRectDraw::TP_RM, Gcalc::calcTexUv(&uv, UV_BG_EDGE_R_H, 1.0f), W_BG_EDGE_R, H_BG_EDGE_R, TFW_DISPF_RANG_0);


	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void FocusMenuWindow::destroy()
{
	MenuWindow::destroy();

	// 背景タイルの削除
	if(_trdrawBgH != 0L)
	{
		_trdrawBgH->destroy();
		delete _trdrawBgH;
		_trdrawBgH = 0L;
	}
	if(_trdrawBgN != 0L)
	{
		_trdrawBgN->destroy();
		delete _trdrawBgN;
		_trdrawBgN = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	要素の矩形を作成
**//*---------------------------------------------------------------------*/
void FocusMenuWindow::makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth)
{
	f32 x = xBase + (depth * 20);
	f32 y = yBase + (depth * 20);

	if(TFW_IS_FLAG(block->_typef, Block::TF_TITLE))
	{
		rectBlock->set(x, y, 0, 0);	// ノードの矩形サイズを設定（タイトル）
		rectText->set(x, y, 0, 0);	// テキストの矩形を設定（タイトル）
	}
	else
	{
		rectBlock->set(x, y, sizeTxt->w() + (X_PADDING * 2), sizeTxt->h() + (Y_PADDING * 2));	// ノードの矩形サイズを設定
		rectText->set(x + X_PADDING, y + Y_PADDING, sizeTxt->w(), sizeTxt->h());				// テキストの矩形を設定
	}
}

/*---------------------------------------------------------------------*//**
	要素の描画
**//*---------------------------------------------------------------------*/
void FocusMenuWindow::drawBlock(const RenderCtx* rc, const Block* block, bool isFocus)
{
	Renderer* rdr = rc->getRenderer();

	if(!block->isHidden())	// 隠し属性でなければ
	{
		// 背景矩形描画
		RectF32 rectBlock((f32)(block->_rect->x() - getCurScrollPos()->x()), (f32)(block->_rect->y() - getCurScrollPos()->y()), (f32)block->_rect->w(), (f32)block->_rect->h());
		rdr->setSolidColor(255, 255, 255, 255);
		if(isFocus)	{	_trdrawBgH->draw(rc, &rectBlock);	}	// フォーカス
		else		{	_trdrawBgN->draw(rc, &rectBlock);	}

		// テキスト描画
		if(block->isDisabled())	// 無効
		{	
			rdr->setSolidColor((u8)(block->_color->r() * 127.0f), (u8)(block->_color->g() * 127.0f), (u8)(block->_color->b() * 127.0f), 255);
		}
		else	// 有効
		{	
			if(isFocus)	{	rdr->setSolidColor(255 - (u8)(block->_color->r() * 255.0f), 255 - (u8)(block->_color->g() * 255.0f), 255 - (u8)(block->_color->b() * 255.0f), 255);	}	// フォーカス
			else		{	rdr->setSolidColor((u8)(block->_color->r() * 255.0f), (u8)(block->_color->g() * 255.0f), (u8)(block->_color->b() * 255.0f), 255);					}
		}
		block->_strdraw->draw(rc, - (f32)getCurScrollPos()->x(), - (f32)getCurScrollPos()->y());
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
