/***********************************************************************//**
 *	EtkBody.cpp
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2012/05/25.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EtkBody.h"

// Friends
#include "EtkExecCtx.h"
#include "EtkNotifier.h"
#include "EtkRenderCtx.h"
#include "EtkTaskModDef.h"
#include "PauseScreen.h"
#include "../debug/Test.h"
#include "../effect/ScreenEffect.h"
#include "../gui/EtkGui.h"
#include "../particle/ParticleSys.h"
#include "../sound/SoundMngr.h"

// External
#include "../../tfw/common/PerformanceMeter.h"
#include "../../tfw/common/ResourcePool.h"
#include "../../tfw/debug/ScreenTrace.h"
#include "../../tfw/framemod/FedTaskSys.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/lib/StopWatch.h"
#include "../../tfw/lib/TypeUtils.h"
#include "../../tfw/ui/Keyboard.h"
#include "../../tfw/ui/TouchPanel.h"
#include "../../tfw/ui/MotionController.h"

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EtkBody 静的変数

EtkBody* EtkBody::_body = 0L;

//==========================================================================
// EtkBody メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

#if 0
/*---------------------------------------------------------------------*//**
	シーングラフマネージャの取得
**//*---------------------------------------------------------------------*/
SgManager* EtkBody::getSceneGraphMngr() const
{
	return _tfw->getSgManager();
}

/*---------------------------------------------------------------------*//**
	リソースプールの取得
**//*---------------------------------------------------------------------*/
ResourcePool* EtkBody::getResourcePool() const
{
	return _tfw->getResourcePool();
}
#endif

/*---------------------------------------------------------------------*//**
	ID マネージャの取得
**//*---------------------------------------------------------------------*/
IdManager* EtkBody::getIdManager() const
{
	return _tfw->getResourcePool()->getIdManager();
}

/*---------------------------------------------------------------------*//**
	スクリーンの幅を得る
**//*---------------------------------------------------------------------*/
f32 EtkBody::getScreenWidth() const
{
	return _tfw->getView()->getWidth();
}

