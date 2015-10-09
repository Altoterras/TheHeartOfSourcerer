/***********************************************************************//**
 *	EdevTest.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _EDEV_DEBUG_EDEV_TEST_H_
#define _EDEV_DEBUG_EDEV_TEST_H_

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
typedef Rect<f32> RectF;
class Camera;
class PerformanceMeter;
namespace smenu
{
	class Menu;
}

TFW_END_NS
using namespace smenu;

EDEV_BEGIN_NS

class SubTestBase;

/*---------------------------------------------------------------------*//**
 *	デバッグ クラス
 *
**//*---------------------------------------------------------------------*/
class EdevTest : public etk::Test
{
	//======================================================================
	// 定数
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
	// プロパティ
public:
	// パフォーマンスメーターを得る
	inline PerformanceMeter* getPerformanceMeter() { return _pmeter; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void onKeydownFunctionKey(s32 funckey);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EdevTest();
	~EdevTest();
	bool create(f32 spfRender, f32 spfLogic);
	void destroy();
	void prepareExec();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);			// 3D 描画
	void draw(const RenderCtx* rc);				// 2D 描画

	//======================================================================
	// 変数
private:
	PerformanceMeter*		_pmeter;
	Menu*					_menu;
	Camera*					_camMvRef;
};

EDEV_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _EDEV_DEBUG_EDEV_TEST_H_
