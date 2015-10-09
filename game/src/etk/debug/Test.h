/***********************************************************************//**
 *	Test.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2012/05/28.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_DEBUG_TEST_H_
#define _ETK_DEBUG_TEST_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

#if defined(_DEBUG)
#define IS_TEST_FLAG(f_)		((Test::getTestFlags() & (f_)) != 0)
#else
#define IS_TEST_FLAG			//
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class PerformanceMeter;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	イベント
 *
**//*---------------------------------------------------------------------*/
class Test : public FrameExecDraw
{
	//======================================================================
	// 定数
public:
	// テストフラグ
	static const u32 F_SHOW_PMETER		= 0x00000001;
	static const u32 F_SHOW_DBGMENUST	= 0x00000002;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// パフォーマンスメーターを得る
	virtual PerformanceMeter* getPerformanceMeter() = 0;

	static unsigned int getTestFlags() { return _testflags; }
	static void setTestFlags(u32 flag, bool on) { TFW_SET_FLAG(_testflags, flag, on); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void onKeydownFunctionKey(s32 funckey) = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	//Test();
	virtual ~Test() {}
	virtual bool create(f32 spfRender, f32 spfLogic) = 0;
	virtual void destroy() = 0;
	virtual void prepareExec() = 0;

	//======================================================================
	// 変数
private:
	static u32 _testflags;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_DEBUG_TEST_H_
