/***********************************************************************//**
 *	TouchPanel.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TouchPanel.h"

// Friends
#include "Pad.h"
#include "../lib/Vector.h"
#include "../lib/Point.h"
#include "../gcmn/View.h"
#include "../string/VcString.h"

// External

// Library
#include <limits.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define FRAME_CLICK			(0)			// クリックと判定されるフレーム数
#define FRAME_DRAG			(4)			// ドラッグと判定されるフレーム数
#define SPPED_FLIP			(2)			// フリップと判定されるスピード
#define RANGE_CLICK_NEAR	(32)		// クリックと判定される初期位置との位置差
#define FRAME_DCLICK_ALLOW	(15)		// 許可する１回目のクリックと２回目のクリックの間のフレーム数
#define FRAME_REPEAT_FW		(30)		// リピートカウントの最初の待ちフレーム

#define FRAME_REPEAT		(7)			// リピートカウント（ビット演算をするので“2 の累乗 - 1”である必要がある）

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TouchPanel メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	タッチしているかどうかの判定
**//*---------------------------------------------------------------------*/
bool TouchPanel::isTouching() const
{
	for(s32 i = 0; i < NUM_TOUCHES; i++)
	{
		if(_ti[i]._isTouch)	{	return true;	}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	タッチしている数を得る
**//*---------------------------------------------------------------------*/
s32 TouchPanel::getTouchingNum() const
{
	s32 cnt = 0;
	for(s32 i = 0; i < NUM_TOUCHES; i++)
	{
		if(_ti[i]._isTouch)	{	cnt++;	}
	}
	return cnt;
}

/*---------------------------------------------------------------------*//**
	タップ トリガ判定
**//*---------------------------------------------------------------------*/
bool TouchPanel::isTapTrigger(PointF32* ptNow) const
{
	if(_cntTouchFrame != TCNT_BEGIN)			{	return false;	}
	if(ptNow != 0L)		{	ptNow->set(_ptTouchLast->x(), _ptTouchLast->y());		}
	return true;
}

/*---------------------------------------------------------------------*//**
	タップ リリース判定
**//*---------------------------------------------------------------------*/
bool TouchPanel::isTapRelease(PointF32* ptNow, PointF32* ptStart) const
{
	if(_cntTouchFrame != TCNT_RELEASE)			{	return false;	}
	if(ptNow != 0L)		{	ptNow->set(_ptTouchLast->x(), _ptTouchLast->y());		}
	if(ptStart != 0L)	{	ptStart->set(_ptTouchStart->x(), _ptTouchStart->y());	}
	return true;
}

/*---------------------------------------------------------------------*//**
	タップ リピート判定
**//*---------------------------------------------------------------------*/
bool TouchPanel::isTapRepeat(PointF32* ptNow, PointF32* ptStart) const
{
	if(isTapTrigger(ptNow))
	{
		if(ptStart != 0L)	{	ptNow->set(_ptTouchStart->x(), _ptTouchStart->y());	}
		return true;
	}
	if(_cntTouchFrame <= FRAME_REPEAT_FW)		{	return false;	}
	if((_cntTouchFrame & FRAME_REPEAT) != 0)	{	return false;	}
	if(ptNow != 0L)		{	ptNow->set(_ptTouchLast->x(), _ptTouchLast->y());		}
	if(ptStart != 0L)	{	ptStart->set(_ptTouchStart->x(), _ptTouchStart->y());	}
	return true;
}

/*---------------------------------------------------------------------*//**
	タップ リピート リリース判定
**//*---------------------------------------------------------------------*/
bool TouchPanel::isTapRepeatRelease(PointF32* ptNow, PointF32* ptStart) const
{
	if(isTapRelease(ptNow, 0L))
	{
		if(ptStart != 0L)	{	ptNow->set(_ptTouchStart->x(), _ptTouchStart->y());	}
		return true;
	}
	if(_cntTouchFrame <= FRAME_REPEAT_FW)		{	return false;	}
	if((_cntTouchFrame & FRAME_REPEAT) != 0)	{	return false;	}
	if(ptNow != 0L)		{	ptNow->set(_ptTouchLast->x(), _ptTouchLast->y());		}
	if(ptStart != 0L)	{	ptStart->set(_ptTouchStart->x(), _ptTouchStart->y());	}
	return true;
}

/*---------------------------------------------------------------------*//**
	クリック リリース判定
**//*---------------------------------------------------------------------*/
bool TouchPanel::isClickRelease(PointF32* pt) const
{
	if(_res->_act != ACT_CLICK)					{	return false;	}
	if(pt != 0L)	{	pt->set(_ptTouchLast->x(), _ptTouchLast->y());	}
	return true;
}

/*---------------------------------------------------------------------*//**
	ダブルクリック リリース判定
**//*---------------------------------------------------------------------*/
bool TouchPanel::isDoubleClickRelease(PointF32* pt) const
{
	if(_cntTouchFrame != TCNT_RELEASE)			{	return false;	}
	if(!TFW_IS_NEAR(_ptTouchLast->x(), _ptTouchStart->x(), RANGE_CLICK_NEAR) || !TFW_IS_NEAR(_ptTouchLast->y(), _ptTouchStart->y(), RANGE_CLICK_NEAR))	{	return false;	}
	if(_cntDclick < FRAME_DCLICK_ALLOW)			{	return false;	}
	if(pt != 0L)	{	pt->set(_ptTouchLast->x(), _ptTouchLast->y());	}
	return true;
}

/*---------------------------------------------------------------------*//**
	タッチ位置を取得
**//*---------------------------------------------------------------------*/
bool TouchPanel::getTouchPosition(PointF32* pt, s32 idxTouch) const
{
	ASSERT(idxTouch < NUM_TOUCHES);
	if(_ti[idxTouch]._isTouch)
	{
		pt->x() = _ti[idxTouch]._ptCur->x();
		pt->y() = _ti[idxTouch]._ptCur->y();
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	タッチ開始位置を取得
**//*---------------------------------------------------------------------*/
void TouchPanel::getTouchStartPosition(PointF32* pt) const
{
	pt->copy(_ptTouchStart);
}

/*---------------------------------------------------------------------*//**
	カーソル位置を取得
**//*---------------------------------------------------------------------*/
void TouchPanel::getCursorPosition(PointF32* pt) const
{
	pt->copy(_ptCursorNow);
}

/*---------------------------------------------------------------------*//**
	最終動作を得る
**//*---------------------------------------------------------------------*/
TouchPanel::Action TouchPanel::getLastAction() const
{
	return _res->_act;
}

/*---------------------------------------------------------------------*//**
	最終動作のクリックパラメータを得る
**//*---------------------------------------------------------------------*/
void TouchPanel::getClickParam(PointF32* ptClick, PointF32* ptCur, PointF32* ptPrev) const
{
	getActionParam(ptClick, 0L, ptCur, ptPrev, 0L, 0L, 0L);
}

/*---------------------------------------------------------------------*//**
	最終動作のドラッグパラメータを得る
**//*---------------------------------------------------------------------*/
void TouchPanel::getDragParam(PointF32* ptStart, PointF32* ptCur, PointF32* ptPrev) const
{
	getActionParam(ptStart, 0L, ptCur, ptPrev, 0L, 0L, 0L);
}

/*---------------------------------------------------------------------*//**
	最終動作のフリップパラメータを得る
**//*---------------------------------------------------------------------*/
void TouchPanel::getFlipParam(PointF32* ptStart, f32* speed, PointF32* ptCur, PointF32* ptPrev) const
{
	getActionParam(ptStart, 0L, ptCur, ptPrev, speed, 0L, 0L);
}

/*---------------------------------------------------------------------*//**
	最終動作のズームパラメータを得る
**//*---------------------------------------------------------------------*/
void TouchPanel::getZoomParam(PointF32* ptStart, PointF32* ptCenter, f32* rCur, f32* rPrev) const
{
	getActionParam(ptStart, ptCenter, 0L, 0L, 0L, rCur, rPrev);
}

/*---------------------------------------------------------------------*//**
	最終動作のキャンセルパラメータを得る
**//*---------------------------------------------------------------------*/
void TouchPanel::getCancelParam(PointF32* ptStart, PointF32* ptCur, PointF32* ptPrev) const
{
	getActionParam(ptStart, 0L, ptCur, ptPrev, 0L, 0L, 0L);
}

/*---------------------------------------------------------------------*//**
	最終動作の全パラメータを得る
**//*---------------------------------------------------------------------*/
void TouchPanel::getActionParam(PointF32* ptStart, PointF32* ptCenter, PointF32* ptCur, PointF32* ptPrev, f32* speed, f32* rCur, f32* rPrev) const
{
	if(ptStart != 0L)	{	ptStart->set(_res->_ptStart->x(), _res->_ptStart->y());		}
	if(ptCenter != 0L)	{	ptCenter->set(_res->_ptCenter->x(), _res->_ptCenter->y());	}
	if(ptCur != 0L)		{	ptCur->set(_res->_ptCur->x(), _res->_ptCur->y());			}
	if(ptPrev != 0L)	{	ptPrev->set(_res->_ptPrev->x(), _res->_ptPrev->y());		}
	if(speed != 0L)		{	*speed = _res->_speed;										}
	if(rCur != 0L)		{	*rCur = _res->_rCur;										}
	if(rPrev != 0L)		{	*rPrev = _res->_rPrev;										}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TouchPanel::TouchPanel()
	: _moveact(MA_NULL)
	, _cntTouchFrame(TCNT_NOTHING)
	, _cntDclick(0)
	, _cntMaxTouch(0)
	, _speedCur(0.0f)
	, _ptTouchStart(0L)
	, _ptTouchLast(0L)
	, _ptTouchPrev(0L)
	, _ptCursorNow(0L)
	, _ti(0L)
	, _res(0L)
	, _viewRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TouchPanel::~TouchPanel()
{
	ASSERT(_ptTouchPrev == 0L);
	ASSERT(_ptTouchLast == 0L);
	ASSERT(_ptTouchStart == 0L);
	ASSERT(_ptCursorNow == 0L);
	ASSERT(_ti == 0L);
	ASSERT(_res == 0L);
}

/*---------------------------------------------------------------------*//**
	構築
**//*---------------------------------------------------------------------*/
bool TouchPanel::create(View* viewRef)
{
	// 各種変数確保
	_ptTouchStart = new PointF32(F32_PMAX, F32_PMAX);
	_ptTouchLast = new PointF32(F32_PMAX, F32_PMAX);
	_ptTouchPrev = new PointF32(F32_PMAX, F32_PMAX);
	_ptCursorNow = new PointF32(F32_PMAX, F32_PMAX);
	_ti = new Ti[NUM_TOUCHES];
	_res = new Res();

	// ビュー参照の保持
	_viewRef = viewRef;

	// カウンタリセット
	_cntTouchFrame = TCNT_NOTHING;
	_cntDclick = 0;
	_cntMaxTouch = 0;
	_speed.reset();

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void TouchPanel::destroy()
{
	delete _ptCursorNow;
	_ptCursorNow = 0L;
	delete _ptTouchPrev;
	_ptTouchPrev = 0L;
	delete _ptTouchLast;
	_ptTouchLast = 0L;
	delete _ptTouchStart;
	_ptTouchStart = 0L;
	delete[] _ti;
	_ti = 0L;
	delete _res;
	_res = 0L;
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void TouchPanel::exec()
{
	///STRACE("{!1!} speed: _cntTouchFrame=%d\n", _cntTouchFrame);

	// アクションがない場合は返答情報リセット
	if(!_res->_action)
	{
		_res->reset();
	}
	_res->_action = false;

	// 反応済みフラグをオフに
	TFW_SET_FLAG(_eflags, EF_REACTED, false);

	// ダブルクリック判定用の処理
	if(_cntTouchFrame == TCNT_ON_RELEASE)
	{
		if(TFW_IS_NEAR(_ptTouchLast->x(), _ptTouchStart->x(), RANGE_CLICK_NEAR) && TFW_IS_NEAR(_ptTouchLast->y(), _ptTouchStart->y(), RANGE_CLICK_NEAR))	// クリック開始位置との差が許容範囲であれば
		{
			// クリック判定されたときに既定の n フレーム数に設定し、n ～ n×2 のフレームが残っていたらダブルクリックであろう、というアルゴリズム
			if(_cntDclick == 0)	// はじめてのクリック
			{
				_cntDclick = FRAME_DCLICK_ALLOW;
				_ptTouchPrev->copy(_ptTouchStart);	// はじめてのタッチの場所を覚えておく
			}
			else if(_cntDclick > 0)	// ２回目以降のクリック
			{
				if(TFW_IS_NEAR(_ptTouchLast->x(), _ptTouchPrev->x(), RANGE_CLICK_NEAR) && TFW_IS_NEAR(_ptTouchLast->y(), _ptTouchPrev->y(), RANGE_CLICK_NEAR))	// はじめてのクリック位置との差が許容範囲であれば
				{
					_cntDclick = FRAME_DCLICK_ALLOW * 2;
				}
				else
				{
					_cntDclick = FRAME_DCLICK_ALLOW;	// はじめてのタッチと同様の処理
					_ptTouchPrev->copy(_ptTouchStart);	// タッチの場所を覚えておく
				}
			}
		}

		// 反応済みフラグを付け替える
		if(TFW_IS_FLAG(_eflags, EF_REACTED_TAP))
		{
			TFW_SET_FLAG(_eflags, EF_REACTED, true);		// 反応フラグをオンに
			TFW_SET_FLAG(_eflags, EF_REACTED_TAP, false);	// タップ反応フラグをオフに
		}
	}
	if(_cntDclick > 0)	{	_cntDclick--;	}

	// タッチカウンタ更新
	if (_cntTouchFrame >= TCNT_ON_BEGIN)		// タッチ中
	{
		if(_cntTouchFrame >= 0x7fffffff)	{	_cntTouchFrame = TCNT_BEGIN + 1;	}
		else								{	_cntTouchFrame++;					}
	}
	else						// タッチなし
	{
		if((_cntTouchFrame == TCNT_ON_RELEASE) || (_cntTouchFrame == TCNT_ON_CANCEL))
		{
			_cntTouchFrame++;				// それぞれの状態へ
		}
		else if(_cntTouchFrame != TCNT_NOTHING)
		{
			_cntTouchFrame = TCNT_NOTHING;	// 無効状態へ

			if (isDragOperating()) { setDragOperating(false); }	// ドラッグ操作フラグをオフに
		}

		// なんらの理由でドラッグ中に終了イベントを取れなかったときの救済処理
		if(_moveact == MA_DRAG)
		{
			_moveact = MA_NULL;
		}
	}
	
	// フリップ判定用のスピード更新
	if(_speedCur <= U16_MAX)	{	_speed.push((u16)_speedCur);	}
	else						{	_speed.push(U16_MAX);			}
	_speedCur = 0;
}

/*---------------------------------------------------------------------*//**
	タッチ開始
**//*---------------------------------------------------------------------*/
void TouchPanel::beganTouches(Touch* arrTouch, s32 numArray)
{
	///TRACE(VcString::format("* beganTouches\n"));
	if(!isTouching())
	{
		_moveact = MA_NULL;
		for(s32 i = 0; i < NUM_TOUCHES; i++)	{	_ti[i].reset();	}
	}
	if(_cntTouchFrame < 0)
	{
		_cntTouchFrame = TCNT_ON_BEGIN;
	}

	////TRACE("\n");
	////TRACE(VcString::format("#1# beganTouches _ti[0] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[0]._isTouch, _ti[0]._ptCur->x(), _ti[0]._ptCur->y(), _ti[0]._ptPrev->x(), _ti[0]._ptPrev->y()));
	////TRACE(VcString::format("#1# beganTouches _ti[1] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[1]._isTouch, _ti[1]._ptCur->x(), _ti[1]._ptCur->y(), _ti[1]._ptPrev->x(), _ti[1]._ptPrev->y()));
	convTouches(arrTouch, numArray, EVC_BEGIN);	// タッチ配列を内部座標に変換
	////TRACE(VcString::format("#2# beganTouches _ti[0] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[0]._isTouch, _ti[0]._ptCur->x(), _ti[0]._ptCur->y(), _ti[0]._ptPrev->x(), _ti[0]._ptPrev->y()));
	////TRACE(VcString::format("#2# beganTouches _ti[1] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[1]._isTouch, _ti[1]._ptCur->x(), _ti[1]._ptCur->y(), _ti[1]._ptPrev->x(), _ti[1]._ptPrev->y()));

	for(s32 i = 0; i < NUM_TOUCHES; i++)	{	_ti[i]._ptPrev->copy(_ti[i]._ptCur);	}	// 差分用のバックアップを無効化する
	///TRACE(VcString::format("### * 1 * cur [0]={%d, %d}, [1]={%d, %d}, / prev [0]={%d, %d}, [1]={%d, %d}\n", _ti[0]._ptCur->x(), _ti[0]._ptCur->y(), _ti[1]._ptCur->x(), _ti[1]._ptCur->y(), _ti[0]._ptPrev->x(), _ti[0]._ptPrev->y(), _ti[1]._ptPrev->x(), _ti[1]._ptPrev->y()));
	
	// 一連の操作がマルチタップかどうかのカウンタ
	if(_cntMaxTouch < getTouchingNum())	{	_cntMaxTouch = getTouchingNum();	}

	// タッチ位置を保存
	_ptTouchStart->copy(arrTouch[0]._pt);
	_ptTouchLast->copy(_ptTouchStart);
}

/*---------------------------------------------------------------------*//**
	タッチ移動
**//*---------------------------------------------------------------------*/
void TouchPanel::movedTouches(Touch* arrTouch, s32 numArray)
{
	///TRACE(VcString::format("* movedTouches\n"));

	////TRACE("\n");
	////TRACE(VcString::format("#1# movedTouches _ti[0] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[0]._isTouch, _ti[0]._ptCur->x(), _ti[0]._ptCur->y(), _ti[0]._ptPrev->x(), _ti[0]._ptPrev->y()));
	////TRACE(VcString::format("#1# movedTouches _ti[1] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[1]._isTouch, _ti[1]._ptCur->x(), _ti[1]._ptCur->y(), _ti[1]._ptPrev->x(), _ti[1]._ptPrev->y()));
	convTouches(arrTouch, numArray, EVC_MOVE);	// タッチ配列を内部座標に変換
	////TRACE(VcString::format("#2# movedTouches _ti[0] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[0]._isTouch, _ti[0]._ptCur->x(), _ti[0]._ptCur->y(), _ti[0]._ptPrev->x(), _ti[0]._ptPrev->y()));
	////TRACE(VcString::format("#2# movedTouches _ti[1] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[1]._isTouch, _ti[1]._ptCur->x(), _ti[1]._ptCur->y(), _ti[1]._ptPrev->x(), _ti[1]._ptPrev->y()));
	
	// タッチ位置を保存
	_ptTouchLast->copy(arrTouch[0]._pt);
	
	// 動作決定
	if(getTouchingNum() >= 2)
	{
		_moveact = MA_ZOOM;	// ズーム処理へ
	}
	else
	{
		// ある程度の長さの押し続け、ある程度の距離を移動した場合はドラッグとみなす
		if(	(_cntTouchFrame >= FRAME_DRAG) &&
			((TFW_ABS(_ptTouchLast->x() - _ptTouchStart->x()) > RANGE_CLICK_NEAR) ||
			(TFW_ABS(_ptTouchLast->y() - _ptTouchStart->y()) > RANGE_CLICK_NEAR)) )
		{
			_moveact = MA_DRAG;	// ドラッグ処理へ
		}
	}

	// 動作毎の処理
	switch(_moveact)
	{
	case MA_DRAG:
		{
			// ドラッグ処理
			for(int i = 0; i < NUM_TOUCHES; i++)
			{
				if(_ti[i]._isTouch)
				{
					_res->set(ACT_DRAG, _ptTouchStart, _ti[i]._ptCur,  _ti[i]._ptPrev);
					break;
				}
			}
			///TRACE(VcString::format("### drag %d, %d, %d, %d, %d, %d\n", _ptClick->x(), _ptClick->y(), _ti[0]._ptCur->x(), _ti[0]._ptCur->y(), _ti[0]._ptPrev->x(), _ti[0]._ptPrev->y()));
		}
		break;
	case MA_ZOOM:
		{
			// ズーム処理
			PointF32 ptCenter( (_ti[0]._ptCur->x() + _ti[1]._ptCur->x()) / 2, (_ti[0]._ptCur->y() + _ti[1]._ptCur->y()) / 2 );
			f32 rCur = (*_ti[1]._ptCur - *_ti[0]._ptCur).length() * 20;
			f32 rPrev = (*_ti[1]._ptPrev - *_ti[0]._ptPrev).length() * 20;
			_res->set(ACT_ZOOM, _ptTouchStart, &ptCenter, rCur, rPrev);
			///TRACE(VcString::format("### * 2 * cur [0]={%d, %d}, [1]={%d, %d}, r=%d / prev [0]={%d, %d}, [1]={%d, %d}, r=%d\n", _ti[0]._ptCur->x(), _ti[0]._ptCur->y(), _ti[1]._ptCur->x(), _ti[1]._ptCur->y(), rCur, _ti[0]._ptPrev->x(), _ti[0]._ptPrev->y(), _ti[1]._ptPrev->x(), _ti[1]._ptPrev->y(), rPrev));
			///TRACE(VcString::format("### zoom %d, %d, %d, %d, c=%d, p=%d\n", _ptClick->x(), _ptClick->y(), ptCenter.x(), ptCenter.y(), rCur, rPrev));
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	タッチ終了
**//*---------------------------------------------------------------------*/
void TouchPanel::endedTouches(Touch* arrTouch, s32 numArray)
{
	///TRACE(VcString::format("* endedTouches\n"));
	s32 cntTouchFrameBk = _cntTouchFrame;
	s32 cntMaxTouchBk = _cntMaxTouch;
	s32 movefuncBk = _moveact;
	f32	speedLastBk = (f32)_speed.getAverage();
	s32 numTouchBk = getTouchingNum();

	////TRACE("\n");
	////TRACE(VcString::format("#1# endTouches _ti[0] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[0]._isTouch, _ti[0]._ptCur->x(), _ti[0]._ptCur->y(), _ti[0]._ptPrev->x(), _ti[0]._ptPrev->y()));
	////TRACE(VcString::format("#1# endTouches _ti[1] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[1]._isTouch, _ti[1]._ptCur->x(), _ti[1]._ptCur->y(), _ti[1]._ptPrev->x(), _ti[1]._ptPrev->y()));
	convTouches(arrTouch, numArray, EVC_END);	// タッチ配列を内部座標に変換
	////TRACE(VcString::format("#2# endTouches _ti[0] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[0]._isTouch, _ti[0]._ptCur->x(), _ti[0]._ptCur->y(), _ti[0]._ptPrev->x(), _ti[0]._ptPrev->y()));
	////TRACE(VcString::format("#2# endTouches _ti[1] %d, cur={%d, %d}, prev={%d, %d}\n", _ti[1]._isTouch, _ti[1]._ptCur->x(), _ti[1]._ptCur->y(), _ti[1]._ptPrev->x(), _ti[1]._ptPrev->y()));

	if(!isTouching())
	{
		_moveact = MA_NULL;
		_cntTouchFrame = TCNT_ON_RELEASE;
		_cntMaxTouch = 0;
	}

	_speed.reset();	// 速度スタックをリセット

	_ptTouchLast->copy(arrTouch[0]._pt);	// タッチ位置を保存

	s32 numTouchNew = getTouchingNum();
	if(numTouchNew == 1)
	{
		if(numTouchBk == 2)	// 2 本指が 1 本指になった
		{
			// 現在の残った指の位置を開始位置とする
			for(int i = 0; i < NUM_TOUCHES; i++)
			{
				if(_ti[i]._isTouch)
				{
					_ptTouchStart->copy(_ti[i]._ptCur);
					break;
				}
			}
		}
	}
	else if(numTouchNew == 0)
	{
		if((movefuncBk == MA_NULL) || (movefuncBk == MA_DRAG))
		{
			if(TFW_IS_NEAR(_ptTouchLast->x(), _ptTouchStart->x(), RANGE_CLICK_NEAR) && TFW_IS_NEAR(_ptTouchLast->y(), _ptTouchStart->y(), RANGE_CLICK_NEAR))
			{
				if(cntTouchFrameBk >= FRAME_CLICK)
				{
					// クリック処理
					_res->set(ACT_CLICK, _ptTouchStart, _ti[0]._ptCur,  _ti[0]._ptPrev);
					///TRACE(VcString::format("### click %d, %d, %d, %d\n", _ptClick->x(), _ptClick->y(), _ti[0]._ptCur->x(), _ti[0]._ptCur->y()));
					return;
				}
			}

			if(speedLastBk >= SPPED_FLIP)
			{
				if(cntMaxTouchBk <= 1)	// 一本指での操作である必要がある
				{
					// フリップ処理
					_res->set(ACT_FLIP, _ptTouchStart, speedLastBk, _ti[0]._ptCur, _ti[0]._ptPrev);
					///TRACE(VcString::format("### flip %d, %d, %d, %d, %d, %d\n", _ptClick->x(), _ptClick->y(), _ti[0]._ptCur->x(), _ti[0]._ptCur->y(), _ti[0]._ptPrev->x(), _ti[0]._ptPrev->y()));
					return;
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	タッチキャンセル
**//*---------------------------------------------------------------------*/
void TouchPanel::cancelledTouches(Touch* arrTouch, s32 numArray)
{
	///TRACE(VcString::format("* cancelledTouches\n"));

	convTouches(arrTouch, numArray, EVC_END);	// タッチ配列を内部座標に変換
	if(!isTouching())
	{
		_moveact = MA_NULL;
		_cntTouchFrame = TCNT_ON_CANCEL;
		_cntMaxTouch = 0;
	}

	_speed.reset();	// 速度スタックをリセット

	// キャンセル処理
	_res->set(ACT_CANCEL, _ptTouchStart, _ti[0]._ptCur,  _ti[0]._ptPrev);
	///TRACE(VcString::format("### cancel %d, %d, %d, %d\n", _ptClick->x(), _ptClick->y(), _ti[0]._ptCur->x(), _ti[0]._ptCur->y()));

	// タッチ位置を保存
	_ptTouchLast->copy(arrTouch[0]._pt);
}

/*---------------------------------------------------------------------*//**
	カーソル移動
**//*---------------------------------------------------------------------*/
void TouchPanel::moveCursor(f32 x, f32 y)
{
	PointF32 ptScreen(x, y);
	_viewRef->convRealToLogicalPoint(_ptCursorNow, &ptScreen);

	/*
	// 内部座標に変換してセット
	switch(_viewRef->getRotZ())
	{
	case TFW_DISPF_RANG_0:
		_ptCursorNow->set(
			(s32)((                        (x * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleX())),
			(s32)((                        (y * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleY())));
		break;
	case TFW_DISPF_RANG_90:
		_ptCursorNow->set(
			(s32)((                        (y * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleX())),
			(s32)((_viewRef->getHeight() - (x * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleY())));
		break;
	case TFW_DISPF_RANG_180:
		_ptCursorNow->set(
			(s32)((_viewRef->getWidth()  - (x * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleX())),
			(s32)((_viewRef->getHeight() - (y * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleY())));
		break;
	case TFW_DISPF_RANG_270:
		_ptCursorNow->set(
			(s32)((_viewRef->getWidth()  - (y * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleX())),
			(s32)((                        (x * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleY())));
		break;
	}
	*/
}

/*---------------------------------------------------------------------*//**
	タッチ配列を内部座標に変換
**//*---------------------------------------------------------------------*/
void TouchPanel::convTouches(Touch* arrTouch, s32 numArray, EventCase evc)
{
	f32 lenlen[NUM_TOUCHES];
	bool isset[NUM_TOUCHES];
	for(int j = 0; j < NUM_TOUCHES; j++)	{	isset[j] = false;	}

	for(int i = 0; i < numArray; i++)
	{
		_viewRef->convRealToLogicalPoint(arrTouch[i]._pt, arrTouch[i]._pt);
	}
	/*
	// 画面の回転を適用
	switch(_viewRef->getRotZ())
	{
	case TFW_DISPF_RANG_0:
		for(s32 i = 0; i < numArray; i++)
		{
			arrTouch[i].set(
				(s32)((                        (arrTouch[i]._x * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleX())),
				(s32)((                        (arrTouch[i]._y * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleY())));
		}
		break;
	case TFW_DISPF_RANG_90:
		for(s32 i = 0; i < numArray; i++)
		{
			arrTouch[i].set(
//				(s32)((                        (arrTouch[i]._y * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleY())),
//				(s32)((_viewRef->getWidth()  - (arrTouch[i]._x * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleX())));
				(s32)((                        (arrTouch[i]._y * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleX())),
				(s32)((_viewRef->getHeight() - (arrTouch[i]._x * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleY())));
		}
		break;
	case TFW_DISPF_RANG_180:
		for(s32 i = 0; i < numArray; i++)
		{
			arrTouch[i].set(
				(s32)((_viewRef->getWidth()  - (arrTouch[i]._x * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleX())),
				(s32)((_viewRef->getHeight() - (arrTouch[i]._y * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleY())));
		}
		break;
	case TFW_DISPF_RANG_270:
		for(s32 i = 0; i < numArray; i++)
		{
			arrTouch[i].set(
//				(s32)((_viewRef->getHeight() - (arrTouch[i]._y * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleY())),
//				(s32)((                        (arrTouch[i]._x * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleX())));
				(s32)((_viewRef->getWidth()  - (arrTouch[i]._y * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleX())),
				(s32)((                        (arrTouch[i]._x * _viewRef->getContentScale()) * _viewRef->getInvLogicalScaleY())));
		}
		break;
	}
	*/

	// 前回のものをバックアップする
	for(int j = 0; j < NUM_TOUCHES; j++)	{	_ti[j]._ptPrev->copy(_ti[j]._ptCur);	}

	// 近い位置を探して保存
	f32 lenlenMinMin = F32_PMAX;
	for(int i = 0; i < numArray; i++)
	{
		// それぞれとの長さ（の２乗）を求める
		for(int j = 0; j < NUM_TOUCHES; j++)
		{
			if(	isset[j] ||
				((evc == EVC_BEGIN) && _ti[j]._isTouch) ||
				((evc != EVC_BEGIN) && !_ti[j]._isTouch)	)
			{
				// 除外される値
				lenlen[j] = F32_PMAX;
			}
			else
			{
				// 未セットの _ptTouchesCur との距離（の２乗）を求める
				f32 xSub = arrTouch[i]._pt->x() - _ti[j]._ptCur->x();
				f32 ySub = arrTouch[i]._pt->y() - _ti[j]._ptCur->y();
				lenlen[j] = (xSub * xSub) + (ySub * ySub);
				if(evc == EVC_BEGIN)	{	lenlen[j] = 0;	}
			}
		}

		// 最小の長さを求める
		s32 jMin = 0;
		f32 lenlenMin = F32_PMAX;
		for(int j = 0; j < NUM_TOUCHES; j++)
		{
			if(lenlenMin > lenlen[j])
			{
				lenlenMin = lenlen[j];
				jMin = j;
			}
		}

		// 値をセットする
		_ti[jMin]._ptCur->copy(arrTouch[i]._pt);
		_ti[jMin]._isTouch = (evc != EVC_END);
		isset[jMin] = true;

		// 最小の長さを記憶する
		if(lenlenMinMin > lenlenMin)
		{
			lenlenMinMin = lenlenMin;
		}
	}

	// 最小距離をスピードとして記憶
	if(_speedCur < lenlenMinMin) { _speedCur = lenlenMinMin; }

	///TRACE(VcString::format("lenlen={%d,%d},isset={%d,%d}\n", lenlen[0], lenlen[1], isset[0], isset[1]));
	///TRACE(VcString::format("cur={{%d,%d},{%d,%d}},prev={{%d,%d},{%d,%d}}, %d\n", _ti[0]._ptCur->_v[0], _ti[0]._ptCur->_v[1], _ti[1]._ptCur->_v[0], _ti[1]._ptCur->_v[1], _ti[0]._ptPrev->_v[0], _ti[0]._ptPrev->_v[1], _ti[1]._ptPrev->_v[0], _ti[1]._ptPrev->_v[1], _speedLast));
}

//==========================================================================
// TouchPanel::Touch メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TouchPanel::Touch::Touch()
	: _pt(new PointF32(-1, -1))
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TouchPanel::Touch::~Touch()
{
	delete _pt;
}

//==========================================================================
// TouchPanel::Ti メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TouchPanel::Ti::Ti()
	: _isTouch(false)
{
	_ptCur = new PointF32();
	_ptPrev = new PointF32();
	reset();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TouchPanel::Ti::~Ti()
{
	delete _ptCur;
	delete _ptPrev;
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void TouchPanel::Ti::reset()
{
	_isTouch = false;
	_ptCur->set(S32_MIN, S32_MIN);
	_ptPrev->set(S32_MIN, S32_MIN);
}

//==========================================================================
// TouchPanel::Res メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TouchPanel::Res::Res()
	: _action(false)
	, _act(TouchPanel::ACT_NULL)
	, _ptStart(new PointF32())
	, _ptCur(new PointF32())
	, _ptPrev(new PointF32())
	, _ptCenter(new PointF32())
	, _speed(0)
	, _rCur(0.0f)
	, _rPrev(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TouchPanel::Res::~Res()
{
	delete _ptStart;
	delete _ptCur;
	delete _ptPrev;
	delete _ptCenter;
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void TouchPanel::Res::reset()
{
	_action = false;
	_act = TouchPanel::ACT_NULL;
	_ptStart->set(S32_MIN, S32_MIN);
	_ptCur->set(S32_MIN, S32_MIN);
	_ptPrev->set(S32_MIN, S32_MIN);
	_ptCenter->set(S32_MIN, S32_MIN);
	_rCur = 0;
	_rPrev = 0;
}

/*---------------------------------------------------------------------*//**
	値のセット
**//*---------------------------------------------------------------------*/
void TouchPanel::Res::set(Action act, const PointF32* ptStart, const PointF32* ptCur, const PointF32* ptPrev)
{
	_action = true;
	_act = act;
	_ptStart->copy(ptStart);
	_ptCur->copy(ptCur);
	_ptPrev->copy(ptPrev);
	_ptCenter->set(S32_MIN, S32_MIN);
	_speed = 0;
	_rCur = 0;
	_rPrev = 0;
}

/*---------------------------------------------------------------------*//**
	値のセット
**//*---------------------------------------------------------------------*/
void TouchPanel::Res::set(Action act, const PointF32* ptStart, f32 speed, const PointF32* ptCur, const PointF32* ptPrev)
{
	_action = true;
	_act = act;
	_ptStart->copy(ptStart);
	_ptCur->copy(ptCur);
	_ptPrev->copy(ptPrev);
	_ptCenter->set(S32_MIN, S32_MIN);
	_speed = speed;
	_rCur = 0;
	_rPrev = 0;
}

/*---------------------------------------------------------------------*//**
	値のセット
**//*---------------------------------------------------------------------*/
void TouchPanel::Res::set(Action act, const PointF32* ptStart,  const PointF32* ptCenter, f32 rCur, f32 rPrev)
{
	_action = true;
	_act = act;
	_ptStart->copy(ptStart);
	_ptCur->set(S32_MIN, S32_MIN);
	_ptPrev->set(S32_MIN, S32_MIN);
	_ptCenter->copy(ptCenter);
	_speed = 0;
	_rCur = rCur;
	_rPrev = rPrev;
}

//==========================================================================
// TouchPanel::ValueStack メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TouchPanel::ValueStack::ValueStack()
{
	reset();
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void TouchPanel::ValueStack::reset()
{
	_idxStack = 0;
	for(int i = 0; i < NUM_STACK; i++)
	{
		_stack[i] = 0;
	}
}

/*---------------------------------------------------------------------*//**
	値を設定
**//*---------------------------------------------------------------------*/
void TouchPanel::ValueStack::push(u16 val)
{
	_stack[_idxStack] = val;

	_idxStack++;
	if(_idxStack >= NUM_STACK)	{	_idxStack = 0;	}
}

/*---------------------------------------------------------------------*//**
	最大値を得る
**//*---------------------------------------------------------------------*/
u16 TouchPanel::ValueStack::getMax() const
{
	u16 valMax = 0;
	for(int i = 0; i < NUM_STACK; i++)
	{
		if(valMax < _stack[i])	{	valMax = _stack[i];	}
	}
	return valMax;
}

/*---------------------------------------------------------------------*//**
	平均値を得る
**//*---------------------------------------------------------------------*/
u16 TouchPanel::ValueStack::getAverage() const
{
	s32 valSum = 0;
	for(int i = 0; i < NUM_STACK; i++)
	{
		valSum += _stack[i];
	}
	return (u16)(valSum / NUM_STACK);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS