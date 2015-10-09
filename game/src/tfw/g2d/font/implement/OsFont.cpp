/***********************************************************************//**
 *	OsFont.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2011/12/08.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "OsFont.h"

// Friends
#include "../../../gcmn/Renderer.h"
#include "../../../gcmn/RendererUtils.h"
#include "../../../framemod/RenderCtx.h"
#include "../../../string/CStringBase.h"
#include "../../../txt/Txt.h"
#include "../../../txt/implement/UnicodeTxt.h"
#include "../../../txt/implement/Utf8Txt.h"

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
int OsFont::getCharDefaultWidth() const
{
	return _fontsize;
}

/*---------------------------------------------------------------------*//**
	デフォルトの文字高さを得る
**//*---------------------------------------------------------------------*/
int OsFont::getCharDefaultHeight() const
{
	return _fontsize;
}

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	文字幅を得る
**//*---------------------------------------------------------------------*/
f32 OsFont::getCharWidth(const u32 c, s32 fontsize) const
{
	switch(c)
	{
	case ' ':
		return (f32)fontsize;
	case '\t':
		return fontsize * 4 * _rateHankaku;
	case Txt::NLCODE_CR:
	case Txt::NLCODE_LF:
	case Txt::NLCODE_CRLF:
	case '\0':
		return 0;
	default:
		#if defined(_WINDOWS)
		{
			TCHAR cstr[3];
			int nc;
			SIZE size;
			if(c <= 0xff)
			{
				cstr[0] = (TCHAR)c;
				cstr[1] = '\0';
				nc = 1;
			}
			else if(c <= 0xffff)
			{
				cstr[0] = (TCHAR)((c >> 8) & 0xff);
				cstr[1] = (TCHAR)(c & 0xff);
				cstr[2] = '\0';
				nc = 2;
			}
			HFONT hfontBk = (HFONT)::SelectObject(_hdc, _hfont);
			::GetTextExtentPoint32(_hdc, cstr, nc, &size);
			::SelectObject(_hdc, hfontBk);
			return size.cx * (fontsize / (f32)_fontsize);
		}
		#else
			return (f32)_fontsize;
		#endif
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
OsFont::OsFont()
	: _fontsize(0)
	, _rateHankaku(0.5f)
#if defined(_WINDOWS)
	, _hbmp(0L)
	, _hfont(0L)
	, _hdc(0L)
	, _bitsBmp(0L)
#endif
{
	#if defined(_WINDOWS)
		TFW_ZERO_MEMORY(&_bmi, sizeof(BITMAPINFO));
		TFW_ZERO_MEMORY(&_rectBmp, sizeof(RECT));
	#endif
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
OsFont::~OsFont()
{
	#if defined(_WINDOWS)
		ASSERT(_hbmp == 0L);
		ASSERT(_hfont == 0L);
		ASSERT(_hdc == 0L);
		ASSERT(_bitsBmp == 0L);
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool OsFont::create(CStringBase* nameFont, s32 fontsize, f32 rateHankaku)
{
	_fontsize = fontsize;
	_rateHankaku = rateHankaku;

	#if defined(_WINDOWS)
	{
		// デバイスコンテキストを作成する
		_hdc	= ::CreateCompatibleDC(NULL);
		if(_hdc == 0L)	{	goto FAILED;	}

		// フォントを作成する
		s32 widthFont = _fontsize;
		s32 heightFont = _fontsize;
		LOGFONT lf;
		TFW_ZERO_MEMORY(&lf, sizeof(LOGFONT));
		lf.lfHeight					= - heightFont;		///- ::MulDiv(heightFont, ::GetDeviceCaps(_hdc, LOGPIXELSY), 72);
		lf.lfWidth					= 0;				///widthFont;
		lf.lfEscapement				= 0;
		lf.lfOrientation			= 0;
		lf.lfWeight					= FW_NORMAL;
		lf.lfItalic					= FALSE;
		lf.lfUnderline				= FALSE;
		lf.lfStrikeOut				= 0;
		lf.lfCharSet				= DEFAULT_CHARSET;	// SHIFTJIS_CHARSET
		lf.lfOutPrecision			= OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision			= CLIP_DEFAULT_PRECIS;
		lf.lfQuality				= ANTIALIASED_QUALITY;
		#if defined(UNICODE)
		{
			Utf8Txt utf8txt;
			UnicodeTxt unitxt;
			utf8txt.setSingleString(&VcString(nameFont));
			unitxt.copy(&utf8txt);
			_tcscpy(lf.lfFaceName, unitxt.getRawWideString()->getRaw());
		}
		#else
			::strcpy(lf.lfFaceName, nameFont->getRaw());
		#endif
		_hfont	 = ::CreateFontIndirect(&lf);
		if(_hfont == NULL)	{	goto FAILED;	}

		// ビットマップを作成する
		_rectBmp.left = 0;
		_rectBmp.top = 0;
		_rectBmp.right = fontsize;
		_rectBmp.bottom = fontsize;
		TFW_ZERO_MEMORY(&_bmi, sizeof(BITMAPINFO));
		_bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		_bmi.bmiHeader.biWidth			= (_rectBmp.right - _rectBmp.left);
		_bmi.bmiHeader.biHeight			= (_rectBmp.bottom - _rectBmp.top);
		_bmi.bmiHeader.biPlanes			= 1;
		_bmi.bmiHeader.biCompression	= BI_RGB;
		_bmi.bmiHeader.biBitCount		= 32;
		_bmi.bmiHeader.biSizeImage		= _bmi.bmiHeader.biWidth * _bmi.bmiHeader.biHeight * (_bmi.bmiHeader.biBitCount / 8);
		_hbmp	= ::CreateDIBSection(_hdc, &_bmi, DIB_RGB_COLORS, (VOID**)&_bitsBmp, NULL, 0);
		if(_hbmp == NULL)	{	goto FAILED;	}

		return true;

	FAILED:
		destroy();

		return false;
	}
	#else
	{
		return false;
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void OsFont::destroy()
{
	#if defined(_WINDOWS)
	{
		// 作成した GDI オブジェクトを破棄
		if(_hbmp != 0L)		{	::DeleteObject(_hbmp);		}
		_hbmp = 0L;
		if(_hfont != 0L)	{	::DeleteObject(_hfont);		}
		_hfont = 0L;
		if(_hdc != 0L)		{	::DeleteDC(_hdc);			}
		_hdc = 0L;
		_bitsBmp = 0L;
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void OsFont::draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const RenderCtx* rc) const
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
		#if defined(_WINDOWS)
		{
			f32 w = 0.0f;
			f32 h = 0.0f;
			TCHAR cstr[3];
			drawCalcChar(cstr, &w, &h, c, fontsize);
			if(rc != 0L)
			{
				rc->getRenderer()->setEnableTexture(true);
				u32 glidTex = drawPrepareCharTex(cstr);
				RendererUtils::draw2dTextureRect(rc->getRenderer(), x, y, w, h, 0.0f, 0.0f, 1.0f, 1.0f);
				drawCleanupCharTex(glidTex);
			}

			if(width != 0L)	{	*width = w;	}
		}
		#endif
		break;
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画（色つき）
**//*---------------------------------------------------------------------*/
void OsFont::draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const
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
		#if defined(_WINDOWS)
		{
			f32 w = 0.0f;
			f32 h = 0.0f;
			TCHAR cstr[3];
			drawCalcChar(cstr, &w, &h, c, fontsize);
			if(rc != 0L)
			{
				rc->getRenderer()->setEnableTexture(true);
				u32 glidTex = drawPrepareCharTex(cstr);
				RectF32 vtx(x, y, w, h);
				RectF32 uv(0.0f, 0.0f, 1.0f, 1.0f);
				RendererUtils::draw2dColorTextureRect(rc->getRenderer(), &vtx, &uv, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom);
				drawCleanupCharTex(glidTex);
			}

			if(width != 0L)	{	*width = w;	}
		}
		#endif
		break;
	}
}

/*---------------------------------------------------------------------*//**
	上下左右が欠けたフォントの 2D 描画
**//*---------------------------------------------------------------------*/
void OsFont::drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc) const
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
			#if defined(_WINDOWS)
			{
				f32 w = 0.0f;
				f32 h = 0.0f;
				TCHAR cstr[3];
				drawCalcChar(cstr, &w, &h, c, fontsize);

				RectF32 vtx(
					x + xChipLeft,
					y + yChipTop, 
					w,
					h	);
				vtx.w() -= xChipLeft + xChipRight;
				if(vtx.w() > 0.0f)
				{
					vtx.h() -= yChipTop + yChipBottom;
					if(vtx.h() > 0.0f)
					{
						if(rc != 0L)
						{
							f32 rUpW = 1.0f / (f32)fontsize;
							f32 rVpH = 1.0f / (f32)fontsize;
							f32 tuChipLeft = xChipLeft * rUpW;
							f32 tvChipTop = yChipTop * rVpH;
							RectF32 uv(
								tuChipLeft,
								tvChipTop,
								1.0f - tuChipLeft - (xChipRight * rUpW),
								1.0f - tvChipTop - (yChipBottom * rVpH)	);
							rc->getRenderer()->setEnableTexture(true);
							u32 glidTex = drawPrepareCharTex(cstr);
							RendererUtils::draw2dTextureRect(rc->getRenderer(), &vtx, &uv);
							drawCleanupCharTex(glidTex);
						}
					}

					if(width != 0L)			{	*width = vtx.width();	}
				}
				else if(width != 0L)	{	*width = 0.0f;	}
			}
			#endif
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	上下左右が欠けたフォントの 2D 描画（色つき）
**//*---------------------------------------------------------------------*/
void OsFont::drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const
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
			#if defined(_WINDOWS)
			{
				f32 w = 0.0f;
				f32 h = 0.0f;
				TCHAR cstr[3];
				drawCalcChar(cstr, &w, &h, c, fontsize);

				RectF32 vtx(
					x + xChipLeft,
					y + yChipTop, 
					w,
					h	);
				vtx.w() -= xChipLeft + xChipRight;
				if(vtx.w() > 0.0f)
				{
					vtx.h() -= yChipTop + yChipBottom;
					if(vtx.h() > 0.0f)
					{
						if(rc != 0L)
						{
							f32 rUpW = 1.0f / (f32)fontsize;
							f32 rVpH = 1.0f / (f32)fontsize;
							f32 tuChipLeft = xChipLeft * rUpW;
							f32 tvChipTop = yChipTop * rVpH;
							RectF32 uv(
								tuChipLeft,
								tvChipTop,
								1.0f - tuChipLeft - (xChipRight * rUpW),
								1.0f - tvChipTop - (yChipBottom * rVpH)	);
							rc->getRenderer()->setEnableTexture(true);
							u32 glidTex = drawPrepareCharTex(cstr);
							RendererUtils::draw2dColorTextureRect(rc->getRenderer(), &vtx, &uv, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom);
							drawCleanupCharTex(glidTex);
						}
					}

					if(width != 0L)			{	*width = vtx.width();	}
				}
				else if(width != 0L)	{	*width = 0.0f;	}
			}
			#endif
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画サブメソッド
**//*---------------------------------------------------------------------*/
void OsFont::render(const u32 c, const Vector3F* pos, const Vector3F* widthChar, const Vector3F* heightChar, const RenderCtx* rc) const
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
		#if defined(_WINDOWS)
		{
			Vector3F width = *widthChar * _rateHankaku;

			if(rc != 0L)
			{
				TCHAR cstr[3];
				int nc = 0;
				#if defined(UNICODE)
					if(c <= 0xffff)
					{
						cstr[0] = (u16)c;
						cstr[1] = '\0';
						nc = 1;
					}
					else if(c <= 0xffffffff)
					{
						cstr[0] = (u16)((c >> 16) & 0xffff);
						cstr[1] = (u16)(c & 0xffff);
						cstr[2] = '\0';
						nc = 2;
					}
				#else
					if(c <= 0xff)
					{
						cstr[0] = (u8)c;
						cstr[1] = '\0';
						nc = 1;
					}
					else if(c <= 0xffff)
					{
						cstr[0] = (u8)((c >> 8) & 0xff);
						cstr[1] = (u8)(c & 0xff);
						cstr[2] = '\0';
						nc = 2;
					}
				#endif

				Vector3F vtx[4];
				vtx[0].set(pos->x(), pos->y(), pos->z());
				vtx[1].set(pos->x() + heightChar->x(), pos->y() + heightChar->y(), pos->z() + heightChar->z());
				vtx[2].set(pos->x() + width.x(), pos->y() + width.y(), pos->z() + width.z());
				vtx[3].set(pos->x() + width.x() + heightChar->x(), pos->y() + width.y() + heightChar->y(), pos->z() + width.z() + heightChar->z());
				Vector2F uv[4];
				uv[0].set(0.0f, 0.0f);
				uv[1].set(0.0f, - 1.0f);
				uv[2].set(1.0f, 0.0f);
				uv[3].set(1.0f, -1.0f);
				rc->getRenderer()->setEnableTexture(true);
				u32 glidTex = drawPrepareCharTex(cstr);
				RendererUtils::render3dTriangles(rc->getRenderer(), vtx, 0L, uv, 0L, 4, RendererUtils::TRIANGLE_STRIP);
				drawCleanupCharTex(glidTex);
			}
		}
		#endif
		break;
	}
}

#if defined(_WINDOWS)
/*---------------------------------------------------------------------*//**
	文字サイズを計算する
**//*---------------------------------------------------------------------*/
void OsFont::drawCalcChar(TCHAR cstr[3], f32* w, f32* h, const u32 c, s32 fontsize) const
{
	int nc = 0;
	#if defined(UNICODE)
		if(c <= 0xffff)
		{
			cstr[0] = (u16)c;
			cstr[1] = '\0';
			nc = 1;
			*w = (f32)fontsize * _rateHankaku;
		}
		else if(c <= 0xffffffff)
		{
			cstr[0] = (u16)((c >> 16) & 0xffff);
			cstr[1] = (u16)(c & 0xffff);
			cstr[2] = '\0';
			nc = 2;
			*w = (f32)fontsize;
		}
	#else
		if(c <= 0xff)
		{
			cstr[0] = (u8)c;
			cstr[1] = '\0';
			nc = 1;
			*w = (f32)fontsize * _rateHankaku;
		}
		else if(c <= 0xffff)
		{
			cstr[0] = (u8)((c >> 8) & 0xff);
			cstr[1] = (u8)(c & 0xff);
			cstr[2] = '\0';
			nc = 2;
			*w = (f32)fontsize;
		}
	#endif
	*h = (f32)fontsize;
}

/*---------------------------------------------------------------------*//**
	文字テクスチャ用意
**//*---------------------------------------------------------------------*/
u32 OsFont::drawPrepareCharTex(TCHAR cstr[3]) const
{
	// デバイスコンテキストの設定
	int mapmodeBk = ::SetMapMode(_hdc, MM_TEXT);
	UINT textcolorBk = ::SetTextColor(_hdc, RGB(255, 255, 255));
	UINT bkcolorBk = ::SetBkColor(_hdc, 0x000000);
	UINT textalignBk = ::SetTextAlign(_hdc, TA_TOP);
	int bkmodeBk = ::SetBkMode(_hdc, TRANSPARENT);
	HBITMAP hbmpBk = (HBITMAP)::SelectObject(_hdc, _hbmp);
	HFONT hfontBk = (HFONT)::SelectObject(_hdc, _hfont);

	// ビットマップを黒で塗りつぶす
	::FillRect(_hdc, &_rectBmp, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	// ビットマップに描画する
	::TextOut(_hdc, 0, 0, (LPTSTR)cstr, (int)_tcslen(cstr));

	// デバイスコンテキストを戻す
	::SelectObject(_hdc, hfontBk);
	::SelectObject(_hdc, hbmpBk);
	::SetBkMode(_hdc, bkmodeBk);
	::SetTextAlign(_hdc, textalignBk);
	::SetBkColor(_hdc, bkcolorBk);
	::SetTextColor(_hdc, textcolorBk);
	::SetMapMode(_hdc, mapmodeBk);
	::GdiFlush();

	// 透明度ビットの設定
	for(u32 i = 0; i < _bmi.bmiHeader.biSizeImage; i+=4)
	{
		_bitsBmp[i + 3] = _bitsBmp[i + 0];	// 青の成分（BGRAの順らしい…）をαにコピーする
	}

	// OpenGL にテクスチャを転送する
	u32 pxfmt = GL_RGBA;
	u32 glidTex = 0;
	GLsizei width = (GLsizei)(_rectBmp.right - _rectBmp.left);
	GLsizei height = (GLsizei)(_rectBmp.bottom - _rectBmp.top);
	::glGenTextures(1, &glidTex);
	if(glidTex != 0L)
	{
		::glBindTexture(GL_TEXTURE_2D, glidTex);
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		::glTexImage2D(GL_TEXTURE_2D, 0, pxfmt, width, height, 0, pxfmt, GL_UNSIGNED_BYTE, _bitsBmp);
	}
	else
	{
		TRACE("{OsFont::drawPrepareCharTex} glGenTextures failed. pxfmt=0x%08x, width=%d, height=%d\n", pxfmt, width, height);
	}

	return glidTex;
}

/*---------------------------------------------------------------------*//**
	文字テクスチャ後処理
**//*---------------------------------------------------------------------*/
void OsFont::drawCleanupCharTex(u32 glidTex) const
{
	::glBindTexture(GL_TEXTURE_2D, 0);
	::glDeleteTextures(1, &glidTex);
}
#endif

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
