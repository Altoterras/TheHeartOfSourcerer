/***********************************************************************//**
 *  PanelText.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/11.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PanelText.h"

// Friends
#include "../g2d/font/Font.h"
#include "../g2d/StringDrawer.h"
#include "../gcmn/Renderer.h"
#include "../lib/Rect.h"
#include "../lib/Color.h"
#include "../string/VcString.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PanelText メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	読み上げアニメーション中かを得る
**//*---------------------------------------------------------------------*/
bool PanelText::isReadingAnimation() const
{
	return _strdraw->isReadingAnimationEnabled() && !_strdraw->isReadingAnimationEnd();
}

/*---------------------------------------------------------------------*//**
	縦スクロールアニメーション中かを得る
**//*---------------------------------------------------------------------*/
bool PanelText::isVerticalScrollAnimation() const
{
	return _strdraw->isVerticalScrollAnimationEnabled() && !_strdraw->isVerticalScrollAnimationEnd();
}

/*---------------------------------------------------------------------*//**
	設定されたテキストが複数ページに渡るかどうかを得る
**//*---------------------------------------------------------------------*/
bool PanelText::isMultiplePagesText() const
{
	return _linesOnePage < _numLine;
}

/*---------------------------------------------------------------------*//**
	フォントを取得する
**//*---------------------------------------------------------------------*/
Font* PanelText::getFont() const
{
	return _strdraw->getFont();
}

