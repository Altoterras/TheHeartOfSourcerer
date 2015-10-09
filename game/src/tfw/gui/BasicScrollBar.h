/***********************************************************************//**
 *  BasicScrollBar.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2010/03/05.
 *  Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_GUI_BASIC_SCROLL_BAR_H_
#define _TFW_GUI_BASIC_SCROLL_BAR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
template<class TYPE> class Point;
typedef Point<f32> PointF32;
class TouchPanel;

/*---------------------------------------------------------------------*//**
 *	基本スクロールバー
 *	※	テクスチャなどは派生クラスで実装すること
 *	
**//*---------------------------------------------------------------------*/
class BasicScrollBar
{
	//======================================================================
	// 定数
public:

protected:
	// 生成フラグ
	static const u16	CF_HORIZ		= 0x0001;		// 横タイプ
	static const u16	CF_IGNORE_HIDE	= 0x0002;		// HIDE フラグを無視する

	// 動作制御フラグ
	static const u16	EF_DISABLED		= 0x0001;		// 無効
	static const u16	EF_HIDE			= 0x0002;		// スクロールバー不要
	static const u16	EF_CAPTURE		= 0x0004;		// ドラッグ時にマウスの動作をキャプチャ

	// 動作設定値
	static const s32	MAX_HOLD_ACTIVE_COUNT_DEFAULT	= 30;

	//======================================================================
	// クラス
public:
	class ScrollBarExecRes : public ExecRes
	{
		//==================================================================
		// 定数
	public:
		// 動作アクション
		enum Action
		{
			ACT_NULL,
			ACT_CLICK_UP_ARROW,
			ACT_CLICK_DOWN_ARROW,
			ACT_DRAG_KNOB,
			ACT_CLICK_PAGE_UP,
			ACT_CLICK_PAGE_DOWN,
			ACT_FLIP_UP,
			ACT_FLIP_DOWN,
			ACT_ZOOM_IN,
			ACT_ZOOM_OUT,
		};
	
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// 動作アクションを得る
		inline Action	getAction() const		{	return (Action)_value1;		}
		// 値を得る
		inline f32		getValue() const		{	return _value2;				}

		// 動作アクションを設定する
		inline void		setAction(Action act)	{	_value1 = act;				}
		// 値を設定する
		inline void		setValue(f32 val)		{	_value2 = val;				}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 矩形を得る
	inline const RectF32*	getRectangle() const				{	return _rectThis;				}
	// 有効かどうかを得る
	inline bool				isEnable() const					{	return !TFW_IS_FLAG(_eflags, EF_DISABLED);			}
	// 表示されているかどうかを得る
	bool					isShow() const;
	// スクロール値の現在値を得る
	inline f32				getScrollPos() const				{	return _valCur;					}
	// スクロール値の最小値を得る
	inline f32				getScrollMin() const				{	return _valMin;					}
	// スクロール値の最大値を得る
	inline f32				getScrollMax() const				{	return _valMax;					}
	// スクロール値のページ値を得る
	inline f32				getScrollPage() const				{	return _valPage;				}
	// 横型かどうか
	inline bool				isHorizontal() const				{	return TFW_IS_FLAG(_cflags, CF_HORIZ);				}

	// 矩形を設置する
	virtual void			setRectangle(const RectF32* rect);
	// 有効かどうかを設定する
	inline void				setEnable(bool isEnable)			{	TFW_SET_FLAG(_eflags, EF_DISABLED, !isEnable);		}
	// スクロール不要時に隠すのを無視する
	inline void				setIgnoreHide(bool isEnable)		{	TFW_SET_FLAG(_cflags, CF_IGNORE_HIDE, isEnable);	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool			hitTest(const PointF32* pt);
	virtual void			setScrollMinMax(f32 valMin, f32 valMax);
	virtual void			setScrollPos(f32 valCur);
	virtual void			setScrollPage(f32 valPage);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	BasicScrollBar();
	virtual ~BasicScrollBar();
	virtual bool	create(const RectF32* rect);
	virtual void	destroy();
	virtual void	exec(ExecRes* res, TouchPanel* ui);
	virtual void	draw(const RenderCtx* rc);

protected:
	void			execStartCapture(const PointF32* pt);
	virtual void	drawBackground(const RenderCtx* rc, const RectF32* rect);
	virtual void	drawKnob(const RenderCtx* rc, const RectF32* rect);
	virtual void	drawUpButton(const RenderCtx* rc, const RectF32* rect, bool isEnabled);
	virtual void	drawDownButton(const RenderCtx* rc, const RectF32* rect, bool isEnabled);
	virtual void	setKnobRectangle();

	//======================================================================
	// 変数
protected:
	// 動作周り
	u16				_cflags;				// 生成フラグ
	u16				_eflags;				// 実行制御フラグ
	f32				_valMin;				// 最小値
	f32				_valMax;				// 最大値
	f32				_valCur;				// 現在値
	f32				_valPage;				// ページサイズ
	f32				_valDragStart;			// ドラッグ時開始位置

	// 描画周り
protected:
	RectF32*		_rectThis;				// 全体の矩形
	RectF32*		_rectUpArw;				// 値下降矢印の矩形
	RectF32*		_rectDownArw;			// 値上昇矢印の矩形
	RectF32*		_rectKnob;				// ノブの矩形
	f32				_pxDragStart;			// ドラッグ時開始座標
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_GUI_SCROLL_BAR_H_
