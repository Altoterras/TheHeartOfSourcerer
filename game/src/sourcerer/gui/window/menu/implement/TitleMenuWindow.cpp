/***********************************************************************//**
 *	TitleMenuWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/15.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TitleMenuWindow.h"

// Friends
#include "../../../menu/GameMenuShowParam.h"
#include "../../../../body/Game.h"
#include "../../../../mode/title/TitleRole.h"

// External
#include "../../../../../tfw/collection/Tree.h"
#include "../../../../../tfw/lib/Rect.h"
#include "../../../../../tfw/lib/Size.h"
#include "../../../../../tfw/lib/Color.h"
#include "../../../../../tfw/g2d/StringDrawer.h"
#include "../../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../../tfw/gcmn/Gcalc.h"
#include "../../../../../tfw/gcmn/Renderer.h"
#include "../../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../../tfw/smenu/MenuTreeNode.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define COLOR_RECT_N_LT		 21,  31,  31, 255
#define COLOR_RECT_N_RT		 29,  39,  39, 255
#define COLOR_RECT_N_LB		 33,  43,  43, 255
#define COLOR_RECT_N_RB		 37,  47,  47, 255
#define COLOR_RECT_H_LT		 42,  62,  62, 255
#define COLOR_RECT_H_RT		 58,  78,  78, 255
#define COLOR_RECT_H_LB		 66,  84,  84, 255
#define COLOR_RECT_H_RB		 74,  94,  94, 255

#define X_PADDING			8
#define Y_PADDING			8

#define W_TEX	(1024.0f)	// テクスチャ全体の幅
#define H_TEX	(1024.0f)	// テクスチャ全体の高さ
#define UV_PSNS_ON	386.0f, 642.0f, 60.0f, 28.0f, W_TEX, H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TitleMenuWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TitleMenuWindow::TitleMenuWindow()
	: _texTitleRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool TitleMenuWindow::create(TreeNode<MenuTreeNode>* tnodeSrc, s32 depth, Font* font, s32 fontsize, Texture* tex, void* paramShow, void* paramCreate)
{
	// タイトルテクスチャを取得しておく
	const GameMenuShowParam* sprm = (const GameMenuShowParam*)paramShow;
	TitleRole* own = (TitleRole*)sprm->_ownerRef;
	_texTitleRef = own->getTitleTexture();

	return MenuWindow::create(tnodeSrc, depth, font, fontsize, tex, paramShow, paramCreate);
}

/*---------------------------------------------------------------------*//**
	作成通知
**//*---------------------------------------------------------------------*/
void TitleMenuWindow::onCreatePanel()
{
	MenuWindow::onCreatePanel();

	setNoSelfCencel();	// 自己キャンセルを無効化
}

