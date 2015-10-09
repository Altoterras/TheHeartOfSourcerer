/***********************************************************************//**
 *	AtArBody.cpp
 *	Enlight AtArBody Application
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
#include "AtArBody.h"

// Friends
#include "AtArFixedData.h"
#include "LoadingScreen.h"
#include "MaterialManager.h"
#include "MovieFileTextureIos.h"
#include "TorchIos.h"
#include "VideoCameraIos.h"
#include "../debug/AtArTest.h"
#include "../file/AtArFileIdDef.h"
#include "../gui/AtArFontSet.h"
#include "../gui/AtArGui.h"
#include "../particle/AtArParticleSys.h"
#include "../scene/ArScene.h"
#include "../scene/MovieScene.h"
#include "../scene/TitleScene.h"

// External
#include "../../etk/body/EtkExecCtx.h"
#include "../../etk/body/EtkRenderCtx.h"
#include "../../etk/body/OsDepMod.h"
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
#include "../../tfw/string/VcString.h"
#include "../../tfw/string/CcString.h"
#include "../../tfw/ui/Keyboard.h"
#include "../../tfw/ui/TouchPanel.h"
#include "../../tfw/ui/MotionController.h"

// Library

////////////////////////////////////////////////////////////////////////////

ATAR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define IS_WINDOWS_IDFILE_DEVMODE_UNPACK	(1)	// デバッグ時に個別ファイルから読み込む
					
////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// AtArBody メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AtArBody::AtArBody()
	: _fontset(0L)
	, _camDefault(0L)
	, _optflags(0)
	, _mtrlmng(0L)
	, _vcam(0L)
	, _mtex(0L)
	, _torch(0L)
	, _ldscr(0L)
	, _scene(SCENE_TITLE)
{
	for(int i = 0; i < NUM_SCENE; i++)
	{
		_fsScenes[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
AtArBody::~AtArBody()
{
	cleanup(TBCASE_CLOSE);

	ASSERT(_fontset == 0L);
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_SCENE; i++)
		{
			ASSERT(_fsScenes[i] == 0L);
		}
	#endif
	ASSERT(_mtrlmng == 0L);
	ASSERT(_vcam == 0L);
	ASSERT(_mtex == 0L);
	ASSERT(_torch == 0L);
	ASSERT(_ldscr == 0L);
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool AtArBody::init(s32 widthView, s32 heightView, f32 scaleContent, u32 dispflags, f32 spfRender, f32 spfLogic, LangId langidDefault, StartBodyCase sbcase, u32 bcflags, OsDepMod* osdep, PsnsBase* psns)
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

	// タイトルシーンの作成
	TitleScene* ttscene = new TitleScene();
	if((ttscene == 0L) || !ttscene->create(((AtArGui*)_gui)->getGuiTexture()))
	{
		delete ttscene;
		ttscene = 0L;
	}
	_fsScenes[SCENE_TITLE] = ttscene;

	// AR シーンの作成
	ArScene* arscene = new ArScene();
	if((arscene == 0L) || !arscene->create(((AtArGui*)_gui)->getGuiTexture()))
	{
		delete arscene;
		arscene = 0L;
	}
	_fsScenes[SCENE_AR] = arscene;

	// ムービーシーンの作成
	MovieScene* movscene = new MovieScene();
	if((movscene == 0L) || !movscene->create())
	{
		delete movscene;
		movscene = 0L;
	}
	_fsScenes[SCENE_MOVIE] = movscene;
	
	// 素材管理を作成
	VcString pathDocs, pathTmp;
	_osdepRef->getDocumentDirPath(&pathDocs, true);
	_osdepRef->getTemporaryDirPath(&pathTmp);
	_mtrlmng = new MaterialManager();
	if((_mtrlmng == 0L) || !_mtrlmng->create(&pathDocs, &pathTmp))
	{
		delete _mtrlmng;
		_mtrlmng = 0L;
		return false;
	}
	
	// ムービーテクスチャの作成
	MovieFileTextureIos* mtex = new MovieFileTextureIos();
	if((mtex == 0L) || (!mtex->create(512, 512)))
	{
		ASSERT(false);
		delete mtex;
		return false;
	}
	_mtex = mtex;
	
	// 懐中電灯の作成
	TorchIos* torch = new TorchIos();
	if((torch == 0L) || (!torch->create()))
	{
		ASSERT(false);
		delete torch;
		return false;
	}
	_torch = torch;
	
	// ビデオカメラの作成
	VideoCameraIos* vcam = new VideoCameraIos();
//	if((vcam == 0L) || (!vcam->create(VideoCameraIos::VRESO_192x144)))
	if((vcam == 0L) || (!vcam->create(VideoCameraIos::VRESO_352x288)))
//	if((vcam == 0L) || (!vcam->create(VideoCameraIos::VRESO_640x480)))
	{
		ASSERT(false);
		delete vcam;
		return false;
	}
	_vcam = vcam;
	
	// ローディングスクリーンの作成
	LoadingScreen* ldscr = new LoadingScreen();
	if((ldscr == 0L) || (!ldscr->create(SYSTEM_LOADING_PNG)))
	{
		ASSERT(false);
		delete ldscr;
		return false;
	}
	_ldscr = ldscr;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// システムの開始

	#if DEBUG_FULLMODE
		debug_onDebugMode(999);
	#endif

	// フェードインから開始
	#if 0
	if(_screffScn != 0L)
	{
		ColorU8 colBlack(0, 0, 0, 255);
		_screffScn->setColor1(&colBlack);
		_screffScn->setMaxFrame(30);
		_screffScn->start(ScreenEffect::MODE_FADE_IN);
	}
	#endif

	// タイトルから
	_scene = SCENE_TITLE;
	_fsScenes[SCENE_TITLE]->beginScene(0L);

	prepareUpdateFrameContext(0.0f);	// 初回描画用のコンテキスト作成
	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void AtArBody::cleanup(TerminateBodyCase tbcase)
{
	TRACELN("{AtArBody::cleanup} called. tbcase=%d", tbcase);

	// ローディングスクリーンの破棄
	if(_ldscr != 0L)
	{
		_ldscr->destroy();
		_ldscr = 0L;
	}

	// ビデオカメラの破棄
	if(_vcam != 0L)
	{
		_vcam->destroy();
		_vcam = 0L;
	}
	
	// 懐中電灯の破棄
	if(_torch != 0L)
	{
		_torch->destroy();
		_torch = 0L;
	}

	// ムービーテクスチャの破棄
	if(_mtex != 0L)
	{
		_mtex->destroy();
		_mtex = 0L;
	}
	
	// 素材管理の削除
	if(_mtrlmng != 0L)
	{
		_mtrlmng->destroy();
		delete _mtrlmng;
		_mtrlmng = 0L;
	}

	// ムービーシーンの削除
	MovieScene* movscene = (MovieScene*)_fsScenes[SCENE_MOVIE];
	if(movscene != 0L)
	{
		movscene->destroy();
		delete movscene;
		_fsScenes[SCENE_MOVIE] = 0L;
	}

	// AR シーンの削除
	ArScene* arscene = (ArScene*)_fsScenes[SCENE_AR];
	if(arscene != 0L)
	{
		arscene->destroy();
		delete arscene;
		_fsScenes[SCENE_AR] = 0L;
	}

	// タイトルシーンの削除
	TitleScene* ttscene = (TitleScene*)_fsScenes[SCENE_TITLE];
	if(ttscene != 0L)
	{
		ttscene->destroy();
		delete ttscene;
		_fsScenes[SCENE_TITLE] = 0L;
	}

	// フォントセット解放
	if(_fontset != 0L)
	{
		_fontset->destroy();
		delete _fontset;
		_fontset = 0L;
	}

	// 固定データの後処理
	AtArFixedData::cleanup();

	EtkBody::cleanup(tbcase);
}

/*---------------------------------------------------------------------*//**
	フレーム更新コンテキストの準備
**//*---------------------------------------------------------------------*/
void AtArBody::prepareUpdateFrameContext(f32 frameDelta)
{
	((EtkExecCtx*)_ecUpdateFrame)->set(
		frameDelta,
		0, // (_isNoAnimIntp ? ExecCtx::EF_NO_ANIM_INTERP : 0) | (_evsys->isWaitMovieMessageNow() || _evsys->isEventPause() ? ExecCtx::EF_PAUSE_ANIM : 0)
		_touchp,
		_keybrd,
		_moctrlr,
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
void AtArBody::execFrame(const ExecCtx* ec)
{
	ExecRes eres;
	_touchp->exec();
	_keybrd->exec();
	_moctrlr->exec();
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
	_ptclsys->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x770077);	// パフォーマンス計測
	_mtrlmng->exec(0L, ec);
	eres.reset();
	_fsScenes[_scene]->exec(&eres, ec);
	if(eres.isDone())
	{
		_fsScenes[_scene]->endScene(&eres);
		_scene = (Scene)eres.getGeneralValue1();
		_fsScenes[_scene]->beginScene(&eres);
	}
	_ldscr->exec(&eres, ec);
	EXEC_NO3D:
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x777700);	// パフォーマンス計測
	_sndmng->exec(ec);
	if(_scrtrc != 0L)	{	_scrtrc->exec(0L, ec);	}
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x777777);	// パフォーマンス計測
}

