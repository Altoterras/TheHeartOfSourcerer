/***********************************************************************//**
 *	EasyStringDrawer.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2011/12/13.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G2D_EASY_STRING_DRAWER_H_
#define _TFW_G2D_EASY_STRING_DRAWER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../string/CcString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Font;
class RenderCtx;
class Txt;

/*---------------------------------------------------------------------*//**
 *	簡易文字列描画
 *
**//*---------------------------------------------------------------------*/
class EasyStringDrawer
{
	//======================================================================
	// 定数
public:
	// アライン指定
	static const u32 F_ALIGN_LEFT		= 0x00000001;
	static const u32 F_ALIGN_CENTER		= 0x00000002;
	static const u32 F_ALIGN_RIGHT		= 0x00000004;
	static const u32 F_VALIGN_TOP		= 0x00000010;
	static const u32 F_VALIGN_MIDDLE	= 0x00000020;
	static const u32 F_VALIGN_BOTTOM	= 0x00000040;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static void draw(const Font* font, const CStringBase* str, f32 x, f32 y, s32 fontsize, const RenderCtx* rc);
	inline static void draw(const Font* font, CcString str, f32 x, f32 y, s32 fontsize, const RenderCtx* rc) { draw(font, &str, x, y, fontsize, rc); }
	static void draw(const Font* font, const Txt* txt, f32 x, f32 y, s32 fontsize, const RenderCtx* rc);

	static void draw(const Font* font, const CStringBase* str, f32 x, f32 y, f32 w, f32 h, s32 fontsize, u32 flags, const RenderCtx* rc);
	inline static void draw(const Font* font, CcString str, f32 x, f32 y, f32 w, f32 h, s32 fontsize, u32 flags, const RenderCtx* rc) { draw(font, &str, x, y, w, h, fontsize, flags, rc); }
	static void draw(const Font* font, const Txt* txt, f32 x, f32 y, f32 w, f32 h, s32 fontsize, u32 flags, const RenderCtx* rc);

	static void draw(const Font* font, const CStringBase* str, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc);
	inline static void draw(const Font* font, CcString str, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) { draw(font, &str, x, y, fontsize, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom, rc); }
	static void draw(const Font* font, const Txt* txt, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc);

	static void drawChipped(const Font* font, const CStringBase* str, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc);
	inline static void drawChipped(const Font* font, CcString str, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc) { drawChipped(font, &str, x, y, fontsize, xChipLeft, yChipTop, xChipRight, yChipBottom, rc); }
	static void drawChipped(const Font* font, const Txt* txt, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc);

	static void drawChipped(const Font* font, const CStringBase* str, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc);
	inline static void drawChipped(const Font* font, CcString str, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) { drawChipped(font, &str, x, y, fontsize, xChipLeft, yChipTop, xChipRight, yChipBottom, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom, rc); }
	static void drawChipped(const Font* font, const Txt* txt, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc);

	static void render(const Font* font, const CStringBase* str, const Vector3F* pos, const Vector3F* widthChar, const Vector3F* heightChar, const RenderCtx* rc);
	inline static void render(const Font* font, CcString str, const Vector3F* pos, const Vector3F* widthChar, const Vector3F* heightChar, const RenderCtx* rc) { render(font, &str, pos, widthChar, heightChar, rc); }
	static void render(const Font* font, const Txt* txt, const Vector3F* pos, const Vector3F* widthChar, const Vector3F* heightChar, const RenderCtx* rc);
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G2D_FONT_FONT_H_
