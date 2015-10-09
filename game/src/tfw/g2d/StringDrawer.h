/***********************************************************************//**
 *	StringDrawer.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/11.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *	
**//***********************************************************************/

#ifndef _TFW_G2D_STRING_DRAWER_H_
#define _TFW_G2D_STRING_DRAWER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
	
////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
template<class TYPE> class Size;
typedef Size<f32> SizeF32;
template<class TYPE> class Point;
typedef Point<f32> PointF32;
template<class TYPE> class Array;
class CStringBase;
class VcString;
class Font;
class RenderCtx;

/*---------------------------------------------------------------------*//**
 *	文字列描画
 *	※	全角文字は Shift-JIS に限られる
 *
**//*---------------------------------------------------------------------*/
class StringDrawer
{
	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	読み上げアニメーション
	 *
	**//*-----------------------------------------------------------------*/
	class ReadingAnim
	{
	public:
		bool		_isAnim;				// アニメーションをするかどうか
		f32			_cntAnim;				// アニメーションカウンタ
		f32			_cntMax;				// アニメーションカウンタ最大値
		f32			_fcntIntvl;				// 次の文字までのインターバル値
		f32			_fcntIntvlSetting;		// 次の文字までのインターバル値設定値
		f32			_rateAnimSpeedSetting;	// 経過フレーム に対するレート（～ 1.0f）
		
	public:
		ReadingAnim()
		{
			clear();
		}
		
		void clear()
		{
			reset();
			_isAnim	= false;
			_fcntIntvlSetting = 0.0f;
			_rateAnimSpeedSetting = 1.0f;
		}
		
		void reset()
		{
			_cntAnim = 0.0f;
			_cntMax = 0.0f;
			_fcntIntvl = 0.0f;
		}
	};
	/*-----------------------------------------------------------------*//**
	 *	縦スクロール時のアニメーション
	 *
	**//*-----------------------------------------------------------------*/
	class VtAnim
	{
	public:
		bool		_isAnim;				// アニメーション刷るかどうか
		s32			_indexDrawStartPrev;	// テキスト描画開始インデックス、縦スクロールアニメーション中の仮値
		s32			_indexDrawStartNext;	// テキスト描画開始インデックス、縦スクロールアニメーション後の値
		f32			_yOffsetTrg;			// 目標の描画時 Y オフセット
		f32			_yOffsetPrevLine;		// アニメーション前の行の描画時 Y オフセット
		f32			_yOffsetNextLine;		// アニメーション後の行の描画時 Y オフセット
		f32			_rateAnimSpeedSetting;	// 経過フレーム に対するレート（～ 1.0f）
		
	public:
		VtAnim()
		{
			clear();
		}
		
		void clear()
		{
			_isAnim	= false;
			reset();
		}
		
		void reset()
		{
			_indexDrawStartPrev = 0;
			_indexDrawStartNext = 0;
			_yOffsetTrg = 0.0f;
			_yOffsetPrevLine = 0.0f;
			_yOffsetNextLine = 0.0f;
			_rateAnimSpeedSetting = 1.0f;
		}
	};
	
	/*-----------------------------------------------------------------*//**
	 *	描画結果
	 *
	**//*-----------------------------------------------------------------*/
	class DrawProcResult
	{
	public:
		f32			_width;
		f32			_height;
		s32			_idxLast;
		bool		_isLastLf;

	public:
		DrawProcResult()
		{
			_width = 0.0f;
			_height = 0.0f;
			_idxLast = 0;
			_isLastLf = false;
		}
	};

	/*-----------------------------------------------------------------*//**
	 *	タグアナライザ
	 *
	**//*-----------------------------------------------------------------*/
	class TagAnalyzer
	{
		//==================================================================
		// 定数
	private:
		static const s32	STEP_NULL		= 0;
		static const s32	STEP_TAGSTR		= 1;
		static const s32	STEP_KEYSTR		= 2;
		static const s32	STEP_VALUESTR	= 3;
		static const s32	STEP_END		= 4;

		//==================================================================
		// クラス
	private:
		class KeyValue
		{
		public:
			VcString*	_key;
			VcString*	_value;
			
			KeyValue(VcString* key);
			KeyValue(VcString* key, VcString* val);
			~KeyValue();
		};