/*---------------------------------------------------------------------*//**
	矩形設定
**//*---------------------------------------------------------------------*/
void PanelText::setRectangle(const RectF32* rect)
{
	_strdraw->setRectangle(rect);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	テキストを設定
**//*---------------------------------------------------------------------*/
void PanelText::setText(const VcString* text, bool isAdd)
{
	// テキスト設定
	if(isAdd)
	{
		*_strTxt += *text;
		_strdraw->updateString();
	}
	else
	{	
		*_strTxt = *text;
		_strdraw->setString(_strTxt);
	}
	
	// 描画シミュレーション
	_strdraw->drawTest(_sizeTxt);

	// ライン数を得る
	_numLine = (s32)(_sizeTxt->h() / (f32)_strdraw->getCharDefaultHeight());
	
	// ページリセット
	_lineScrollCur = 0;

	// 終了フラグリセット
	_isScrollDone = false;

	// オートスクロール
	if(_isAutoScroll)
	{
		// 最終ページへスクロール
		_lineScrollCur = _numLine - _linesOnePage;
		if(_lineScrollCur < 0)	{	_lineScrollCur = 0;	}
		_strdraw->setDrawStartStrLine(_lineScrollCur);
	}
}

/*---------------------------------------------------------------------*//**
	次の行を表示する
	読み上げアニメーション中の場合は、アニメーションをスキップする
**//*---------------------------------------------------------------------*/
void PanelText::nextPage()
{
	if(_numLine <= 0)	{	return;	}

	bool isEndThisPage = false;

	if(_strdraw->isReadingAnimationEnabled())			// 読み上げアニメーション有効
	{
		if(!_strdraw->isReadingAnimationEnd())			// 読み上げアニメーション中
		{
			_strdraw->finishReadingAnimation();			// 読み上げアニメーションを終了させる
			isEndThisPage = true;	// このページを終えた
		}
	}

	if(_strdraw->isVerticalScrollAnimationEnabled())	// 縦スクロールアニメーション有効
	{
		if(!_strdraw->isVerticalScrollAnimationEnd())	// 縦スクロールアニメーション中
		{
			_strdraw->finishVerticalScrollAnimation();	// 縦スクロールアニメーションを終了させる
			isEndThisPage = true;	// このページを終えた
		}
	}

	if(isEndThisPage)
	{
		return;
	}

	if((_lineScrollCur + _linesOnePage) < _numLine)
	{
		_lineScrollCur += _linesOnePage;
		_strdraw->setDrawStartStrLine(_lineScrollCur);
	}
	else
	{
		_isScrollDone = true;
	}
}

/*---------------------------------------------------------------------*//**
	読み上げアニメーションをするかどうかを設定する
**//*---------------------------------------------------------------------*/
void PanelText::setReadingAnimation(bool isEnable, f32 speed, f32 interval)
{
	_strdraw->setReadingAnimation(isEnable, speed, interval);
}

/*---------------------------------------------------------------------*//**
	立てスクロールアニメーションをするかどうかを設定する
**//*---------------------------------------------------------------------*/
void PanelText::setVerticalScrollAnimation(bool isEnable, f32 speed)
{
	_strdraw->setVerticalScrollAnimation(isEnable, speed);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PanelText::PanelText()
	: _fontRef(0L)
	, _strdraw(0L)
	, _strTxt(0L)
	, _color(0L)
	, _sizeTxt(0L)
	, _numLine(0)
	, _linesOnePage(0)
	, _lineScrollCur(0)
	, _isScrollDone(false)
	, _isAutoScroll(false)
	, _isUnused(false)
	, _flags(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PanelText::~PanelText()
{
	ASSERT(_strdraw == 0L);
	ASSERT(_strTxt == 0L);
	ASSERT(_color == 0L);
	ASSERT(_sizeTxt == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PanelText::create(Font* fontRef, s32 fontsize, const RectF32* rect, const ColorU8* color)
{
	ASSERT(_strTxt == 0L);	// 未作成のはず

	// 値保存
	_fontRef = fontRef;

	// テキスト文字列の作成
	_strTxt = new VcString();

	// 文字色を作成
	_color = new ColorU8(*color);
	
	// テキスト描画の作成
	_strdraw = new StringDrawer();
	if(!_strdraw->create())
	{
		return false;
	}
	_strdraw->setRectangle(rect);
	_strdraw->setFont(fontRef);
	_strdraw->setFontSize(fontsize);
	_strdraw->setReadingAnimation(false, 1.0f, 0.0f);
	_strdraw->setVerticalScrollAnimation(false, 1.0f);

	// テキスト描画サイズを作成
	_sizeTxt = new SizeF32();
	
	// １ページあたりの描画可能行数を得る
	_linesOnePage = (s32)(_strdraw->getRectangle()->h() / (_strdraw->getCharDefaultHeight() + _strdraw->getTrackingY()));

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PanelText::destroy()
{
	// テキスト描画サイズを削除
	delete _sizeTxt;
	_sizeTxt = 0L;

	// テキスト描画の削除
	if(_strdraw != 0L)
	{
		_strdraw->destroy();
		delete _strdraw;
		_strdraw = 0L;
	}

	// 文字色を削除
	delete _color;
	_color = 0L;

	// テキスト文字列の削除
	delete _strTxt;
	_strTxt = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void PanelText::exec(ExecRes* res, const ExecCtx* ec)
{
	_strdraw->exec(ec->getDeltaFrame());

	if(_isAutoScroll)	// オートスクロールの場合
	{
		if(_lineScrollCur > 0)
		{
			if(_strdraw->isVerticalScrollAnimationEnabled() && _strdraw->isVerticalScrollAnimationEnd())
			{
				s32 indexDrawStart = _strdraw->getDrawStartStringIndex();	// スクロール結果としての文字列描画開始インデックス
				_strTxt->remove(0, indexDrawStart);							// 不要な行を削除する
				_lineScrollCur = 0;											// 縦スクロール位置もリセット
				_strdraw->setString(_strTxt);								// 文字列再設定
			}
		}
	}
	else				// 手動スクロールの場合
	{
		// スクロールが終わっているかどうか調べる
		if(!_isScrollDone && (_numLine > 0))
		{
			if(!_strdraw->isReadingAnimationEnabled() || (_strdraw->isReadingAnimationEnabled() && _strdraw->isReadingAnimationEnd()))
			{
				if((_lineScrollCur + _linesOnePage) >= _numLine)
				{
					_isScrollDone = true;
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void PanelText::draw(const RenderCtx* rc)
{
	Renderer* rdr = rc->getRenderer();

	// アライン
	f32 xOffset = 0.0f;
	f32 yOffset = 0.0f;
	if(TFW_IS_FLAG(_flags, F_ALIGN_CENTER))
	{
		xOffset = (_strdraw->getRectangle()->w() - _sizeTxt->w()) * 0.5f;
	}
	if(TFW_IS_FLAG(_flags, F_ALIGN_RIGHT))
	{
		xOffset = _strdraw->getRectangle()->w() - _sizeTxt->w();
	}
	if(TFW_IS_FLAG(_flags, F_VALIGN_MIDDLE))
	{
		yOffset = (_strdraw->getRectangle()->h() - _sizeTxt->h()) * 0.5f;
	}
	if(TFW_IS_FLAG(_flags, F_VALIGN_BOTTOM))
	{
		yOffset = _strdraw->getRectangle()->h() - _sizeTxt->h();
	}

	// 背後描画
	if(TFW_IS_FLAG(_flags, F_DRAW__MASK_BACK))
	{
		f32 xOfsBk = 0.0f;
		f32 yOfsBk = 0.0f;

		// 影文字
		if(TFW_IS_FLAG(_flags, F_DRAW__MASK_SHADOW))
		{
			rdr->setSolidColor(0, 0, 0, 255);
			if(TFW_IS_FLAG(_flags, F_DRAW_LEFT_SHADOW))			{	xOfsBk = -1;	}
			else if(TFW_IS_FLAG(_flags, F_DRAW_RIGHT_SHADOW))	{	xOfsBk = 1;		}
			if(TFW_IS_FLAG(_flags, F_DRAW_TOP_SHADOW))			{	yOfsBk = -1;	}
			else if(TFW_IS_FLAG(_flags, F_DRAW_BOTTOM_SHADOW))	{	yOfsBk = 1;		}
		}

		// 光沢文字
		if(TFW_IS_FLAG(_flags, F_DRAW__MASK_GLOS))
		{
			rdr->setSolidColor(255, 255, 255, 255);
			if(TFW_IS_FLAG(_flags, F_DRAW_LEFT_GLOS))			{	xOfsBk = -1;	}
			else if(TFW_IS_FLAG(_flags, F_DRAW_RIGHT_GLOS))		{	xOfsBk = 1;		}
			if(TFW_IS_FLAG(_flags, F_DRAW_TOP_GLOS))			{	yOfsBk = -1;	}
			else if(TFW_IS_FLAG(_flags, F_DRAW_BOTTOM_GLOS))	{	yOfsBk = 1;		}
		}

		_strdraw->draw(rc, xOffset + xOfsBk, yOffset + yOfsBk);
	}

	// 文字色の設定
	if(_isUnused)
	{
		rdr->setSolidColor(_color->r() >> 1, _color->g() >> 1, _color->b() >> 1, _color->a() >> 1);
	}
	else
	{
		rdr->setSolidColor(_color);
	}

	// 文字列描画
	_strdraw->draw(rc, xOffset, yOffset);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
