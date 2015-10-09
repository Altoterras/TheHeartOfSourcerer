/***********************************************************************//**
 *	BitmapFontMultiByte.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/09/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "BitmapFontMultiByte.h"

// Friends
#include "../FontCharDefData.h"
#include "../ProportionalFontData.h"
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

//==========================================================================
// BitmapFontMultiByte メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	デフォルトの文字幅を得る
**//*---------------------------------------------------------------------*/
int BitmapFontMultiByte::getCharDefaultWidth() const
{
	return _finf->_wChar;
}

/*---------------------------------------------------------------------*//**
	デフォルトの文字高さを得る
**//*---------------------------------------------------------------------*/
int BitmapFontMultiByte::getCharDefaultHeight() const
{
	return _finf->_hChar;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	文字の文字幅を得る
**//*---------------------------------------------------------------------*/
f32 BitmapFontMultiByte::getCharWidth(const u32 c, s32 fontsize) const
{
	if(c <= 127)	// ASCII
	{
		switch(c)
		{
		case Txt::NLCODE_CR:
		case Txt::NLCODE_LF:
		case Txt::NLCODE_CRLF:
			return 0;
		case '\t':
			return ((fontsize != 0) ? fontsize : _finf->_wChar) * 4 * _rateHankaku;
		default:
			if((_pfdatRef != 0L) && isProportinalChar(c))
			{
				f32 r = (fontsize != 0) ? (fontsize / (f32)_finf->_wChar) : 1.0f;
				const ProportionalFontData::ProportionalCharInfo* pci = &(_pfdatRef->_pci[c - 0x20]);
				return (f32)(pci->_xEnd - pci->_xStart) * r * _finf->_scale;
			}
			else
			{
				return ((fontsize != 0) ? fontsize : _finf->_wChar) * _rateHankaku;
			}
		}
	}
	else
	{
		return (f32)((fontsize != 0) ? fontsize : _finf->_wChar);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BitmapFontMultiByte::BitmapFontMultiByte()
	: _rateHankaku(0.5f)
	, _texRef(0L)
	, _fcddRef(0L)
	, _pfdatRef(0L)
	, _finf(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BitmapFontMultiByte::~BitmapFontMultiByte()
{
	ASSERT(_finf == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool BitmapFontMultiByte::create(Texture* texRef, const ProportionalFontData* pfdatRef, const FontCharDefData* fcddRef, s32 fontsize, f32 rateHankaku, f32 scale)
{
	_finf = new FontInfo();
	_texRef = texRef;
	_fcddRef = fcddRef;
	_pfdatRef = pfdatRef;
	_rateHankaku = rateHankaku;

	f32 ivscale = 1.0f / scale;
	_finf->_tu1px	= 1.0f / (f32)_texRef->getWidth();
	_finf->_tv1px	= 1.0f / (f32)_texRef->getHeight();
	_finf->_maskX	= 0x3f;
	_finf->_shiftY	= 6;
	_finf->_wChar	= (u16)fontsize;
	_finf->_hChar	= (u16)fontsize;
	_finf->_tuChar	= _finf->_wChar * _finf->_tu1px * ivscale;
	_finf->_tvChar	= _finf->_hChar * _finf->_tv1px * ivscale;
	_finf->_twChar	= _finf->_tuChar - _finf->_tu1px;
	_finf->_thChar	= _finf->_tvChar - _finf->_tv1px;
	_finf->_scale	= scale;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void BitmapFontMultiByte::destroy()
{
	delete _finf;
	_finf = 0L;
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void BitmapFontMultiByte::draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const RenderCtx* rc) const
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
			u32 ci = _fcddRef->getCharIndex(c);
			f32 r = fontsize / (f32)_finf->_wChar;
			RectF32 vtx;
			RectF32 tuv;
			vtx.left() = x;
			vtx.top() = y;
			vtx.height() = _finf->_hChar * r;
			tuv.left() = (f32)((ci & _finf->_maskX) * _finf->_tuChar) + _finf->_tu1px;
			vtx.width(), tuv.width();
			if((c <= 127) && (_pfdatRef != 0L) && isProportinalChar(c))
			{
				const ProportionalFontData::ProportionalCharInfo* pci = &(_pfdatRef->_pci[c - 0x20]);
				tuv.width() = (f32)(pci->_xEnd - pci->_xStart);
				vtx.width() = tuv.width() * r * _finf->_scale;
				tuv.left() = tuv.left() + ((f32)pci->_xStart * _finf->_tu1px);
				tuv.width() = tuv.width() * _finf->_tu1px;
			}
			else
			{
				vtx.width() = _finf->_wChar * r;
				tuv.width() = _finf->_twChar;
			}

			if(rc != 0L)
			{
				tuv.top() = (f32)((ci >> _finf->_shiftY) * _finf->_tvChar) + _finf->_tv1px;
				tuv.height() = _finf->_thChar;
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
void BitmapFontMultiByte::draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const
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
			u32 ci = _fcddRef->getCharIndex(c);
			f32 r = fontsize / (f32)_finf->_wChar;
			RectF32 vtx;
			RectF32 tuv;
			vtx.left() = x;
			vtx.top() = y;
			vtx.height() = _finf->_hChar * r;
			tuv.left() = (f32)((ci & _finf->_maskX) * _finf->_tuChar) + _finf->_tu1px;
			vtx.width(), tuv.width();
			if((c <= 127) && (_pfdatRef != 0L) && isProportinalChar(c))
			{
				const ProportionalFontData::ProportionalCharInfo* pci = &(_pfdatRef->_pci[c - 0x20]);
				tuv.width() = (f32)(pci->_xEnd - pci->_xStart);
				vtx.width() = tuv.width() * r * _finf->_scale;
				tuv.left() = tuv.left() + ((f32)pci->_xStart * _finf->_tu1px);
				tuv.width() = tuv.width() * _finf->_tu1px;
			}
			else
			{
				vtx.width() = _finf->_wChar * r;
				tuv.width() = _finf->_twChar;
			}

			if(rc != 0L)
			{
				tuv.top() = (f32)((ci >> _finf->_shiftY) * _finf->_tvChar) + _finf->_tv1px;
				tuv.height() = _finf->_thChar;
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
void BitmapFontMultiByte::drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc) const
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
			f32 vh = (_finf->_hChar * r) - (yChipTop + yChipBottom);
			if(vh > 0.0f)
			{
				u32 ci = _fcddRef->getCharIndex(c);
				f32 rUpW = _finf->_tuChar / (f32)fontsize;
				f32 rVpH = _finf->_tvChar / (f32)fontsize;
				f32 tuChipLeft = xChipLeft * rUpW;
				f32 tvChipTop = yChipTop * rVpH;
				f32 tw, vw;
				f32 tu = (f32)((ci & _finf->_maskX) * _finf->_tuChar) + _finf->_tu1px + tuChipLeft;
				if((c <= 127) && (_pfdatRef != 0L) && isProportinalChar(c))
				{
					const ProportionalFontData::ProportionalCharInfo* pci = &(_pfdatRef->_pci[c - 0x20]);
					tw = (f32)(pci->_xEnd - pci->_xStart);
					vw = tw * r * _finf->_scale - (xChipLeft + xChipRight);
					tu = tu + ((f32)pci->_xStart * _finf->_tu1px);
					tw = (tw * _finf->_tu1px) - tuChipLeft - (xChipRight * rUpW);
				}
				else
				{
					vw = (_finf->_wChar * r) - (xChipLeft + xChipRight);
					tw = _finf->_twChar - tuChipLeft - (xChipRight * rUpW);
				}

				if(vw > 0.0f)
				{
					if(rc != 0L)
					{
						f32 tv = (f32)((ci >> _finf->_shiftY) * _finf->_tvChar) + _finf->_tv1px + tvChipTop;
						f32 th = _finf->_thChar - tvChipTop - (yChipBottom * rVpH);
						rc->getRenderer()->bindTexture(_texRef);
						RectF32 vtx(x + xChipLeft, y + yChipTop, vw, vh);
						RectF32 uv(tu, tv, tw, th);
						RendererUtils::draw2dTextureRect(rc->getRenderer(), &vtx, &uv);
					}

					if(width != 0L)			{	*width = vw;	}
				}
				else if(width != 0L)	{	*width = 0.0f;	}
			}
			else if(width != 0L)	{	*width = 0.0f;	}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	上下左右が欠けたフォントの 2D 描画（色つき）
**//*---------------------------------------------------------------------*/
void BitmapFontMultiByte::drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const
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
			draw(width, c, x, y, fontsize, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom, rc);
		}
		else
		{
			f32 r = fontsize / (f32)_finf->_wChar;
			f32 vh = (_finf->_hChar * r) - (yChipTop + yChipBottom);
			if(vh > 0.0f)
			{
				u32 ci = _fcddRef->getCharIndex(c);
				f32 rUpW = _finf->_tuChar / (f32)fontsize;
				f32 rVpH = _finf->_tvChar / (f32)fontsize;
				f32 tuChipLeft = xChipLeft * rUpW;
				f32 tvChipTop = yChipTop * rVpH;
				f32 tw, vw;
				f32 tu = (f32)((ci & _finf->_maskX) * _finf->_tuChar) + _finf->_tu1px + tuChipLeft;
				if((c <= 127) && (_pfdatRef != 0L) && isProportinalChar(c))
				{
					const ProportionalFontData::ProportionalCharInfo* pci = &(_pfdatRef->_pci[c - 0x20]);
					tw = (f32)(pci->_xEnd - pci->_xStart);
					vw = tw * r * _finf->_scale - (xChipLeft + xChipRight);
					tu = tu + ((f32)pci->_xStart * _finf->_tu1px);
					tw = (tw * _finf->_tu1px) - tuChipLeft - (xChipRight * rUpW);
				}
				else
				{
					vw = (_finf->_wChar * r) - (xChipLeft + xChipRight);
					tw = _finf->_twChar - tuChipLeft - (xChipRight * rUpW);
				}

				if(vw > 0.0f)
				{
					if(rc != 0L)
					{
						f32 tv = (f32)((ci >> _finf->_shiftY) * _finf->_tvChar) + _finf->_tv1px + tvChipTop;
						f32 th = _finf->_thChar - tvChipTop - (yChipBottom * rVpH);
						rc->getRenderer()->bindTexture(_texRef);
						RectF32 vtx(x + xChipLeft, y + yChipTop, vw, vh);
						RectF32 uv(tu, tv, tw, th);
						RendererUtils::draw2dColorTextureRect(rc->getRenderer(), &vtx, &uv, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom);
					}

					if(width != 0L)			{	*width = vw;	}
				}
				else if(width != 0L)	{	*width = 0.0f;	}
			}
			else if(width != 0L)	{	*width = 0.0f;	}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画サブメソッド
**//*---------------------------------------------------------------------*/
void BitmapFontMultiByte::render(const u32 c, const Vector3F* pos, const Vector3F* widthChar, const Vector3F* heightChar, const RenderCtx* rc) const
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
			u32 ci = _fcddRef->getCharIndex(c);
			f32 tu = (f32)((ci & _finf->_maskX) * _finf->_tuChar) + _finf->_tu1px;
			f32 tv = (f32)((ci >> _finf->_shiftY) * _finf->_tvChar) + _finf->_tv1px;
			f32 tw = _finf->_twChar;
			f32 th = _finf->_thChar;
			Vector3F width(widthChar);

			if(c <= 127)	// ASCII
			{
				width *= _rateHankaku;
				tu *= 0.5f;
				tw *= 0.5f;
			}

			if(rc != 0L)
			{
				rc->getRenderer()->bindTexture(_texRef);
				Vector3F vtx[4];
				vtx[0].set(pos->x(), pos->y(), pos->z());
				vtx[1].set(pos->x() + heightChar->x(), pos->y() + heightChar->y(), pos->z() + heightChar->z());
				vtx[2].set(pos->x() + width.x(), pos->y() + width.y(), pos->z() + width.z());
				vtx[3].set(pos->x() + width.x() + heightChar->x(), pos->y() + width.y() + heightChar->y(), pos->z() + width.z() + heightChar->z());
				Vector2F tex[4];
				tex[0].set(tu, tv);
				tex[1].set(tu, tv - th);
				tex[2].set(tu + tw, tv);
				tex[3].set(tu + tw, tv - th);
				RendererUtils::render3dTriangles(rc->getRenderer(), vtx, 0L, tex, 0L, 4, RendererUtils::TRIANGLE_STRIP);
			}
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
