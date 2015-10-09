/***********************************************************************//**
 *	SourceListMenuWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/10.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SourceListMenuWindow.h"

// Friends
#include "../../implement/SourceListWindow.h"
#include "../../../menu/GameMenuCreateParam.h"
#include "../../../../body/Game.h"
#include "../../../../gui/GameGui.h"
#include "../../../../source/Source.h"

// External
#include "../../../../../tfw/lib/Rect.h"
#include "../../../../../tfw/g2d/StringDrawer.h"
#include "../../../../../tfw/gcmn/Gcalc.h"
#include "../../../../../tfw/gcmn/Renderer.h"
#include "../../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../../tfw/txt/TxtUtils.h"
#include "../../../../../tfw/txt/implement/SjisTxt.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define XOFS_ICON					2
#define YOFS_ICON					4
#define W_ICON						24
#define H_ICON						24
#define W_TEX						(1024.0f)					// テクスチャ全体の幅
#define H_TEX						(1024.0f)					// テクスチャ全体の高さ
#define UV_LOCK_ICON				928, 0, 24, 24

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SourceListMenuWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	フレーム処理 - ダブルタッチ後の応答
**//*---------------------------------------------------------------------*/
void SourceListMenuWindow::onDoubleTapRelease(MenuExecRes* cres, TouchPanel* ui, const PointF32* ptTouch, Block* block)
{
	if(block == 0L)							{	return;	}
	Source* source = reinterpret_cast<Source*>(block->_param);
	if(source == 0L)						{	return;	}
	((SourceListWindow*)_gmcpRef->_owner)->runSourceEditor(source, (source->getFixedSourceDef() != 0L));
}

/*---------------------------------------------------------------------*//**
	要素の描画
**//*---------------------------------------------------------------------*/
void SourceListMenuWindow::drawBlock(const RenderCtx* rc, const Block* block, bool isFocus)
{
	if(block->isHidden() || TFW_IS_FLAG(block->_typef, Block::TF_TITLE))	{	return;	}	// 隠し属性やタイトルは除外

	ListMenuWindowBase::drawBlock(rc, block, isFocus);

	// ロックアイコン
	const Source* source = reinterpret_cast<const Source*>(block->_param);
	Renderer* rdr = rc->getRenderer();
	rdr->bindTexture(_texRef);	// テクスチャセット
	if(source->getFixedSourceDef() != 0L)
	{
		RectF32 rectIcon(	(f32)(block->_rect->x() + block->_rectOfsAnim->x() - getCurScrollPos()->x() + XOFS_ICON),
							(f32)(block->_rect->y() + block->_rectOfsAnim->y() - getCurScrollPos()->y() + YOFS_ICON),
							W_ICON,
							H_ICON	);
		RectF32 uv;
		RendererUtils::draw2dTextureRect(rdr, &rectIcon, Gcalc::calcTexUv(&uv, UV_LOCK_ICON, W_TEX, H_TEX, Env_get2drRate()));
	}
}

/*---------------------------------------------------------------------*//**
	文字列を調整する
	
	@retval true 文字列を修正した
	@retval false 文字列を修正しなかった
**//*---------------------------------------------------------------------*/
bool SourceListMenuWindow::adjustBlockString(VcString* str, RectF32* rectText, StringDrawer* strdraw, const RectF32* rectBlock)
{
	const f32 X_PAD = 8;
	const f32 Y_PAD = 4;
	f32 wBlock = rectBlock->width() - X_PAD - X_PAD;
	f32 hBlock = rectBlock->height() - Y_PAD - Y_PAD;
	if(rectText->width() > wBlock)
	{
		// 初期制限文字数を設定
		u32 limit = (s32)(str->getLength() * wBlock / rectText->width());
		if(rectText->height() >= hBlock)	{ limit = (s32)(limit * hBlock / rectText->height()); }
		limit -= 3;	// "..." 分

		// 収まるまでループする
		for(int safecnt = 0; safecnt < 20; safecnt++)
		{
			VcString strWk = *str;

			// 文字列を短くする
			SjisTxt txt;
			txt.setSingleString(&strWk);
			if(TxtUtils::shortenByRawCharCount(&txt, limit))	// 長さ制限
			{
				strWk.copy(txt.getRawString());
				strWk.add("...");
			}

			// 描画シミュレーションを行って実際のサイズを求める
			SizeF32 sizeTxt;
			strdraw->drawTest(&sizeTxt, &strWk);

			// 成功
			if(sizeTxt.width() < wBlock)
			{
				rectText->set(rectBlock->x() + ((rectBlock->w() - sizeTxt.w()) / 2), rectBlock->y() + ((rectBlock->h() - sizeTxt.h()) / 2), sizeTxt.w(), sizeTxt.h());	// テキストの矩形を設定
				*str = strWk;
				return true;
			}

			// 初期制限文字数を厳しくしてリトライ
			if(sizeTxt.height() >= hBlock)
			{
				limit = (s32)(limit * hBlock / sizeTxt.height());
			}
			else
			{
				limit--;
			}
		}

	}

	return false;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
