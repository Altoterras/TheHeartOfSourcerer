/***********************************************************************//**
 *	EasyStringDrawer.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2011/12/13.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EasyStringDrawer.h"

// Friends
#include "../g2d/font/Font.h"
#include "../lib/Vector.h"
#include "../txt/TxtChar.h"
#include "../txt/TxtIterator.h"
#include "../txt/implement/SjisTxt.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BitmapFontWbyte メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	文字列 2D 描画
	※	ただし、文字列は Shift-JIS 限定とする
		他の文字エンコードを使用したい場合は Txt クラス版を使用すること
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::draw(const Font* font, const CStringBase* str, f32 x, f32 y, s32 fontsize, const RenderCtx* rc)
{
	f32 wc;
	f32 xLeft = x;
	const char* sz = str->getRaw();
	int lenStr = str->getLength();
	for(int i = 0; i < lenStr; i++)
	{
		u32 c = sz[i];
		if(c == '\n')
		{
			x = xLeft;
			y += fontsize;
			continue;
		}
		if(SjisTxt::isByteSjisFst(c) && (i < lenStr))	// Shift-JIS の最初の文字の場合
		{
			c = TFW_U8_TO_U16(sz[i], sz[i + 1]);
			i++;
		}
		font->draw(&wc, c, x, y, fontsize, rc);
		x += wc;
	}
}

/*---------------------------------------------------------------------*//**
	テキスト 2D 描画
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::draw(const Font* font, const Txt* txt, f32 x, f32 y, s32 fontsize, const RenderCtx* rc)
{
	f32 wc;
	f32 xLeft = x;
	for(TxtIterator it = txt->iterator(); it.has(); it.next())
	{
		TxtChar tc = it.cur();
		u32 c = tc.getCode();
		if((c == Txt::NLCODE_CR) || (c == Txt::NLCODE_LF) || (c == Txt::NLCODE_CRLF))
		{
			x = xLeft;
			y += fontsize;
			continue;
		}
		font->draw(&wc, c, x, y, fontsize, rc);
		x += wc;
	}
}

/*---------------------------------------------------------------------*//**
	アライン付き文字列 2D 描画
	※	ただし、文字列は Shift-JIS 限定とする
		他の文字エンコードを使用したい場合は Txt クラス版を使用すること
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::draw(const Font* font, const CStringBase* str, f32 x, f32 y, f32 w, f32 h, s32 fontsize, u32 flags, const RenderCtx* rc)
{
	f32 wStr = - F32_PMAX;
	f32 hStr = - F32_PMAX;
	f32 xLeft = x;
	f32 yTop = y;
	f32 wc;
	if(!TFW_IS_FLAG(flags, F_ALIGN_LEFT) || !TFW_IS_FLAG(flags, F_VALIGN_TOP))
	{
		const char* sz = str->getRaw();
		int lenStr = str->getLength();
		hStr = y;
		for(int i = 0; i < lenStr; i++)
		{
			u32 c = sz[i];
			if(c == '\n')
			{
				x = xLeft;
				y += fontsize;
				if(hStr < y)	{	hStr = y;	}
				continue;
			}
			if(SjisTxt::isByteSjisFst(c) && (i < lenStr))	// Shift-JIS の最初の文字の場合
			{
				c = TFW_U8_TO_U16(sz[i], sz[i + 1]);
				i++;
			}
			wc = font->getCharWidth(c, fontsize);
			x += wc;
			if(wStr < x)	{	wStr = x;	}
		}
		wStr -= xLeft;
		hStr = hStr - yTop + fontsize;
		if(TFW_IS_FLAG(flags, F_ALIGN_RIGHT))			{	xLeft = xLeft + (w - wStr);				}
		else if(TFW_IS_FLAG(flags, F_ALIGN_CENTER))		{	xLeft = xLeft + ((w - wStr) * 0.5f);	}
		if(TFW_IS_FLAG(flags, F_VALIGN_BOTTOM))			{	yTop = yTop + (h - hStr);				}
		else if(TFW_IS_FLAG(flags, F_VALIGN_MIDDLE))	{	yTop = yTop + ((h - hStr) * 0.5f);		}
	}
	x = xLeft;
	y = yTop;
	const char* sz = str->getRaw();
	int lenStr = str->getLength();
	for(int i = 0; i < lenStr; i++)
	{
		u32 c = sz[i];
		if(c == '\n')
		{
			x = xLeft;
			y += fontsize;
			continue;
		}
		if(SjisTxt::isByteSjisFst(c) && (i < lenStr))	// Shift-JIS の最初の文字の場合
		{
			c = TFW_U8_TO_U16(sz[i], sz[i + 1]);
			i++;
		}
		font->draw(&wc, c, x, y, fontsize, rc);
		x += wc;
	}
}

/*---------------------------------------------------------------------*//**
	アライン付きテキスト 2D 描画
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::draw(const Font* font, const Txt* txt, f32 x, f32 y, f32 w, f32 h, s32 fontsize, u32 flags, const RenderCtx* rc)
{
	f32 wStr = - F32_PMAX;
	f32 hStr = - F32_PMAX;
	f32 xLeft = x;
	f32 yTop = y;
	f32 wc;
	if(!TFW_IS_FLAG(flags, F_ALIGN_LEFT) || !TFW_IS_FLAG(flags, F_VALIGN_TOP))
	{
		for(TxtIterator it = txt->iterator(); it.has(); it.next())
		{
			TxtChar tc = it.cur();
			u32 c = tc.getCode();
			hStr = y;
			if((c == Txt::NLCODE_CR) || (c == Txt::NLCODE_LF) || (c == Txt::NLCODE_CRLF))
			{
				x = xLeft;
				y += fontsize;
				if(hStr < y)	{	hStr = y;	}
				continue;
			}
			wc = font->getCharWidth(c, fontsize);
			x += wc;
			if(wStr < x)	{	wStr = x;	}
		}
		wStr -= xLeft;
		hStr = hStr - yTop + fontsize;
		if(TFW_IS_FLAG(flags, F_ALIGN_RIGHT))			{	xLeft = xLeft + (w - wStr);				}
		else if(TFW_IS_FLAG(flags, F_ALIGN_CENTER))		{	xLeft = xLeft + ((w - wStr) * 0.5f);	}
		if(TFW_IS_FLAG(flags, F_VALIGN_BOTTOM))			{	yTop = yTop + (h - hStr);				}
		else if(TFW_IS_FLAG(flags, F_VALIGN_MIDDLE))	{	yTop = yTop + ((h - hStr) * 0.5f);		}
	}
	x = xLeft;
	y = yTop;
	for(TxtIterator it = txt->iterator(); it.has(); it.next())
	{
		TxtChar tc = it.cur();
		u32 c = tc.getCode();
		if((c == Txt::NLCODE_CR) || (c == Txt::NLCODE_LF) || (c == Txt::NLCODE_CRLF))
		{
			y += fontsize;
			x = xLeft;
			continue;
		}
		font->draw(&wc, c, x, y, fontsize, rc);
		x += wc;
	}
}

/*---------------------------------------------------------------------*//**
	文字列 2D 描画（色つき）
	※	ただし、文字列は Shift-JIS 限定とする
		他の文字エンコードを使用したい場合は Txt クラス版を使用すること
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::draw(const Font* font, const CStringBase* str, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc)
{
	f32 xLeft = x;
	f32 w;
	const char* sz = str->getRaw();
	int lenStr = str->getLength();
	for(int i = 0; i < lenStr; i++)
	{
		u32 c = sz[i];
		if(c == '\n')
		{
			x = xLeft;
			y += fontsize;
			continue;
		}
		if(SjisTxt::isByteSjisFst(c) && (i < lenStr))	// Shift-JIS の最初の文字の場合
		{
			c = TFW_U8_TO_U16(sz[i], sz[i + 1]);
			i++;
		}
		font->draw(&w, c, x, y, fontsize, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom, rc);
		x += w;
	}
}

/*---------------------------------------------------------------------*//**
	テキスト 2D 描画（色つき）
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::draw(const Font* font, const Txt* txt, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc)
{
	f32 xLeft = x;
	f32 w;
	for(TxtIterator it = txt->iterator(); it.has(); it.next())
	{
		TxtChar tc = it.cur();
		u32 c = tc.getCode();
		if((c == Txt::NLCODE_CR) || (c == Txt::NLCODE_LF) || (c == Txt::NLCODE_CRLF))
		{
			y += fontsize;
			x = xLeft;
			continue;
		}
		font->draw(&w, c, x, y, fontsize, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom, rc);
		x += w;
	}
}

/*---------------------------------------------------------------------*//**
	上下左右が欠けた文字列 2D 描画
	※	ただし、文字列は Shift-JIS 限定とする
		他の文字エンコードを使用したい場合は Txt クラス版を使用すること
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::drawChipped(const Font* font, const CStringBase* str, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc)
{
	if((xChipLeft == 0.0f) && (yChipTop == 0.0f) && (xChipRight == 0.0f) && (yChipBottom == 0.0f))
	{
		draw(font, str, x, y, fontsize, rc);
		return;
	}
	f32 xLeft = x;
	f32 w;
	const char* sz = str->getRaw();
	int lenStr = str->getLength();
	for(int i = 0; i < lenStr; i++)
	{
		u32 c = sz[i];
		if(c == '\n')
		{
			x = xLeft;
			y += fontsize;
			continue;
		}
		if(SjisTxt::isByteSjisFst(c) && (i < lenStr))	// Shift-JIS の最初の文字の場合
		{
			c = TFW_U8_TO_U16(sz[i], sz[i + 1]);
			i++;
		}
		font->drawChipped(&w, c, x, y, fontsize, xChipLeft, yChipTop, xChipRight, yChipBottom, rc);
		x += w;
	}
}

/*---------------------------------------------------------------------*//**
	上下左右が欠けたテキスト 2D 描画
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::drawChipped(const Font* font, const Txt* txt, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc)
{
	if((xChipLeft == 0.0f) && (yChipTop == 0.0f) && (xChipRight == 0.0f) && (yChipBottom == 0.0f))
	{
		draw(font, txt, x, y, fontsize, rc);
		return;
	}
	f32 xLeft = x;
	f32 w;
	for(TxtIterator it = txt->iterator(); it.has(); it.next())
	{
		TxtChar tc = it.cur();
		u32 c = tc.getCode();
		if((c == Txt::NLCODE_CR) || (c == Txt::NLCODE_LF) || (c == Txt::NLCODE_CRLF))
		{
			x = xLeft;
			y += fontsize;
			continue;
		}
		font->drawChipped(&w, c, x, y, fontsize, xChipLeft, yChipTop, xChipRight, yChipBottom, rc);
		x += w;
	}
}

/*---------------------------------------------------------------------*//**
	上下左右が欠けた文字列 2D 描画（色つき）
	※	ただし、文字列は Shift-JIS 限定とする
		他の文字エンコードを使用したい場合は Txt クラス版を使用すること
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::drawChipped(const Font* font, const CStringBase* str, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc)
{
	if((xChipLeft == 0.0f) && (yChipTop == 0.0f) && (xChipRight == 0.0f) && (yChipBottom == 0.0f))
	{
		draw(font, str, x, y, fontsize, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom, rc);
		return;
	}
	f32 xLeft = x;
	f32 w;
	const char* sz = str->getRaw();
	int lenStr = str->getLength();
	for(int i = 0; i < lenStr; i++)
	{
		u32 c = sz[i];
		if(c == '\n')
		{
			x = xLeft;
			y += fontsize;
			continue;
		}
		if(SjisTxt::isByteSjisFst(c) && (i < lenStr))	// Shift-JIS の最初の文字の場合
		{
			c = TFW_U8_TO_U16(sz[i], sz[i + 1]);
			i++;
		}
		font->drawChipped(&w, c,x, y,  fontsize, xChipLeft, yChipTop, xChipRight, yChipBottom, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom, rc);
		x += w;
	}
}

/*---------------------------------------------------------------------*//**
	上下左右が欠けたテキスト 2D 描画（色つき）
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::drawChipped(const Font* font, const Txt* txt, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc)
{
	if((xChipLeft == 0.0f) && (yChipTop == 0.0f) && (xChipRight == 0.0f) && (yChipBottom == 0.0f))
	{
		draw(font, txt, x, y, fontsize, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom, rc);
		return;
	}
	f32 xLeft = x;
	f32 w;
	for(TxtIterator it = txt->iterator(); it.has(); it.next())
	{
		TxtChar tc = it.cur();
		u32 c = tc.getCode();
		if((c == Txt::NLCODE_CR) || (c == Txt::NLCODE_LF) || (c == Txt::NLCODE_CRLF))
		{
			x = xLeft;
			y += fontsize;
			continue;
		}
		font->drawChipped(&w, c, x, y, fontsize, xChipLeft, yChipTop, xChipRight, yChipBottom, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom, rc);
		x += w;
	}
}

/*---------------------------------------------------------------------*//**
	文字列 3D 描画
	※	ただし、文字列は Shift-JIS 限定とする
		他の文字エンコードを使用したい場合は Txt クラス版を使用すること
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::render(const Font* font, const CStringBase* str, const Vector3F* pos, const Vector3F* widthChar, const Vector3F* heightChar, const RenderCtx* rc)
{
	Vector3F posWk(pos);
	Vector3F posLeft(pos);
	const char* sz = str->getRaw();
	int lenStr = str->getLength();
	for(int i = 0; i < lenStr; i++)
	{
		u32 c = sz[i];
		if(c == '\n')
		{
			posWk = posLeft + *heightChar;
			continue;
		}
		if(SjisTxt::isByteSjisFst(c) && (i < lenStr))	// Shift-JIS の最初の文字の場合
		{
			c = TFW_U8_TO_U16(sz[i], sz[i + 1]);
			i++;
		}
		font->render(c, &posWk, widthChar, heightChar, rc);
		posWk += *widthChar;
	}
}

/*---------------------------------------------------------------------*//**
	テキスト 3D 描画
**//*---------------------------------------------------------------------*/
void EasyStringDrawer::render(const Font* font, const Txt* txt, const Vector3F* pos, const Vector3F* widthChar, const Vector3F* heightChar, const RenderCtx* rc)
{
	Vector3F posWk(pos);
	Vector3F posLeft(pos);
	for(TxtIterator it = txt->iterator(); it.has(); it.next())
	{
		TxtChar tc = it.cur();
		u32 c = tc.getCode();
		if((c == Txt::NLCODE_CR) || (c == Txt::NLCODE_LF) || (c == Txt::NLCODE_CRLF))
		{
			posWk = posLeft + *heightChar;
			continue;
		}
		font->render(c, &posWk, widthChar, heightChar, rc);
		posWk += *widthChar;
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
