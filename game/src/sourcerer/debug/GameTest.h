/***********************************************************************//**
 *	GameTest.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_DEBUG_GAME_TEST_H_
#define _SRCR_DEBUG_GAME_TEST_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../etk/debug/Test.h"

#include "../../tfw/collection/List.h"
#include "../../tfw/framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
class PerformanceMeter;
namespace smenu
{
	class Menu;
}

TFW_END_NS
using namespace smenu;

SRCR_BEGIN_NS

class SubTestBase;

/*---------------------------------------------------------------------*//**
 *	デバッグ クラス
 *
**//*---------------------------------------------------------------------*/
class GameTest : public etk::Test
{
	//======================================================================
	// 定数
public:
	// テストフラグ
	static const u32 F_SHOW_ACT_FRAME	= 0x00000010;
	static const u32 F_SHOW_MY_POS		= 0x00000020;

private:
	enum DbgDisp
	{
		DBGDISP_NULL,
		DBGDISP_ACTFRAME,
		DBGDISP_MYUNITPOS,
		NUM_DBGDISP
	};

	//======================================================================
	// クラス
private:
	class PlayActAll
	{
	public:
		class PlayAct
		{
		public:
			s32 _storyid;
			s32 _evidAct;
			PlayAct() : _storyid(0), _evidAct(0) {}
		};
		List<PlayAct*> _listPlayAct;
		s32 _index;
		s32 _cnt;
		PlayActAll() : _index(-1), _cnt(0) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// パフォーマンスメーターを得る
	PerformanceMeter* getPerformanceMeter() { return _pmeter; }
	// 地面コリジョン表示モードかどうかを得る
	inline bool isShowGcol() const { return _cntShowGcol == 100; }

	// 地面コリジョン表示モードの ON / OFF
	inline void setShowGcol(bool isEnable) { _cntShowGcol = isEnable ? 100 : 0; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void changeTestViewMode();
	void reloadFixedData();
	void onKeydownFunctionKey(s32 funckey);
	void startPlayActAll();

	static void testBasicScript();
	static void testSoumaScript(void* paramRun);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	GameTest();
	~GameTest();
	bool create(f32 spfRender, f32 spfLogic);
	void destroy();
	void prepareExec();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);			// 3D 描画
	void draw(const RenderCtx* rc);				// 2D 描画

	//======================================================================
	// 変数
private:
	SubTestBase*			_subtest;
	PerformanceMeter*		_pmeter;
	Menu*					_menu;
	RectF32*					_rectTmenu;
	s32						_cntShowGcol;
	s32						_ptclkind;		// パーティクルの種類
	s32						_dispCntF3;		// F3 ガイド表示カウンタ
	DbgDisp					_dbgdisp;
	PlayActAll				_paa;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_DEBUG_GAME_TEST_H_
