/***********************************************************************//**
 *  PanelText.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/11.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_GUI_PANEL_TEXT_H_
#define _TFW_GUI_PANEL_TEXT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Size;
typedef Size<f32> SizeF32;
class Font;
class VcString;
class StringDrawer;

/*---------------------------------------------------------------------*//**
 *	パネルにテキスト描画を付加するモジュール
 *	
**//*---------------------------------------------------------------------*/
class PanelText : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	// フラグ
	static const u32	F_DRAW_TOP_SHADOW		= 0x00000001;
	static const u32	F_DRAW_BOTTOM_SHADOW	= 0x00000002;
	static const u32	F_DRAW_LEFT_SHADOW		= 0x00000004;
	static const u32	F_DRAW_RIGHT_SHADOW		= 0x00000008;
	static const u32	F_DRAW_TOP_GLOS			= 0x00000010;
	static const u32	F_DRAW_BOTTOM_GLOS		= 0x00000020;
	static const u32	F_DRAW_LEFT_GLOS		= 0x00000040;
	static const u32	F_DRAW_RIGHT_GLOS		= 0x00000080;
	static const u32	F_ALIGN_MASK			= 0x0F000000;
	static const u32	F_ALIGN_LEFT			= 0x01000000;
	static const u32	F_ALIGN_RIGHT			= 0x02000000;
	static const u32	F_ALIGN_CENTER			= 0x04000000;
	static const u32	F_VALIGN_MASK			= 0xF0000000;
	static const u32	F_VALIGN_TOP			= 0x10000000;
	static const u32	F_VALIGN_BOTTOM			= 0x20000000;
	static const u32	F_VALIGN_MIDDLE			= 0x40000000;
private:
	static const u32	F_DRAW__MASK_BACK		= 0x000000ff;
	static const u32	F_DRAW__MASK_SHADOW		= 0x0000000f;
	static const u32	F_DRAW__MASK_GLOS		= 0x000000f0;

	//======================================================================
	// クラス
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 読み上げアニメーション中かを得る
	bool			isReadingAnimation() const;
	// 縦スクロールアニメーション中かを得る
	bool			isVerticalScrollAnimation() const;
	// 設定されたテキストが複数ページに渡るかどうかを得る
	bool			isMultiplePagesText() const;
	// 動作を停止しているかどうかを得る
	inline bool		isScrollDone() const				{	return _isScrollDone;								}
	// フォントを取得する
	Font*			getFont() const;
	// フラグを得る
	inline u32		getFlags() const					{	return _flags;										}
	// アライメントを得る
	inline u32		getAlign() const					{	return _flags & F_ALIGN_MASK;						}
	// バーティカルアライメントを得る
	inline u32		getValign() const					{	return _flags & F_VALIGN_MASK;						}

	// 矩形を設定する
	virtual void	setRectangle(const RectF32* rect);
	// オートスクロールするかどうかを設定する
	inline void		setAutoScroll(bool isEnable)		{	_isAutoScroll = isEnable;							}
	// フラグを設定する
	inline void		setFlags(u32 flags, bool on)		{	TFW_SET_FLAG(_flags, flags, on);					}
	// アライメントを設定する
	inline void 	setAlign(u32 flagAlign)				{	_flags = (_flags & ~F_ALIGN_MASK) | flagAlign;		}
	// バーティカルアライメントを設定する
	inline void 	setValign(u32 flagValign)			{	_flags = (_flags & ~F_VALIGN_MASK) | flagValign;	}
	// 未使用設定
	inline void		setUnused(bool on)					{	_isUnused = on;										}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void			setText(const VcString* text, bool isAdd);
	void			nextPage();
	void			setReadingAnimation(bool isEnable, f32 speed, f32 interval);
	void			setVerticalScrollAnimation(bool isEnable, f32 speed);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PanelText();
	virtual ~PanelText();
	virtual bool	create(Font* fontRef, s32 fontsize, const RectF32* rect, const ColorU8* color);
	virtual void	destroy();
	virtual void	exec(ExecRes* res, const ExecCtx* ec);
	virtual void	draw(const RenderCtx* rc);

	//======================================================================
	// 変数
protected:
	Font*			_fontRef;
	StringDrawer*	_strdraw;
	VcString*		_strTxt;
	ColorU8*		_color;
	SizeF32*		_sizeTxt;

	s32				_numLine;
	s32				_linesOnePage;
	s32				_lineScrollCur;
	s32				_cntFrame;
	bool			_isScrollDone;
	bool			_isAutoScroll;
	bool			_isUnused;
	u32				_flags;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_GUI_PANEL_TEXT_H_