/*---------------------------------------------------------------------*//**
	フレーム描画
**//*---------------------------------------------------------------------*/
void AtArBody::renderFrame(const RenderCtx* rc)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 3D 描画
	if(_isSkip3d)	{	goto RENDER_2DSCENE;	}	// 3D 処理のスキップ

	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x0000ff);	// パフォーマンス計測
	rc->getRenderer()->setDefault3dRenderState();					// デフォルトの 3D 描画ステイトに設定する
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x000077);	// パフォーマンス計測
	_tfw->render(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x007700);	// パフォーマンス計測
	_fsScenes[_scene]->render(rc);
	_ldscr->render(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x007777);	// パフォーマンス計測
	#if defined(_DEBUG)
		_test->render(rc);
	#endif

	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x777777);	// パフォーマンス計測

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 2D 描画

	RENDER_2DSCENE:
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0x0000ff);	// パフォーマンス計測
	rc->getRenderer()->setDefault2dRenderState();					// デフォルトの 2D 描画ステイトに設定する
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0x000077);	// パフォーマンス計測
	_fsScenes[_scene]->draw(rc);
	_ldscr->draw(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0x007700);	// パフォーマンス計測
	_tasksys->draw(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0x007777);	// パフォーマンス計測
	#if defined(_DEBUG)
		_test->draw(rc);
	#endif
	if(_scrtrc != 0L)
	{
		rc->getRenderer()->setSolidColor(127, 127, 255, 255);
		_scrtrc->draw(rc);
	}
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0xffffff);	// パフォーマンス計測
}

