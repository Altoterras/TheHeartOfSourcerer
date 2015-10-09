/***********************************************************************//**
 *	BasicModelBody.cpp
 *	Enlight BasicModelBody Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// マクロ

#define DEBUG_SHOW_MAIN_ARGS	(0)
#define DEBUG_FULLMODE			(0)
#define DEBUG_DROP_FRAME_TEST	(0)
#if defined(_ENLIGHT_PRPZL01)
	#define DEBUG_PRPZL_FIRST		(1)
	#define DEBUG_MODELVW_FIRST		(0)
	#define DEBUG_BOOKVW_FIRST		(0)
#else
	#define DEBUG_PRPZL_FIRST		(0)
	#define DEBUG_MODELVW_FIRST		(1)
	#define DEBUG_BOOKVW_FIRST		(0)
	#define DEBUG_TXTEDITOR_FIRST	(0)
#endif

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "BasicModelBody.h"

// Friends
#include "BasicModelFixedData.h"
#include "ModelViewer.h"
#include "../debug/BasicModelTest.h"
#include "../file/BasicModelFileIdDef.h"
#include "../gui/BasicModelFontSet.h"
#include "../gui/BasicModelGui.h"

// External
#include "../../etk/body/EtkExecCtx.h"
#include "../../etk/body/EtkRenderCtx.h"
#include "../../etk/effect/ScreenEffect.h"
#include "../../etk/sg/SorSgm.h"
#include "../../etk/sound/SoundMngr.h"
#include "../../tfw/Tfw.h"
#include "../../tfw/common/PerformanceMeter.h"
#include "../../tfw/common/ResourcePool.h"
#include "../../tfw/debug/ScreenTrace.h"
#include "../../tfw/file/IdFile.h"
#include "../../tfw/framemod/FedTaskSys.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/Point.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/lib/StopWatch.h"
#include "../../tfw/lib/TypeUtils.h"
#include "../../tfw/serialize/Serializer.h"
#include "../../tfw/string/CcString.h"
#include "../../tfw/string/VcString.h"
#include "../../tfw/ui/Keyboard.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

BASICMODEL_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define IS_WINDOWS_IDFILE_DEVMODE_UNPACK	(1)	// デバッグ時に個別ファイルから読み込む
					
////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BasicModelBody メソッド

//==========================================================================
// Game メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	マルチタスキングモードかどうかを得る
**//*---------------------------------------------------------------------*/
bool BasicModelBody::isMultiTasking() const
{
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BasicModelBody::BasicModelBody()
	: _fontset(0L)
	, _modelview(0L)
	, _camDefault(0L)
	, _optflags(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BasicModelBody::~BasicModelBody()
{
	cleanup(TBCASE_CLOSE);

	ASSERT(_fontset == 0L);
	ASSERT(_modelview == 0L);
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool BasicModelBody::init(s32 widthView, s32 heightView, f32 scaleContent, u32 dispflags, f32 spfRender, f32 spfLogic, LangId langidDefault, StartBodyCase sbcase, u32 bcflags, OsDepMod* osdep, PsnsBase* psns)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 基本設定

	#if defined(_DEBUG) && defined(_WINDOWS) && IS_WINDOWS_IDFILE_DEVMODE_UNPACK
		IdFile::enableDevMode(g_filedefTbfPack, &CcString(""));
	#endif

	// 親クラス（ツールキット）による初期化
	if(!EtkBody::init(widthView, heightView, scaleContent, dispflags, spfRender, spfLogic, langidDefault, sbcase, bcflags, osdep, psns))
	{
		return false;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 各システムモジュールの初期化

	// モデルビューワーの作成
	_modelview = new ModelViewer();
	if((_modelview == 0L) || !_modelview->create())
	{
		delete _modelview;
		_modelview = 0L;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// システムの開始

	#if DEBUG_FULLMODE
		debug_onDebugMode(999);
	#endif

	// フェードイン
	if(_screffScn != 0L)
	{
		ColorU8 colBlack(0, 0, 0, 255);
		_screffScn->setColor1(&colBlack);
		_screffScn->setMaxFrame(30);
		_screffScn->start(ScreenEffect::MODE_FADE_IN);
	}

	prepareUpdateFrameContext(0.0f);	// 初回描画用のコンテキスト作成
	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void BasicModelBody::cleanup(TerminateBodyCase tbcase)
{
	TRACELN("{BasicModelBody::cleanup} called. tbcase=%d", tbcase);

	// フォントセット解放
	if(_fontset != 0L)
	{
		_fontset->destroy();
		delete _fontset;
		_fontset = 0L;
	}

	// モデルビューワーの削除
	if(_modelview != 0L)
	{
		_modelview->destroy();
		delete _modelview;
		_modelview = 0L;
	}

	// 固定データの後処理
	BasicModelFixedData::cleanup();

	EtkBody::cleanup(tbcase);
}

/*---------------------------------------------------------------------*//**
	フレーム更新コンテキストの準備
**//*---------------------------------------------------------------------*/
void BasicModelBody::prepareUpdateFrameContext(f32 frameDelta)
{
	((EtkExecCtx*)_ecUpdateFrame)->set(
		frameDelta,
		0, // (_isNoAnimIntp ? ExecCtx::EF_NO_ANIM_INTERP : 0) | (_evsys->isWaitMovieMessageNow() || _evsys->isEventPause() ? ExecCtx::EF_PAUSE_ANIM : 0)
		_touchp,
		_keybrd,
		0L,
		_cntBodyExec,
		_frameBodyElapsed,
		_gui->isOpenedLockGui(),
		false	);

	((EtkRenderCtx*)_rcUpdateFrame)->set(
		_tfw->getRenderer(),
		_cntBodyExec,
		_frameBodyElapsed	);
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void BasicModelBody::execFrame(const ExecCtx* ec)
{
	_touchp->exec();
	_keybrd->exec();
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x0000ff);	// パフォーマンス計測
	#if defined(_DEBUG)
		_test->exec(0L, ec);
	#endif
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0xff0000);	// パフォーマンス計測
	_gui->exec(0L, ec);
	_tasksys->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x770000);	// パフォーマンス計測
	if(_isSkip3d)	{	goto EXEC_NO3D;	}	// 3D 処理のスキップ
	_tfw->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x770077);	// パフォーマンス計測
	_modelview->exec(0L, ec);
	EXEC_NO3D:
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x777700);	// パフォーマンス計測
	_sndmng->exec(ec);
	if(_scrtrc != 0L)	{	_scrtrc->exec(0L, ec);	}
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x777777);	// パフォーマンス計測
}

