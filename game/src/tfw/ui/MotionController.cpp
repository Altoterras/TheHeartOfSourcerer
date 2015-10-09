/***********************************************************************//**
 *	MotionController.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2013/07/28.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MotionController.h"

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
#define RANGE_CLICK_NEAR	(10)		// クリックと判定される初期位置との位置差
#define FRAME_DCLICK_ALLOW	(15)		// 許可する１回目のクリックと２回目のクリックの間のフレーム数
#define FRAME_REPEAT_FW		(30)		// リピートカウントの最初の待ちフレーム
#define FRAME_REPEAT		(7)			// リピートカウント（ビット演算をするので“2 の累乗 - 1”である必要がある）

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MotionController メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	モーション操作中かどうかの判定
**//*---------------------------------------------------------------------*/
bool MotionController::isMotion() const
{
	if(_mi._isMotion)	{	return true;	}
	return false;
}

/*---------------------------------------------------------------------*//**
	モーション終了判定
**//*---------------------------------------------------------------------*/
bool MotionController::isMotionEnd() const
{
	if(_cntMotionFrame != TCNT_END)			{	return false;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	最終動作を得る
**//*---------------------------------------------------------------------*/
MotionController::Action MotionController::getLastAction() const
{
	return _res._act;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MotionController::MotionController()
	: _cntMotionFrame(TCNT_NOTHING)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MotionController::~MotionController()
{
}

/*---------------------------------------------------------------------*//**
	構築
**//*---------------------------------------------------------------------*/
bool MotionController::create()
{
	// カウンタリセット
	_cntMotionFrame = TCNT_NOTHING;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MotionController::destroy()
{
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void MotionController::exec()
{
	///STRACE("{!1!} speed: _cntMotionFrame=%d\n", _cntMotionFrame);

	// アクションがない場合は返答情報リセット
	if(!_res._action)
	{
		_res.reset();
	}
	_res._action = false;

	// 反応済みフラグをオフに
	TFW_SET_FLAG(_eflags, EF_REACTED, false);

	// モーションカウンタ更新
	if(_cntMotionFrame >= 0)		// タッチ中
	{
		if(_cntMotionFrame >= 0x7fffffff)	{	_cntMotionFrame = TCNT_BEGIN + 1;	}
		else								{	_cntMotionFrame++;					}
	}
	else							// タッチなし
	{
		if((_cntMotionFrame == TCNT_ON_END) || (_cntMotionFrame == TCNT_ON_CANCEL))
		{
			_cntMotionFrame++;				// それぞれの状態へ
		}
		else if(_cntMotionFrame != TCNT_NOTHING)
		{
			_cntMotionFrame = TCNT_NOTHING;	// 無効状態へ
		}
	}
}

/*---------------------------------------------------------------------*//**
	モーション開始
**//*---------------------------------------------------------------------*/
void MotionController::beganMotion(Motion* motion)
{
	if(!isMotion())
	{
		_mi.reset();
	}
	if(_cntMotionFrame < 0)
	{
		_cntMotionFrame = TCNT_ON_BEGIN;
	}

	convMotion(motion, EVC_BEGIN);	// タッチ配列を内部座標に変換
}

/*---------------------------------------------------------------------*//**
	モーション終了
**//*---------------------------------------------------------------------*/
void MotionController::endedMotion(Motion* motion)
{
	s32 cntTouchFrameBk = _cntMotionFrame;

	convMotion(motion, EVC_END);	// タッチ配列を内部座標に変換

	if(!isMotion())
	{
		_cntMotionFrame = TCNT_ON_END;
	}

	_res.set(ACT_SHAKE);
}

/*---------------------------------------------------------------------*//**
	モーションキャンセル
**//*---------------------------------------------------------------------*/
void MotionController::cancelledMotion(Motion* motion)
{
	convMotion(motion, EVC_END);	// タッチ配列を内部座標に変換
	if(!isMotion())
	{
		_cntMotionFrame = TCNT_ON_CANCEL;
	}

	// キャンセル処理
	_res.set(ACT_CANCEL);
}

/*---------------------------------------------------------------------*//**
	モーション配列を内部座標に変換
**//*---------------------------------------------------------------------*/
void MotionController::convMotion(Motion* motion, EventCase evc)
{
	_mi._isMotion = (evc != EVC_END);
}

//==========================================================================
// MotionController::Mi メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MotionController::Mi::Mi()
	: _isMotion(false)
{
	reset();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MotionController::Mi::~Mi()
{
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void MotionController::Mi::reset()
{
	_isMotion = false;
}

//==========================================================================
// MotionController::Res メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MotionController::Res::Res()
	: _action(false)
	, _act(MotionController::ACT_NULL)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MotionController::Res::~Res()
{
}

/*---------------------------------------------------------------------*//**
	リセット
**//*---------------------------------------------------------------------*/
void MotionController::Res::reset()
{
	_action = false;
	_act = MotionController::ACT_NULL;
}

/*---------------------------------------------------------------------*//**
	値のセット
**//*---------------------------------------------------------------------*/
void MotionController::Res::set(Action act)
{
	_action = true;
	_act = act;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS