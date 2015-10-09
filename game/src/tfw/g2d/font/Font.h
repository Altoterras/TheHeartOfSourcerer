/***********************************************************************//**
 *	Font.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G2D_FONT_FONT_H_
#define _TFW_G2D_FONT_FONT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
	
////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class RenderCtx;
class CStringBase;
class Txt;

/*---------------------------------------------------------------------*//**
 *	フォント クラス
 *
**//*---------------------------------------------------------------------*/
class Font
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字の標準幅を得る
	virtual int getCharDefaultWidth() const = 0;
	// 文字の標準高さを得る
	virtual int getCharDefaultHeight() const = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual f32 getCharWidth(const u32 c, s32 fontsize) const = 0;

	virtual void draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const RenderCtx* rc) const = 0;
	inline void draw(const u32 c, f32 x, f32 y, s32 fontsize, const RenderCtx* rc) const { draw(0L, c, x, y, fontsize, rc); }

	virtual void draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const = 0;
	inline void draw(const u32 c, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const { draw(0L, c, x, y, fontsize, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom, rc); }

	virtual void drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc) const = 0;
	inline void drawChipped(const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc) const { drawChipped(0L, c, x, y, fontsize, xChipLeft, yChipTop, xChipRight, yChipBottom, rc); }

	virtual void drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const = 0;
	inline void drawChipped(const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const { drawChipped(0L, c, x, y, fontsize, xChipLeft, yChipTop, xChipRight, yChipBottom, colorLeftTop, colorRightTop, colorLeftBottom, colorRightBottom, rc); }

	virtual void render(const u32 c, const Vector3F* pos, const Vector3F* widthChar, const Vector3F* heightChar, const RenderCtx* rc) const = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	virtual ~Font() {}
	virtual void destroy() {}
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G2D_FONT_FONT_H_
