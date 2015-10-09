/***********************************************************************//**
 *	StringDrawer.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/11.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StringDrawer.h"

// Friends
#include "font/Font.h"
#include "../collection/Array.h"
#include "../framemod/RenderCtx.h"
#include "../gcmn/View.h"
#include "../gcmn/Renderer.h"
#include "../lib/Color.h"
#include "../lib/Rect.h"
#include "../lib/Point.h"
#include "../lib/Size.h"
#include "../string/StringUtils.h"
#include "../txt/implement/SjisTxt.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define FONTSIZE_DEFAULT		(16)
#define TRACKING_X_DEFAULT		(-1)
#define TRACKING_Y_DEFAULT		(0)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StringDrawer::TagAnalyzer::KeyValue メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*-----------------------------------------------------------------*//**
	コンストラクタ
**//*-----------------------------------------------------------------*/
StringDrawer::TagAnalyzer::KeyValue::KeyValue(VcString* key)
{
	_key = new VcString(key);
	_value = 0L;
}

/*-----------------------------------------------------------------*//**
	コンストラクタ
**//*-----------------------------------------------------------------*/
StringDrawer::TagAnalyzer::KeyValue::KeyValue(VcString* key, VcString* val)
{
	_key = new VcString(key);
	_value = new VcString(val);
}

/*-----------------------------------------------------------------*//**
	デストラクタ
**//*-----------------------------------------------------------------*/
StringDrawer::TagAnalyzer::KeyValue::~KeyValue()
{
	delete _key;
	delete _value;
}

//======================================================================
// StringDrawer::TagAnalyzer メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*-----------------------------------------------------------------*//**
	属性数を得る
**//*-----------------------------------------------------------------*/
u32 StringDrawer::TagAnalyzer::getAtrbNum() const
{
	if(_arrKeyValue == 0L)	{	return 0;	}
	return _arrKeyValue->getCount();
}

/*-----------------------------------------------------------------*//**
	属性キーを得る
**//*-----------------------------------------------------------------*/
VcString* StringDrawer::TagAnalyzer::getAtrbKey(s32 idx) const
{
	if(_arrKeyValue == 0L)	{	return 0L;	}
	return _arrKeyValue->getAt(idx)->_key;
}

