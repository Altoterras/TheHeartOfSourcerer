/***********************************************************************//**
 *	TxtBox.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/11/11.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TxtBox.h"

// Friends
#include "TxtBoxExecRes.h"

// External

// Library
#include "../../../tfw/g2d/font/Font.h"
#if TEST_OS_FONT
#include "../../../tfw/g2d/font/implement/OsFont.h"
#endif
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/gcmn/RendererUtils.h"
#include "../../../tfw/lib/Color.h"
#include "../../../tfw/lib/Rect.h"
#include "../../../tfw/lib/Point.h"
#include "../../../tfw/txt/Txt.h"
#include "../../../tfw/txt/TxtChar.h"
#include "../../../tfw/txt/TxtIterator.h"
#include "../../../tfw/txt/TxtPos.h"
#include "../../../tfw/txt/TxtUtils.h"
#if TEST_UTF8	// UTF-8 test
#include "../../../tfw/txt/implement/SjisTxt.h"
#include "../../../tfw/txt/implement/Utf8Txt.h"
#endif
#include "../../../tfw/ui/TouchPanel.h"
#include "../../../tfw/ui/Keyboard.h"

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define COL_TAB					(4)
#define FONTSIZE_DEFAULT		(16)
#define TRACKING_X_DEFAULT		(-2)
#define TRACKING_Y_DEFAULT		(0)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TxtBox メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	矩形設定
**//*---------------------------------------------------------------------*/
void TxtBox::setRectangle(const RectF32* rect)
{
	// 矩形を設定
	_rectFrame->copy(rect);
	_rectAvailable->set(rect->x() + _xPadding, rect->y() + _yPadding, rect->w() - _xPadding - _xPadding, rect->h() - _yPadding - _yPadding);
	
	// 表示可能行数・列数
	_lineDisp	= (s32)(_rectAvailable->h() / (_hFont + _yTracking));
	_colDisp	= (s32)(_rectAvailable->w() / (_wFont + _xTracking));

	scrollByCursor();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	モードを設定する
**//*---------------------------------------------------------------------*/
void TxtBox::setMode(ModeKind mode)
{
	if(_mode == mode)	{	return;	}
	_mode = mode;
}

/*---------------------------------------------------------------------*//**
	テキスト設定
**//*---------------------------------------------------------------------*/
void TxtBox::setText(const Txt* txt, bool isForceCursorReset)
{
	bool isCursorReset = isForceCursorReset;

	if(_txt != txt)
	{
		_txt->setText(txt);
		isCursorReset = true;
	}

	// カーソル位置・スクロール位置リセット
	if(isCursorReset)
	{
		_cursor.reset(_txtRef);		// カーソル位置リセット
		_ptScrollTrg->set(0, 0);	// スクロール目的位置リセット
		_ptScrollCur->set(0, 0);	// スクロール現在位置リセット
		_ptScrollMax->set(0, 0);	// スクロール最大位置リセット
	}

	drawTextProc(0L, true, 0L);	// 描画シミュレーション
	_isChanged = true;			// 変更された
}

/*---------------------------------------------------------------------*//**
	アクティブテキストの位置を設定する
**//*---------------------------------------------------------------------*/
void TxtBox::setActiveText(u32 idxRawBegin, u32 idxRawEnd)
{
	_idxAtvBegin = idxRawBegin;
	_idxAtvEnd = idxRawEnd;

	// アクティブテキストの位置にカーソルを持っていく
	if(_cursor._tpos->getRawIndex() >= _idxAtvEnd)
	{
		do
		{
			if(!_cursor.left())
			{
				break;
			}
		}
		while(_cursor._tpos->getRawIndex() > _idxAtvBegin);
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置にスクロール
	}
	else if(_cursor._tpos->getRawIndex() < _idxAtvBegin)
	{
		u32 idxTrg = _idxAtvBegin + _colDisp - 1;
		if(idxTrg > _idxAtvEnd)
		{
			idxTrg = _idxAtvEnd;
		}
		do
		{
			if(!_cursor.right())
			{
				break;
			}
		}
//		while(_cursor._tpos->getRawIndex() <= _idxAtvEnd);
		while(_cursor._tpos->getRawIndex() < idxTrg);
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置にスクロール
	}
}

/*---------------------------------------------------------------------*//**
	両端空白削除

	@return トリミングを行ったかどうか
**//*---------------------------------------------------------------------*/
bool TxtBox::trim()
{
	// トリム処理
	if(!_txt->trim())	{	return false;	}

	// カーソル位置・スクロール位置リセット
	_cursor.reset(_txtRef);		// カーソル位置リセット
	_ptScrollTrg->set(0, 0);	// スクロール目的位置リセット
	_ptScrollCur->set(0, 0);	// スクロール現在位置リセット
	_ptScrollMax->set(0, 0);	// スクロール最大位置リセット

	drawTextProc(0L, true, 0L);	// 描画シミュレーション
	_isChanged = true;			// 変更された
	return true;
}

/*---------------------------------------------------------------------*//**
	テキストのコピー
**//*---------------------------------------------------------------------*/
bool TxtBox::copyText(Txt** txt) const
{
	// 選択範囲を得る
	TxtPos* tposStart;
	TxtPos* tposEnd;
	_sel.getNormalizePos(&tposStart, &tposEnd);

	// 選択範囲の部分テキストを作成する
	return _txt->subtext(txt, tposStart, tposEnd);
}

/*---------------------------------------------------------------------*//**
	テキストのペースト
**//*---------------------------------------------------------------------*/
bool TxtBox::pasteText(const Txt* txt)
{
	// 選択範囲を削除
	if(_sel.isSelected())	// 選択テキストがある場合
	{
		removeSelectedText();	// 選択部分削除
		_sel.reset();			// 選択解除
	}

	// テキスト挿入
	if(!_txt->insert(_cursor._tpos, txt))
	{
		return false;
	}

	// カーソルを挿入したテキストの末尾に移動
	for(u32 i = 0; i < txt->getCharCount(); i++)
	{
		_cursor.right();
	}

	drawTextProc(0L, true, 0L);	// 描画シミュレーション
	scrollByCursor();			// カーソル位置によるスクロール
	_isChanged = true;			// 変更された
	return true;
}

/*---------------------------------------------------------------------*//**
	スクロール位置設定
**//*---------------------------------------------------------------------*/
void TxtBox::setScrollPosition(const PointF32* posScroll, bool isNoInterpolate)
{
	_ptScrollTrg->copy(posScroll);
	adjustScrollPos();

	// 補間無しの場合は現在位置も設定
	if(isNoInterpolate)
	{
		_ptScrollCur->copy(posScroll);
	}
}

/*---------------------------------------------------------------------*//**
	トラッキング値の設定
**//*---------------------------------------------------------------------*/
void TxtBox::setTracking(f32 xTracking, f32 yTracking)
{
	_xTracking = xTracking;
	_yTracking = yTracking;
}

/*---------------------------------------------------------------------*//**
	パディング値の設定
**//*---------------------------------------------------------------------*/
void TxtBox::setPadding(s32 xPadding, s32 yPadding)
{
	_xPadding = xPadding;
	_yPadding = yPadding;
	setRectangle(_rectFrame);	// 矩形再設定
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TxtBox::TxtBox()
	: _cflags(0)
	, _aflags(0)
	, _rectFrame(0L)
	, _rectAvailable(0L)
	, _ptScrollTrg(0L)
	, _ptScrollCur(0L)
	, _ptScrollMax(0L)
	, _rateScrlIntp(0.0f)
	, _txt(0L)
	, _txtRef(0L)
	, _fontRef(0L)
	, _colTxtFore(0L)
	, _colTxtActive(0L)
	, _colSelectedBg(0L)
	, _mode(MODE_GENERAL)
	, _fontsize(0)
	, _wFont(0)
	, _hFont(0)
	, _xTracking(0)
	, _yTracking(0)
	, _lineDisp(0)
	, _colDisp(0)
	, _xPadding(0)
	, _yPadding(0)
	, _idxAtvBegin(0xffffffff)
	, _idxAtvEnd(0xffffffff)
	, _cursor()
	, _cntFrame(0)
	, _tposDragStart(0L)
	, _isDrag(false)
	, _isChanged(false)
#if TEST_OS_FONT
	, _fontTest(0L)
#endif
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TxtBox::~TxtBox()
{
	ASSERT(_rectFrame == 0L);
	ASSERT(_rectAvailable == 0L);
	ASSERT(_ptScrollTrg == 0L);
	ASSERT(_ptScrollCur == 0L);
	ASSERT(_ptScrollMax == 0L);
	ASSERT(_txt == 0L);
	ASSERT(_colTxtFore == 0L);
	ASSERT(_colTxtActive == 0L);
	ASSERT(_colSelectedBg == 0L);
	ASSERT(_tposDragStart == 0L);
#if TEST_OS_FONT
	ASSERT(_fontTest == 0L);
#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool TxtBox::create(Txt* txtEntr, Font* fontRef, s32 fontsize, Texture* texRef, const RectF32* rect, ModeKind mode, u32 cflags)
{
	// フォントを保存
#if TEST_OS_FONT
	OsFont* fontTest = new OsFont();
	CcString cstrFontName("メイリオ");
	if(fontTest->create(&cstrFontName, 64, 0.5f))
	{
		_fontTest = fontTest;
		_fontRef = _fontTest;
	}
	else
	{
		_fontTest = 0L;
		_fontRef = fontRef;
	}
#else
	_fontRef = fontRef;
#endif

	// デフォルトのフォントサイズを取得
	if(fontsize > 0)
	{
		_wFont = _hFont = _fontsize = fontsize;
	}
	else
	{
		_wFont = _hFont = _fontsize = fontRef->getCharDefaultWidth();
	}
	_wFont /= 2;	// 半角で計算する

	// 矩形を作成
	ASSERT(_rectFrame == 0L);	// 未作成のはず
	_rectFrame = new RectF32();
	_rectAvailable = new RectF32();

	// スクロール位置を作成
	_ptScrollTrg = new PointF32();
	_ptScrollCur = new PointF32();
	_ptScrollMax = new PointF32();
	_rateScrlIntp = /* 【2012/03/26】0.75f; */ /*0.5f*/ 0.4f;

	// 各種色を作成（デフォルト色を設定）
	_colTxtFore = new ColorU8(0, 0, 0, 255);		// テキスト色
	_colTxtActive = new ColorU8(255, 0, 0, 255);	// アクティブテキスト色
	_colSelectedBg = new ColorU8(255, 255, 0, 255);	// 選択背景色

	// モードを設定
	_mode = mode;

	// ドラッグ開始テキスト位置を作成
	_tposDragStart = new TxtPos();

	// その他変数初期化
	_cflags = cflags;
	_cntFrame = 1;
	_xTracking = TRACKING_X_DEFAULT * (_fontsize / (f32)FONTSIZE_DEFAULT);
	_yTracking = TRACKING_Y_DEFAULT * (_fontsize / (f32)FONTSIZE_DEFAULT);
	_isChanged = false;

	// 矩形設定
	setRectangle(rect);

	// 文字列を保存
	_txt = txtEntr;
	_txtRef = _txt;
	_cursor.reset(_txtRef);

	// デフォルトで有効化
	setEnable(true);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void TxtBox::destroy()
{
	// ドラッグ開始テキスト位置を削除
	delete _tposDragStart;
	_tposDragStart = 0L;

	// 文字列を削除
	if(_txt != 0L)
	{
		delete _txt;
		_txt = 0L;
	}

	// 各種色を削除
	delete _colTxtFore;
	_colTxtFore = 0L;
	delete _colTxtActive;
	_colTxtActive = 0L;
	delete _colSelectedBg;
	_colSelectedBg = 0L;

	// ポイントを削除
	delete _ptScrollMax;
	_ptScrollMax = 0L;
	delete _ptScrollCur;
	_ptScrollCur = 0L;
	delete _ptScrollTrg;
	_ptScrollTrg = 0L;

	// 矩形を削除
	delete _rectAvailable;
	_rectAvailable = 0L;
	delete _rectFrame;
	_rectFrame = 0L;

#if TEST_OS_FONT
	if(_fontTest != 0L)
	{
		_fontTest->destroy();
		delete _fontTest;
		_fontTest = 0L;
	}
#endif
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void TxtBox::exec(ExecRes* res, const ExecCtx* ec)
{
	if(!isEnable())	{	return;	}

	// スクロール補間
	if(_ptScrollCur->x() != _ptScrollTrg->x())
	{
		_ptScrollCur->x() = (_ptScrollTrg->x() * (1.0f - _rateScrlIntp)) + (_ptScrollCur->x() * _rateScrlIntp);
	}
	if(_ptScrollCur->y() != _ptScrollTrg->y())
	{
		_ptScrollCur->y() = (_ptScrollTrg->y() * (1.0f - _rateScrlIntp)) + (_ptScrollCur->y() * _rateScrlIntp);
	}

	_cntFrame++;
}

/*---------------------------------------------------------------------*//**
	キーボードイベント送信
**//*---------------------------------------------------------------------*/
bool TxtBox::execKeyboardEvent(TxtBoxExecRes* res, const Keyboard* kbd)
{
	if(!isEnable())	{	return false;	}
	if(_isDrag)		{	return false;	}	// ドラッグ操作中は無視

	bool isReacted = false;
	for(u32 i = 0; i < Keyboard::NUM_KEY; i++)
	{
		if(kbd->isRepeatTrigger(i))
		{
			if(execKeyboardKey(res, i, kbd->isOn(Keyboard::SYSKEY_SHIFT), kbd->isOn(Keyboard::SYSKEY_CTRL), kbd->isOn(Keyboard::SYSKEY_ALT)))
			{
				isReacted = true;
			}
		}
	}
	return isReacted;
}

/*---------------------------------------------------------------------*//**
	キーボードキー実行
**//*---------------------------------------------------------------------*/
bool TxtBox::execKeyboardKey(TxtBoxExecRes* res, s32 key, bool isShift, bool isCtrl, bool isAlt)
{
	if(!isEnable())	{	return false;	}
	if(_isDrag)		{	return false;	}	// ドラッグ操作中は無視

	ASSERT(_txt != 0L);
	if(key == Keyboard::SYSKEY_BS)				// ▼ BackSpace
	{
		if(TFW_IS_FLAG(_aflags, AF_READ_ONLY))	{	return false;	}	// 読み込み禁止時は無視

		if(_sel.isSelected())	// 選択テキストがある場合
		{
			removeSelectedText();
			_sel.reset();						// 選択解除
		}
		else if(_cursor._tpos->getCharIndex() > 0)
		{
			_cursor.left();
			_txt->remove(_cursor._tpos, 1);
			_isChanged = true;			// 変更された
		}
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置によるスクロール
	}
	else if(key == Keyboard::SYSKEY_DEL)		// ▼ Delete
	{
		if(TFW_IS_FLAG(_aflags, AF_READ_ONLY))	{	return false;	}	// 読み込み禁止時は無視

		if(_sel.isSelected())	// 選択テキストがある場合
		{
			removeSelectedText();
			_sel.reset();						// 選択解除
		}
		else if(_cursor._tpos->getCharIndex() < _txt->getCharCount())
		{
			_txt->remove(_cursor._tpos, 1);
			_isChanged = true;			// 変更された
		}
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置によるスクロール
	}
	else if(key == Keyboard::SYSKEY_LEFT)		// ▼ ←
	{
		if(!isShift && _sel.isSelected())			{	_sel.reset();	return true;			}
		if(isShift && _sel._tposStart->isInvalid())	{	_sel._tposStart->copy(_cursor._tpos);	}
		_cursor.left();
		if(isShift)									{	_sel._tposEnd->copy(_cursor._tpos);		}
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置によるスクロール
	}
	else if(key == Keyboard::SYSKEY_RIGHT)		// ▼ →
	{
		if(!isShift && _sel.isSelected())			{	_sel.reset();	return true;			}
		if(isShift && _sel._tposStart->isInvalid())	{	_sel._tposStart->copy(_cursor._tpos);	}
		_cursor.right();
		if(isShift)									{	_sel._tposEnd->copy(_cursor._tpos);		}
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置によるスクロール
	}
	else if(key == Keyboard::SYSKEY_UP)			// ▼ ↑
	{
		if(!isShift && _sel.isSelected())			{	_sel.reset();							}
		if(isShift && _sel._tposStart->isInvalid())	{	_sel._tposStart->copy(_cursor._tpos);	}
		_cursor.up();
		if(isShift)									{	_sel._tposEnd->copy(_cursor._tpos);		}
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置によるスクロール
	}
	else if(key == Keyboard::SYSKEY_DOWN)		// ▼ ↓
	{
		if(!isShift && _sel.isSelected())			{	_sel.reset();							}
		if(isShift && _sel._tposStart->isInvalid())	{	_sel._tposStart->copy(_cursor._tpos);	}
		_cursor.down();
		if(isShift)									{	_sel._tposEnd->copy(_cursor._tpos);		}
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置によるスクロール
	}
	else if(key == Keyboard::SYSKEY_HOME)		// ▼ Home
	{
		if(!isShift && _sel.isSelected())			{	_sel.reset();							}
		if(isShift && _sel._tposStart->isInvalid())	{	_sel._tposStart->copy(_cursor._tpos);	}
		if(isCtrl)
		{
			_cursor.head();
		}
		else
		{
			_cursor.home();
		}
		if(isShift)									{	_sel._tposEnd->copy(_cursor._tpos);		}
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置によるスクロール
	}
	else if(key == Keyboard::SYSKEY_END)		// ▼ End
	{
		if(!isShift && _sel.isSelected())			{	_sel.reset();							}
		if(isShift && _sel._tposStart->isInvalid())	{	_sel._tposStart->copy(_cursor._tpos);	}
		if(isCtrl)
		{
			_cursor.tail();
		}
		else
		{
			_cursor.end();
		}
		if(isShift)									{	_sel._tposEnd->copy(_cursor._tpos);		}
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置によるスクロール
	}
	else if(key == Keyboard::SYSKEY_PGUP)		// ▼ PageUp
	{
		if(!isShift && _sel.isSelected())			{	_sel.reset();							}
		if(isShift && _sel._tposStart->isInvalid())	{	_sel._tposStart->copy(_cursor._tpos);	}
		_cursor.pageup(_lineDisp);
		if(isShift)									{	_sel._tposEnd->copy(_cursor._tpos);		}
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置によるスクロール
	}
	else if(key == Keyboard::SYSKEY_PGDOWN)		// ▼ PageDown
	{
		if(!isShift && _sel.isSelected())			{	_sel.reset();							}
		if(isShift && _sel._tposStart->isInvalid())	{	_sel._tposStart->copy(_cursor._tpos);	}
		_cursor.pagedown(_lineDisp);
		if(isShift)									{	_sel._tposEnd->copy(_cursor._tpos);		}
		drawTextProc(0L, true, 0L);	// 描画シミュレーション
		scrollByCursor();			// カーソル位置によるスクロール
	}
	else if((key == Keyboard::SYSKEY_SHIFT)		// ▼ Shift
		|| (key == Keyboard::SYSKEY_CTRL)		// ▼ Ctrl
		|| (key == Keyboard::SYSKEY_ALT))		// ▼ Alt
	{
		return false;
	}
	else										// ▼ その他の文字キー
	{
		bool isReacted = false;
		// 特殊コマンド
		if(isCtrl || isAlt)
		{
			isReacted = execCommandKeyboardKey(res, key, isShift, isCtrl, isAlt);
		}
		// 文字入力
		if(!isReacted)
		{
			if(TFW_IS_FLAG(_aflags, AF_READ_ONLY))	{	return false;	}	// 読み込み禁止時は無視

			bool isSucceeded = false;
			if(_sel.isSelected())	// 選択テキストがある場合
			{
				if(key == '\t')	// タブキーの場合
				{
					isSucceeded = indentSelectedTextByTabKey();		// 選択行をインデント
				}
				else
				{
					removeSelectedText();				// 選択部分削除
					_sel.reset();						// 選択解除
					TxtChar tcKey((char)key);
					isSucceeded = _txt->insert(_cursor._tpos, &tcKey);
				}
			}
			else
			{
				TxtChar tcKey((char)key);
				isSucceeded = _txt->insert(_cursor._tpos, &tcKey);
			}
			if(isSucceeded)
			{
				_isChanged = true;
				_cursor.right();
				drawTextProc(0L, true, 0L);	// 描画シミュレーション
				scrollByCursor();			// カーソル位置によるスクロール
			}
		}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	コマンド系キーボードキー実行
**//*---------------------------------------------------------------------*/
bool TxtBox::execCommandKeyboardKey(TxtBoxExecRes* res, s32 key, bool isShift, bool isCtrl, bool isAlt)
{
	if(!isEnable())	{	return false;	}
	if(_isDrag)		{	return false;	}	// ドラッグ操作中は無視

	bool isReacted = false;
	if(isCtrl)
	{
		switch(key)
		{
		case 'c':	// コピー
			res->setGeneralValue1(CMD_COPY);
			isReacted = true;
			break;
		case 'v':	// ペースト
			if(TFW_IS_FLAG(_aflags, AF_READ_ONLY))	{	return false;	}	// 読み込み禁止時は無視
			res->setGeneralValue1(CMD_PASTE);
			isReacted = true;
			break;
		case 'a':	// 全選択
			{
				if(_sel.isSelected())	{	_sel.reset();	}
				_cursor.head();
				_sel._tposStart->copy(_cursor._tpos);
				_cursor.tail();
				_sel._tposEnd->copy(_cursor._tpos);
				drawTextProc(0L, true, 0L);	// 描画シミュレーション
				scrollByCursor();			// カーソル位置によるスクロール
				isReacted = true;
			}
			break;
		}
	}
	return isReacted;
}

/*---------------------------------------------------------------------*//**
	タッチパネルイベント送信
**//*---------------------------------------------------------------------*/
bool TxtBox::execTouchPanelEvent(TxtBoxExecRes* res, const TouchPanel* tcp)
{
	if(!isEnable())	{	return false;	}

	bool isReacted = false;
	if(tcp->isTouching())
	{
		PointF32 pt(S32_MIN, S32_MIN);
		tcp->getTouchPosition(&pt, 0);
		if(_rectAvailable->isPointIn(&pt))
		{
			TxtPos tpos;
			calcDispPosToTxtPos(&tpos, (f32)pt.x(), (f32)pt.y());	// テキスト位置を算出する
			if(!tpos.isInvalid())	// テキストを発見できた場合
			{
				// カーソル移動
				if(_cursor._tpos->getCharIndex() < tpos.getCharIndex())
				{
					do
					{
						if(!_cursor.right())
						{
							break;
						}
					}
					while(_cursor._tpos->getCharIndex() < tpos.getCharIndex());
					isReacted = true;
				}
				else if(_cursor._tpos->getCharIndex() > tpos.getCharIndex())
				{
					do
					{
						if(!_cursor.left())
						{
							break;
						}
					}
					while(_cursor._tpos->getCharIndex() > tpos.getCharIndex());
					isReacted = true;
				}

				// ドラッグ操作
				if(_isDrag)
				{
					if(!_tposDragStart->eq(_cursor._tpos))	// 位置移動している場合のみ
					{
						if(_sel._tposStart->isInvalid())
						{
							_sel._tposStart->copy(_tposDragStart);
						}
						_sel._tposEnd->copy(_cursor._tpos);
					}
					isReacted = true;
				}
				else
				{
					_tposDragStart->copy(_cursor._tpos);
					_sel.reset();
					_isDrag = true;
					isReacted = true;
				}
			}
			else if(!_isDrag)
			{
				// 選択範囲リセット
				_sel.reset();
			}
		}

		if(_isDrag && !_rectAvailable->isPointIn(&pt))	// 範囲外ドラッグ
		{
			const f32 DT = 0.1f;	// 座標大きさからスクロール量への変換値
			s32 xs = 0;
			s32 ys = 0;
			if(_rectAvailable->left() > pt.x())			{	xs = (s32)((pt.x() - _rectAvailable->left()) * DT);		}
			else if(_rectAvailable->right() <= pt.x())	{	xs = (s32)((pt.x() - _rectAvailable->right()) * DT);	}
			if(_rectAvailable->top() > pt.y())			{	ys = (s32)((pt.y() - _rectAvailable->top()) * DT);		}
			else if(_rectAvailable->bottom() <= pt.y())	{	ys = (s32)((pt.y() - _rectAvailable->bottom()) * DT);	}
			if(xs != 0)
			{
				_ptScrollTrg->x() += xs;
			}
			if(ys != 0)
			{
				_ptScrollTrg->y() += ys;
			}
			if((xs != 0) || (ys != 0))
			{
				adjustScrollPos();
			}
		}
	}
	else
	{
		// ドラッグを終える
		if(_isDrag)
		{
			_isDrag = false;
			isReacted = true;
		}
	}

	return isReacted || _isDrag;
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void TxtBox::draw(const RenderCtx* rc)
{
	if(!isEnable())	{	return;	}
	
	drawTextProc(rc, false, 0L);

	// 色指定を戻す
	///	rc->getRenderer()->setSolidColor(255, 255, 255, 255);	// 色を戻す
}

/*---------------------------------------------------------------------*//**
	テキスト描画プロシージャ
	※ 描画シミュレーション時にも使う
**//*---------------------------------------------------------------------*/
void TxtBox::drawTextProc(const RenderCtx* rc, bool isSimu, SearchPos* sp)
{
	if(_txtRef == 0L)	{	return;	}
	//u32 lenTxt = _txtRef->getCharCount();

	f32 xBase = (f32)(_rectAvailable->left() - _ptScrollCur->x());
	f32 yBase = (f32)(_rectAvailable->top() - _ptScrollCur->y());
	f32 x = xBase;
	f32 y = yBase;
	f32 xMax = 0;
	f32 yMax = 0;
	s32 cntLine = 0;
	s32 cntCol = 0;

	// 選択範囲
	TxtPos* tposStart = 0L;
	TxtPos* tposEnd = 0L;
	if((rc != 0L) && _sel.isSelected())
	{
		_sel.getNormalizePos(&tposStart, &tposEnd);
	}
	
	// 色指定
	bool isActiveColor = false;
	if(rc != 0L)
	{
		rc->getRenderer()->setSolidColor(_colTxtFore);
	}

	// 文字描画処理ループ
	const RectF32* rectDraw = isSimu ? _rectAvailable : _rectFrame;	// 描画矩形
	bool isFixedWidth = TFW_IS_FLAG(_cflags, CF_FIXED_WIDTH_FONT);
	bool isLf = false;
	u32 idxChar = 0;
	f32 wc;
	f32 hc = (f32)_hFont;
	f32 wt;
	f32 xo = 0.0f;
	f32 xr;
	f32 yb;
	f32 chip[4];
	TxtPos tposPrevForSearch;
	for(TxtIterator it = _txtRef->iterator(); it.has(); it.next())
	{
		TxtChar tc = it.cur();
		//u16 bytes = tc.getBytes();
		u32 c = tc.getCode();
		isLf = Cursor::isNlCode(c);
		#if TEST_UTF8	// UTF-8 test
			if(_txtRef->getEncode() == TxtDef::ENC_UTF8)
			{
				c = TxtUtils::convUnicodeToSjisCode(c);
			}
		#endif

		// 描画する文字の横幅を得る
		wc = _wFont + _xTracking;
		if(c == '\t')
		{
			s32 colRest = COL_TAB - (cntCol & (COL_TAB - 1));
			wc *= colRest;
			cntCol += colRest - 1;
		}
		else if(!isLf)
		{
			wt = _fontRef->getCharWidth(c, _fontsize) + _xTracking;

			if(isFixedWidth)	// 固定幅
			{
				if(TxtUtils::isFullWidth(_txtRef->getEncode(), &tc))	// 全角文字
				{
					cntCol++;
					wc *= 2.0f;
				}
				xo = (wc - wt) * 0.5f;
			}
			else				// 可変幅
			{
				if(TxtUtils::isFullWidth(_txtRef->getEncode(), &tc))	// 全角文字
				{
					cntCol++;
				}
				wc = wt;
			}
		}

		// テキスト位置検索
		if(sp != 0L)
		{
			if((x <= sp->_x) && (sp->_x < (x + wc)) && (y <= sp->_y) && (sp->_y < (y + hc)))
			{
				sp->_w = wc;
				sp->_h = hc;
				sp->_tpos->copy(&it);
				return;	// 見つかったので検索終了
			}
			else if(y > sp->_y)
			{
				sp->_w = 0;
				sp->_h = 0;
				sp->_tpos->copy(&tposPrevForSearch);	// 最後の検索文字の位置を返す
				return;	// その後は検索不要
			}
			tposPrevForSearch.copy(&it);
		}

		// 文字の描画
		yb = y + hc + _yTracking;
		if((yb >= rectDraw->top()) || isSimu)
		{
			xr = x + wc + _xTracking;
			if(((xr >= rectDraw->left()) && (x < rectDraw->right())) || isSimu)
			{
				// 各種描画
				if(rc != 0L)
				{
					// 描画範囲外による欠け計算
					chip[0] = (x < rectDraw->left()) ? (rectDraw->left() - x) : 0.0f;		// left
					chip[1] = (y < rectDraw->top()) ? (rectDraw->top() - y) : 0.0f;			// top
					chip[2] = (xr > rectDraw->right()) ? (xr - rectDraw->right()) : 0.0f;	// right
					chip[3] = (yb > rectDraw->bottom()) ? (yb - rectDraw->bottom()) : 0.0f;	// bottom

					// 読み上げモード時のアクティブ文字表示色変更
					if((_mode == MODE_SRCREAD) && (_idxAtvBegin <= it.getRawIndex()) && (it.getRawIndex() < _idxAtvEnd))
					{
						if(!isActiveColor)
						{
							rc->getRenderer()->setSolidColor(_colTxtActive);
							isActiveColor = true;
						}
					}
					else
					{
						if(isActiveColor)
						{
							rc->getRenderer()->setSolidColor(_colTxtFore);
							isActiveColor = false;
						}
					}

					// 選択範囲描画
					if(tposStart != 0L)
					{
						ASSERT(tposEnd != 0L);
						if((tposStart->getCharIndex() <= idxChar) && (idxChar < tposEnd->getCharIndex()))
						{
							RectF32 rectChipped(x + chip[0], y + chip[1], wc - (chip[0] + chip[2]), hc - (chip[1] + chip[3]));
							if(!rectChipped.isEmpty())
							{
								RendererUtils::draw2dColorRect(rc->getRenderer(), &rectChipped, _colSelectedBg);
								rc->getRenderer()->setSolidColor(_colTxtFore);
							}
						}
					}

					// 文字描画
					if((c != ' ') && (c != '\t') && !isLf)	// 空白以外
					{
						_fontRef->drawChipped(c, x + xo, y, _fontsize, chip[0], chip[1], chip[2], chip[3], rc);
					}
				}

				// カーソル描画
				if(idxChar == _cursor._tpos->getCharIndex())
				{
					drawCursor(rc, x, y, chip, xBase, yBase, isSimu);
				}
			}

			x += wc;
			cntCol++;
			
			if(x > xMax)	{	xMax = x;	}
			if(y > yMax)	{	yMax = y;	}
		}
		
		// 改行判定
		if(isLf)
		{
			x = xBase;
			y += hc + _yTracking;
			cntCol = 0;
			cntLine++;
		}
		if((y > rectDraw->bottom()) && !isSimu)	// シミュレーション時は最後まで行う
		{
			break;
		}

		idxChar++;
	}
	// カーソル描画
	if(idxChar == _cursor._tpos->getCharIndex())
	{
		chip[0] = 0.0f;		// left
		chip[1] = 0.0f;		// top
		chip[2] = 0.0f;		// right
		chip[3] = 0.0f;		// bottom
		drawCursor(rc, x, y, chip, xBase, yBase, isSimu);
	}

	// テキスト位置検索
	if(sp != 0L)
	{
		if(y <= sp->_y)
		{
			if(_txtRef->isValidCharPos(&tposPrevForSearch) && _txtRef->nextPos(&tposPrevForSearch))	// 一つ進める
			{
				sp->_w = 0;
				sp->_h = 0;
				sp->_tpos->copy(&tposPrevForSearch);	// 最後の検索文字の位置を返す
				return;	// その後は検索不要
			}
		}
	}

	// 最大高さに文字の高さ分を加算
	yMax += hc;

	// シミュレーション時にスクロール最大値の保存
	if(isSimu)
	{
		s32 wDefaultChar = (s32)(_wFont + _xTracking);
		s32 hDefaultChar = (s32)(_hFont + _yTracking);
		_ptScrollMax->x() = (s32)(xMax - xBase) - (_rectAvailable->w() - (wDefaultChar * 2));
		_ptScrollMax->y() = (s32)(yMax - yBase) - (_rectAvailable->h() - hDefaultChar);
		if(_ptScrollMax->x() < 0)	{	_ptScrollMax->x() = 0;	}
		if(_ptScrollMax->y() < 0)	{	_ptScrollMax->y() = 0;	}
		adjustScrollPos();
	}
	#if defined(_DEBUG) && 0
		if(rc != 0L)
		{
			STRACE("scroll:{%d/%d,%d/%d}\n", _ptScrollTrg->x(), _ptScrollMax->x(), _ptScrollTrg->y(), _ptScrollMax->y());
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	カーソル描画
**//*---------------------------------------------------------------------*/
void TxtBox::drawCursor(const RenderCtx* rc, f32 x, f32 y, f32 chip[4], f32 xBase, f32 yBase, bool isSimu)
{
	// 描画
	if((_mode != MODE_SRCREAD) && (rc != 0L) && (_cntFrame & 8) == 0)
	{
		_fontRef->drawChipped('|', x, y, _fontsize, chip[0], chip[1], chip[2], chip[3], rc);
	}
	// シミュレーション時にカーソル X 座標を保存
	if(isSimu)
	{
		_cursor._xDisp = x - xBase;
	}
}

/*---------------------------------------------------------------------*//**
	スクロール位置補正
**//*---------------------------------------------------------------------*/
void TxtBox::adjustScrollPos()
{
	if(_ptScrollTrg->x() < 0)
	{
		_ptScrollTrg->x() = 0;
	}
	else if(_ptScrollTrg->x() > _ptScrollMax->x())
	{
		_ptScrollTrg->x() = _ptScrollMax->x();
	}
	if(_ptScrollTrg->y() < 0)
	{
		_ptScrollTrg->y() = 0;
	}
	else if(_ptScrollTrg->y() > _ptScrollMax->y())
	{
		_ptScrollTrg->y() = _ptScrollMax->y();
	}
}

/*---------------------------------------------------------------------*//**
	カーソル位置によるスクロール処理
**//*---------------------------------------------------------------------*/
void TxtBox::scrollByCursor()
{
	f32 wChar = _wFont + _xTracking;
	f32 hChar = _hFont + _yTracking;
	f32 x = _cursor._xDisp;
	f32 y = _cursor._line * hChar;
	f32 width = _rectAvailable->w() - (wChar * 2);
	f32 height = _rectAvailable->h() - hChar;
	if(x < _ptScrollTrg->x())
	{
		_ptScrollTrg->x() = x;
	}
	else if((x + wChar) > (_ptScrollTrg->x() + width))
	{
		_ptScrollTrg->x() = (x + wChar) - width;
	}
	if(y < _ptScrollTrg->y())
	{
		_ptScrollTrg->y() = y;
	}
	else if((y + hChar) > (_ptScrollTrg->y() + height))
	{
		_ptScrollTrg->y() = (y + hChar) - height;
	}
	adjustScrollPos();
}

/*---------------------------------------------------------------------*//**
	選択されたテキストを削除する
**//*---------------------------------------------------------------------*/
void TxtBox::removeSelectedText()
{
	if(!_sel.isSelected())	{	return;	}

	// 選択範囲を得る
	TxtPos* tposStart;
	TxtPos* tposEnd;
	_sel.getNormalizePos(&tposStart, &tposEnd);

	// カーソルを選択開始位置に移動させる
	if(_cursor._tpos->getCharIndex() > tposStart->getCharIndex())
	{
		do
		{
			if(!_cursor.left())
			{
				break;
			}
		}
		while(_cursor._tpos->getCharIndex() > tposStart->getCharIndex());
	}
	else if(_cursor._tpos->getCharIndex() < tposStart->getCharIndex())
	{
		do
		{
			if(!_cursor.right())
			{
				break;
			}
		}
		while(_cursor._tpos->getCharIndex() < tposStart->getCharIndex());
	}

	// 選択範囲を削除
	TxtPosArray tpa(_cursor._tpos);
	if(!_txt->remove(&tpa, tposStart, tposEnd->getCharIndex() - tposStart->getCharIndex()))
	{
		return;
	}

	_isChanged = true;			// 変更された
}

/*---------------------------------------------------------------------*//**
	タブキーによる選択テキストの行頭インデント
**//*---------------------------------------------------------------------*/
bool TxtBox::indentSelectedTextByTabKey()
{
	if(!_sel.isSelected())	{	return false;	}
	TxtPos* tposStart;
	TxtPos* tposEnd;
	_sel.getNormalizePos(&tposStart, &tposEnd);

	// 開始位置の行頭へ移動
	TxtPos tposWk(*tposStart);
	TxtPos tposLh(tposWk);
	TxtChar tcWk;
	while(_txt->isValidCharPos(&tposWk))
	{
		if(!_txt->prevPos(&tposWk, &tcWk))
		{
			break;
		}
		if(Cursor::isNlCode(tcWk.getCode()))
		{
			break;
		}
		tposLh.copy(&tposWk);
	}

	// タブを挿入
	VcString strTab('\t');
	Txt* txtTab = _txt->newSameType();
	txtTab->setSingleString(&strTab);
	TxtPosArray tpa(_cursor._tpos);
	if(!_txt->insert(&tpa, &tposLh, txtTab))
	{
		delete txtTab;
		return false;
	}

	// 次の行等へ移動
	tposWk.copy(&tposLh);
	while(_txt->isValidCharPos(&tposLh) && tposLh.le(tposEnd))
	{
		if(!_txt->nextPos(&tposLh, &tcWk))
		{
			break;
		}
		if(Cursor::isNlCode(tcWk.getCode()))
		{
			if((tposLh.getCharIndex() - tposWk.getCharIndex()) > 2)
			{
				// タブを挿入
				TxtPosArray tpa(_cursor._tpos);
				if(!_txt->insert(&tpa, &tposLh, txtTab))
				{
					delete txtTab;
					return false;
				}
				tposWk.copy(&tposLh);
			}
		}
	}

	delete txtTab;
	_isChanged = true;			// 変更された
	return true;
}

/*---------------------------------------------------------------------*//**
	クリックした場所のテキスト位置を得る
**//*---------------------------------------------------------------------*/
bool TxtBox::calcDispPosToTxtPos(TxtPos* pos, f32 x, f32 y)
{
	SearchPos sp;
	sp._x = x;
	sp._y = y;
	drawTextProc(0L, true, &sp);
	pos->copy(sp._tpos);
	return sp._w != 0;
}


//==========================================================================
// TxtBox::Cursor メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TxtBox::Cursor::Cursor()
	: _txtRef(0L)
	, _tpos(new TxtPos())
	, _line(0)
	, _col(0)
	, _colt(0)
	, _xDisp(0)
{
}

/*---------------------------------------------------------------------*//**
	コピー コンストラクタ
**//*---------------------------------------------------------------------*/
TxtBox::Cursor::Cursor(const Cursor& src)
	: _tpos(new TxtPos())
{
	copy(&src);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TxtBox::Cursor::~Cursor()
{
	delete _tpos;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void TxtBox::Cursor::copy(const Cursor* src)
{
	this->_txtRef = src->_txtRef;
	this->_tpos->copy(src->_tpos);
	this->_line = src->_line;
	this->_col = src->_col;
	this->_colt = src->_colt;
	this->_xDisp = src->_xDisp;
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void TxtBox::Cursor::reset(const Txt* txtRef)
{
	_txtRef = txtRef;
	_tpos->set(0, 0);
	_line = 0;
	_col = 0;
	_colt = 0;
	_xDisp = 0;
}

/*---------------------------------------------------------------------*//**
	左（←）キー
**//*---------------------------------------------------------------------*/
bool TxtBox::Cursor::left()
{
	if((_tpos->getCharIndex() < 1) || (_txtRef->getCharCount() < _tpos->getCharIndex()))
	{
		return false;
	}
	bool isEnd = false;
	TxtChar tcCur;
	do
	{
		if(!_txtRef->prevPos(_tpos, &tcCur))
		{
			isEnd = true;	// 端まで行った
			break;
		}
		_col--;

		u32 cCur = tcCur.getCode();
		if(cCur == '\t')
		{
			_col -= COL_TAB - 1;
			TxtPos tposWk(*_tpos);
			for(int i = 0; i < (COL_TAB - 1); i++)
			{
				if(tposWk.getCharIndex() <= 0)	{	break;	}
				if(!_txtRef->prevPos(&tposWk, &tcCur))	{	break;	}
				cCur = tcCur.getCode();
				if(isNlCode(cCur) || (cCur == '\t'))
				{
					break;
				}
				_col++;
			}
		}
		else if(isNlCode(cCur))
		{
			_line--;
			if(_tpos->getCharIndex() >= 1)
			{
				TxtPos tposWk(*_tpos);
				_txtRef->prevPos(&tposWk);
				getLineCharNum(&_col, 0L, &tposWk, COL_TAB);
			}
			else
			{
				_col = 0;
			}
		}
		else if(TxtUtils::isFullWidth(_txtRef->getEncode(), &tcCur))
		{
			_col--;
		}
		else if((cCur < 0x20) || (cCur == 0x7f))	// それ以外の認識できない文字
		{
			_col++;	// スキップ
			continue;
		}
		break;
	}
	while((1 <= _tpos->getCharIndex()) && (_tpos->getCharIndex() <= _txtRef->getCharCount()));
	_colt = _col;	// 目的となる行番号を保存
	return !isEnd;
}

/*---------------------------------------------------------------------*//**
	右（→）キー
**//*---------------------------------------------------------------------*/
bool TxtBox::Cursor::right()
{
	if(!_txtRef->isValidCharPos(_tpos))
	{
		return false;
	}
	bool isEnd = false;
	TxtChar tcCur;
	do
	{
		if(!_txtRef->nextPos(_tpos, &tcCur))
		{
			isEnd = true;	// 端まで行った
			break;
		}
		_col++;

		u32 cCur = tcCur.getCode();
		if(cCur == '\t')
		{
			_col += COL_TAB - 1 - ((_col - 1) & (COL_TAB - 1));
		}
		else if(isNlCode(cCur))
		{
			_line++;
			_col = 0;
		}
		else if(TxtUtils::isFullWidth(_txtRef->getEncode(), &tcCur))
		{
			_col++;
		}
		else if((cCur < 0x20) || (cCur == 0x7f))	// それ以外の認識できない文字
		{
			_col--;	// スキップ
			continue;
		}
		break;
	}
	while(_txtRef->isValidCharPos(_tpos));
	_colt = _col;	// 目的となる行番号を保存
	return !isEnd;
}

/*---------------------------------------------------------------------*//**
	上（↑）キー
**//*---------------------------------------------------------------------*/
bool TxtBox::Cursor::up()
{
	if(_line <= 0)	{	return false;	}
	bool isEnd = false;
	u32 lineBk = _line;
	///u32 colBk = _col;
	u32 coltBk = _colt;
	TxtPos tposBk2;
	s32 lineBk2;
	s32 colBk2;
	// まずは前の行まで戻る
	while(true)
	{
		if(!left())
		{
			// 文字列の最初まで到達した
			isEnd = true;
			goto END;	// 終了処理へ
		}
		if(_line <= (lineBk - 1))
		{
			break;
		}
	}
	ASSERT(_line == (lineBk - 1));
	// 目的値に達するまで戻る
	tposBk2.copy(_tpos);
	lineBk2 = _line;
	colBk2 = _col;
	while(true)
	{
		if(_col == coltBk)	// 目的値に達した
		{
			goto END;
		}
		if(_col < coltBk)	// 目的値を過ぎた
		{
			_tpos->copy(&tposBk2);
			_line = lineBk2;
			_col = colBk2;
			goto END;	// 終了処理へ
		}
		tposBk2.copy(_tpos);
		lineBk2 = _line;
		colBk2 = _col;
		if(!left())
		{
			// 文字列の最初まで到達した
			isEnd = true;
			goto END;	// 終了処理へ
		}
		if((lineBk >= 2) && (_line <= (lineBk - 2)))	// 更に前の行まで進んでしまった
		{
			// 前の行に比べて行の長さが足りないときの処理
			_tpos->copy(&tposBk2);
			_line = lineBk2;
			_col = colBk2;
			goto END;	// 終了処理へ
		}
	}

END:	// 終了処理
	// 目的値を戻して終了
	_colt = coltBk;
	return !isEnd;
}

/*---------------------------------------------------------------------*//**
	下（↓）キー
**//*---------------------------------------------------------------------*/
bool TxtBox::Cursor::down()
{
	bool isEnd = false;
	u32 lineBk = _line;
	///u32 colBk = _col;
	u32 coltBk = _colt;
	TxtPos tposBk2;
	u32 lineBk2;
	u32 colBk2;
	// まずは次の行まで進める
	while(true)
	{
		if(!right())
		{
			// 文字列の最後まで到達した
			isEnd = true;
			goto End;	// 終了処理へ
		}
		if(_line >= (lineBk + 1))
		{
			break;
		}
	}
	ASSERT(_line == (lineBk + 1));
	// 目的値に達するまで戻る
	while(true)
	{
		if(_col >= coltBk)	// 目的値に達した
		{
			goto End;	// 終了処理へ
		}
		tposBk2.copy(_tpos);
		lineBk2 = _line;
		colBk2 = _col;
		if(!right())
		{
			// 文字列の最後まで到達した
			isEnd = true;
			goto End;	// 終了処理へ
		}
		if(_line >= (lineBk + 2))	// 更に次の行まで進んでしまった
		{
			// 前の行に比べて行の長さが足りないときの処理
			_tpos->copy(&tposBk2);
			_line = lineBk2;
			_col = colBk2;
			goto End;	// 終了処理へ
		}
	}

End:	// 終了処理
	// 目的値を戻して終了
	_colt = coltBk;
	return !isEnd;
}

/*---------------------------------------------------------------------*//**
	Home キー
**//*---------------------------------------------------------------------*/
void TxtBox::Cursor::home()
{
	while(_col != 0)
	{
		if(!left())		// ←
		{
			break;
		}
	}
}

/*---------------------------------------------------------------------*//**
	End キー
**//*---------------------------------------------------------------------*/
void TxtBox::Cursor::end()
{
	u32 lineTrg = _line;
	u32 lineBk, colBk, coltBk;
	TxtPos tposBk;
	while(true)
	{
		tposBk.copy(_tpos);
		lineBk = _line;
		colBk = _col;
		coltBk = _colt;
		if(!right())	// →
		{
			// カーソルが動かなかった
			break;
		}
		if(lineTrg != _line)		// 行が変わった
		{
			_tpos->copy(&tposBk);
			_line = lineBk;
			_col = colBk;
			_colt = coltBk;
			break;
		}
	}
}

/*---------------------------------------------------------------------*//**
	カーソルをテキストの先頭へ移動
**//*---------------------------------------------------------------------*/
void TxtBox::Cursor::head()
{
	reset(_txtRef);
}

/*---------------------------------------------------------------------*//**
	カーソルをテキストの先頭へ移動
**//*---------------------------------------------------------------------*/
void TxtBox::Cursor::tail()
{
	while(true)
	{
		if(!right())	// →
		{
			// カーソルが動かなくなるまで
			break;
		}
	}
}

/*---------------------------------------------------------------------*//**
	Page Up キー
**//*---------------------------------------------------------------------*/
void TxtBox::Cursor::pageup(u32 line)
{
	u32 lineTrg = (_line > line) ? _line - line : 0;
	while(true)
	{
		if(!up())		// ↑
		{
			break;
		}
		if(lineTrg == _line)	// 目的行に達した
		{
			break;
		}
	}
}

/*---------------------------------------------------------------------*//**
	Page Down キー
**//*---------------------------------------------------------------------*/
void TxtBox::Cursor::pagedown(u32 line)
{
	u32 lineTrg = _line + line;
	while(true)
	{
		if(!down())		// ↓
		{
			break;
		}
		if(lineTrg == _line)	// 目的行に達した
		{
			break;
		}
	}
}

/*---------------------------------------------------------------------*//**
	行のカラム数，文字数を得る
**//*---------------------------------------------------------------------*/
bool TxtBox::Cursor::getLineCharNum(u32* numCol, u32* numChar, const TxtPos* tpos, u32 tabcnt) const
{
	// 不正なインデックスをチェック
	if(!_txtRef->isValidCharPos(tpos))
	{
		if(numCol != 0L)	{	*numCol = 0;	}
		if(numChar != 0L)	{	*numChar = 0;	}
		return false;
	}

	TxtChar tcWk;
	TxtPos tposCurWk;
	TxtPos tposPrevWk(*tpos);

	// 行頭のインデックスを求める
	while(true)
	{
		tposCurWk.copy(&tposPrevWk);
		if(!_txtRef->prevPos(&tposPrevWk, &tcWk))
		{
			break;
		}
		if(isNlCode(tcWk.getCode()))
		{
			break;
		}
	}

	tposPrevWk.copy(&tposCurWk);
	u32 col = 0;

	// 行末まで
	while(true)
	{
		if(!_txtRef->nextPos(&tposCurWk, &tcWk))
		{
			break;
		}
		if(isNlCode(tcWk.getCode()))									{	break;									}
		else if(tcWk.getCode() == '\t')									{	col += tabcnt - (col & (tabcnt - 1));	}
		else if(TxtUtils::isFullWidth(_txtRef->getEncode(), &tcWk))		{	col += 2;								}
		else															{	col++;									}
	}

	if(numCol != 0L)	{	*numCol = col;	}
	if(numChar != 0L)	{	*numChar = tposCurWk.getCharIndex() - tposPrevWk.getCharIndex();	}

	return true;
}

/*---------------------------------------------------------------------*//**
	改行コード判定
**//*---------------------------------------------------------------------*/
bool TxtBox::Cursor::isNlCode(u32 code)
{
	return (code == Txt::NLCODE_CR) || (code == Txt::NLCODE_LF) || (code == Txt::NLCODE_CRLF);
}


//==========================================================================
// TxtBox::Selector メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TxtBox::Selector::Selector()
	: _tposStart(new TxtPos())
	, _tposEnd(new TxtPos())
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TxtBox::Selector::~Selector()
{
	delete _tposStart;
	delete _tposEnd;
}

/*---------------------------------------------------------------------*//**
	選択を解除
**//*---------------------------------------------------------------------*/
void TxtBox::Selector::reset()
{
	_tposStart->invalidate();
	_tposEnd->invalidate();
}

/*---------------------------------------------------------------------*//**
	選択されていないかを得る
**//*---------------------------------------------------------------------*/
bool TxtBox::Selector::isSelected() const
{
	return !_tposEnd->isInvalid();
}

/*---------------------------------------------------------------------*//**
	開始・終了が正規化された位置を得る
**//*---------------------------------------------------------------------*/
void TxtBox::Selector::getNormalizePos(TxtPos** tposStart, TxtPos** tposEnd) const
{
	if(_tposStart->gt(_tposEnd))
	{
		*tposStart = _tposEnd;
		*tposEnd = _tposStart;
		return;
	}

	*tposStart = _tposStart;
	*tposEnd = _tposEnd;
}


//==========================================================================
// TxtBox::SearchPos メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TxtBox::SearchPos::SearchPos()
	: _x(F32_PMAX)
	, _y(F32_PMAX)
	, _w(0.0f)
	, _h(0.0f)
	, _tpos(new TxtPos())
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TxtBox::SearchPos::~SearchPos()
{
	delete _tpos;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
