/***********************************************************************//**
 *	Game.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// マクロ

#define DEBUG_SHOW_MAIN_ARGS	(0)
#define DEBUG_FULLMODE			(0)
#define DEBUG_DROP_FRAME_TEST	(0)
#define DEBUG_VAR_RESOLUTION	(1)
#if defined(_ENLIGHT_PRPZL01)
	#define DEBUG_PRPZL_FIRST		(1)
	#define DEBUG_MODELVW_FIRST		(0)
	#define DEBUG_BOOKVW_FIRST		(0)
#else
	#define DEBUG_PRPZL_FIRST		(0)
	#if defined(_WINDOWS)
		#define DEBUG_MODELVW_FIRST		(1)
	#else
		#define DEBUG_MODELVW_FIRST		(0)
	#endif
	#define DEBUG_BOOKVW_FIRST		(0)
	#define DEBUG_TXTEDITOR_FIRST	(0)
#endif

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Game.h"

// Friends
#include "Conductor.h"
#include "GameExecCtx.h"
#include "GameNotifier.h"
#include "GameRenderCtx.h"
#include "GamePauseScreen.h"
#include "GameTaskModDef.h"
#include "MoveMap.h"
#include "../confront/Confront.h"
#include "../debug/GameTest.h"
#include "../effect/GaugeAnimEffect.h"
#include "../effect/AutoDecorationEffect.h"
#include "../episode/StoryManager.h"
#include "../episode/data/FixedData.h"
#include "../episode/char/pc/MyUnit.h"
#include "../episode/party/MyParty.h"
#include "../episode/save/SaveStructure.h"
#include "../episode/save/Score.h"
#include "../episode/save/Trophy.h"
#include "../event/EventIdDef.h"
#include "../event/EventData.h"
#include "../event/EventSys.h"
#include "../file/GameFileIdDef.h"
#include "../gui/FocusCursor.h"
#include "../gui/GameFontSet.h"
#include "../gui/GameGui.h"
#include "../gui/window/menu/MenuWindow.h"
#include "../magic/MagicSys.h"
#include "../map/Map.h"
#include "../mode/GameMode.h"
#include "../mode/GameModeParam.h"
#include "../particle/GameParticleSys.h"
#include "../quest/QuestManager.h"
#include "../save/SaveData.h"
#include "../save/SaveSys.h"
#include "../save/SysConf.h"
#include "../sound/GameSoundDef.h"
#include "../souma/script/SoumaScript.h"
#include "../spirit/SpiritManager.h"
#include "../status/StatusDrawer.h"
#include "../unit/UnitManager.h"

// External
#include "../../etk/effect/ScreenEffect.h"
#include "../../etk/sg/SorSgm.h"
#include "../../tfw/Tfw.h"
#include "../../tfw/framemod/FedTaskSys.h"
#include "../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/Point.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/serialize/Serializer.h"
#include "../../tfw/string/VcString.h"
#include "../../tfw/string/CcString.h"
#include "../../tfw/ui/Keyboard.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define IS_WINDOWS_IDFILE_DEVMODE_UNPACK	(1)	// デバッグ時に個別ファイルから読み込む
#define REPLACE_EXEC_RENDER					(1)	// Exec と Render を置換する
					
////////////////////////////////////////////////////////////////////////////
// グローバル変数
	
#if DEBUG_SHOW_MAIN_ARGS
SRCR_END_NS
extern const char*	g_queryApp;
SRCR_BEGIN_NS
#endif
					
////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Game メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	ワイド解像度で動作中かを得る
**//*---------------------------------------------------------------------*/
bool Game::isWideResolution() const
{
	return (_widthLgcWide != _widthLgcNoWide) && (_widthLgc == _widthLgcWide);
}

/*---------------------------------------------------------------------*//**
	マルチタスキングモードかどうかを得る
**//*---------------------------------------------------------------------*/
bool Game::isMultiTasking() const
{
	SysConf* sysconf = _svsys->getSysConf(); ASSERT(sysconf != 0L);
	return !sysconf->isEcoMultiTasking();
}

