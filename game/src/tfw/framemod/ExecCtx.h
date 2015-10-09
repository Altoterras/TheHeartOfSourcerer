/***********************************************************************//**
 *  ExecCtx.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FRAMEMOD_EXEC_CTX_H_
#define _TFW_FRAMEMOD_EXEC_CTX_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	フレーム制御コンテキスト
 *
**//*---------------------------------------------------------------------*/
class ExecCtx
{
	//======================================================================
	// 定数
public:
	// 実行制御フラグ
	static const u32	EF_NO_ANIM_INTERP	= 0x00000001;	// アニメーション補間なし
	static const u32	EF_PAUSE_ANIM		= 0x00000002;	// アニメーション一時停止
	static const u32	EF__CUSTOM__MASK_	= 0xffff0000;	// カスタムフラグマスク

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 前フレームからの差分経過時間を得る
	inline f32 getDeltaFrame() const { return _frameDelta; }
	// 実行制御フラグを得る
	inline u32 getExecFlags() const { return _eflags; }

	// 実行制御フラグを設定する
	inline void setExecFlags(u32 f, bool is) { TFW_SET_FLAG(_eflags, f, is); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	ExecCtx() : _frameDelta(0.0f), _eflags(0)
	{
	}

	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	ExecCtx(f32 frameDelta, u32 eflags) : _frameDelta(frameDelta), _eflags(eflags)
	{
	}

	/*-----------------------------------------------------------------*//**
		デストラクタ
	**//*-----------------------------------------------------------------*/
	virtual ~ExecCtx()
	{
	}

	/*-----------------------------------------------------------------*//**
		セット
	**//*-----------------------------------------------------------------*/
	void set(f32 frameDelta, u32 eflags)
	{
		_frameDelta = frameDelta;
		_eflags = eflags;
	}

	//======================================================================
	// メンバ変数
private:
	f32 _frameDelta;	// 前フレームからの差分経過時間
	u32 _eflags;		// 実行制御フラグ
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_FRAMEMOD_EXEC_CTX_H_