/*---------------------------------------------------------------------*//**
	フレーム描画
**//*---------------------------------------------------------------------*/
void BasicModelBody::renderFrame(const RenderCtx* rc)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 3D 描画
	if(_isSkip3d)	{	goto RENDER_2DSCENE;	}	// 3D 処理のスキップ

	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x0000ff);	// パフォーマンス計測
	rc->getRenderer()->setDefault3dRenderState();					// デフォルトの 3D 描画ステイトに設定する
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x00ff00);	// パフォーマンス計測
	_tfw->render(rc);
	///rdr->setSolidColorForce(255, 255, 255, 255);	// モデル描画によるカラー変更を強制リセット
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x000077);	// パフォーマンス計測
	_modelview->render(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x007700);	// パフォーマンス計測
	#if defined(_DEBUG)
		_test->render(rc);
	#endif

	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x777777);	// パフォーマンス計測

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2D 描画

	RENDER_2DSCENE:
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0x0000ff);	// パフォーマンス計測
	rc->getRenderer()->setDefault2dRenderState();					// デフォルトの 2D 描画ステイトに設定する
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0x00ff00);	// パフォーマンス計測
	_tasksys->draw(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0xffff00);	// パフォーマンス計測
	#if defined(_DEBUG)
		_test->draw(rc);
	#endif
	if(_scrtrc != 0L)	{	_scrtrc->draw(rc);	}
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0xffffff);	// パフォーマンス計測
}

