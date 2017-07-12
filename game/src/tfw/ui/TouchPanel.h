/***********************************************************************//**
 *	TouchPanel.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_UI_TOUCH_PANEL_H_
#define _TFW_UI_TOUCH_PANEL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "Ui.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Point;
typedef Point<f32> PointF32;
class Pad;
class View;

/*---------------------------------------------------------------------*//**
 *	タッチパネル
 *
**//*---------------------------------------------------------------------*/
class TouchPanel : public Ui
{
	//======================================================================
	// 定数
public:
	enum Action
	{
		ACT_NULL = 0,
		ACT_CLICK,
		ACT_DRAG,
		ACT_FLIP,
		ACT_ZOOM,
		ACT_CANCEL,
	};

private:
	enum MoveAct
	{
		MA_NULL = 0,
		MA_DRAG,
		MA_FLIP,
		MA_ZOOM,
	};
	
	enum EventCase
	{
		EVC_BEGIN,
		EVC_MOVE,
		EVC_END,
	};
	
	static const s32 NUM_TOUCHES		= 2;

	static const s32 TCNT_NOTHING		= -1;
	static const s32 TCNT_ON_RELEASE	= -101;
	static const s32 TCNT_RELEASE		= -100;
	static const s32 TCNT_ON_CANCEL		= -201;
	static const s32 TCNT_CANCEL		= -200;
	static const s32 TCNT_ON_BEGIN		= 0;
	static const s32 TCNT_BEGIN			= 1;

	static const u32 EF_REACTED_TAP		= 0x00010000;	// タップに反応済みであるため、リリースも無視する
	static const u32 EF_REACTED_DRAGOP	= 0x00020000;	// ドラッグ操作フラグ（ドラッグ操作中は他の操作を回避するためのフラグ）

	//======================================================================
	// クラス
public:
	class Touch
	{
	public:
		PointF32*	_pt;
		Touch();
		~Touch();
	};

private:
	class Ti
	{
	public:
		bool		_isTouch;
		PointF32*	_ptCur;
		PointF32*	_ptPrev;

		Ti();
		~Ti();
		void reset();
	};

	class Res
	{
	public:
		bool		_action;
		Action		_act;
		PointF32*	_ptStart;
		PointF32*	_ptCur;
		PointF32*	_ptPrev;
		PointF32*	_ptCenter;
		f32			_speed;
		f32			_rCur;
		f32			_rPrev;

		Res();
		~Res();
		void reset();
		void set(Action act, const PointF32* ptStart, const PointF32* ptCur, const PointF32* ptPrev);
		void set(Action act, const PointF32* ptStart, f32 speed, const PointF32* ptCur, const PointF32* ptPrev);
		void set(Action act, const PointF32* ptStart, const PointF32* ptCenter, f32 rCur, f32 rPrev);
	};

	class ValueStack
	{
	public:
		static const s32	NUM_STACK = 8;
		u16			_stack[NUM_STACK];
		u16			_idxStack;

		ValueStack();
		void reset();
		void push(u16 val);
		u16 getMax() const;
		u16 getAverage() const;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// タッチしているかどうかの判定
	bool isTouching() const;
	// タッチしている数を得る
	s32 getTouchingNum() const;
	// タップ トリガ判定
	bool isTapTrigger(PointF32* ptNow) const;
	// タップ リリース判定
	bool isTapRelease(PointF32* ptNow, PointF32* ptStart) const;
	// タップ リピート判定
	bool isTapRepeat(PointF32* ptNow, PointF32* ptStart) const;
	// タップ リピート リリース判定
	bool isTapRepeatRelease(PointF32* ptNow, PointF32* ptStart) const;
	// クリック リリース判定
	bool isClickRelease(PointF32* pt) const;
	// ダブルクリック リリース判定
	bool isDoubleClickRelease(PointF32* pt) const;
	// タッチ位置を取得
	bool getTouchPosition(PointF32* pt, s32 idxTouch) const;
	inline bool getTouchPosition(PointF32* pt) const { return getTouchPosition(pt, 0); }
	// タッチ開始位置を取得
	void getTouchStartPosition(PointF32* pt) const;
	// カーソル位置を取得
	void getCursorPosition(PointF32* pt) const;
	// ドラッグ操作フラグを取得する
	bool isDragOperating() const { return TFW_IS_FLAG(_eflags, EF_REACTED_DRAGOP); }
	// ドラッグ操作フラグを設定する
	void setDragOperating(bool isOp) { TFW_SET_FLAG(_eflags, EF_REACTED_DRAGOP, isOp); }

	Action getLastAction() const;
	void getClickParam(PointF32* ptClick, PointF32* ptCur, PointF32* ptPrev) const;
	void getDragParam(PointF32* ptStart, PointF32* ptCur, PointF32* ptPrev) const;
	void getFlipParam(PointF32* ptStart, f32* speed, PointF32* ptCur, PointF32* ptPrev) const;
	void getZoomParam(PointF32* ptStart, PointF32* ptCenter, f32* rCur, f32* rPrev) const;
	void getCancelParam(PointF32* ptStart, PointF32* ptCur, PointF32* ptPrev) const;

	inline void	setReactedTap(bool isReacted) { TFW_SET_FLAG(_eflags, EF_REACTED_TAP, isReacted); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TouchPanel();
	virtual ~TouchPanel();
	bool create(View* viewRef);
	void destroy();
	virtual void exec();

	void beganTouches(Touch* arrTouch, s32 numArray);
	void movedTouches(Touch* arrTouch, s32 numArray);
	void endedTouches(Touch* arrTouch, s32 numArray);
	void cancelledTouches(Touch* arrTouch, s32 numArray);
	void moveCursor(f32 x, f32 y);

private:
	void getActionParam(PointF32* ptStart, PointF32* ptCenter, PointF32* ptCur, PointF32* ptPrev, f32* speed, f32* rCur, f32* rPrev) const;
	void convTouches(Touch* arrTouch, s32 numArray, EventCase evc);

	//======================================================================
	// 変数
private:
	MoveAct		_moveact;
	s32			_cntTouchFrame;		// タッチ継続フレームカウンタ
	u32			_cntDclick;			// ダブルクリック用カウンタ
	s32			_cntMaxTouch;		// 一連の操作が複数タップだったかどうか
	f32			_speedCur;			// フレームの Move スピード
	PointF32*	_ptTouchStart;		// クリック開始時の位置
	PointF32*	_ptTouchLast;		// 最後のクリック位置
	PointF32*	_ptTouchPrev;		// 前回のクリック位置
	PointF32*	_ptCursorNow;		// 現在のカーソル位置
	Ti*			_ti;				// タッチ情報
	Res*		_res;				// タッチリザルト
	ValueStack	_speed;				// 移動速度
	View*		_viewRef;			// ビューへのポインタ
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_UI_TOUCH_PANEL_H_