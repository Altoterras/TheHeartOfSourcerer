/***********************************************************************//**
 *	ItemListMenuWindow.cpp
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
#include "ItemListMenuWindow.h"

// Friends
#include "../../../../item/Item.h"
#include "../../../../item/ItemManager.h"

// External
#include "../../../../../tfw/lib/Rect.h"
#include "../../../../../tfw/gcmn/Gcalc.h"
#include "../../../../../tfw/gcmn/Renderer.h"
#include "../../../../../tfw/gcmn/RendererUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define XOFS_ICON					3
#define YOFS_ICON					4
#define W_ICON						24
#define H_ICON						24
#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ
#define UV_ITEM_ICON_GRASS_S		 976.0f / W_TEX,  0.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX
#define UV_ITEM_ICON_STONE_S		1000.0f / W_TEX,  0.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX
#define UV_ITEM_ICON_WEAPON_S		 928.0f / W_TEX, 24.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX
#define UV_ITEM_ICON_SHIELD_S		 952.0f / W_TEX, 24.0f / H_TEX, 24.0f / W_TEX, 24.0f / H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ItemListMenuWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	要素の描画
**//*---------------------------------------------------------------------*/
void ItemListMenuWindow::drawBlock(const RenderCtx* rc, const Block* block, bool isFocus)
{
	if(block->isHidden() || TFW_IS_FLAG(block->_typef, Block::TF_TITLE))	{	return;	}	// 隠し属性やタイトルは除外

	ListMenuWindowBase::drawBlock(rc, block, isFocus);

	// アイテムアイコン
	const Item* item = reinterpret_cast<const Item*>(block->_param);
	const ItemDef* itmdf = (item != 0L) ? item->getItemDef() : 0L;
	if((itmdf != 0L) && ((ItemDef::CAT_STONE <= itmdf->getCategory()) && (itmdf->getCategory() <= ItemDef::CAT_SHIELD)))
	{
		Renderer* rdr = rc->getRenderer();
		rdr->bindTexture(_texRef);	// テクスチャセット
		const RectF32 uvItemIcon[4] =
		{
			RectF32( UV_ITEM_ICON_STONE_S ),
			RectF32( UV_ITEM_ICON_GRASS_S ),
			RectF32( UV_ITEM_ICON_WEAPON_S ),
			RectF32( UV_ITEM_ICON_SHIELD_S ),
		};
		RectF32 rectIcon(		(f32)(block->_rect->x() + block->_rectOfsAnim->x() - getCurScrollPos()->x() + XOFS_ICON),
							(f32)(block->_rect->y() + block->_rectOfsAnim->y() - getCurScrollPos()->y() + YOFS_ICON),
							W_ICON,
							H_ICON	);
		RendererUtils::draw2dTextureRect(rdr, &rectIcon, &uvItemIcon[itmdf->getCategory() - 2]);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