/*---------------------------------------------------------------------*//**
	対決モードかを得る
**//*---------------------------------------------------------------------*/
bool Game::isConfront() const
{
	return (_confront != 0L) && _confront->isConfrontNow();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	言語を設定する
**//*---------------------------------------------------------------------*/
void Game::setLangId(LangId langid)
{
	if(_langid != langid)	// 変更あり
	{
		EtkBody::setLangId(langid);
	}
	else
	{
		EtkBody::setLangId(langid);
	}
}

/*---------------------------------------------------------------------*//**
	ビューのリサイズ
**//*---------------------------------------------------------------------*/
void Game::resizeView(f32 widthView, f32 heightView)
{
	_widthLgcReal = widthView * _tfw->getView()->getInvCoordinatesScale() * _tfw->getView()->getInvLogicalScaleX();
	_heightLgcReal = heightView * _tfw->getView()->getInvCoordinatesScale() * _tfw->getView()->getInvLogicalScaleY();
	normalizeLogicalScreenSize();
	//TRACELN("{Game::resizeView} lrw=%f, lrh=%f, lw=%f, lh=%f", _widthLgcReal, _heightLgcReal, _widthLgc, _heightLgc);

	EtkBody::resizeView(widthView, heightView);

	getGameNotifier()->notifyResizeView();
}

/*---------------------------------------------------------------------*//**
	ゲームに入る
**//*---------------------------------------------------------------------*/
bool Game::enterGame(EnterGameKind egkind, s32 svdid, s32 storyid)
{
	getGameNotifier()->notifyEnterGame();

	switch(egkind)
	{
	case EGKIND_NEW:		// ニューゲーム
		if(!_svsys->newGame())					{	return false;	}
		break;
	case EGKIND_RESTART:	// ストーリーを最初からやり直す
		if(!_svsys->restartGame(svdid))			{	return false;	}
		break;
	case EGKIND_LOAD:		// ロードゲーム
		if(!_svsys->load(svdid, false))			{	return false;	}
		break;
	case EGKIND_DEBUG:		// デバッグ
		if(!_svsys->debug_enterMap(storyid))	{	return false;	}
		break;
	}

	_gmode->changeEndMode(GameMode::MODE_GAME_STARTGAME);
	return true;
}

/*---------------------------------------------------------------------*//**
	アニメーション補間禁止を開始する
**//*---------------------------------------------------------------------*/
s32 Game::startNoAnimInterpolation()
{
	_isNoAnimIntp = true;
	return ++_hintNoAnimIntpCtrl;
}

/*---------------------------------------------------------------------*//**
	アニメーション補間禁止を終了する
**//*---------------------------------------------------------------------*/
void Game::endNoAnimInterpolation(s32 hintCtrl)
{
	if((hintCtrl != 0) && (_hintNoAnimIntpCtrl != hintCtrl))
	{
		return;
	}
	_isNoAnimIntp = false;
}

/*---------------------------------------------------------------------*//**
	可変スクリーンサイズモードの切り替え
**//*---------------------------------------------------------------------*/
void Game::changeScreenSizeMode(bool isScale)
{
#if 1
	if(isScale)	// スケールアスペクトへ
	{
		_cntScreenSizeMode++;
	}
	else		// 固定アスペクトへ
	{
		_cntScreenSizeMode--;
	}
	View::ViewMode vmode = View::VMODE_SCALE;
	if(_cntScreenSizeMode < 0)
	{
		vmode = View::VMODE_FIX;
	}
	if(vmode == _tfw->getView()->getViewMode())
	{
		return;
	}
	_tfw->getView()->setViewMode(vmode, false);
	normalizeLogicalScreenSize();
	_tfw->getView()->setLogicalSize(getLogicalWidth(), getLogicalHeight(), true);
	_screffScn->setScreenSize(getLogicalWidth(), getLogicalHeight());
	_screffWhl->setScreenSize(getLogicalWidth(), getLogicalHeight());
	TRACELN("{Game::changeScreenSizeMode} isscale=%d, lw=%f, lh=%f", isScale, getLogicalWidth(), getLogicalHeight());

#else
	s32 widthLgcNew = isWide ? _widthLgcWide : _widthLgcNoWide;
	if(widthLgcNew != _widthLgc)
	{
		_widthLgc = widthLgcNew;
		_tfw->getView()->setLogicalSize(getLogicalWidth(), getLogicalHeight());
		_screffScn->setScreenSize(getLogicalWidth(), getLogicalHeight());
		_screffWhl->setScreenSize(getLogicalWidth(), getLogicalHeight());

		_notif->notifyChangeLogicalScreenSize();	// 論理スクリーンサイズ変更通知
	}

#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Game::Game()
	: _fontset(0L)
	, _svsys(0L)
	, _score(0L)
	, _trophy(0L)
	, _evsys(0L)
	, _map(0L)
	, _sprtmng(0L)
	, _unitmng(0L)
	, _gmode(0L)
	, _conductor(0L)
	, _ggaeff(0L)
	, _adeff(0L)
	, _mgcsys(0L)
	, _mvmap(0L)
	, _script(0L)
	, _strymng(0L)
	, _qsmng(0L)
	, _partyMy(0L)
	, _camDefault(0L)
	, _widthLgc(0)
	, _heightLgc(0)
	, _widthLgcWide(0)
	, _widthLgcNoWide(0)
	, _widthLgcReal(0)
	, _heightLgcReal(0)
	, _confront(0L)
	, _isNoAnimIntp(false)
	, _hintNoAnimIntpCtrl(0)
	, _cntScreenSizeMode(0)
	, _optflags(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Game::~Game()
{
	cleanup(TBCASE_CLOSE);

	ASSERT(_fontset == 0L);
	ASSERT(_svsys == 0L);
	ASSERT(_score == 0L);
	ASSERT(_trophy == 0L);
	ASSERT(_evsys == 0L);
	ASSERT(_map == 0L);
	ASSERT(_sprtmng == 0L);
	ASSERT(_unitmng == 0L);
	ASSERT(_gmode == 0L);
	ASSERT(_conductor == 0L);
	ASSERT(_ggaeff == 0L);
	ASSERT(_adeff == 0L);
	ASSERT(_mgcsys == 0L);
	ASSERT(_mvmap == 0L);
	ASSERT(_script == 0L);
	ASSERT(_strymng == 0L);
	ASSERT(_qsmng == 0L);
	ASSERT(_partyMy == 0L);
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool Game::init(f32 widthLogical, f32 heightLogical, f32 widthView, f32 heightView, f32 scaleCoord, u32 dispflags, f32 spfRender, f32 spfLogic, LangId langidDefault, StartBodyCase sbcase, u32 bcflags, OsDepMod* osdep, PsnsBase* psns)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 基本設定

	#if defined(_DEBUG) && defined(_WINDOWS) && IS_WINDOWS_IDFILE_DEVMODE_UNPACK
		IdFile::enableDevMode(g_filedefTbfPack, &CcString(""));
	#endif

	// 論理スクリーンサイズを得る
#if 0
	if(TFW_IS_FLAG(dispflags, TFW_DISPF_WIDE))
	{
		_widthLgcNoWide = getStandardWidth();
		f32 scale = heightView / (f32)getStandardHeight();
		_widthLgc = _widthLgcWide = (s32)(widthView / scale);
		_heightLgc = getStandardHeight();
	}
	else
	{
		_widthLgc = _widthLgcWide = _widthLgcNoWide = getStandardWidth();
		_heightLgc = getStandardHeight();
	}
#elif 0
	_widthLgc = _widthLgcReal = (s32)(widthView / scaleCoord);
	_heightLgc = _heightLgcReal = (s32)(heightView / scaleCoord);
#else
	_widthLgc = _widthLgcReal = widthLogical;
	_heightLgc = _heightLgcReal = heightLogical;
#endif

	// 親クラス（ツールキット）による初期化
	if(!EtkBody::init(widthLogical, heightLogical, widthView, heightView, scaleCoord, dispflags, spfRender, spfLogic, langidDefault, sbcase, bcflags, osdep, psns))
	{
		return false;
	}

	#if 0//DEBUG_VAR_RESOLUTION	// VMODE_FIX test
		_widthLgc = getStandardWidth();
		_heightLgc = getStandardHeight();
		_tfw->getView()->setViewMode(View::VMODE_FIX, false);
		_tfw->getView()->setLogicalSize(_widthLgc, _heightLgc, true);
	#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// ベースシステムとシステムコンフィグの初期化

	// セーブシステムの作成
	SaveSys* svsys = new SaveSys();
	if(!svsys->create())
	{
		delete svsys;
		ASSERTM(false, "SaveSys::create failed.");
		return false;
	}
	_svsys = svsys;

	// システムコンフィグを読み込む
	_svsys->loadSysConf();
	
	// ポーズの有効・無効設定
	SysConf* sysconf = _svsys->getSysConf(); ASSERT(sysconf != 0L);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 各ゲームモジュールの初期化

	// スコアの作成
	Score* score = new Score(psns);
	if(!score->create())
	{
		delete score;
		ASSERTM(false, "Score::create failed.");
		return false;
	}
	_score = score;

	// トロフィーの作成
	Trophy* trophy = new Trophy(psns);
	if(!trophy->create())
	{
		delete trophy;
		ASSERTM(false, "Trophy::create failed.");
		return false;
	}
	_trophy = trophy;

	// 対決モジュールの作成
	Confront* confront = new Confront();
	if(!confront->create())
	{
		delete confront;
		ASSERTM(false, "{ConfrontMode::create} Confront-create failed.");
		return false;
	}
	_confront = confront;

	// マップ移動モジュールの作成
	MoveMap* mvmap = new MoveMap();
	if(!mvmap->create())
	{
		delete mvmap;
		ASSERTM(false, "{MoveMap::create} MoveMap-create failed.");
		return false;
	}
	_mvmap = mvmap;

	// イベントシステムの作成
	EventSys* evsys = new EventSys();
	if(!evsys->create())
	{
		delete evsys;
		ASSERTM(false, "EventSys::create failed.");
		return false;
	}
	_evsys = evsys;
	if(!_tasksys->registerExecModule(_evsys, GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_EVENT), GameTaskModDef::MOD_EVENT, 0)) { ASSERT(false); }
	if(!_tasksys->registerDrawModule(_evsys, GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_EVENT), false, GameTaskModDef::MOD_EVENT, 0)) { ASSERT(false); }
	_evsys->setEnableWaitMovieMessage(!TFW_IS_FLAG(_optflags, Game::OPTF_AUTO_FLOW_MOVIE_MSG));	// ムービー台詞待ち設定

	// スピリットマネージャ作成
	SpiritManager* sprtmng = new SpiritManager();
	if(!sprtmng->create())
	{
		delete sprtmng;
		ASSERTM(false, "SpiritManager::create failed.");
		return false;
	}
	_sprtmng = sprtmng;

	// ユニットマネージャ作成
	UnitManager* unitmng = new UnitManager();
	if(!unitmng->create())
	{
		delete unitmng;
		ASSERTM(false, "UnitManager::create failed.");
		return false;
	}
	_unitmng = unitmng;

	// ゲームモードの作成
	GameMode* gmode = new GameMode();
	if(!gmode->create())
	{
		delete gmode;
		ASSERTM(false, "GameMode::create failed.");
		return false;
	}
	_gmode = gmode;

	// ストーリー管理の作成
	StoryManager* strymng = new StoryManager();
	if(!strymng->create(_evsys))
	{
		delete strymng;
		ASSERTM(false, "StoryManager::create failed.");
		return false;
	}
	_strymng = strymng;

	// クエスト管理の作成
	QuestManager* qsmng = new QuestManager();
	if(!qsmng->create(_evsys))
	{
		delete qsmng;
		ASSERTM(false, "QuestManager::create failed.");
		return false;
	}
	_qsmng = qsmng;

	// ソウマ クラスタ システムの作成
	MagicSys* mgcsys = new MagicSys();
	if(!mgcsys->create())
	{
		delete mgcsys;
		ASSERTM(false, "MagicSys::create failed.");
		return false;
	}
	_mgcsys = mgcsys;

	// マップ作成
	Map* map = new Map();
	if(!map->create(((GameGui*)_gui)->getGuiTexture()))
	{
		delete map;
		ASSERTM(false, "Map::create failed.");
		return false;
	}
	_map = map;

	// ゲージ アニメーション エフェクトの作成
	GaugeAnimEffect* ggaeff = new GaugeAnimEffect();
	if(!ggaeff->create())
	{
		delete ggaeff;
		ASSERTM(false, "GaugeAnimEffect::create failed.");
		return false;
	}
	_ggaeff = ggaeff;
	if(!_tasksys->registerExecModule(_ggaeff, GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_GAUGEANIMEFF), GameTaskModDef::MOD_GAUGEANIMEFF, 0)) { ASSERT(false); }
	if(!_tasksys->registerDrawModule(_ggaeff, GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_GAUGEANIMEFF), false, GameTaskModDef::MOD_GAUGEANIMEFF, 0)) { ASSERT(false); }

	// 元素エネルギー ユーティリティの初期化
	StatusDrawer::init();

	// テクスチャ フェイシャルの作成
	AutoDecorationEffect* adeff = new AutoDecorationEffect();
	if(!adeff->create())
	{
		delete adeff;
		ASSERTM(false, "AutoDecorationEffect::create failed.");
		return false;
	}
	_adeff = adeff;

	// コンダクターの作成
	Conductor* conductor = new Conductor();
	if(!conductor->create())
	{
		delete conductor;
		ASSERTM(false, "Conductor::create failed.");
		return false;
	}
	_conductor = conductor;

	// BGM の登録
	ASSERT(_sndmng != 0L);
	_sndmng->registerBgms(GameSoundDef::_bgms, GameSoundDef::NUM_BGM);

	// PC パーティを作成
	MyParty* partyMy = new MyParty();
	if(!partyMy->create())
	{
		delete partyMy;
		ASSERTM(false, "My Party::create failed.");
		return false;
	}
	_partyMy = partyMy;

	// ソウマ スクリプトを作成する
	SoumaScript* script = new SoumaScript();
	if(!script->create())
	{
		delete script;
		ASSERTM(false, "SoumaScript::create failed.");
		return false;
	}
	_script = script;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 各ゲームデータの初期化

	// システム固定イベントの読み込み
	((EventData*)_evsys->getEvData())->loadSystemFixedEvent();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// ゲームシステムの開始

	#if DEBUG_FULLMODE
		debug_onDebugMode(999);
	#endif

	// システム駆動開始
	{
		u32 gmpflags = 0;
		GameMode::ModeKind modek = GameMode::MODE_NULL;

		if(TFW_IS_FLAG(bcflags, BCF_BOOT_PASTE_SAVES))
		{
			TFW_SET_FLAG(gmpflags, GameModeParam::F_PASTE_SAVES_BOOT, true);	// データ引き継ぎ起動
		}
		
		if(sbcase == SBCASE_ENTERFG)
		{
			TFW_SET_FLAG(gmpflags, GameModeParam::F_RESUME_START, true);		// 再開からスタート
			modek = GameMode::MODE_GAME_TITLE;
		}
		else
		{
			// 初期ゲームモードを設定する
			#if defined(_DEBUG) && DEBUG_PRPZL_FIRST
				modek = GameMode::MODE_GAME_PRPZL_PLAY;
			#elif defined(_DEBUG) && DEBUG_MODELVW_FIRST
				modek = GameMode::MODE_GAME_MODELVW;
			#elif defined(_DEBUG) && DEBUG_BOOKVW_FIRST
				modek = GameMode::MODE_GAME_BOOKVW;
			#elif defined(_DEBUG) && DEBUG_TXTEDITOR_FIRST
				modek = GameMode::MODE_GAME_TXTEDITOR;
			#else
				modek = GameMode::MODE_GAME_TITLE;
			#endif
		}

		((GameModeParam*)_gmode->getParameter())->setFlags(gmpflags, true);
		_gmode->changeEndMode(modek);
		_gmode->begin(0L);
	}

	// フェードイン
	if(_screffScn != 0L)
	{
		ColorU8 colBlack(0, 0, 0, 255);
		_screffScn->setColor1(&colBlack);
		_screffScn->setMaxFrame(1 * FRAMERATE);
		_screffScn->start(ScreenEffect::MODE_FADE_IN);
	}

	prepareUpdateFrameContext(0.0f);	// 初回描画用のコンテキスト作成
	return true;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
void Game::cleanup(TerminateBodyCase tbcase)
{
	TRACELN("{Game::cleanup} called. tgscase=%d", tbcase);
	
	// 再開セーブデータのセーブ
	if((_svsys != 0L) && (_gmode != 0L) && (_evsys != 0L))
	{
		_svsys->saveResumeAuto();
	}

	// 全イベントの終了
	if(_evsys != 0L)	{	_evsys->terminateAllEvent(0);		}

	// 行動の停止
	if(_sprtmng != 0L)	{	_sprtmng->notifyTerminateGame();	}

	// ゲームモードを終了する
	if(_gmode != 0L)	{	_gmode->end(0L);	}

	// ソウマ スクリプトを削除
	if(_script != 0L)
	{
		_script->destroy();
		delete _script;
		_script = 0L;
	}

	// PC パーティを削除
	if(_partyMy != 0L)
	{
		_partyMy->destroy();
		delete _partyMy;
		_partyMy = 0L;
	}

	// コンダクターの削除
	if(_conductor != 0L)
	{
		_conductor->destroy();
		delete _conductor;
		_conductor = 0L;
	}

	// テクスチャ フェイシャルを削除
	if(_adeff != 0L)
	{
		_adeff->destroy();
		delete _adeff;
		_adeff = 0L;
	}

	// ゲージ アニメーション エフェクトを削除
	if(_ggaeff != 0L)
	{
		_ggaeff->destroy();
		delete _ggaeff;
		_ggaeff = 0L;
	}

	// マップ解放
	if(_map != 0L)
	{
		_map->destroy();
		delete _map;
		_map = 0L;
	}

	// マ法システムの削除
	if(_mgcsys != 0L)
	{
		_mgcsys->destroy();
		delete _mgcsys;
		_mgcsys = 0L;
	}

	// クエスト管理の削除
	if(_qsmng != 0L)
	{
		_qsmng->destroy();
		delete _qsmng;
		_qsmng = 0L;
	}

	// ストーリー管理の削除
	if(_strymng != 0L)
	{
		_strymng->destroy();
		delete _strymng;
		_strymng = 0L;
	}

	// ゲームモード解放
	if(_gmode != 0L)
	{
		_gmode->destroy();
		delete _gmode;
		_gmode = 0L;
	}

	// ユニットマネージャ破棄
	if(_unitmng != 0L)
	{
		_unitmng->destroy();
		delete _unitmng;
		_unitmng = 0L;
	}

	// スピリットマネージャ破棄
	if(_sprtmng != 0L)
	{
		_sprtmng->destroy();
		delete _sprtmng;
		_sprtmng = 0L;
	}

	// イベントシステム解放
	if(_evsys != 0L)
	{
		_evsys->destroy();
		delete _evsys;
		_evsys = 0L;
	}

	// マップ移動モジュールの削除
	if(_mvmap != 0L)
	{
		_mvmap->destroy();
		delete _mvmap;
		_mvmap = 0L;
	}

	// 対決モジュールの削除
	if(_confront != 0L)
	{
		_confront->destroy();
		delete _confront;
		_confront = 0L;
	}

	// フォントセット解放
	if(_fontset != 0L)
	{
		_fontset->destroy();
		delete _fontset;
		_fontset = 0L;
	}

	// トロフィーの削除
	if(_trophy != 0L)
	{
		_trophy->destroy();
		delete _trophy;
		_trophy = 0L;
	}

	// スコアの削除
	if(_score != 0L)
	{
		_score->destroy();
		delete _score;
		_score = 0L;
	}

	// 固定データの後処理
	FixedData::cleanup();

	// セーブシステムの削除
	if(_svsys != 0L)
	{
		_svsys->destroy();
		delete _svsys;
		_svsys = 0L;
	}

	EtkBody::cleanup(tbcase);
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool Game::serialize(Serializer* ser) const
{
	bool isSuccess = true;

	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		svst->_gamesvst._optflags = _optflags;		// オプションフラグの書き込み
	}
	else						// ダイレクトモード
	{
		ser->store(&_optflags);						// オプションフラグの書き込み
	}
	
	// スコア
	if(!_score->serialize(ser))		{	isSuccess = false;	}

	// トロフィー
	if(!_trophy->serialize(ser))	{	isSuccess = false;	}

	// ストーリー
	if(!_strymng->serialize(ser))	{	isSuccess = false;	}

	// マップ移動
	if(!_mvmap->serialize(ser))		{	isSuccess = false;	}

	// マップ
	if(!_map->serialize(ser))		{	isSuccess = false;	}

	// パーティ
	if(!_partyMy->serialize(ser))	{	isSuccess = false;	}

	// クエスト
	if(!_qsmng->serialize(ser))		{	isSuccess = false;	}

	// ユニット
	if(!_unitmng->serialize(ser))	{	isSuccess = false;	}

	return isSuccess;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool Game::deserialize(Serializer* ser)
{
	bool isSuccess = true;

	// オプションフラグ
	if(ser->isStructureMode())	// ストラクチャモード
	{
		const SaveStructure* svst = (SaveStructure*)ser->getStructure();
		_optflags = svst->_gamesvst._optflags;		// オプションフラグの読み込み
	}
	else						// ダイレクトモード
	{
		ser->restore(&_optflags);					// オプションフラグの読み込み
	}

	// イベント
	_evsys->setEnableWaitMovieMessage(!TFW_IS_FLAG(_optflags, Game::OPTF_AUTO_FLOW_MOVIE_MSG));	// ムービー台詞待ち設定

	// スコア
	if(!_score->deserialize(ser))	{	isSuccess = false;	}

	// トロフィー
	if(!_trophy->deserialize(ser))	{	isSuccess = false;	}

	// ストーリー
	if(!_strymng->deserialize(ser))	{	isSuccess = false;	}

	// マップ移動
	if(!_mvmap->deserialize(ser))	{	isSuccess = false;	}

	// マップ
	if(!_map->deserialize(ser))		{	isSuccess = false;	}

	// パーティ
	if(!_partyMy->deserialize(ser))	{	isSuccess = false;	}

	// クエスト
	if(_qsmng->deserialize(ser))	{	isSuccess = false;	}

	// ユニット
	if(!_unitmng->deserialize(ser))	{	isSuccess = false;	}

	return isSuccess;
}

/*---------------------------------------------------------------------*//**
	フレーム更新コンテキストの準備
**//*---------------------------------------------------------------------*/
void Game::prepareUpdateFrameContext(f32 frameDelta)
{
	((GameExecCtx*)_ecUpdateFrame)->set(
		frameDelta,
		(_isNoAnimIntp ? ExecCtx::EF_NO_ANIM_INTERP : 0) | (_evsys->isWaitMovieMessageNow() || _evsys->isEventPause() ? ExecCtx::EF_PAUSE_ANIM : 0),
		_touchp,
		_keybrd,
		_cntBodyExec,
		_frameBodyElapsed,
		_gui->isOpenedLockGui(),
		_evsys->isEventPause()	);

	((GameRenderCtx*)_rcUpdateFrame)->set(
		_fontset,
		_tfw->getRenderer(),
		_cntBodyExec,
		_frameBodyElapsed	);
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Game::execFrame(const ExecCtx* ec)
{
	_touchp->exec();
	_keybrd->exec();
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x0000ff);	// パフォーマンス計測
	#if defined(_DEBUG)
		_test->exec(0L, ec);
	#endif
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x00ff00);	// パフォーマンス計測
	_svsys->exec(ec);
	_mvmap->exec();
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x00ffff);	// パフォーマンス計測
	_strymng->exec(0L, ec);
	_qsmng->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0xff0000);	// パフォーマンス計測
	_gui->exec(0L, ec);
	_tasksys->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0xff00ff);	// パフォーマンス計測
	_adeff->exec(0L, ec);
	_gmode->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0xffff00);	// パフォーマンス計測
	_map->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0xffffff);	// パフォーマンス計測
	_sprtmng->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x000077);	// パフォーマンス計測
	_unitmng->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x007700);	// パフォーマンス計測
	_mgcsys->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x007777);	// パフォーマンス計測
	_conductor->exec(ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x770000);	// パフォーマンス計測
	if(_isSkip3d)	{	goto EXEC_NO3D;	}	// 3D 処理のスキップ
	_tfw->exec(0L, ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x770077);	// パフォーマンス計測
	_ptclsys->exec(0L, ec);
	EXEC_NO3D:
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x777700);	// パフォーマンス計測
	_sndmng->exec(ec);
	if(_scrtrc != 0L)	{	_scrtrc->exec(0L, ec);	}
	StatusDrawer::exec(ec);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xffff77, 0x777777);	// パフォーマンス計測
	///STRACE("w=%d,h=%d,sx=%f,sy=%f\n", _widthLgcReal, _heightLgcReal, _tfw->getView()->getLogicalScaleX(), _tfw->getView()->getLogicalScaleY());
}

/*---------------------------------------------------------------------*//**
	フレーム描画
**//*---------------------------------------------------------------------*/
void Game::renderFrame(const RenderCtx* rc)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 3D 描画
	if(_isSkip3d)	{	goto RENDER_2DSCENE;	}	// 3D 処理のスキップ

	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x0000ff);	// パフォーマンス計測
	rc->getRenderer()->setDefault3dRenderState();					// デフォルトの 3D 描画ステイトに設定する
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x00ff00);	// パフォーマンス計測
	_tfw->render(rc);
	///rdr->setSolidColorForce(255, 255, 255, 255);	// モデル描画によるカラー変更を強制リセット
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0x00ffff);	// パフォーマンス計測
	_gmode->render(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0xff0000);	// パフォーマンス計測
	_evsys->render(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0xff00ff);	// パフォーマンス計測
	_map->render(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0xffff00);	// パフォーマンス計測
	_unitmng->render(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0xff77ff, 0xffffff);	// パフォーマンス計測
	_adeff->render(rc);
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
	_gmode->draw(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0x00ffff);	// パフォーマンス計測
	_conductor->draw(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0xff0000);	// パフォーマンス計測
	_tasksys->draw(rc);
	/* . . . . . . . . . . . . . . . . . . . . . . . . . . */	PLOT_PM(0x77ffff, 0xff00ff);	// パフォーマンス計測
	_pausescr->draw(rc);
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
ExecCtx* Game::makeExecCtx()
{
	return new GameExecCtx();
}