		//==================================================================
		// 変数
	private:
		s32					_step;
		VcString*			_tagstr;
		Array<KeyValue*>*	_arrKeyValue;
		
		VcString*			_strb1;
		VcString*			_strb2;
		
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:	
		VcString* getTagStr() const { return _tagstr; }
		u32 getAtrbNum() const;
		VcString* getAtrbKey(s32 idx) const;
		VcString* getAtrbValue(s32 idx) const;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		TagAnalyzer();
		~TagAnalyzer();
		bool isHook() const;
		bool pushChar(char c);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 範囲矩形を得る
	inline const RectF32* getRectangle() const { return _rectExt; }
	inline RectF32* rectangle() { return _rectExt; }
	// 設定フォントを得る
	inline Font* getFont() const { return _fontRef; }
	// 描画文字列を得る
	inline const CStringBase* getTargetString() const { return _strTrgRef; }
	// デフォルトの文字幅を得る
	s32 getCharDefaultWidth();
	// デフォルトの文字高さを得る
	s32 getCharDefaultHeight();
	// 字間を得る
	inline f32 getTrackingX() const { return _xTracking; }
	// 行間を得る
	inline f32 getTrackingY() const { return _yTracking; }
	// 禁則処理が有効化を得る
	inline bool isEnableHyphenation() const { return _isHyphenation; }
	// 文字読み上げアニメーション機能が有効かを得る
	inline bool isReadingAnimationEnabled() const { return _rdanim._isAnim; }
	// 縦スクロールアニメーション機能が有効かを得る
	inline bool isVerticalScrollAnimationEnabled() const { return _vtanim._isAnim; }
	// 現在の描画開始文字列インデックスを得る
	inline s32 getDrawStartStringIndex() const { return _indexDrawStart; }

	// 禁則処理の有効・無効を設定する
	inline void setEnableHyphenation(bool isEnable) { _isHyphenation = isEnable; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setRectangle(const RectF32* rect);
	void setString(const CStringBase* strRef);
	void updateString();
	void setFont(Font* font);
	void setFontSize(s32 fontsize);
	void setTracking(f32 xTracking, f32 yTracking);
	void setScrollPosition(f32 yScroll);
	void setDrawStartStrIndex(s32 index);
	void setDrawStartStrLine(s32 line);
	void setReadingAnimation(bool isEnable, f32 speed, f32 interval);
	void setVerticalScrollAnimation(bool isEnable, f32 speed);

	bool isReadingAnimationEnd();
	bool isVerticalScrollAnimationEnd();
	void finishReadingAnimation();
	void finishVerticalScrollAnimation();
	void calcReadingAnimMaxCount();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StringDrawer();
	virtual ~StringDrawer();

	bool create();
	bool create(Font* font, s32 fontsize, const RectF32* rect, const CStringBase* str);
	void destroy();
	void exec(f32 frameDelta);
	void drawTest(SizeF32* sizeRes);
	void drawTest(SizeF32* sizeRes, const CStringBase* strForTest);
	void draw(const RenderCtx* rc);
	void draw(const RenderCtx* rc, f32 xOffset, f32 yOffset);

private:
	void drawProc(DrawProcResult* res, const RenderCtx* rc, const CStringBase* str, bool isOverRectY, s32 indexDrawStart, s32 indexDrawEnd, f32 alphaLastChar, s32 lineBreak, f32 xOffsetThis, f32 yOffsetThis, f32 xOffsetTxt, f32 yOffsetTxt);
	void drawEffectTag(const RenderCtx* rc, TagAnalyzer* taz);

	//======================================================================
	// 変数
private:
	const CStringBase*	_strTrgRef;				// 描画対象文字列
	s32					_lenText;				// テキストの長さ
	Font*				_fontRef;				// フォント
	s32					_fontsize;				// フォントサイズ（px）
	f32					_xTracking;				// 字間
	f32					_yTracking;				// 行間
	f32					_yScroll;				// 手動スクロール Y 位置
	bool				_isHyphenation;			// 禁則処理を行う

	RectF32*			_rectExt;				// 矩形

	s32					_indexDrawStart;		// テキストの描画開始インデックス
	ReadingAnim			_rdanim;				// 読み上げアニメーション情報
	VtAnim				_vtanim;				// 縦スクロールアニメーション情報
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_G2D_STRING_DRAWER_H_

