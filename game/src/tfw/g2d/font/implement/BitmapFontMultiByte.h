/***********************************************************************//**
 *	BitmapFontMultiByte.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/09/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G2D_FONT_BITMAP_FONT_MULTI_BYTE_H_
#define _TFW_G2D_FONT_BITMAP_FONT_MULTI_BYTE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Font.h"

#include "../../../gcmn/Texture.h"
	
////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FontCharDefData;
class ProportionalFontData;
class Texture;

/*---------------------------------------------------------------------*//**
 *	ワイドバイト文字 ビットマップ フォント クラス
 *		※現状では Shift-JIS のみに対応
 *
**//*---------------------------------------------------------------------*/
class BitmapFontMultiByte : public Font
{
	//======================================================================
	// クラス
private:
	class FontInfo
	{
	public:
		u32	_maskX;
		u32 _shiftY;
		u16 _wChar;
		u16 _hChar;
		f32 _tuChar;
		f32 _tvChar;
		f32 _twChar;
		f32 _thChar;
		f32 _tu1px;
		f32 _tv1px;
		f32 _scale;

		FontInfo() : _maskX(0), _shiftY(0), _wChar(0), _hChar(0), _tuChar(0.0f), _tvChar(0.0f), _twChar(0.0f), _thChar(0.0f), _tu1px(0.0f), _tv1px(0.0f), _scale(0.0f) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字の標準幅を得る
	virtual int getCharDefaultWidth() const;
	// 文字の標準高さを得る
	virtual int getCharDefaultHeight() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual f32 getCharWidth(const u32 c, s32 fontsize) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	BitmapFontMultiByte();
	virtual ~BitmapFontMultiByte();
	bool create(Texture* texRef, const ProportionalFontData* pfdatRef, const FontCharDefData* fcddRef, s32 fontsize, f32 rateHankaku, f32 scale);
	void destroy();

public:
	virtual void draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const RenderCtx* rc) const;
	virtual void draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const;
	virtual void drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc) const;
	virtual void drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const;
	virtual void render(const u32 c, const Vector3F* pos, const Vector3F* widthChar, const Vector3F* heightChar, const RenderCtx* rc) const;

private:
	static inline bool isProportinalChar(u32 c) { return (0x20 <= c) && (c <= 0x7e); }

	//======================================================================
	// 変数
private:
	f32 _rateHankaku;
	Texture* _texRef;
	const FontCharDefData* _fcddRef;
	const ProportionalFontData* _pfdatRef;
	FontInfo* _finf;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G2D_FONT_BITMAP_FONT_MULTI_BYTE_H_

