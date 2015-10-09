/***********************************************************************//**
 *	EtkExecCtx.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2012/05/29.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_BODY_ETK_EXEC_CTX_H_
#define _ETK_BODY_ETK_EXEC_CTX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/ExecCtx.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Keyboard;
class MotionController;
class TouchPanel;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ツールキット フレーム制御コンテキスト
 *
**//*---------------------------------------------------------------------*/
class EtkExecCtx : public ExecCtx
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// タッチパネルを得る
	inline TouchPanel* getTouchPanel() const { return _touchpRef; }
	// キーボードを得る
	inline Keyboard* getKeyboard() const { return _keybrdRef; }
	// モーションコントローラを得る
	inline MotionController* getMotionController() const { return _moctrlrRef; }
	// ボディカウンタを取得する
	inline u32 getBodyExecCount() const { return _cntBodyExec; }	// このカウンタは可変フレームに関係なくインクリメントされる値であることに注意
	// ボディの経過フレームを取得する
	inline f32 getBodyElapsedFrame() const { return _frameBodyElapsed; }
	// 差分経過時間 1024 倍整数値を得る
	inline s32 getMilliDeltaFrame() const { return _miframeDelta; }
	// GUI による操作ロック中かを得る
	inline bool isOpenedLockGui() const { return _isOpenedLockGui; }
	// イベントポーズ中かどうかを得る
	inline bool isEventPause() const { return _isEventPause; }
	// イベント等による操作ロック中かを得る
	virtual bool isLockOperating() const { return false; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	EtkExecCtx()
		: _touchpRef(0L)
		, _keybrdRef(0L)
		, _moctrlrRef(0L)
		, _cntBodyExec(0)
		, _frameBodyElapsed(0.0l)
		, _miframeDelta(0)
		, _isOpenedLockGui(false)
		, _isEventPause(false)
	{
	}

	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	EtkExecCtx(f32 frameDelta, u32 eflags, TouchPanel* touchpRef, Keyboard* keybrdRef, MotionController* moctrlrRef, u32 cntBodyExec, f32 frameBodyElapsed, bool isOpenedLockGui, bool isEventPause)
		: ExecCtx(frameDelta, eflags)
		, _touchpRef(touchpRef)
		, _keybrdRef(keybrdRef)
		, _moctrlrRef(moctrlrRef)
		, _cntBodyExec(cntBodyExec)
		, _frameBodyElapsed(frameBodyElapsed)
		, _miframeDelta((s32)(frameDelta * 1024.0f))
		, _isOpenedLockGui(isOpenedLockGui)
		, _isEventPause(isEventPause)
	{
	}

	/*-----------------------------------------------------------------*//**
		セット
	**//*-----------------------------------------------------------------*/
	void set(f32 frameDelta, u32 eflags, TouchPanel* touchpRef, Keyboard* keybrdRef, MotionController* moctrlrRef, u32 cntBodyExec, f32 frameBodyElapsed, bool isOpenedLockGui, bool isEventPause)
	{
		ExecCtx::set(frameDelta, eflags);
		_touchpRef = touchpRef;
		_keybrdRef = keybrdRef;
		_moctrlrRef = moctrlrRef;
		_cntBodyExec = cntBodyExec;
		_frameBodyElapsed = frameBodyElapsed;
		_miframeDelta = (s32)(frameDelta * 1024.0f);
		_isOpenedLockGui = isOpenedLockGui;
		_isEventPause = isEventPause;
	}

	//======================================================================
	// 変数
private:
	TouchPanel* _touchpRef;
	Keyboard* _keybrdRef;
	MotionController* _moctrlrRef;
	u32 _cntBodyExec;
	f32 _frameBodyElapsed;
	s32 _miframeDelta;			// 差分経過時間 1024 倍整数値
	bool _isOpenedLockGui;
	bool _isEventPause;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_BODY_ETK_EXEC_CTX_H_
