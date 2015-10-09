/***********************************************************************//**
 *	BitmapFontAscii.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "BitmapFontAscii.h"

// Friends
#include "../../../gcmn/Texture.h"
#include "../../../gcmn/Renderer.h"
#include "../../../gcmn/RendererUtils.h"
#include "../../../framemod/RenderCtx.h"
#include "../../../string/CStringBase.h"
#include "../../../txt/Txt.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//======================================================================
// アクセサ

/*---------------------------------------------------------------------*//**
	デフォルトの文字幅を得る
**//*---------------------------------------------------------------------*/
int BitmapFontAscii::getCharDefaultWidth() const
{
	return _finf->_wChar;
}

/*---------------------------------------------------------------------*//**
	デフォルトの文字高さを得る
**//*---------------------------------------------------------------------*/
int BitmapFontAscii::getCharDefaultHeight() const
{
	return _finf->_hChar;
}

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	文字幅を得る
**//*---------------------------------------------------------------------*/
f32 BitmapFontAscii::getCharWidth(const u32 c, s32 fontsize) const
{
	switch(c)
	{
	case Txt::NLCODE_CR:
	case Txt::NLCODE_LF:
	case Txt::NLCODE_CRLF:
	case '\0':
		return 0;
	case '\t':
		return ((fontsize != 0) ? fontsize : _finf->_wChar) * 4 * _rateHankaku;
	default:
		return ((fontsize != 0) ? fontsize : _finf->_wChar) * _rateHankaku;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BitmapFontAscii::BitmapFontAscii()
	: _rateHankaku(0.75f)
	, _texRef(0L)
	, _finf(0L)
	, _uBase(0.0f)
	, _vBase(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BitmapFontAscii::~BitmapFontAscii()
{
	ASSERT(_finf == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool BitmapFontAscii::create(Texture* texRef, s32 fontsize, f32 rateHankaku, f32 scale)
{
	_finf = new FontInfo();
	_texRef = texRef;
	_rateHankaku = rateHankaku;

	f32 ivscale = 1.0f / scale;
	_finf->_maskX	= 0xf;
	_finf->_shiftY	= 4;
	_finf->_wChar	= (u16)fontsize;
	_finf->_hChar	= (u16)fontsize;
	_finf->_uChar	= _finf->_wChar / (f32)_texRef->getWidth() * ivscale;
	_finf->_vChar	= _finf->_hChar / (f32)_texRef->getHeight() * ivscale;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void BitmapFontAscii::destroy()
{
	delete _finf;
	_finf = 0L;
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void BitmapFontAscii::draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const RenderCtx* rc) const
{
	switch(c)
	{
	case ' ':
		if(width != 0L)	{	*width = fontsize * _rateHankaku;		}
		break;
	case '\t':
		if(width != 0L)	{	*width = fontsize * _rateHankaku * 4;	}
		break;
	case '\0':
	case Txt::NLCODE_CR:
	case Txt::NLCODE_LF:
	case Txt::NLCODE_CRLF:
		if(width != 0L)	{	*width = 0.0f;							}
		break;
	default:
		{
			f32 r = fontsize / (f32)_finf->_wChar;
			RectF32 vtx;
			vtx.left() = x;
			vtx.top() = y;
			vtx.width() = _finf->_wChar * r * _rateHankaku;
			if(rc != 0L)
			{
				vtx.height() = _finf->_hChar * r;
				RectF32 tuv(
					_uBase + (f32)(c & _finf->_maskX) * _finf->_uChar,
					_vBase + (f32)(c >> _finf->_shiftY) * _finf->_vChar,
					_finf->_uChar,
					_finf->_vChar);
				rc->getRenderer()->bindTexture(_texRef);
				RendererUtils::draw2dTextureRect(rc->getRenderer(), &vtx, &tuv);
			}

			if(width != 0L)	{	*width = vtx.width();	}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画（色つき）
**//*---------------------------------------------------------------------*/
void BitmapFontAscii::draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const
{
	switch(c)
	{
	case ' ':
		if(width != 0L)	{	*width = fontsize * _rateHankaku;		}
		break;
	case '\t':
		if(width != 0L)	{	*width = fontsize * _rateHankaku * 4;	}
		break;
	case '\0':
	case Txt::NLCODE_CR:
	case Txt::NLCODE_LF:
	case Txt::NLCODE_CRLF:
		if(width != 0L)	{	*width = 0.0f;							}
		break;
	default:
		{
			f32 r = fontsize / (f32)_finf->_wChar;
			RectF32 vtx;
			vtx.left() = x;
			vtx.top() = y;
			vtx.width() = _finf->_wChar * r * _rateHankaku;
			if(rc != 0L)
			{
				vtx.height() = _finf->_hChar * r;
				RectF32 tuv(
					_uBase + (f32)(c & _finf->_maskX) * _finf->_uChar,
					_vBase + (f32)(c >> _finf->_shiftY) * _finf->_vChar,
					_finf->_uChar,
					_finf->_vChar);
				rc->getRenderer()->bindTexture(_texRef);
				RendererUtils::draw2dColorTextureRect(rc->getRenderer(), &vtx, &tuv, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom);
			}

			if(width != 0L)	{	*width = vtx.width();	}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	上下左右が欠けたフォントの 2D 描画
**//*---------------------------------------------------------------------*/
void BitmapFontAscii::drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc) const
{
	switch(c)
	{
	case ' ':
		if(width != 0L)	{	*width = (fontsize * _rateHankaku) - (xChipLeft + xChipRight);		}
		break;
	case '\t':
		if(width != 0L)	{	*width = (fontsize * _rateHankaku * 4) - (xChipLeft + xChipRight);	}
		break;
	case '\0':
	case Txt::NLCODE_CR:
	case Txt::NLCODE_LF:
	case Txt::NLCODE_CRLF:
		if(width != 0L)	{	*width = 0.0f;	}
		break;
	default:
		if((xChipLeft == 0.0f) && (yChipTop == 0.0f) && (xChipRight == 0.0f) && (yChipBottom == 0.0f))
		{
			draw(width, c, x, y, fontsize, rc);
		}
		else
		{
			f32 r = fontsize / (f32)_finf->_wChar;
			RectF32 vtx(
				x + xChipLeft,
				y + yChipTop, 
				_finf->_wChar * r * _rateHankaku,
				_finf->_hChar * r	);
			vtx.width() -= xChipLeft + xChipRight;
			if(vtx.width() > 0.0f)
			{
				vtx.h() -= yChipTop + yChipBottom;
				if(vtx.h() > 0.0f)
				{
					if(rc != 0L)
					{
						f32 rUpW = _finf->_uChar / (f32)fontsize;
						f32 rVpH = _finf->_vChar / (f32)fontsize;
						f32 tuChipLeft = xChipLeft * rUpW;
						f32 tvChipTop = yChipTop * rVpH;
						RectF32 uv(
							(f32)((c & _finf->_maskX) * _finf->_uChar) + tuChipLeft,
							(f32)((c >> _finf->_shiftY) * _finf->_vChar) + tvChipTop,
							_finf->_uChar - tuChipLeft - (xChipRight * rUpW),
							_finf->_vChar - tvChipTop - (yChipBottom * rVpH)	);
						rc->getRenderer()->bindTexture(_texRef);
						RendererUtils::draw2dTextureRect(rc->getRenderer(), &vtx, &uv);
					}
				}

				if(width != 0L)			{	*width = vtx.width();	}
			}
			else if(width != 0L)	{	*width = 0.0f;	}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	上下左右が欠けたフォントの 2D 描画（色つき）
**//*---------------------------------------------------------------------*/
void BitmapFontAscii::drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const
{
	switch(c)
	{
	case ' ':
		if(width != 0L)	{	*width = (fontsize * _rateHankaku) - (xChipLeft + xChipRight);		}
		break;
	case '\t':
		if(width != 0L)	{	*width = (fontsize * _rateHankaku * 4) - (xChipLeft + xChipRight);	}
		break;
	case '\0':
	case Txt::NLCODE_CR:
	case Txt::NLCODE_LF:
	case Txt::NLCODE_CRLF:
		if(width != 0L)	{	*width = 0.0f;	}
		break;
	default:
		if((xChipLeft == 0.0f) && (yChipTop == 0.0f) && (xChipRight == 0.0f) && (yChipBottom == 0.0f))
		{
			draw(width, c, x, y, fontsize, rc);
		}
		else
		{
			f32 r = fontsize / (f32)_finf->_wChar;
			RectF32 vtx(
				x + xChipLeft,
				y + yChipTop, 
				_finf->_wChar * r * _rateHankaku,
				_finf->_hChar * r	);
			vtx.width() -= xChipLeft + xChipRight;
			if(vtx.width() > 0.0f)
			{
				vtx.h() -= yChipTop + yChipBottom;
				if(vtx.h() > 0.0f)
				{
					if(rc != 0L)
					{
						f32 rUpW = _finf->_uChar / (f32)fontsize;
						f32 rVpH = _finf->_vChar / (f32)fontsize;
						f32 tuChipLeft = xChipLeft * rUpW;
						f32 tvChipTop = yChipTop * rVpH;
						RectF32 uv(
							(f32)((c & _finf->_maskX) * _finf->_uChar) + tuChipLeft,
							(f32)((c >> _finf->_shiftY) * _finf->_vChar) + tvChipTop,
							_finf->_uChar - tuChipLeft - (xChipRight * rUpW),
							_finf->_vChar - tvChipTop - (yChipBottom * rVpH)	);
						rc->getRenderer()->bindTexture(_texRef);
						RendererUtils::draw2dColorTextureRect(rc->getRenderer(), &vtx, &uv, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom);
					}
				}

				if(width != 0L)			{	*width = vtx.width();	}
			}
			else if(width != 0L)	{	*width = 0.0f;	}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画サブメソッド
**//*---------------------------------------------------------------------*/
void BitmapFontAscii::render(const u32 c, const Vector3F* pos, const Vector3F* widthChar, const Vector3F* heightChar, const RenderCtx* rc) const
{
	switch(c)
	{
	case ' ':
	case '\t':
	case '\0':
	case Txt::NLCODE_CR:
	case Txt::NLCODE_LF:
	case Txt::NLCODE_CRLF:
		break;
	default:
		{
			Vector3F width = *widthChar * _rateHankaku;
			if(rc != 0L)
			{
				f32 u = _uBase + (f32)(c & _finf->_maskX) * _finf->_uChar;
				f32 v = _vBase + (f32)(c >> _finf->_shiftY) * _finf->_vChar;
				rc->getRenderer()->bindTexture(_texRef);
				Vector3F vtx[4];
				vtx[0].set(pos->x(), pos->y(), pos->z());
				vtx[1].set(pos->x() + heightChar->x(), pos->y() + heightChar->y(), pos->z() + heightChar->z());
				vtx[2].set(pos->x() + width.x(), pos->y() + width.y(), pos->z() + width.z());
				vtx[3].set(pos->x() + width.x() + heightChar->x(), pos->y() + width.y() + heightChar->y(), pos->z() + width.z() + heightChar->z());
				Vector2F uv[4];
				uv[0].set(u, - v);
				uv[1].set(u, - v - _finf->_vChar);
				uv[2].set(u + _finf->_uChar, - v);
				uv[3].set(u + _finf->_uChar, - v - _finf->_vChar);
				RendererUtils::render3dTriangles(rc->getRenderer(), vtx, 0L, uv, 0L, 4, RendererUtils::TRIANGLE_STRIP);
			}
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