/*-----------------------------------------------------------------*//**
	属性値を得る
**//*-----------------------------------------------------------------*/
VcString* StringDrawer::TagAnalyzer::getAtrbValue(s32 idx) const
{
	if(_arrKeyValue == 0L)	{	return 0L;	}
	return _arrKeyValue->getAt(idx)->_value;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*-----------------------------------------------------------------*//**
	処理を占有しているかどうか
 
	@retval	true	占有中
	@retval	true	非占有
**//*-----------------------------------------------------------------*/
bool StringDrawer::TagAnalyzer::isHook() const
{
	return _step != STEP_NULL;
}

/*-----------------------------------------------------------------*//**
	１文字解析

	@param	c		文字
	@retval	true	解析終了
	@retval false	未処理、もしくは解析中
**//*-----------------------------------------------------------------*/
bool StringDrawer::TagAnalyzer::pushChar(char c)
{
	switch(_step)
	{
	case STEP_NULL:
		// { が見つかったらタグに入る
		if(c == '{')
		{
			if(_arrKeyValue == 0L)
			{
				// ここで初めてオブジェクトインスタンスの確保
				_arrKeyValue = new Array<KeyValue*>(true, true);
				_tagstr = new VcString();
				_strb1 = new VcString();
				_strb2 = new VcString();
			}
			else
			{
				// KeyValue 配列の全削除
				_arrKeyValue->removeAll();
			}

			_tagstr->set("");
			_step = STEP_TAGSTR;
		}
		break;
	case STEP_TAGSTR:
		if(c == ' ')
		{
			_tagstr->set(_strb1);
			_strb1->emptyEasy();
			_step = STEP_KEYSTR;
		}
		else if(c == '}')
		{
			_tagstr->set(_strb1);
			_strb1->emptyEasy();
			_step = STEP_END;
		}
		else
		{
			_strb1->add(c);
		}
		break;
	case STEP_KEYSTR:
		if(c == '=')
		{
			_step = STEP_VALUESTR;
		}
		else if(c == '}')
		{
			_arrKeyValue->add(new KeyValue(_strb1));
			_strb1->emptyEasy();
			_step = STEP_END;
		}
		else
		{
			_strb1->add(c);
		}
		break;
	case STEP_VALUESTR:
		if(c == ' ')
		{
			_arrKeyValue->add(new KeyValue(_strb1, _strb2));
			_strb1->emptyEasy();
			_strb2->emptyEasy();
			_step = STEP_KEYSTR;
		}
		else if(c == '}')
		{
			_arrKeyValue->add(new KeyValue(_strb1, _strb2));
			_strb1->emptyEasy();
			_strb2->emptyEasy();
			_step = STEP_END;
		}
		else
		{
			_strb2->add(c);
		}
		break;
	case STEP_END:
		_step = STEP_NULL;
		return true;
	}
	
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*-----------------------------------------------------------------*//**
	コンストラクタ
**//*-----------------------------------------------------------------*/
StringDrawer::TagAnalyzer::TagAnalyzer()
	: _step(STEP_NULL)
	, _tagstr(0L)
	, _arrKeyValue(0L)
	, _strb1(0L)
	, _strb2(0L)
{
}

/*-----------------------------------------------------------------*//**
	デストラクタ
**//*-----------------------------------------------------------------*/
StringDrawer::TagAnalyzer::~TagAnalyzer()
{
	if(_arrKeyValue != 0L)
	{
		delete _arrKeyValue;
		delete _tagstr;
		delete _strb1;
		delete _strb2;

		_arrKeyValue = 0L;
	}
}

//==========================================================================
// StringDrawer メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	デフォルトの文字幅を得る
**//*---------------------------------------------------------------------*/
s32 StringDrawer::getCharDefaultWidth()
{
	if(_fontsize != 0)	{	return _fontsize;	}
	if(_fontRef == 0L)	{	return 0;			}
	return _fontRef->getCharDefaultWidth();
}

/*---------------------------------------------------------------------*//**
	デフォルトの文字高さを得る
**//*---------------------------------------------------------------------*/
s32 StringDrawer::getCharDefaultHeight()
{
	if(_fontsize != 0)	{	return _fontsize;	}
	if(_fontRef == 0L)	{	return 0;			}
	return _fontRef->getCharDefaultHeight();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	矩形を設定する
**//*---------------------------------------------------------------------*/
void StringDrawer::setRectangle(const RectF32* rect)
{
	_rectExt->copy(rect);
}

/*---------------------------------------------------------------------*//**
	文字列を設定する
		※	strRef は内部でコピーしないので、外部（呼び出し元）で
			実体を保持すること
**//*---------------------------------------------------------------------*/
void StringDrawer::setString(const CStringBase* strRef)
{
	_strTrgRef = strRef;
	_lenText = _strTrgRef->getLength();
	_indexDrawStart = 0;
	_yScroll = 0.0f;
	
	// 読み上げアニメーションのセットアップ
	if(_rdanim._isAnim)
	{
		_rdanim.reset();
		calcReadingAnimMaxCount();	// 読み上げアニメーションの最大カウンタを計算する
	}
	// 縦スクロールアニメーションのリセット
	_vtanim.reset();
}

/*---------------------------------------------------------------------*//**
	文字列更新通知
**//*---------------------------------------------------------------------*/
void StringDrawer::updateString()
{
	_lenText = _strTrgRef->getLength();

	// 読み上げアニメーションのセットアップ
	if(_rdanim._isAnim)
	{
		calcReadingAnimMaxCount();	// 読み上げアニメーションの最大カウンタを計算する
	}
}

/*---------------------------------------------------------------------*//**
	フォントを設定する
**//*---------------------------------------------------------------------*/
void StringDrawer::setFont(Font* font)
{
	_fontRef = font;
}

/*---------------------------------------------------------------------*//**
	フォントサイズを設定する
**//*---------------------------------------------------------------------*/
void StringDrawer::setFontSize(s32 fontsize)
{
	_fontsize = fontsize;
}

/*---------------------------------------------------------------------*//**
	トラッキングを設定する
**//*---------------------------------------------------------------------*/
void StringDrawer::setTracking(f32 xTracking, f32 yTracking)
{
	_xTracking = xTracking;
	_yTracking = yTracking;
}

/*---------------------------------------------------------------------*//**
	手動スクロール値を設定する（現在、縦スクロールのみ）
**//*---------------------------------------------------------------------*/
void StringDrawer::setScrollPosition(f32 yScroll)
{
	if(_vtanim._isAnim)
	{
		_vtanim._yOffsetTrg = yScroll;
	}
	else
	{
		_yScroll = yScroll;
	}
}

/*---------------------------------------------------------------------*//**
	描画開始文字インデックスを設定する
**//*---------------------------------------------------------------------*/
void StringDrawer::setDrawStartStrIndex(s32 index)
{
	_indexDrawStart = index;
	
	// 読み上げアニメーションのセットアップ
	_rdanim.reset();
	calcReadingAnimMaxCount();	// 読み上げアニメーションの最大カウンタを計算する
}

/*---------------------------------------------------------------------*//**
	描画開始文字列行を設定する
**//*---------------------------------------------------------------------*/
void StringDrawer::setDrawStartStrLine(s32 line)
{
	//s32 indexDrawStartBk = _indexDrawStart;

	DrawProcResult res;
	drawProc(
		&res,				// 終了インデックスが知りたいので DrawProcResult を渡す
		0L,					// シミュレーションのため、レンダラは使わない
		_strTrgRef,			// 対象文字列
		true,				// 終了インデックスが知りたいので、指定矩形でブレイクしない
		0,					// 終了インデックスが知りたいので、文字列の最初から処理する
		_lenText - 1,		// 終了インデックスが知りたいので、文字列の最後まで処理する
		0.0f,				// 最終文字アルファ値はシミュレーション時は無意味
		line,				// 指定行でブレイクさせる
		0, 0, 0, 0	);		// オフセット無し
	_indexDrawStart = (res._isLastLf && ((res._idxLast + 1) < _lenText)) ? res._idxLast + 1 : res._idxLast;

	// 縦スクロールアニメーション設定
	if(_vtanim._isAnim)
	{
		if(_vtanim._indexDrawStartNext != _indexDrawStart)
		{
			_vtanim._indexDrawStartNext = _indexDrawStart;
			_vtanim._yOffsetNextLine = - (f32)(res._height);
			_vtanim._yOffsetTrg = _vtanim._yOffsetNextLine - _vtanim._yOffsetPrevLine;
		}
	}
	
	// 読み上げアニメーションのセットアップ
	_rdanim.reset();
	calcReadingAnimMaxCount();	// 読み上げアニメーションの最大カウンタを計算する
}

/*---------------------------------------------------------------------*//**
	文字読み上げアニメーションを設定する

	@param	isEnable	文字読み上げアニメーションをするかどうか
	@param	speed		読み上げ速度（経過フレームに対するカウントレート）
						※デフォルト値は 1.0f
	@param	interval	次の文字までのインターバルカウント
**//*---------------------------------------------------------------------*/
void StringDrawer::setReadingAnimation(bool isEnable, f32 speed, f32 interval)
{
	_rdanim._isAnim = isEnable;
	_rdanim._rateAnimSpeedSetting = speed > 0.0f ? speed : 1.0f;	// 0 以下は不正値
	_rdanim._fcntIntvlSetting = interval;
}

/*---------------------------------------------------------------------*//**
	縦スクロールアニメーションを設定する

	@param	isEnable	縦スクロールアニメーションをするかどうか
	@param	speed		読み上げ速度（経過フレームに対するカウントレート）
						※デフォルト値は 1.0f
**//*---------------------------------------------------------------------*/
void StringDrawer::setVerticalScrollAnimation(bool isEnable, f32 speed)
{
	_vtanim._isAnim = isEnable;
	_vtanim._rateAnimSpeedSetting = speed > 0.0f ? speed : 1.0f;	// 0 以下は不正値
}

/*---------------------------------------------------------------------*//**
	文字読み上げアニメーションが終了しているかを得る
**//*---------------------------------------------------------------------*/
bool StringDrawer::isReadingAnimationEnd()
{
	return _rdanim._isAnim && (_rdanim._cntAnim >= _rdanim._cntMax);
}

/*---------------------------------------------------------------------*//**
	縦スクロールアニメーションが終了したかを得る
**//*---------------------------------------------------------------------*/
bool StringDrawer::isVerticalScrollAnimationEnd()
{
	return _vtanim._isAnim && (_yScroll == _vtanim._yOffsetTrg);
}

/*---------------------------------------------------------------------*//**
	文字読み上げアニメーションを終了する
**//*---------------------------------------------------------------------*/
void StringDrawer::finishReadingAnimation()
{
	_rdanim._cntAnim = _rdanim._cntMax;
}

/*---------------------------------------------------------------------*//**
	縦スクロールアニメーションを終了する
**//*---------------------------------------------------------------------*/
void StringDrawer::finishVerticalScrollAnimation()
{
	_yScroll = _vtanim._yOffsetTrg;
}

/*---------------------------------------------------------------------*//**
	文字読み上げアニメーションの最大カウンタを計算する
**//*---------------------------------------------------------------------*/
void StringDrawer::calcReadingAnimMaxCount()
{
	if(!_rdanim._isAnim)	{	return;	}
	DrawProcResult res;
	drawProc(
		&res,				// 終了インデックスが知りたいので DrawProcResult を渡す
		0L,					// シミュレーションのため、レンダラは使わない
		_strTrgRef,			// 対象文字列
		false,				// 指定矩形でループをブレイクする
		_indexDrawStart,	// 実際の描画文字数を得たいので、開始位置は実際と同じ
		_lenText - 1,		// 指定矩形でブレイクするので、文字列最後まで処理するように指定する
		0.0f,				// 最終文字アルファ値はシミュレーション時は無意味
		-1,					// 指定行で描画を終了しない
		0, 0, 0, 0	);		// オフセット無し
	_rdanim._cntMax = (f32)(res._idxLast - _indexDrawStart) + 1.0f;	// +1.0f は最後の文字を α=1.0f まで描画するためのもの
	ASSERT(((_indexDrawStart + _rdanim._cntMax) <= _lenText) || (_lenText == 0));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StringDrawer::StringDrawer()
	: _strTrgRef(0L)
	, _lenText(0)
	, _fontRef(0L)
	, _fontsize(0)
	, _xTracking(0.0f)
	, _yTracking(0.0f)
	, _yScroll(0.0f)
	, _isHyphenation(true)	// 禁則処理はデフォルトで ON に
	, _rectExt(0L)
	, _indexDrawStart(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StringDrawer::~StringDrawer()
{
	ASSERT(_rectExt == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool StringDrawer::create()
{
	return create(0L, FONTSIZE_DEFAULT, 0L, 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool StringDrawer::create(Font* font, s32 fontsize, const RectF32* rect, const CStringBase* str)
{
	_rectExt = new RectF32();
	_indexDrawStart = 0;
	_rdanim.clear();
	_fontsize = fontsize;
	_xTracking = TRACKING_X_DEFAULT * (_fontsize / (f32)FONTSIZE_DEFAULT);
	_yTracking = TRACKING_Y_DEFAULT * (_fontsize / (f32)FONTSIZE_DEFAULT);
	
	if(font != 0L)	{	setFont(font);		}
	if(rect != 0L)	{	setRectangle(rect);	}
	if(str != 0L)	{	setString(str);		}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void StringDrawer::destroy()
{
	delete _rectExt;
	_rectExt = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StringDrawer::exec(f32 frameDelta)
{
	if(_rdanim._isAnim)	// 読み上げアニメーション時
	{
		if(_rdanim._fcntIntvl > 0)	// インターバル中
		{
			_rdanim._fcntIntvl -= frameDelta;
		}
		else
		{
			// 読み上げアニメーションをカウントする
			if(_rdanim._cntAnim < _rdanim._cntMax)
			{
				_rdanim._cntAnim += frameDelta * _rdanim._rateAnimSpeedSetting;
				if(_rdanim._cntAnim > _rdanim._cntMax)
				{
					_rdanim._cntAnim = _rdanim._cntMax;
				}
			}
			
			_rdanim._fcntIntvl = _rdanim._fcntIntvlSetting;	// インターバルに入る
		}
	}

	if(_vtanim._isAnim)	// 縦スクロールアニメーション時
	{
		// アニメーション縦スクロール位置更新
		static const f32 ERROR_F = 0.001f;
		bool isJustAnimEnd = false;
		if(_yScroll > _vtanim._yOffsetTrg)
		{
			_yScroll -= frameDelta * _vtanim._rateAnimSpeedSetting;
			if((_yScroll - ERROR_F) <= _vtanim._yOffsetTrg)	{	isJustAnimEnd = true;	}
		}
		else if(_yScroll < _vtanim._yOffsetTrg)
		{
			_yScroll += frameDelta * _vtanim._rateAnimSpeedSetting;
			if((_yScroll + ERROR_F) >= _vtanim._yOffsetTrg)	{	isJustAnimEnd = true;	}
		}
		// アニメーション終了処理
		if(isJustAnimEnd)
		{
			if(_vtanim._indexDrawStartPrev != _vtanim._indexDrawStartNext)	// 描画開始位置変更による縦スクロールアニメーションであった場合
			{
				_vtanim._indexDrawStartPrev = _vtanim._indexDrawStartNext;
				_vtanim._yOffsetPrevLine = _vtanim._yOffsetNextLine;
				_yScroll = _vtanim._yOffsetTrg = 0.0f;
			}
			else
			{
				_yScroll = _vtanim._yOffsetTrg;
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画シミュレーション
**//*---------------------------------------------------------------------*/
void StringDrawer::drawTest(SizeF32* sizeRes)
{
	DrawProcResult res;
	drawProc(
		&res,				// サイズが知りたいので DrawProcResult を渡す
		0L,					// シミュレーションのため、レンダラは使わない
		_strTrgRef,			// 対象文字列
		true,				// 全体の描画サイズを得たいので指定矩形で処理を終了しない
		0,					// シミュレーションのため、文字列最初から処理する
		_lenText - 1,		// シミュレーションのため、文字列最後まで処理する
		0.0f,				// 最終文字アルファ値はシミュレーション時は無意味
		-1,					// 指定行で描画を終了しない
		0, 0, 0, 0	);		// オフセット無し
	if(sizeRes != 0)
	{
		sizeRes->set(res._width, res._height);
	}
}

/*---------------------------------------------------------------------*//**
	現在の設定のまま別の文字列を描画シミュレーション
**//*---------------------------------------------------------------------*/
void StringDrawer::drawTest(SizeF32* sizeRes, const CStringBase* strForTest)
{
	DrawProcResult res;
	drawProc(
		&res,							// サイズが知りたいので DrawProcResult を渡す
		0L,								// シミュレーションのため、レンダラは使わない
		strForTest,						// 引数で指定された対象文字列
		true,							// 全体の描画サイズを得たいので指定矩形で処理を終了しない
		0,								// シミュレーションのため、文字列最初から処理する
		strForTest->getLength() - 1,	// シミュレーションのため、文字列最後まで処理する
		0.0f,							// 最終文字アルファ値はシミュレーション時は無意味
		-1,								// 指定行で描画を終了しない
		0, 0, 0, 0);					// オフセット無し
	if(sizeRes != 0)
	{
		sizeRes->set(res._width, res._height);
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void StringDrawer::draw(const RenderCtx* rc)
{
	draw(rc, 0.0f, 0.0f);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void StringDrawer::draw(const RenderCtx* rc, f32 xOffset, f32 yOffset)
{
	s32 indexDrawStart = _indexDrawStart;
	s32 indexDrawEnd;
	f32 alphaLastChar;
	if(_rdanim._isAnim)	// 読み上げアニメーション時
	{
		s32 cntAnimI = (s32)_rdanim._cntAnim;
		alphaLastChar = _rdanim._cntAnim - cntAnimI;
		indexDrawEnd = _indexDrawStart + cntAnimI;
		if(indexDrawEnd >= _lenText)	{	indexDrawEnd = _lenText - 1;	}	// _cntMax は最後の文字を alphaLastChar=1.0f まで描画するため、+1.0f してある
	}
	else
	{
		indexDrawEnd = _lenText - 1;
		alphaLastChar = 1.0f;	// 非アニメ時は不透明で描く
	}

	f32 yOffsetText = _yScroll;
	if(_vtanim._isAnim)	// 縦スクロールアニメーション時
	{
		if(_yScroll != _vtanim._yOffsetTrg)	// アニメーション中
		{
			indexDrawStart = _vtanim._indexDrawStartPrev;
		}
	}

	drawProc(
		0L,					// 実際の描画では結果は不要
		rc,					// 実際に描画するため、レンダラを渡す
		_strTrgRef,			// 対象文字列
		false,				// 指定矩形で描画を終了する
		indexDrawStart,		// 開始位置を指定
		indexDrawEnd,		// 終了位置を指定
		alphaLastChar,		// 最終文字アルファ値
		-1,					// 指定行で描画を終了しない
		xOffset,			// 全体の X オフセット
		yOffset,			// 全体の Y オフセット
		0.0f,				// テキストの X オフセット
		yOffsetText	);		// テキストの Y オフセット
}

/*---------------------------------------------------------------------*//**
	描画処理
**//*---------------------------------------------------------------------*/
void StringDrawer::drawProc(DrawProcResult* res, const RenderCtx* rc, const CStringBase* str, bool isOverRectY, s32 indexDrawStart, s32 indexDrawEnd, f32 alphaLastChar, s32 lineBreak, f32 xOffsetThis, f32 yOffsetThis, f32 xOffsetTxt, f32 yOffsetTxt)
{
	if(str == 0L)		{ return; }
	if(_fontRef == 0L)	{ return; }
	if(_lenText == 0)	{ return; }
	
	s32 wFont = getCharDefaultWidth();
	f32 hFont = (f32)getCharDefaultHeight();
	f32 ivhFont = 0.0f;
	f32 xBase = _rectExt->left() + xOffsetThis;
	f32 yBase = _rectExt->top() + yOffsetThis;
	f32 xRight = xBase + _rectExt->width();
	f32 yBottom = yBase + _rectExt->height();
	f32 x = xBase + xOffsetTxt;
	f32 y = yBase + yOffsetTxt;
	f32 xMax = - F32_PMAX;
	f32 yMax = - F32_PMAX;
	
	// タグ解析
	TagAnalyzer taz;

	// 行数カウントは indexDrawStart がゼロのときしか正常に動作しないという制限がある
	ASSERT((lineBreak == -1) || (indexDrawStart == 0));
	s32 cntLine = 0;
	if(lineBreak == 0)
	{
		res->_idxLast = 0;
		return;
	}
	
	// 文字描画処理ループ
	const char* charsStr = str->getRaw();
	s32 idxLast = indexDrawStart;
	bool isLf = false;
	char cCur = 0;
	char cPrev = 0;
	f32 wc;
	u16 cw;
	bool isFinishTag, isWideChar;
	for(s32 idx = indexDrawStart; idx <= indexDrawEnd; idx++)
	{
		cCur = charsStr[idx];
		if(cCur == '\0')	{ break; }
		isLf = (cCur == '\n');
		
		// タグ処理
		isFinishTag = taz.pushChar(cCur);
		if(taz.isHook())	{ continue; }
		
		// タグ反映
		if(isFinishTag)
		{
			drawEffectTag(rc, &taz);
		}

		// 描画する文字の横幅を得る
		wc = 0;
		if(!isLf)
		{
			wc = _fontRef->getCharWidth(cCur, _fontsize);
		}

		// ワイド文字で取得
		cw = (u8)cCur;
		isWideChar = false;
		if(SjisTxt::isByteSjisFst(cCur) && (idx <= indexDrawEnd))	// Shift-JIS の最初の文字の場合
		{
			cw = TFW_U8_TO_U16(cCur, charsStr[idx + 1]);
			isWideChar = true;
		}

		// 改行判定
		bool isNl = ((x + wc + _xTracking) > xRight) || isLf;

		// 禁則処理
		if(_isHyphenation && isNl)
		{
			switch(cw)
			{
			case 0x8141:	// '、'
			case 0x8142:	// '。'
			case 0x8143:	// '，'
			case 0x2C:		// ','
			case 0x2E:		// '.'
				isNl = false;
				break;
			}
		}
		
		// 再改行判定
		if(isNl)
		{
			// ハイフネーション
			if(rc != 0L)
			{
				if(y >= _rectExt->top())
				{
					if((('A' <= cPrev) && (cPrev <= 'Z')) || (('a' <= cPrev) && (cPrev <= 'z')))	// 前の文字もアルファベット
					{
						if((('A' <= cCur) && (cCur <= 'Z')) || (('a' <= cCur) && (cCur <= 'z')))	// 今回の文字もアルファベットであったら
						{
							_fontRef->draw('-', x, y, wFont, rc);
						}
					}
				}
			}

			x = xBase;
			y += hFont + _yTracking;
			cntLine++;
			
			// 指定行ブレイク
			if(lineBreak == cntLine)
			{
				idxLast = idx;
				break;
			}
		}
		if(!isOverRectY && ((y + hFont + _yTracking) > (yBottom + 0.01f)))	// + 0.01f は浮動小数点誤差対策
		{
			break;
		}

		// 実際の描画
		if(!isLf)
		{
			if(rc != 0L)
			{
				// 最終文字のアルファ値設定
				if(idx == indexDrawEnd)
				{
					if((0.0f <= alphaLastChar) && (alphaLastChar < 1.0f))
					{
						rc->getRenderer()->setColorAlpha((u8)(alphaLastChar * 255.0f));
					}
				}

				if(y < yBase)	// 矩形から上部にはみ出している場合
				{
					if((y + hFont) >= _rectExt->top())
					{
						if(ivhFont == 0.0f)	{	ivhFont = 1.0f / hFont;	}
						f32 yClipped = _rectExt->top() - y;
						ColorU8 colTop(*rc->getRenderer()->getSolidColor());
						ColorU8 colBottom(colTop);
						u8 alphaBk = colBottom.a();
						///colTop.a() = (u8)((1.0f - (yClipped * ivhFont)) * alphaBk);
						///colBottom.a() = (u8)((1.0f - (yClipped / hFont)) * alphaBk);
						f32 afTop = 1.0f - (yClipped * 2.0f * ivhFont);
						if(afTop < 0.0f)	{	 afTop = 0.0f;	}
						colTop.a() = (u8)(afTop * alphaBk);
						_fontRef->drawChipped((u32)cw, x, y, wFont, 0.0f, yClipped, 0.0f, 0.0f, &colTop, &colTop, &colBottom, &colBottom, rc);
						///rc->getRenderer()->setColorAlpha(alphaBk);
					}
				}
				else
				{
					_fontRef->draw((u32)cw, x, y, wFont, rc);
				}
			}

			x += wc + _xTracking;				// 次の X 位置
			if(x > xMax)	{	xMax = x;	}	// 最大 X 位置保存
			if(y > yMax)	{	yMax = y;	}	// 最大 Y 位置保存
			idxLast = idx;						// 最終インデックス保存
			if(isWideChar)	{	idx++;		}	// ワイド文字はインデックスを送る
			cPrev = cCur;
		}
	}

	// 最大高さに文字の高さ分を加算
	yMax += hFont + _yTracking;

	// 最後の文字列インデックスを記憶する
	if(res != 0L)
	{
		res->_idxLast = idxLast;
		res->_isLastLf = isLf;
	}
	
	// サイズを記憶する
	if(res != 0L)
	{
		res->_width = xMax - xBase;
		res->_height = yMax - yBase;
	}
}

/*---------------------------------------------------------------------*//**
	描画時にタグを反映させる
**//*---------------------------------------------------------------------*/
void StringDrawer::drawEffectTag(const RenderCtx* rc, TagAnalyzer* taz)
{
	if(!taz->getTagStr()->equals("style"))
	{
		return;
	}

	for(s32 ia = 0; ia < (s32)taz->getAtrbNum(); ia++)
	{
		VcString* key = taz->getAtrbKey(ia);
		VcString* val = taz->getAtrbValue(ia);
		// 色
		if((key != 0L) && key->equals("color") && (val != 0L))
		{
			// #RRGGBB 形式
			if(val->getAt(0) == '#')
			{
				if(val->getLength() == 7)
				{
					char cbuf[3] = { 0, 0, 0 };
					cbuf[0] = val->getAt(1);
					cbuf[1] = val->getAt(2);
					u8 r = (u8)StringUtils::toInteger((const char*)cbuf, 16);
					cbuf[0] = val->getAt(3);
					cbuf[1] = val->getAt(4);
					u8 g = (u8)StringUtils::toInteger((const char*)cbuf, 16);
					cbuf[0] = val->getAt(5);
					cbuf[1] = val->getAt(6);
					u8 b = (u8)StringUtils::toInteger((const char*)cbuf, 16);
					if(rc != 0L)	{	rc->getRenderer()->setSolidColor(r, g, b);	}
				}
				else if(val->getLength() == 9)
				{
					char cbuf[3] = { 0, 0, 0 };
					cbuf[0] = val->getAt(1);
					cbuf[1] = val->getAt(2);
					u8 r = (u8)StringUtils::toInteger((const char*)cbuf, 16);
					cbuf[0] = val->getAt(3);
					cbuf[1] = val->getAt(4);
					u8 g = (u8)StringUtils::toInteger((const char*)cbuf, 16);
					cbuf[0] = val->getAt(5);
					cbuf[1] = val->getAt(6);
					u8 b = (u8)StringUtils::toInteger((const char*)cbuf, 16);
					cbuf[0] = val->getAt(7);
					cbuf[1] = val->getAt(8);
					u8 a = (u8)StringUtils::toInteger((const char*)cbuf, 16);
					if(rc != 0L)	{	rc->getRenderer()->setSolidColor(r, g, b, a);	}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