/*---------------------------------------------------------------------*//**
	フレーム制御コンテキストの作成実装
**//*---------------------------------------------------------------------*/
ExecCtx* BasicModelBody::makeExecCtx()
{
	return new EtkExecCtx();
}

/*---------------------------------------------------------------------*//**
	フレーム描画コンテキストの作成実装
**//*---------------------------------------------------------------------*/
RenderCtx* BasicModelBody::makeRenderCtx()
{
	return new EtkRenderCtx();
}

/*---------------------------------------------------------------------*//**
	ポーズの作成実装
**//*---------------------------------------------------------------------*/
PauseScreen* BasicModelBody::makePauseScreen()
{
	/*
	// 専用ポーズの作成
	GamePause* pause = new GamePause();
	if(!pause->create())
	{
		delete pause;
		ASSERTM(false, "GamePause::create failed.");
		return 0L;
	}

	return pause;
	*/
	return 0;
}

/*---------------------------------------------------------------------*//**
	パーティクルシステムの作成実装
**//*---------------------------------------------------------------------*/
ParticleSys* BasicModelBody::makeParticleSys()
{
	/*
	const ParticleDefTbl* ptcldftbl = BasicModelFixedData::getParticleDefTable();

	sourcerer::SorSgm* sg = (sourcerer::SorSgm*)getSceneGraphManager();
	SgNodeChain* sgnc = sg->getKindChain(sourcerer::SorSgm::EK_ZSORT);

	ParticleSys* ptclsys = new BasicModelParticleSys();
	if(!ptclsys->create(ptcldftbl, sgnc))
	{
		delete ptclsys;
		ASSERTM(false, "ParticleSys::create failed.");
		return false;
	}
	return ptclsys;
	*/
	return 0L;
}

/*---------------------------------------------------------------------*//**
	シーングラフマネージャの作成実装
**//*---------------------------------------------------------------------*/
SgManager* BasicModelBody::makeSceneGraphManager()
{
	return new etk::SorSgm();
}

/*---------------------------------------------------------------------*//**
	GUI の作成実装
**//*---------------------------------------------------------------------*/
EtkGui* BasicModelBody::makeGui()
{
	// 固定データの初期化
	if(!BasicModelFixedData::init())
	{
		ASSERTM(false, "FixedData::init failed.");
		return 0L;
	}

	// フォントセットの作成
	BasicModelFontSet* fontset = new BasicModelFontSet();
	if(!fontset->create())
	{
		delete fontset;
		ASSERTM(false, "GameFontSet::create failed.");
		return 0L;
	}
	_fontset = fontset;

	// GUI の作成
	BasicModelGui* gui = new BasicModelGui();
	if(!gui->create(_fontset, _tasksys))
	{
		delete gui;
		ASSERTM(false, "GameGui::create failed.");
		return 0L;
	}
	return gui;
}

/*---------------------------------------------------------------------*//**
	テストモジュールの作成実装
**//*---------------------------------------------------------------------*/
Test* BasicModelBody::makeTest(f32 spfRender, f32 spfLogic)
{
	Test* test = new BasicModelTest();
	if(!test->create(spfRender, spfLogic))
	{
		delete test;
		ASSERTM(false, "Test::create failed.");
		return 0L;
	}
	return test;
}

/*---------------------------------------------------------------------*//**
	スクリーントレースのフォント取得
**//*---------------------------------------------------------------------*/
Font* BasicModelBody::acquireScreenTraceFont()
{
	return _fontset->getFont(BasicModelFontSet::EN);
}

////////////////////////////////////////////////////////////////////////////

BASICMODEL_END_NS
