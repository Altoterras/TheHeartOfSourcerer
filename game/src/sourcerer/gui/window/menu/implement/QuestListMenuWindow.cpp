/***********************************************************************//**
 *	QuestListMenuWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QuestListMenuWindow.h"

// Friends
#include "../../../../body/Game.h"
#include "../../../../gui/GameGui.h"
#include "../../../../quest/QuestArticle.h"

// External
#include "../../../../../tfw/lib/Rect.h"
#include "../../../../../tfw/gcmn/Renderer.h"
#include "../../../../../tfw/gcmn/RendererUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define W_TEX						(1024.0f)					// テクスチャ全体の幅
#define H_TEX						(1024.0f)					// テクスチャ全体の高さ

#define X_MBLOCK					(208)
#define XOFS_ICON					3
#define YOFS_ICON					4
#define W_ICON						24
#define H_ICON						24
#define UV_QSTSTAT_ICON_ACCEPTED	1000.0f / W_TEX, 320.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX
#define UV_QSTSTAT_ICON_ACHVED		1000.0f / W_TEX, 392.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX
#define UV_QSTSTAT_ICON_CANCELED	1000.0f / W_TEX, 368.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX
#define UV_QSTSTAT_ICON_FINISHED	1000.0f / W_TEX, 344.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QuestListMenuWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	要素の矩形を作成
**//*---------------------------------------------------------------------*/
void QuestListMenuWindow::makeBlockRect(RectF32* rectBlock, RectF32* rectText, const Block* block, f32 xBase, f32 yBase, const SizeF32* sizeTxt, const SizeF32* sizeAllTxtMax, s32 depth)
{
	makeBlockRectDefault(rectBlock, rectText, block, X_MBLOCK, yBase, sizeTxt, sizeAllTxtMax, depth);
}

/*---------------------------------------------------------------------*//**
	要素の描画
**//*---------------------------------------------------------------------*/
void QuestListMenuWindow::drawBlock(const RenderCtx* rc, const Block* block, bool isFocus)
{
	if(block->isHidden() || TFW_IS_FLAG(block->_typef, Block::TF_TITLE))	{	return;	}	// 隠し属性やタイトルは除外

	ListMenuWindowBase::drawBlock(rc, block, isFocus);

	// アイコン描画
	Renderer* rdr = rc->getRenderer();
	const QuestArticle* qsta = reinterpret_cast<const QuestArticle*>(block->_param);
	RectF32 rectIcon(		(f32)(block->_rect->x() + block->_rectOfsAnim->x() - getCurScrollPos()->x() + XOFS_ICON),
						(f32)(block->_rect->y() + block->_rectOfsAnim->y() - getCurScrollPos()->y() + YOFS_ICON),
						W_ICON,
						H_ICON	);
	rdr->bindTexture(_texRef);	// テクスチャセット
	RectF32 uv;
	if(qsta->getStat() == QuestArticle::STAT_ACCEPT)
	{
		RendererUtils::draw2dTextureRect(rdr, &rectIcon, RectF32::setout(&uv, UV_QSTSTAT_ICON_ACCEPTED));
	}
	else if(qsta->getStat() == QuestArticle::STAT_ACHVED)
	{
		RendererUtils::draw2dTextureRect(rdr, &rectIcon, RectF32::setout(&uv, UV_QSTSTAT_ICON_ACHVED));
	}
	else if(TFW_IS_FLAG(qsta->getFlags(), QuestArticle::F_FINISHED))
	{
		RendererUtils::draw2dTextureRect(rdr, &rectIcon, RectF32::setout(&uv, UV_QSTSTAT_ICON_FINISHED));
	}
	else
	{
		RendererUtils::draw2dTextureRect(rdr, &rectIcon, RectF32::setout(&uv, UV_QSTSTAT_ICON_CANCELED));
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
