/***********************************************************************//**
 *	TiledRectDraw.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2010/09/05.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TiledRectDraw.h"

// Friends
#include "../framemod/RenderCtx.h"
#include "../gcmn/Texture.h"
#include "../gcmn/Renderer.h"
#include "../gcmn/RendererUtils.h"
#include "../lib/Rect.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TiledRectDraw メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	テクスチャを設定する
**//*---------------------------------------------------------------------*/
void TiledRectDraw::setTexture(Texture* texRef)
{
	_texRef = texRef;
}

/*---------------------------------------------------------------------*//**
	タイル情報を設定する
	※	左右、上下タイルは必ずしも存在しないければならないわけではないが、
		存在する場合は、左右対称、上下対称でなければならない

	@param rot 表示回転フラグ TFW_DISPF_RANG_*
**//*---------------------------------------------------------------------*/
void TiledRectDraw::setTile(TiledRectDraw::TilePlace tp, const RectF32* uvwh, f32 wDraw, f32 hDraw, u32 rot)
{
	if(_ti[tp] == 0L)
	{
		_ti[tp] = new TileInfo();
	}
	_ti[tp]->_uvwh->copy(uvwh);
	_ti[tp]->_size->set(wDraw, hDraw);
	_ti[tp]->_rot = rot;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TiledRectDraw::TiledRectDraw()
	: _texRef(0L)
{
	for(int i = 0; i < NUM_TP; i++)
	{
		_ti[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TiledRectDraw::~TiledRectDraw()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_TP; i++)
		{
			ASSERT(_ti[i] == 0L);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool TiledRectDraw::create()
{
	return create(0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool TiledRectDraw::create(Texture* texRef)
{
	_texRef = texRef;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void TiledRectDraw::destroy()
{
	for(int i = 0; i < NUM_TP; i++)
	{
		delete _ti[i];
		_ti[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void TiledRectDraw::draw(const RenderCtx* rc, const RectF32* rect)
{
	draw(rc, rect->x(), rect->y(), rect->w(), rect->h());
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void TiledRectDraw::draw(const RenderCtx* rc, f32 xRect, f32 yRect, f32 wRect, f32 hRect)
{
	Renderer* rdr = rc->getRenderer();
	rdr->bindTexture(_texRef);

	// 枠の部分で使う、幅と高さ（それぞれ中央で判定）
	f32 wFrame =
		( (_ti[TP_LM] != 0L) ? _ti[TP_LM]->_size->w() : 0L ) +
		( (_ti[TP_RM] != 0L) ? _ti[TP_RM]->_size->w() : 0L );
	f32 hFrame = 
		( (_ti[TP_CT] != 0L) ? _ti[TP_CT]->_size->h() : 0L ) +
		( (_ti[TP_CB] != 0L) ? _ti[TP_CB]->_size->h() : 0L );
	// 縦横のスケール
	f32 xscale = (wFrame > wRect) ? (wRect / wFrame) : 1.0f;
	f32 yscale = (hFrame > hRect) ? (hRect / hFrame) : 1.0f;

	f32 x = xRect;
	f32 y = yRect;
	f32 hRest = hRect;
	f32 h;

	// 上の行
	h = drawLine(rdr, &_ti[TP_LT], x, y, xscale, yscale, xRect, wRect);
	hRest -= h;
	y += h;

	// 下の行
	h = drawLine(rdr, &_ti[TP_LB], x, yRect + hRect - (hFrame * yscale * 0.5f), xscale, yscale, xRect, wRect);
	hRest -= h;

	// 中央の行
	{
		if(_ti[TP_CM] != 0L)
		{
			h = _ti[TP_CM]->_size->h() * yscale;
		}

		// ループ部
		while((h < hRest) && (h > 0.0f))
		{
			h = drawLine(rdr, &_ti[TP_LM], x, y, xscale, yscale, xRect, wRect);
			y += h;
			hRest -= h;
		}

		// 縮小表示
		if((hRest > 0.0f) && (h > 0.0f))
		{
			yscale = hRest / h;
			h = drawLine(rdr, &_ti[TP_LM], x, y, xscale, yscale, xRect, wRect);
		}
	}
}

/*---------------------------------------------------------------------*//**
	行描画
**//*---------------------------------------------------------------------*/
f32 TiledRectDraw::drawLine(Renderer* rdr, TileInfo** tiLineArray, f32 x, f32 y, f32 xscale, f32 yscale, f32 xRect, f32 wRect)
{
	f32 w;
	f32 h = 0.0f;
	f32 wRest = wRect;

	// 左
	const TileInfo* tiWk = *(tiLineArray + 0);
	if(tiWk != 0)
	{
		w = tiWk->_size->w() * xscale;
		h = tiWk->_size->h() * yscale;
		RendererUtils::draw2dTextureRect(rdr, x, y, w, h, tiWk->_uvwh->x(), tiWk->_uvwh->y(), tiWk->_uvwh->w(), tiWk->_uvwh->h(), tiWk->_rot);
		x += w;
		wRest -= w;
	}

	// 右
	tiWk = *(tiLineArray + 2);
	if(tiWk != 0)
	{
		w = tiWk->_size->w() * xscale;
		h = tiWk->_size->h() * yscale;
		RendererUtils::draw2dTextureRect(rdr, xRect + wRect - w, y, w, h, tiWk->_uvwh->x(), tiWk->_uvwh->y(), tiWk->_uvwh->w(), tiWk->_uvwh->h(), tiWk->_rot);
		wRest -= w;
	}

	// 中央
	tiWk = *(tiLineArray + 1);
	if(tiWk != 0)
	{
		w = tiWk->_size->w();
		h = tiWk->_size->h() * yscale;

		// ループ部
		while(w <= wRest)
		{
			RendererUtils::draw2dTextureRect(rdr, x, y, w, h, tiWk->_uvwh->x(), tiWk->_uvwh->y(), tiWk->_uvwh->w(), tiWk->_uvwh->h(), tiWk->_rot);
			x += w;
			wRest -= w;
		}

		// 縮小表示
		if(wRest > 0.0f)
		{
			RendererUtils::draw2dTextureRect(rdr, x, y, wRest, h, tiWk->_uvwh->x(), tiWk->_uvwh->y(), tiWk->_uvwh->w(),tiWk->_uvwh->h(), tiWk->_rot);
		}
	}

	return h;
}

//==========================================================================
// TiledRectDraw::TileInfo メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TiledRectDraw::TileInfo::TileInfo()
	: _uvwh(new RectF32)
	, _size(new SizeF32)
	, _rot(TFW_DISPF_RANG_0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TiledRectDraw::TileInfo::~TileInfo()
{
	delete _uvwh;
	delete _size;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
