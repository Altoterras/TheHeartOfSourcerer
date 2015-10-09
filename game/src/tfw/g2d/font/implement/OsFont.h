/***********************************************************************//**
 *	OsFont.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2011/12/08.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_G2D_FONT_OS_FONT_H_
#define _TFW_G2D_FONT_OS_FONT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Font.h"
	
////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class CStringBase;

/*---------------------------------------------------------------------*//**
 *	OS依存フォント
 *		※	主にテスト・実験用途
 *			現在は Windows のみサポート
 *
**//*---------------------------------------------------------------------*/
class OsFont : public Font
{
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
	OsFont();
	virtual ~OsFont();
	virtual bool create(CStringBase* nameFont, s32 fontsize, f32 rateHankaku);
	virtual void destroy();

protected:
	virtual void draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const RenderCtx* rc) const;
	virtual void draw(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const;
	virtual void drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const RenderCtx* rc) const;
	virtual void drawChipped(f32* width, const u32 c, f32 x, f32 y, s32 fontsize, f32 xChipLeft, f32 yChipTop, f32 xChipRight, f32 yChipBottom, const ColorU8* colorLeftTop, const ColorU8* colorRightTop, const ColorU8* colorLeftBottom, const ColorU8* colorRightBottom, const RenderCtx* rc) const;
	virtual void render(const u32 c, const Vector3F* pos, const Vector3F* widthChar, const Vector3F* heightChar, const RenderCtx* rc) const;

#if defined(_WINDOWS)
	void drawCalcChar(TCHAR cstr[3], f32* w, f32* h, const u32 c, s32 fontsize) const;
	u32 drawPrepareCharTex(TCHAR cstr[3]) const;
	void drawCleanupCharTex(u32 glidTex) const;
#endif

	//======================================================================
	// 変数
private:
	s32			_fontsize;
	f32			_rateHankaku;
#if defined(_WINDOWS)
	HFONT		_hfont;					// 描画フォントハンドル
	HDC			_hdc;					// フォント描画デバイスコンテキストハンドル
	BITMAPINFO	_bmi;					// ビットマップ情報
	HBITMAP		_hbmp;					// ビットマップハンドル
	BYTE*		_bitsBmp;				// ビットマップ先頭ビット
	RECT		_rectBmp;				// ビットマップの矩形
#endif
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G2D_FONT_OS_FONT_H_