/*---------------------------------------------------------------------*//**
	フレーム制御コンテキストの作成実装
**//*---------------------------------------------------------------------*/
ExecCtx* AtArBody::makeExecCtx()
{
	return new EtkExecCtx();
}

/*---------------------------------------------------------------------*//**
	フレーム描画コンテキストの作成実装
**//*---------------------------------------------------------------------*/
RenderCtx* AtArBody::makeRenderCtx()
{
	return new EtkRenderCtx();
}

/*---------------------------------------------------------------------*//**
	ポーズの作成実装
**//*---------------------------------------------------------------------*/
PauseScreen* AtArBody::makePauseScreen()
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
ParticleSys* AtArBody::makeParticleSys()
{
	const ParticleDefTbl* ptcldftbl = AtArFixedData::getParticleDefTable();

	etk::SorSgm* sg = (etk::SorSgm*)getSceneGraphManager();
	SgNodeChain* sgnc = sg->getKindChain(etk::SorSgm::EK_ZSORT);

	ParticleSys* ptclsys = new AtArParticleSys();
	if(!ptclsys->create(ptcldftbl, sgnc))
	{
		delete ptclsys;
		ASSERTM(false, "ParticleSys::create failed.");
		return false;
	}
	return ptclsys;
}

/*---------------------------------------------------------------------*//**
	シーングラフマネージャの作成実装
**//*---------------------------------------------------------------------*/
SgManager* AtArBody::makeSceneGraphManager()
{
	return new etk::SorSgm();
}

/*---------------------------------------------------------------------*//**
	GUI の作成実装
**//*---------------------------------------------------------------------*/
EtkGui* AtArBody::makeGui()
{
	// 固定データの初期化
	if(!AtArFixedData::init())
	{
		ASSERTM(false, "FixedData::init failed.");
		return 0L;
	}

	// フォントセットの作成
	AtArFontSet* fontset = new AtArFontSet();
	if(!fontset->create())
	{
		delete fontset;
		ASSERTM(false, "GameFontSet::create failed.");
		return 0L;
	}
	_fontset = fontset;

	// GUI の作成
	AtArGui* gui = new AtArGui();
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
Test* AtArBody::makeTest(f32 spfRender, f32 spfLogic)
{
	Test* test = new AtArTest();
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
Font* AtArBody::acquireScreenTraceFont()
{
	return _fontset->getFont(AtArFontSet::EN);
}

////////////////////////////////////////////////////////////////////////////

ATAR_END_NS