/*---------------------------------------------------------------------*//**
	スクリーンの高さを得る
**//*---------------------------------------------------------------------*/
f32 EtkBody::getScreenHeight() const
{
	return _tfw->getView()->getHeight();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	言語を設定する
**//*---------------------------------------------------------------------*/
void EtkBody::setLangId(LangId langid)
{
	if(_langid != langid)	// 変更あり
	{
		_langid = langid;	// 新しい言語に設定

		_notif->notifyChangeLanguage();	// 言語変更通知
	}
}

/*---------------------------------------------------------------------*//**
	ビューのリサイズ
**//*---------------------------------------------------------------------*/
void EtkBody::resizeView(f32 widthView, f32 heightView)
{
	// フレームワークにサイズ変更を通達
	if(_tfw != 0L)
	{
		_tfw->resizeView(widthView, heightView);
	}
}

/*---------------------------------------------------------------------*//**
	バックグラウンドに入る
**//*---------------------------------------------------------------------*/
void EtkBody::enterBackground()
{
	_isBackground = true;

	_sndmng->pauseBgm();		// BGM 一時停止
	_sndmng->stopAllSe();		// SE を全て停止
}

/*---------------------------------------------------------------------*//**
	バックグラウンドを抜ける
**//*---------------------------------------------------------------------*/
void EtkBody::leaveBackground()
{
	_isBackground = false;

	if(_pausescr != 0L)	{	_pausescr->start();	}	// 一時停止スクリーンの開始
	_sndmng->resumeBgm();		// BGM 再開
}

/*---------------------------------------------------------------------*//**
	フレーム更新
**//*---------------------------------------------------------------------*/
void EtkBody::updateFrame(f32 frameDelta)
{
	// テスト実行準備
	#if defined(_DEBUG)
		_test->prepareExec();
	#endif
	
	// 一つ前のフレームが時間のかかるフレームであった場合の処理
	if(_isWaitFrame)
	{
		frameDelta = 1.0f;
		_isWaitFrame = false;
	}
	// ２フレーム以上はスキップしない
	else if(frameDelta > 2.0f)
	{
		frameDelta = 2.0f;
	}
	// フレーム落ちテスト
	#if DEBUG_DROP_FRAME_TEST
	{
		static int s_sideDropFrameTest = 0;
		if(s_sideDropFrameTest & 3)	{	frameDelta = 2.0f;	}
		else						{	frameDelta = 0.1f;	}
		s_sideDropFrameTest++;
	}
	#endif
	// ２倍フレームデバッグ
	#if defined(_DEBUG)
		if(_isDoubleFrameDebug)	{	frameDelta = 2.0f;	}
	#endif
	
	// フレーム制御コンテキストの用意
	prepareUpdateFrameContext(frameDelta);

	// フレーム制御処理
	if((_pausescr != 0L) && _pausescr->isPausing())
	{
		execPauseFrame(_ecUpdateFrame);
	}
	else
	{
		execFrame(_ecUpdateFrame);
	}

	// カウンタのインクリメント
	_cntBodyExec++;						// このカウンタは可変フレームに関係なくインクリメントされる値であることに注意
	_frameBodyElapsed += frameDelta;
	
	#if DEBUG_SHOW_MAIN_ARGS
		STRACE("query=%s\n", g_queryApp ? g_queryApp : "");
	#endif
}

/*---------------------------------------------------------------------*//**
	フレーム描画
**//*---------------------------------------------------------------------*/
void EtkBody::renderFrame()
{
	// フレーム描画処理
	Renderer* rdr = _tfw->getRenderer();
	rdr->beginRenderFrame();		// フレーム描画開始
	renderFrame(_rcUpdateFrame);
	rdr->endRenderFrame();			// フレーム描画終了
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EtkBody::EtkBody()
	: _tfw(0L)
	, _touchp(0L)
	, _keybrd(0L)
	, _moctrlr(0L)
	, _pausescr(0L)
	, _screffScn(0L)
	, _screffWhl(0L)
	, _ptclsys(0L)
	, _sndmng(0L)
	, _scrtrc(0L)
	, _tasksys(0L)
	, _gui(0L)
	, _notif(0L)
#if defined(_DEBUG)
	, _test(0L)
#endif
	, _osdepRef(0L)
	, _psnsRef(0L)
	, _swRef(0L)
	, _langid(LANGID_EN)
	, _scaleResoInv(0L)
	, _rslflags(0)
	, _exflags(0)
	, _spfBase(0.0f)
	, _ecUpdateFrame(0L)
	, _rcUpdateFrame(0L)
	, _isSkip3d(false)
	, _isWaitFrame(false)
	, _isBackground(false)
	, _isExitReq(false)
	, _cntBodyExec(0)
	, _frameBodyElapsed(0.0f)
#if defined(_DEBUG)
	, _isDoubleFrameDebug(false)
#endif
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EtkBody::~EtkBody()
{
	ASSERT(_tfw == 0L);
	ASSERT(_touchp == 0L);
	ASSERT(_keybrd == 0L);
	ASSERT(_moctrlr == 0L);
	ASSERT(_pausescr == 0L);
	ASSERT(_screffScn == 0L);
	ASSERT(_screffWhl == 0L);
	ASSERT(_ptclsys == 0L);
	ASSERT(_sndmng == 0L);
	ASSERT(_scrtrc == 0L);
	ASSERT(_tasksys == 0L);
	ASSERT(_gui == 0L);
	ASSERT(_notif == 0L);
	#if defined(_DEBUG)
		ASSERT(_test == 0L);
	#endif
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool EtkBody::init(f32 widthLogical, f32 heightLogical, f32 widthView, f32 heightView, f32 scaleCoord, u32 dispflags, f32 spfRender, f32 spfLogic, LangId langidDefault, StartBodyCase sbcase, u32 bcflags, OsDepMod* osdep, PsnsBase* psns)
{
	// 自己参照用静的変数にセット
	_body = this;

	// デフォルト言語設定
	_langid = langidDefault;

	// 高解像度設定
	if(TFW_IS_FLAG(bcflags, BCF_RD_RESOLUTION))			{	TFW_SET_FLAG(_rslflags, RSLF_RD, true);	}
	else if(TFW_IS_FLAG(bcflags, BCF_HD_RESOLUTION))	{	TFW_SET_FLAG(_rslflags, RSLF_HD, true);	}
///	if(TFW_IS_FLAG(bcflags, BCF_WIDE_DISPLAY))			{	TFW_SET_FLAG(_rslflags, RSLF_WD, true);	}
	_scaleResoInv = TFW_IS_FLAG(_rslflags, RSLF_RD) ? 0.25f : TFW_IS_FLAG(_rslflags, RSLF_HD) ? 0.5f : 1.0f;

	// 拡張フラグ設定
	if(TFW_IS_FLAG(bcflags, BCF_SAVE_RESOURCE_RUN))		{ TFW_SET_FLAG(_exflags, EXF_SAVE_RESOURCE_RUN, true); }

	// 基準フレームレート設定
	_spfBase = spfRender;

	// デバッグ設定
	#if defined(_DEBUG)
		_isDoubleFrameDebug = TFW_IS_FLAG(bcflags, BCF_DEBUG_DOUBLE_FRAME);
	#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// フレームワークの初期化

	// OS 依存部への参照を保存
	_osdepRef = osdep; ASSERT(_osdepRef != 0L);

	// PSNS への参照を保存
	_psnsRef = psns; ASSERT(_psnsRef != 0L);

	// 各モジュールの作成
	// 作成順は、
	//	フレームワーク／ライブラリ系 → 論理／データ系 → 描画系 → 音系

	// Terras Framework の作成
	View* view = new View();
	if(!view->create())
	{
		delete view;
		ASSERTM(false, "View::create failed.");
		return false;
	}
	view->setViewMode(View::VMODE_SCALE, false);
	view->setDisplayFlags(dispflags, false);
	view->setCoordinatesScale(scaleCoord);
	view->setRealSize(widthView, heightView, false);
	view->setLogicalSize(getLogicalWidth(), getLogicalHeight(), true);
	_tfw = new Tfw();
	if(!_tfw->create(makeSceneGraphManager(), view))
	{
		delete _tfw;
		ASSERTM(false, "Tfw::create failed.");
		return false;
	}

	// TypeUtils の初期化
	TypeUtils::init();

	#if 0
		// VcString::split テスト
		///VcStringArray sa = VcString("###1234#567#789ax^03*;`#arzfgsl;k;r,###sat4#eeeeeeeeeeeeeeee").split('#');
		VcStringArray sa = VcString("aaa").split('#');
		for(int i = 0; i < sa.getCount(); i++)
		{
			TRACE(*sa.getString(i) + "\n");
		}
	#endif

	// タッチパネル UI の作成
	TouchPanel* touchp = new TouchPanel();
	if(!touchp->create(_tfw->getView()))
	{
		delete touchp;
		ASSERTM(false, "TouchPanel::create failed.");
		return false;
	}
	_touchp = touchp;

	// キーボード UI の作成
	Keyboard* keybrd = new Keyboard();
	if(!keybrd->create())
	{
		delete keybrd;
		ASSERTM(false, "Keyboard::create failed.");
		return false;
	}
	_keybrd = keybrd;
	
	// モーションコントローラ UI の作成
	MotionController* moctrlr = new MotionController();
	if(!moctrlr->create())
	{
		delete moctrlr;
		ASSERTM(false, "MotionController::create failed.");
		return false;
	}
	_moctrlr = moctrlr;

	// ポーズを作成
	_pausescr = makePauseScreen();

	// タスクシステムを作成する
	FedTaskSys* tasksys = new FedTaskSys();
	if(!tasksys->create())
	{
		delete tasksys;
		ASSERTM(false, "FedTaskSys::create failed.");
		return false;
	}
	_tasksys = tasksys;

	// シーン画面効果の作成
	ScreenEffect* screff = new ScreenEffect();
	if(!screff->create(getScreenWidth(), getScreenHeight()))
	{
		delete screff;
		ASSERTM(false, "ScreenEffect::create failed.");
		return false;
	}
	_screffScn = screff;
	_screffScn->setConfFlags(ScreenEffect::CF_ENABLE_EVENT_PAUSE, true);	// イベントポーズ有効
	if(!_tasksys->registerExecModule(_screffScn, EtkTaskModDef::calcExecOrder(EtkTaskModDef::MOD_SCREFF_SCENE), EtkTaskModDef::MOD_SCREFF_SCENE, 0)) { ASSERT(false); }
	if (!_tasksys->registerDrawModule(_screffScn, EtkTaskModDef::calcDrawOrder(EtkTaskModDef::MOD_SCREFF_SCENE), false, EtkTaskModDef::MOD_SCREFF_SCENE, 0)) { ASSERT(false); }

	// 全体画面効果の作成
	screff = new ScreenEffect();
	if(!screff->create(getScreenWidth(), getScreenHeight()))
	{
		delete screff;
		ASSERTM(false, "ScreenEffect::create failed.");
		return false;
	}
	_screffWhl = screff;
	if (!_tasksys->registerExecModule(_screffWhl, EtkTaskModDef::calcExecOrder(EtkTaskModDef::MOD_SCREFF_WHOLE), EtkTaskModDef::MOD_SCREFF_WHOLE, 0)) { ASSERT(false); }
	if (!_tasksys->registerDrawModule(_screffWhl, EtkTaskModDef::calcDrawOrder(EtkTaskModDef::MOD_SCREFF_WHOLE), false, EtkTaskModDef::MOD_SCREFF_WHOLE, 0)) { ASSERT(false); }

	// パーティクルシステムの作成
	_ptclsys = makeParticleSys();

	// 音管理の作成
	SoundMngr* sndmng = new SoundMngr();
	if(!sndmng->create())
	{
		// サウンドの初期化の失敗は容認される（ただしゲーム中音なし）
		//delete sndmng;
		//ASSERTM(false, "SoundMngr::create failed.");
		//return false;
	}
	_sndmng = sndmng;

	// GUI マネージャの作成
	_gui = makeGui();

	// 通知の作成
	_notif = makeNotifier();

	// フレーム制御コンテキストの作成
	_ecUpdateFrame = makeExecCtx();

	// フレーム描画コンテキストの作成
	_rcUpdateFrame = makeRenderCtx();

	// スクリーントレースの作成
	#if defined(_DEBUG)
	{
		ScreenTrace* scrtrc = new ScreenTrace();
		if(!scrtrc->create(acquireScreenTraceFont(), 0, 16, 16))
		{
			delete scrtrc;
			ASSERTM(false, "ScreenTrace::create failed.");
			return false;
		}
		_scrtrc = scrtrc;
	}
	#endif

	// テストモジュールの作成
	#if defined(_DEBUG)
	{
		_test = makeTest(spfRender, spfLogic);
	}
	#endif

	// 各種値初期化
	_cntBodyExec = 1;
	_frameBodyElapsed = 1.0f;

	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void EtkBody::cleanup(TerminateBodyCase tbcase)
{
	// テストモジュール破棄
	#if defined(_DEBUG)
	{
		if(_test != 0L)
		{
			_test->destroy();
			delete _test;
			_test = 0L;
		}
	}
	#endif

	// スクリーントレースの破棄
	#if defined(_DEBUG)
	{
		if(_scrtrc != 0L)
		{
			_scrtrc->destroy();
			delete _scrtrc;
			_scrtrc = 0L;
		}
	}
	#endif

	// フレーム描画コンテキストの削除
	delete _rcUpdateFrame;
	_rcUpdateFrame = 0L;

	// フレーム制御コンテキストの削除
	delete _ecUpdateFrame;
	_ecUpdateFrame = 0L;

	// 通知の削除
	if (_notif != 0L)
	{
		_notif->destroy();
		delete _notif;
		_notif = 0L;
	}

	// GUI マネージャの削除
	if(_gui != 0L)
	{
		_gui->destroy();
		delete _gui;
		_gui = 0L;
	}

	// 音管理の解放
	if(_sndmng != 0L)
	{
		_sndmng->destroy();
		delete _sndmng;
		_sndmng = 0L;
	}

	// パーティクルシステムの削除
	if(_ptclsys != 0L)
	{
		_ptclsys->destroy();
		delete _ptclsys;
		_ptclsys = 0L;
	}

	// 全体画面効果の削除
	if(_screffWhl != 0L)
	{
		_screffWhl->destroy();
		delete _screffWhl;
		_screffWhl = 0L;
	}

	// シーン画面効果の削除
	if(_screffScn != 0L)
	{
		_screffScn->destroy();
		delete _screffScn;
		_screffScn = 0L;
	}

	// タスクシステムを削除する
	if(_tasksys != 0L)
	{
		_tasksys->destroy();
		delete _tasksys;
		_tasksys = 0L;
	}

	// ポーズを削除する
	if(_pausescr != 0L)
	{
		_pausescr->destroy();
		delete _pausescr;
		_pausescr = 0L;
	}
	
	// モーションコントローラ UI 解放
	if(_moctrlr != 0L)
	{
		_moctrlr->destroy();
		delete _moctrlr;
		_moctrlr = 0L;
	}

	// キーボード UI 解放
	if(_keybrd != 0L)
	{
		_keybrd->destroy();
		delete _keybrd;
		_keybrd = 0L;
	}

	// タッチパネル UI 解放
	if(_touchp != 0L)
	{
		_touchp->destroy();
		delete _touchp;
		_touchp = 0L;
	}

	// TypeUtils の後処理
	TypeUtils::cleanup();
	
	// Terras Framework 解放 
	if(_tfw != 0L)
	{
		_tfw->destroy();
		delete _tfw;
		_tfw = 0L;
	}

	// 自己参照用静的変数をクリア
	_body = 0L;
}

/*---------------------------------------------------------------------*//**
	パフォーマンスメーターにプロット（時間計測）する
**//*---------------------------------------------------------------------*/
void EtkBody::plotPerformanceMeter(u32 colorCategory, u32 colorDetail)
{
#if defined(_DEBUG)
	if(_swRef == 0L)	{	return;	}
	PerformanceMeter* pmeter = _test->getPerformanceMeter();
	if(pmeter == 0L)	{	return;	}
	pmeter->pushCheckPoint(_swRef->getElapsedSeconds(), colorCategory, colorDetail);
#endif
}

/*---------------------------------------------------------------------*//**
	フレーム更新コンテキストの準備
**//*---------------------------------------------------------------------*/
void EtkBody::prepareUpdateFrameContext(f32 frameDelta)
{
	((EtkExecCtx*)_ecUpdateFrame)->set(
		frameDelta,
		0,
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
void EtkBody::execFrame(const ExecCtx* ec)
{
	_touchp->exec();
	_keybrd->exec();
	_moctrlr->exec();
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x0000ff);	// パフォーマンス計測
	#if defined(_DEBUG)
		_test->exec(0L, ec);
	#endif
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x770000);	// パフォーマンス計測
	if(_isSkip3d)	{	goto EXEC_NO3D;	}	// 3D 処理のスキップ
	_tfw->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x770077);	// パフォーマンス計測
	_ptclsys->exec(0L, ec);
	EXEC_NO3D:
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x777700);	// パフォーマンス計測
	_sndmng->exec(ec);
	if(_scrtrc != 0L)	{	_scrtrc->exec(0L, ec);	}
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x777777);	// パフォーマンス計測
}

/*---------------------------------------------------------------------*//**
	ポーズ時のフレーム制御
**//*---------------------------------------------------------------------*/
void EtkBody::execPauseFrame(const ExecCtx* ec)
{
	_touchp->exec();
	_keybrd->exec();
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x0000ff);	// パフォーマンス計測
	#if defined(_DEBUG)
		_test->exec(0L, ec);
	#endif
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x770000);	// パフォーマンス計測
	_pausescr->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x777777);	// パフォーマンス計測
}

/*---------------------------------------------------------------------*//**
	フレーム描画
**//*---------------------------------------------------------------------*/
void EtkBody::renderFrame(const RenderCtx* rc)
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
	_ptclsys->render(rc);
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
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0x00ffff);	// パフォーマンス計測
	_pausescr->draw(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0xffff00);	// パフォーマンス計測
	#if defined(_DEBUG)
		_test->draw(rc);
	#endif
	if(_scrtrc != 0L)	{	_scrtrc->draw(rc);	}
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0xffffff);	// パフォーマンス計測
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS