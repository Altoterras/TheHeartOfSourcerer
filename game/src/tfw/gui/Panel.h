/***********************************************************************//**
 *  Panel.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/11.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_GUI_PANEL_H_
#define _TFW_GUI_PANEL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "PanelBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Point;
typedef Point<f32> PointF32;
template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
typedef Rect<f32> RectF32;
class VcString;
class Font;
class PanelText;
class TiledRectDraw;
class Texture;

/*---------------------------------------------------------------------*//**
 *	パネル
 *	
**//*---------------------------------------------------------------------*/
class Panel : public PanelBase
{
	//======================================================================
	// 定数
public:
	// 状態
	enum State
	{
		STATE_NULL,
		STATE_NORMAL,
		STATE_HOVER,
		STATE_ACTIVE,
		NUM_STATE
	};

	// hitExec の返り値
	enum HitExecRes
	{
		HERES_NULL,
		HERES_NOHIT,
		HERES_HIT,
		HERES_HIT_AND_ACTION,
	};

	// フラグ
	static const u32	F_ENABLE				= 0x00000001;		// 有効
	static const u32	F_FRONT					= 0x00000002;		// 前面表示
	static const u32	F_UNUSED				= 0x00000004;		// 使用不能
	static const u32	F_MANUAL_STATE			= 0x00000010;		// 手動ステート変更
	static const u32	F_DRAW_TOP_SHADOW		= 0x00000100;		// 上部背景に影
	static const u32	F_DRAW_BOTTOM_SHADOW	= 0x00000200;		// 下部背景に影
	static const u32	F_DRAW_LEFT_SHADOW		= 0x00000400;		// 左部背景に影
	static const u32	F_DRAW_RIGHT_SHADOW		= 0x00000800;		// 右部背景に影
	static const u32	F_DRAW__MASK_SHADOW		= 0x00000f00;		// 背景影マスク

	static const s32	MAX_HOLD_ACTIVE_COUNT_DEFAULT	= 30;

private:
	// 実行フラグ
	enum ExecFlag
	{
		EF_NULL				= 0x00,
		EF_HOLF_ACTIVE		= 0x01,		// 長押しでアクティブに
		EF_CLICK_ACTIVE		= 0x02,		// クリックでアクティブに
	};

	// 状態情報の型
	enum StateInfoType
	{
		SI_COLOR_ONLY		= 1,		// StateInfo
		SI_ONE_TEX			= 2,		// StateInfoOneTex
		SI_ANIM_TEX			= 3,		// StateInfoAnimTex
		SI_TILE_TEX			= 4,		// StateInfoTileTex
	};

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	状態情報 - 基本
	 *	
	**//*-----------------------------------------------------------------*/
	class StateInfo
	{
	public:
		StateInfoType	_type;
		ColorU8*		_color;			// 色（NULL のことがある）
		StateInfo();
		~StateInfo();
	};

	/*-----------------------------------------------------------------*//**
	 *	状態情報 - １テクスチャ
	 *		単一 UV、テクスチャアニメーションなし
	 *	
	**//*-----------------------------------------------------------------*/
	class StateInfoOneTex : public StateInfo
	{
	public:
		Texture*		_texRef;		// テクスチャへのポインタ
		RectF32*			_uv;			// テクスチャ座標配列
		StateInfoOneTex();
		~StateInfoOneTex();
	};

	/*-----------------------------------------------------------------*//**
	 *	状態情報 - アニメーションテクスチャ
	 *		テクスチャアニメーションあり
	 *	
	**//*-----------------------------------------------------------------*/
	class StateInfoAnimTex : public StateInfo
	{
	public:
		Texture*		_texRef;		// テクスチャへのポインタ
		RectF32*			_uv;			// テクスチャ座標配列
		s16				_numUv;			// テクスチャ座標配列数
		s16				_intvlfrm;		// テクスチャアニメーションのインターバルフレーム
		StateInfoAnimTex();
		~StateInfoAnimTex();
	};

	/*-----------------------------------------------------------------*//**
	 *	状態情報 - タイルテクスチャ
	 *	
	**//*-----------------------------------------------------------------*/
	class StateInfoTileTex : public StateInfo
	{
	public:
		TiledRectDraw*	_tile;			// タイル矩形描画
		StateInfoTileTex();
		~StateInfoTileTex();
	};

private:
	/*-----------------------------------------------------------------*//**
	 *	テキスト機能
	 *	
	**//*-----------------------------------------------------------------*/
	class TextSupport
	{
	public:
		PanelText*		_ptxt;
		s16				_xPaddingLeft;
		s16				_xPaddingRight;
		s16				_yPaddingTop;
		s16				_yPaddingBottom;
		TextSupport();
		~TextSupport();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 矩形を得る
	inline const RectF32*	getRectangle() const				{	return _rect;					}
	// テキスト機能を得る
	inline PanelText*		getPanelText() const				{	return _text->_ptxt;			}
	// 現在の状態を得る
	inline State			getCurrentState() const				{	return _stateCur;				}
	// 有効かどうかを得る
	inline bool 			isEnable() const					{	return TFW_IS_FLAG(_flags, F_ENABLE);	}
	// 使用不能かどうかを得る
	inline bool 			isUnused() const					{	return TFW_IS_FLAG(_flags, F_UNUSED);	}
	// フラグを得る
	inline u32				getFlags() const					{	return _flags;							}

	// フラグを設定する
	inline void 			setFlags(u32 flags, bool on)		{	TFW_SET_FLAG(_flags, flags, on);		}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setRectangle(const RectF32* rect);
	virtual void setHorizontalHitMargin(f32 margin)	{	_marginHitH = margin;			}
	virtual void setVerticalHitMargin(f32 margin)	{	_marginHitV = margin;			}
	virtual void setPosition(const PointF32* pt);
	virtual void setEnable(bool isEnable)			{	TFW_SET_FLAG(_flags, F_ENABLE, isEnable);	}
	virtual void setUnused(bool isUnused);
	virtual bool setCurrentState(State state);
	inline void setHoldHitToActiveCount(s32 cnt)	{	_thcntHoldHitActive = cnt;		}
	inline void serColoroDissolveRatio(f32 ratio)	{	_ratioDissolveColor = ratio;	}
	virtual void setFront(bool isFront)				 {	TFW_SET_FLAG(_flags, F_FRONT, isFront);		}
	virtual bool hitTest(const PointF32* pt);
	virtual bool hit(const PointF32* pt, bool isActive);
	virtual bool hit(const PointF32* ptNow, const PointF32* ptStart, bool isAction);
	virtual bool hit(bool isHover, bool isActive);
	virtual HitExecRes hitExec(const PointF32* ptNow, const PointF32* ptStart, const PointF32* ptAction, bool isActive, bool isReacted);
	virtual bool createText(Font* fontRef, s32 fontsize, const ColorU8* color, s16 xPaddingLeft, s16 xPaddingRight, s16 yPaddingTop, s16 yPaddingBottom, bool isEnableReadingAnimation, bool isEnableVerticalScrollAnimation, bool isAutoScroll);
	virtual bool createText(Font* fontRef, s32 fontsize, const VcString* str, const ColorU8* color, s16 xPaddingLeft, s16 xPaddingRight, s16 yPaddingTop, s16 yPaddingBottom, bool isEnableReadingAnimation, bool isEnableVerticalScrollAnimation, bool isAutoScroll);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Panel();
	virtual ~Panel();
	virtual bool create();
	virtual bool create(const RectF32* rect);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);
	virtual void draw(const RenderCtx* rc);

	virtual void setStateInfo(State state, const ColorU8* color);
	virtual void setStateInfo(State state, Texture* texRef, const RectF32* uv);
	virtual void setStateInfo(State state, Texture* texRef, const RectF32* uv, const ColorU8* color);
	virtual void setStateInfo(State state, Texture* texRef, const RectF32* uv, s16 numUv, s16 intvlfrm);
	virtual void setStateInfo(State state, Texture* texRef, const RectF32* uv, s16 numUv, s16 intvlfrm, const ColorU8* color);
	virtual void setStateInfo(State state, TiledRectDraw* tileEntr);
	virtual void setStateInfo(State state, TiledRectDraw* tileEntr, ColorU8* color);

private:
	StateInfo* newStateInfo(StateInfoType sitype) const;
	void deleteStateInfo(StateInfo* st);
	StateInfo* setStateInfoCommonPre(State state, StateInfoType sitype);
	void setStateInfoCommonPost(StateInfo* st, State state, const ColorU8* color);

	//======================================================================
	// 変数
private:
	RectF32*		_rect;
	ColorU8*		_color;
	TextSupport*	_text;
	StateInfo*		_starr[NUM_STATE - 1];
	State			_stateCur;
	u32				_execflag;
	s32				_cntHit;				// ヒットカウンタ
	s32				_cntActive;				// アクティブカウンタ
	s32				_thcntHoldHitActive;	// ホールドからアクティブへのしきいカウンタ値
	f32				_ratioDissolveColor;
	s32				_idxTexAnim;
	f32				_fcntTexAnim;
	u32				_flags;
	f32				_marginHitH;			// 横方向のヒット時に大きめに判定するマージン
	f32				_marginHitV;			// 縦方向のヒット時に大きめに判定するマージン
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_GUI_PANEL_H_