/*---------------------------------------------------------------------*//**
	要素の矩形を作成
**//*---------------------------------------------------------------------*/
void TitleMenuWindow::makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth)
{
	f32 wScreen = Game::getGame()->getLogicalWidth();//Game::getGame()->getScreenWidth();
	f32 x = (wScreen - sizeTxt->w()) / 2;
	f32 y = yBase;

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
void TitleMenuWindow::drawBlock(const RenderCtx* rc, const Block* block, bool isFocus)
{
	if(!block->isHidden())	// 隠し属性でなければ
	{
		const MenuTreeNode* mtnode = block->_tnode->getObject();	ASSERT(mtnode != 0L);

		u8 alpha = 255;

		// 無効化時
		if(mtnode->isDisabled())
		{
			alpha /= 2;
		}

		// 非前面時
		if(!TFW_IS_FLAG(getFlags(), F_FRONT))
		{
			alpha /= 4;
		}

		// 背景
		ColorU8 colDeep(15, 63, 31, alpha / 4 * 3);
		ColorU8 colPale(127, 191, 255, alpha / 8);
		ColorU8 colLight(255, 255, 255, 0);
		s32 wa = 40;
		f32 hwBlock = (f32)((block->_rect->w() + wa + wa) / 2);
		f32 hhBlock = (f32)((block->_rect->h()) / 2);
		RendererUtils::draw2dColorRect(rc->getRenderer(), (f32)(block->_rect->x() - getCurScrollPos()->x() - wa), (f32)(block->_rect->y() - getCurScrollPos()->y()), hwBlock, hhBlock, &colLight, &colPale, &colPale, &colDeep);
		RendererUtils::draw2dColorRect(rc->getRenderer(), (f32)(block->_rect->x() - getCurScrollPos()->x() - wa + hwBlock), (f32)(block->_rect->y() - getCurScrollPos()->y()), hwBlock, hhBlock, &colPale, &colLight, &colDeep, &colPale);
		RendererUtils::draw2dColorRect(rc->getRenderer(), (f32)(block->_rect->x() - getCurScrollPos()->x() - wa), (f32)(block->_rect->y() - getCurScrollPos()->y() + hhBlock), hwBlock, hhBlock, &colPale, &colDeep, &colLight, &colPale);
		RendererUtils::draw2dColorRect(rc->getRenderer(), (f32)(block->_rect->x() - getCurScrollPos()->x() - wa + hwBlock), (f32)(block->_rect->y() - getCurScrollPos()->y() + hhBlock), hwBlock, hhBlock, &colDeep, &colPale, &colPale, &colLight);

		// テキスト描画（影）
		rc->getRenderer()->setSolidColor((u8)(block->_color->r() * 0.1f), (u8)(block->_color->g() * 0.1f), (u8)(block->_color->b() * 0.1f), alpha);
		block->_strdraw->draw(rc, - (f32)getCurScrollPos()->x() + 1, - (f32)getCurScrollPos()->y() + 1);

		// テキスト描画
		if(mtnode->isDisabled())	// 無効
		{	
			rc->getRenderer()->setSolidColor((u8)(block->_color->r() * 127.0f), (u8)(block->_color->g() * 127.0f), (u8)(block->_color->b() * 127.0f), alpha);
		}
		else						// 有効
		{	
			rc->getRenderer()->setSolidColor((u8)(block->_color->r() * 255.0f), (u8)(block->_color->g() * 255.0f), (u8)(block->_color->b() * 255.0f), alpha);
		}
		block->_strdraw->draw(rc, - (f32)getCurScrollPos()->x(), - (f32)getCurScrollPos()->y());

		// チェック時
		if(mtnode->isChecked())
		{
			// "\x81\x9c" = "●"; 漢字コードは Shift-JIS
			ColorU8 colBk(*rc->getRenderer()->getSolidColor());
			rc->getRenderer()->setSolidColor(0, 0, 0, 255);
			EasyStringDrawer::draw(block->_strdraw->getFont(), "\x81\x9c", (f32)(block->_rect->x() - getCurScrollPos()->x() + 8), (f32)(block->_rect->y() - getCurScrollPos()->y() + 8 + 5), 10, rc);
			rc->getRenderer()->setSolidColor(&colBk);
			EasyStringDrawer::draw(block->_strdraw->getFont(), "\x81\x9c", (f32)(block->_rect->x() - getCurScrollPos()->x() + 8), (f32)(block->_rect->y() - getCurScrollPos()->y() + 8 + 4), 10, rc);
		}

		// PSNS ユーザー時
		if(mtnode->getGeneralParam())
		{
			rc->getRenderer()->bindTexture(_texTitleRef);
			RectF32 vtx, uv;
			rc->getRenderer()->setSolidColor(0, 0, 0, 255);
			RendererUtils::draw2dTextureRect(
				rc->getRenderer(),
				RectF32::setout(&vtx, (f32)(block->_rect->x() - getCurScrollPos()->x() - 30 + 1), (f32)(block->_rect->y() - getCurScrollPos()->y() + 9 + 1), 30, 14),
				Gcalc::calcTexUv(&uv, UV_PSNS_ON, 1.0f)	);
			rc->getRenderer()->setSolidColor(255, 255, 255, 255);
			RendererUtils::draw2dTextureRect(
				rc->getRenderer(),
				RectF32::setout(&vtx, (f32)(block->_rect->x() - getCurScrollPos()->x() - 30), (f32)(block->_rect->y() - getCurScrollPos()->y() + 9), 30, 14),
				Gcalc::calcTexUv(&uv, UV_PSNS_ON, 1.0f)	);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
