/***********************************************************************//**
 *	TxtBox.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/11/11.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_GUI_TXTBOX_TXT_BOX_H_
#define _TFW_GUI_TXTBOX_TXT_BOX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
#include "../../framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Point;
typedef Point<f32> PointF32;
template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
class Font;
class Keyboard;
class Texture;
class TouchPanel;
class Txt;
class TxtBoxExecRes;
class TxtPos;

/*---------------------------------------------------------------------*//**
 *	テキストエディタ
 *
**//*---------------------------------------------------------------------*/
class TxtBox : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	// 動作モード種別
	enum ModeKind
	{
		MODE_GENERAL,		// 一般
		MODE_SRCEDIT,		// スクリプトソース編集モード
		MODE_SRCREAD,		// スクリプトソース読み上げモード
		NUM_MODE
	};

	// 設定フラグ
	static const u16	CF_FIXED_WIDTH_FONT		= 0x0001;		// 固定幅フォント
	// 動作フラグ
	static const u16	AF_READ_ONLY			= 0x0001;		// 読み込み専用
	static const u16	AF_ENABLE				= 0x0002;		// 有効

	// 特殊コマンド
	enum Command
	{
		CMD_NULL,
		CMD_COPY,
		CMD_PASTE,
	};

private:
	#define TEST_OS_FONT						(0)
	#define TEST_UTF8							(0)

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	テキストエディタ
	 *
	**//*-----------------------------------------------------------------*/
	class Cursor
	{
	public:
		const Txt*				_txtRef;
		TxtPos*					_tpos;			// カーソル位置マスター
		u32						_line;			// カーソル位置の行番号
		u32						_col;			// カーソル位置の列番号
		f32						_xDisp;			// 表示 X 座標

	private:
		u32						_colt;			// 列の本来望む位置

	public:
		Cursor();
		Cursor(const Cursor& src);
		~Cursor();
		void					copy(const Cursor* src);
		void					reset(const Txt* txtRef);
		bool					left();
		bool					right();
		bool					up();
		bool					down();
		void					home();
		void					end();
		void					head();
		void					tail();
		void					pageup(u32 line);
		void					pagedown(u32 line);
		bool					getLineCharNum(u32* numCol, u32* numChar, const TxtPos* tpos, u32 tabcnt) const;
		static bool				isNlCode(u32 code);
	};

	/*-----------------------------------------------------------------*//**
	 *	セレクタ（テキスト選択）
	 *
	**//*-----------------------------------------------------------------*/
	class Selector
	{
	public:
		TxtPos*					_tposStart;		// 開始位置
		TxtPos*					_tposEnd;		// 終了位置

	public:
		Selector();
		~Selector();
		void reset();
		bool isSelected() const;
		void getNormalizePos(TxtPos** tposStart, TxtPos** tposEnd) const;
	};

	/*-----------------------------------------------------------------*//**
	 *	テキスト位置検索
	 *
	**//*-----------------------------------------------------------------*/
	class SearchPos
	{
	public:
		f32						_x;				// X 位置
		f32						_y;				// Y 位置
		f32						_w;				// テキスト幅
		f32						_h;				// テキスト高さ
		TxtPos*					_tpos;			// テキスト位置

	public:
		SearchPos();
		~SearchPos();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 有効かどうかを得る
	inline bool				isEnable() const					{	return TFW_IS_FLAG(_aflags, AF_ENABLE);	}
	// テキストを得る
	inline const Txt*		getText() const						{	return _txt;				}
	inline Txt*				text() const						{	return _txt;				}
	// フォントを得る
	inline const Font*		getFont() const						{	return _fontRef;			}
	// 矩形を得る
	inline const RectF32*	getRectangle() const				{	return _rectFrame;			}
	// 矩形を得る
	inline const RectF32*	getAvailableRectangle() const		{	return _rectAvailable;		}
	// スクロール位置を得る
	inline const PointF32*	getScrollPosition() const			{	return _ptScrollTrg;		}
	// スクロール最大位置を得る
	inline const PointF32*	getMaxScrollPosition() const		{	return _ptScrollMax;		}
	// テキスト色を得る
	inline ColorU8*			textColor()							{	return _colTxtFore;			}
	// アクティブテキスト色を得る
	inline ColorU8*			activeTextColor()					{	return _colTxtActive;		}
	// 選択背景色を得る
	inline ColorU8*			selectedBgColor()					{	return _colSelectedBg;		}
	// カーソル行を得る
	inline u32				getCursorLine() const				{	return _cursor._line;		}
	// カーソル列を得る
	inline u32				getCursorColumn() const				{	return _cursor._col;		}
	// 横（X 軸）方向トラッキング値を得る
	inline f32				getTrackingX() const				{	return _xTracking;			}
	// 縦（Y 軸）方向トラッキング値を得る
	inline f32				getTrackingY() const				{	return _yTracking;			}
	// 変更フラグを得る
	inline bool				isChanged() const					{	return _isChanged;			}

	// 有効性を設定する
	virtual void			setEnable(bool isEnable)			{	TFW_SET_FLAG(_aflags, AF_ENABLE, isEnable);	}
	// 矩形を設定する
	virtual void			setRectangle(const RectF32* rect);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void					setReadOnly(bool on) { TFW_SET_FLAG(_aflags, AF_READ_ONLY, on); }
	void					setMode(ModeKind mode);
	void					setText(const Txt* txt, bool isForceCursorReset);
	void					setActiveText(u32 idxRawBegin, u32 idxRawEnd);
	bool					trim();
	bool					copyText(Txt** txt) const;
	bool					pasteText(const Txt* txt);				

	virtual void			setScrollPosition(const PointF32* posScroll, bool isNoInterpolate);
	void					setTracking(f32 xTracking, f32 yTracking);
	void					setPadding(s32 xPadding, s32 yPadding);
	void					resetChangedFlag() { _isChanged = false; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TxtBox();
	virtual ~TxtBox();
	bool					create(Txt* txtEntr, Font* fontRef, s32 fontsize, Texture* texRef, const RectF32* rect, ModeKind mode, u32 cflags);
	void					destroy();
	void					exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	bool					execKeyboardEvent(TxtBoxExecRes* res, const Keyboard* kbd);
	bool					execKeyboardKey(TxtBoxExecRes* res, s32 key, bool isShift, bool isCtrl, bool isAlt);
	bool					execCommandKeyboardKey(TxtBoxExecRes* res, s32 key, bool isShift, bool isCtrl, bool isAlt);
	bool					execTouchPanelEvent(TxtBoxExecRes* res, const TouchPanel* tcp);
	void					draw(const RenderCtx* rc);				// 描画

private:
	void					drawTextProc(const RenderCtx* rc, bool isSimu, SearchPos* sp);
	void					drawCursor(const RenderCtx* rc, f32 x, f32 y, f32 chip[4], f32 xBase, f32 yBase, bool isSimu);
	void					adjustScrollPos();
	void					scrollByCursor();
	void					removeSelectedText();
	bool					indentSelectedTextByTabKey();
	bool					calcDispPosToTxtPos(TxtPos* pos, f32 x, f32 y);

	//======================================================================
	// メンバ変数

private:
	// 設定系
	u16						_cflags;		// 設定フラグ
	u16						_aflags;		// 動作フラグ
	RectF32*				_rectFrame;		// フレームの矩形
	RectF32*				_rectAvailable;	// 有効な領域矩形
	PointF32*				_ptScrollTrg;	// スクロールターゲット位置
	PointF32*				_ptScrollCur;	// スクロール現在位置
	PointF32*				_ptScrollMax;	// スクロール最大位置
	f32						_rateScrlIntp;	// スクロール補間率（0.0f=瞬時, 0.5f=処理毎に半分づつ近づく, 1.0f=NG;近づかない）
	Txt*					_txt;			// テキスト
	const Txt*				_txtRef;		// 参照用テキスト
	Font*					_fontRef;		// フォント
	ColorU8*				_colTxtFore;	// テキスト色
	ColorU8*				_colTxtActive;	// アクティブテキスト色
	ColorU8*				_colSelectedBg;	// 選択背景色
	ModeKind				_mode;			// モード
	s32						_fontsize;		// フォントサイズ
	s32						_wFont;			// フォント横幅
	s32						_hFont;			// フォント縦幅
	f32						_xTracking;		// 字間
	f32						_yTracking;		// 行間
	s32						_lineDisp;		// 表示可能行数
	s32						_colDisp;		// 表示可能列数
	s32						_xPadding;		// X 軸方向パディング
	s32						_yPadding;		// Y 軸方向パディング
	u32						_idxAtvBegin;	// アクティブ表示開始インデックス
	u32						_idxAtvEnd;		// アクティブ表示終了インデックス

	// 作業系
	Cursor					_cursor;		// カーソル
	Selector				_sel;			// セレクタ
	u32						_cntFrame;		// フレームカウンタ
	TxtPos*					_tposDragStart;	// ドラッグ開始テキスト位置
	bool					_isDrag;		// ドラッグ操作中
	bool					_isChanged;		// 変更した
#if TEST_OS_FONT
	Font*					_fontTest;		// テスト用フォント
#endif
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_GUI_TXTBOX_TXT_BOX_H_