/*---------------------------------------------------------------------*//**
	フレーム描画コンテキストの作成実装
**//*---------------------------------------------------------------------*/
RenderCtx* Game::makeRenderCtx()
{
	return new GameRenderCtx();
}

/*---------------------------------------------------------------------*//**
	ポーズの作成実装
**//*---------------------------------------------------------------------*/
PauseScreen* Game::makePauseScreen()
{
	// 専用ポーズの作成
	GamePauseScreen* pausescr = new GamePauseScreen();
	if(!pausescr->create())
	{
		delete pausescr;
		ASSERTM(false, "GamePause::create failed.");
		return 0L;
	}

	return pausescr;
}

/*---------------------------------------------------------------------*//**
	パーティクルシステムの作成実装
**//*---------------------------------------------------------------------*/
ParticleSys* Game::makeParticleSys()
{
	const ParticleDefTbl* ptcldftbl = FixedData::getParticleDefTable();

	SorSgm* sg = getSceneGraph();
	SgNodeChain* sgnc = sg->getKindChain(SorSgm::EK_ZSORT);

	GameParticleSys* ptclsys = new GameParticleSys();
	if(!ptclsys->create(ptcldftbl, sgnc))
	{
		delete ptclsys;
		ASSERTM(false, "ParticleSys::create failed.");
		return 0L;
	}
	return ptclsys;
}

