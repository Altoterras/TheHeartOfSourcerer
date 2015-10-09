/***********************************************************************//**
 *	BasicModelTest.h
 *	Enlight BasicModel Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _BASICMODEL_DEBUG_BASICMODEL_TEST_H_
#define _BASICMODEL_DEBUG_BASICMODEL_TEST_H_

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
class ShapeModel;
namespace smenu
{
	class Menu;
}

TFW_END_NS
using namespace smenu;

BASICMODEL_BEGIN_NS

class SubTestBase;

/*---------------------------------------------------------------------*//**
 *	デバッグ クラス
 *
**//*---------------------------------------------------------------------*/
class BasicModelTest : public etk::Test
{
	//======================================================================
	// 定数
private:

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
	BasicModelTest();
	~BasicModelTest();
	bool create(f32 spfRender, f32 spfLogic);
	void destroy();
	void prepareExec();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void render(const RenderCtx* rc);			// 3D 描画
	void draw(const RenderCtx* rc);				// 2D 描画

	//======================================================================
	// 変数
private:
	PerformanceMeter* _pmeter;
	Menu* _menu;
};

BASICMODEL_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _BASICMODEL_DEBUG_BASICMODEL_TEST_H_