/*---------------------------------------------------------------------*//**
	シーングラフマネージャの作成実装
**//*---------------------------------------------------------------------*/
SgManager* Game::makeSceneGraphManager()
{
	return new SorSgm();
}

/*---------------------------------------------------------------------*//**
	GUI の作成実装
**//*---------------------------------------------------------------------*/
EtkGui* Game::makeGui()
{
	// 固定データの初期化
	if(!FixedData::init())
	{
		ASSERTM(false, "FixedData::init failed.");
		return 0L;
	}

	// フォントセットの作成
	GameFontSet* fontset = new GameFontSet();
	if(!fontset->create())
	{
		delete fontset;
		ASSERTM(false, "GameFontSet::create failed.");
		return 0L;
	}
	_fontset = fontset;

	// GUI の作成
	GameGui* gui = new GameGui();
	if(!gui->create(_fontset, _tasksys))
	{
		delete gui;
		ASSERTM(false, "GameGui::create failed.");
		return 0L;
	}
	return gui;
}

/*---------------------------------------------------------------------*//**
	通知の作成実装
**//*---------------------------------------------------------------------*/
EtkNotifier* Game::makeNotifier()
{
	// ゲーム通知の作成
	GameNotifier* gnotif = new GameNotifier();
	if (!gnotif->create())
	{
		delete gnotif;
		ASSERTM(false, "GameNotifier::create failed.");
		return false;
	}
	return gnotif;
}

/*---------------------------------------------------------------------*//**
	テストモジュールの作成実装
**//*---------------------------------------------------------------------*/
Test* Game::makeTest(f32 spfRender, f32 spfLogic)
{
	Test* test = new GameTest();
	if(!test->create(spfRender, spfLogic))
	{
		delete test;
		ASSERTM(false, "GameTest::create failed.");
		return 0L;
	}
	return test;
}

/*---------------------------------------------------------------------*//**
	スクリーントレースのフォント取得
**//*---------------------------------------------------------------------*/
Font* Game::acquireScreenTraceFont()
{
	return _fontset->getFont(GameFontSet::EN);
}

/*---------------------------------------------------------------------*//**
	論理画面サイズの調整
**//*---------------------------------------------------------------------*/
void Game::normalizeLogicalScreenSize()
{
	if (_tfw->getView()->getViewMode() == View::VMODE_SCALE)	// スケールアスペクトへ
	{
		_widthLgc = _widthLgcReal;
		_heightLgc = _heightLgcReal;
	}
	else	// 固定アスペクトへ
	{
		_widthLgc = getStandardWidth();
		_heightLgc = getStandardHeight();
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
