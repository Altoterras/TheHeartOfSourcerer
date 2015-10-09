/***********************************************************************//**
 *	GameGui.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GameGui.h"

// Friends
#include "EventMovieFront.h"
#include "GameFontSet.h"
#include "IngameFront.h"
#include "CharFronts.h"
#include "MoveCursor.h"
#include "FocusCursor.h"
#include "FocusMenu.h"
#include "window/implement/AriaWindow.h"
#include "window/implement/GameActMsgWindow.h"
#include "window/implement/GameSysMsgWindow.h"
#include "window/implement/ItemListWindow.h"
#include "window/implement/SourceListWindow.h"
#include "window/implement/SubtitlesWindow.h"
#include "window/implement/SourceEditWindow.h"
#include "../body/GameExecCtx.h"
#include "../body/GameTaskModDef.h"
#include "../event/EventSys.h"
#include "../file/GameFileIdDef.h"
#include "../mode/GameMode.h"

// External
#include "../../tfw/framemod/FedTaskSys.h"
#include "../../tfw/gcmn/Texture.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GameGui メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	何か操作をロックする GUI を開いているかどうか
**//*---------------------------------------------------------------------*/
bool GameGui::isOpenedLockGui() const
{
	if(_wndSbt->isShowWindow())			{	return true;	}
	if(_wndSrcedt->isShowWindow())		{	return true;	}
	if(_wndMagicList->isShowWindow())	{	return true;	}
	if(_wndItemList->isShowWindow())	{	return true;	}
	return EtkGui::isOpenedLockGui();
}

/*---------------------------------------------------------------------*//**
	実行制御フラグを設定する
**//*---------------------------------------------------------------------*/
void GameGui::setExecFlags(u32 eflags, bool isValid)
{
	EtkGui::setExecFlags(eflags, isValid);

	// 一時隠しの制御
	bool isTmpHide = TFW_IS_FLAG(_eflags, EF_HIDE_TEMPORARY);
 	for(int i = 0; i < NUM_ARIAWND; i++)
	{
		_wndAria[i]->setActFlags(Window::AWNDF_TMP_HIDE, isTmpHide);
	}
	_wndSbt->setActFlags(Window::AWNDF_TMP_HIDE, isTmpHide);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
GameGui::GameGui()
	: _texGui(0L)
	, _gmkPrevFrame(0)
	, _igfr(0L)
	, _evmfr(0L)
	, _chrfrnt(0L)
	, _mvcsr(0L)
	, _fccsr(0L)
	, _fcmenu(0L)
	, _wndSbt(0L)
	, _wndSrcedt(0L)
	, _wndMagicList(0L)
	, _wndItemList(0L)
{
	for(int i = 0; i < NUM_ARIAWND; i++)	{	_wndAria[i] = 0L;	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
GameGui::~GameGui()
{
	ASSERT(_texGui == 0L);
	ASSERT(_igfr == 0L);
	ASSERT(_evmfr == 0L);
	ASSERT(_chrfrnt == 0L);
	ASSERT(_mvcsr == 0L);
	ASSERT(_fccsr == 0L);
	ASSERT(_fcmenu == 0L);
	ASSERT(_wndSbt == 0L);
	ASSERT(_wndSrcedt == 0L);
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_ARIAWND; i++)	{	ASSERT(_wndAria[i] == 0L);	}
	#endif
	ASSERT(_wndMagicList == 0L);
	ASSERT(_wndItemList == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool GameGui::create(GameFontSet* fontsetRef, FedTaskSys* tasksys)
{
	// GUI テクスチャの読み込み
	Texture* texGui = new Texture();
	if(!texGui->create(FILEID_CONV_LANG( FILEID_CONV_RD( SYSTEM_GUI01_SD_EN_PNG ) ), Texture::FMT_PNG))
	{
		delete texGui;
		ASSERTM(false, "gui Texture::create failed.");
		return false;
	}
	_texGui = texGui;

	// 親の作成
	if(!EtkGui::create(fontsetRef, tasksys))
	{
		return false;
	}

	// 台詞ウインドウを作成する
	SubtitlesWindow* wndSubt = new SubtitlesWindow();
	if(!wndSubt->create(fontsetRef->getFont(GameFontSet::JP_MOVIE), _texGui))
	{
		delete wndSubt;
		ASSERTM(false, "SubtitlesWindow::create failed.");
		return false;
	}
	_wndSbt = wndSubt;

	// ソースエディタウインドウを作成する
	SourceEditWindow* wndSoftkb = new SourceEditWindow();
	if(!wndSoftkb->create(fontsetRef->getFont(GameFontSet::JP), _texGui))
	{
		delete wndSoftkb;
		ASSERTM(false, "SourceEditWindow::create failed.");
		return false;
	}
	_wndSrcedt = wndSoftkb;

	// 詠唱ウインドウを作成する
	for(int i = 0; i < NUM_ARIAWND; i++)
	{
		AriaWindow* wndAria = new AriaWindow();
		if(!wndAria->create(fontsetRef->getFont(GameFontSet::EN), _texGui, AriaWindow::MODE_SPELL))
		{
			delete wndAria;
			ASSERTM(false, "AriaWindow::create failed.");
			return false;
		}
		_wndAria[i] = wndAria;
	}

	// マ法リストウインドウを作成する
	SourceListWindow* wndMagicList = new SourceListWindow();
	if(!wndMagicList->create(fontsetRef->getFont(GameFontSet::JP), _texGui))
	{
		delete wndMagicList;
		ASSERTM(false, "SourceListWindow::create failed.");
		return false;
	}
	_wndMagicList = wndMagicList;

	// アイテムリストウインドウを作成する
	ItemListWindow* wndItemList = new ItemListWindow();
	if(!wndItemList->create(fontsetRef->getFont(GameFontSet::JP), _texGui))
	{
		delete wndItemList;
		ASSERTM(false, "ItemListWindow::create failed.");
		return false;
	}
	_wndItemList = wndItemList;

	// インゲームフロントの作成
	IngameFront* igfr = new IngameFront();
	if(!igfr->create(_texGui))
	{
		delete igfr;
		ASSERTM(false, "IngameFront::create failed.");
		return false;
	}
	_igfr = igfr;

	// イベントムービーフロントの作成
	EventMovieFront* evmfr = new EventMovieFront();
	if(!evmfr->create(_texGui))
	{
		delete evmfr;
		ASSERTM(false, "EventMovieFront::create failed.");
		return false;
	}
	_evmfr = evmfr;

	// フォーカスメニューを作成する
	FocusMenu* fcmenu = new FocusMenu();
	if(!fcmenu->create())
	{
		delete fcmenu;
		ASSERTM(false, "FocusMenu::create failed.");
		return false;
	}
	_fcmenu = fcmenu;

	// フォーカスカーソルを作成する
	FocusCursor* fccsr = new FocusCursor();
	if(!fccsr->create(_texGui, _fcmenu))
	{
		delete fccsr;
		ASSERTM(false, "FocusCursor::create failed.");
		return false;
	}
	_fccsr = fccsr;

	// キャラクタステータスゲージの作成
	CharFronts* chrfrnt = new CharFronts();
	if(!chrfrnt->create(_texGui, _fccsr, _fcmenu))
	{
		delete chrfrnt;
		ASSERTM(false, "CharStatGauge::create failed.");
		return false;
	}
	_chrfrnt = chrfrnt;

	// 移動カーソルを作成する
	MoveCursor* mvcsr = new MoveCursor();
	if(!mvcsr->create(_texGui))
	{
		delete mvcsr;
		ASSERTM(false, "MoveCursor::create failed.");
		return false;
	}
	_mvcsr = mvcsr;

	// タスクシステムに各モジュールを登録する（描画順をそのままヒント ID に流用する）
	if(!tasksys->registerExecModule(_igfr,			GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_INGAMEFRONT),		GameTaskModDef::MOD_INGAMEFRONT				, 0))	{	ASSERT(false);	}
	if(!tasksys->registerExecModule(_evmfr,			GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_EVENTMVFRONT),	GameTaskModDef::MOD_EVENTMVFRONT			, 0))	{	ASSERT(false);	}
	if(!tasksys->registerExecModule(_chrfrnt,		GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_CHARSTATGAUGE),	GameTaskModDef::MOD_CHARSTATGAUGE			, 0))	{	ASSERT(false);	}
	if(!tasksys->registerExecModule(_mvcsr,			GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_MOVECURSOR),		GameTaskModDef::MOD_MOVECURSOR				, 0))	{	ASSERT(false);	}
 	if(!tasksys->registerExecModule(_fccsr,			GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_FOCUSCURSOR),		GameTaskModDef::MOD_FOCUSCURSOR				, 0))	{	ASSERT(false);	}
	if(!tasksys->registerExecModule(_fcmenu,		GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_FOCUSMENU),		GameTaskModDef::MOD_FOCUSMENU				, 0))	{	ASSERT(false);	}
 	for(int i = 0; i < NUM_ARIAWND; i++)
	{
		if (!tasksys->registerExecModule(_wndAria[i], GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_ARIA_WND),		GameTaskModDef::MOD_ARIA_WND				, EtkTaskModDef::MGF_WINDOW))	{ ASSERT(false); }
	}
	if(!tasksys->registerExecModule(_wndSbt,		GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_SUBTITLES_WND),	GameTaskModDef::MOD_SUBTITLES_WND			, EtkTaskModDef::MGF_WINDOW))	{	ASSERT(false);	}
	if(!tasksys->registerExecModule(_wndSrcedt,		GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_SOURCEEDIT_WND),	GameTaskModDef::MOD_SOURCEEDIT_WND			, EtkTaskModDef::MGF_WINDOW))	{	ASSERT(false);	}
	if(!tasksys->registerExecModule(_wndMagicList,	GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_MAGICLIST_WND),	GameTaskModDef::MOD_MAGICLIST_WND			, EtkTaskModDef::MGF_WINDOW))	{	ASSERT(false);	}
	if(!tasksys->registerExecModule(_wndItemList,	GameTaskModDef::calcExecOrder(GameTaskModDef::MOD_ITEMLIST_WND),	GameTaskModDef::MOD_ITEMLIST_WND			, EtkTaskModDef::MGF_WINDOW))	{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_igfr,			GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_INGAMEFRONT),		false, GameTaskModDef::MOD_INGAMEFRONT		, 0))	{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_evmfr,			GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_EVENTMVFRONT),	false, GameTaskModDef::MOD_EVENTMVFRONT		, 0))	{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_chrfrnt,		GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_CHARSTATGAUGE),	false, GameTaskModDef::MOD_CHARSTATGAUGE	, 0))	{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_mvcsr,			GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_MOVECURSOR),		false, GameTaskModDef::MOD_MOVECURSOR		, 0))	{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_fccsr,			GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_FOCUSCURSOR),		false, GameTaskModDef::MOD_FOCUSCURSOR		, 0))	{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_fcmenu,		GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_FOCUSMENU),		false, GameTaskModDef::MOD_FOCUSMENU		, 0))	{	ASSERT(false);	}
	for(int i = 0; i < NUM_ARIAWND; i++)
	{
		if (!tasksys->registerDrawModule(_wndAria[i], GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_ARIA_WND),		false, GameTaskModDef::MOD_ARIA_WND			, EtkTaskModDef::MGF_WINDOW))	{ ASSERT(false); }
	}
	if(!tasksys->registerDrawModule(_wndSbt,		GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_SUBTITLES_WND),	false, GameTaskModDef::MOD_SUBTITLES_WND	, EtkTaskModDef::MGF_WINDOW))	{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_wndSrcedt,		GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_SOURCEEDIT_WND),	false, GameTaskModDef::MOD_SOURCEEDIT_WND	, EtkTaskModDef::MGF_WINDOW))	{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_wndMagicList,	GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_MAGICLIST_WND),	false, GameTaskModDef::MOD_MAGICLIST_WND	, EtkTaskModDef::MGF_WINDOW))	{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_wndItemList,	GameTaskModDef::calcDrawOrder(GameTaskModDef::MOD_ITEMLIST_WND),	false, GameTaskModDef::MOD_ITEMLIST_WND		, EtkTaskModDef::MGF_WINDOW))	{	ASSERT(false);	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void GameGui::destroy()
{
	// 移動カーソルを削除する
	if(_mvcsr != 0L)
	{
		_mvcsr->destroy();
		delete _mvcsr;
		_mvcsr = 0L;
	}

	// キャラクタステータスゲージを削除する
	if(_chrfrnt != 0L)
	{
		_chrfrnt->destroy();
		delete _chrfrnt;
		_chrfrnt = 0L;
	}

	// フォーカスカーソルを削除する
	if(_fccsr != 0L)
	{
		_fccsr->destroy();
		delete _fccsr;
		_fccsr = 0L;
	}

	// フォーカスメニューを削除する
	if(_fcmenu != 0L)
	{
		_fcmenu->destroy();
		delete _fcmenu;
		_fcmenu = 0L;
	}

	// イベントムービーフロントを削除する
	if(_evmfr != 0L)
	{
		_evmfr->destroy();
		delete _evmfr;
		_evmfr = 0L;
	}

	// インゲームフロントを削除する
	if(_igfr != 0L)
	{
		_igfr->destroy();
		delete _igfr;
		_igfr = 0L;
	}

	// アイテムリストウインドウを削除する
	if(_wndItemList != 0L)
	{
		_wndItemList->destroy();
		delete _wndItemList;
		_wndItemList = 0L;
	}

	// マ法リストウインドウを削除する
	if(_wndMagicList != 0L)
	{
		_wndMagicList->destroy();
		delete _wndMagicList;
		_wndMagicList = 0L;
	}

	// 詠唱ウインドウを削除する
	for(int i = 0; i < NUM_ARIAWND; i++)
	{
		if(_wndAria[i] != 0L)
		{
			_wndAria[i]->destroy();
			delete _wndAria[i];
			_wndAria[i] = 0L;
		}
	}

	// ソースエディタウインドウを削除する
	if(_wndSrcedt != 0L)
	{
		_wndSrcedt->destroy();
		delete _wndSrcedt;
		_wndSrcedt = 0L;
	}

	// 台詞ウインドウを削除する
	if(_wndSbt != 0L)
	{
		_wndSbt->destroy();
		delete _wndSbt;
		_wndSbt = 0L;
	}

	// GUI テクスチャを削除する
	if(_texGui != 0L)
	{
		_texGui->destroy();
		delete _texGui;
		_texGui = 0L;
	}

	EtkGui::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void GameGui::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	GameMode* gmode = gec->getGameMode();

	// コンポーネントの有効化・無効化
	GameMode::ModeKind mode = gmode->getChildEndModeKind();
	if(_gmkPrevFrame != mode)
	{
		_gmkPrevFrame = mode;
		if(!TFW_IS_FLAG(_eflags, EF_EXT_ENABLE_CTRL))	// 有効・無効化の外部制御時以外
		{
			switch(mode)
			{
			case GameMode::MODE_GAME_INGAME_WALKING_NORMAL:
			case GameMode::MODE_GAME_INGAME_WALKING_MAGICTYPING:
			case GameMode::MODE_GAME_INGAME_CONFRONT_NORMAL:
			case GameMode::MODE_GAME_INGAME_CONFRONT_MAGICTYPING:
				if(!_igfr->isEnable()) { _igfr->setEnable(true); }
				if(!_chrfrnt->isEnable()) {	_chrfrnt->setEnable(true);						}
				if(!_mvcsr->isEnable(MoveCursor::EDCH_GUI)) { _mvcsr->setEnable(MoveCursor::EDCH_GUI, true); }
				if(!_fccsr->isEnable()) { _fccsr->setEnable(true); }
				if(!_fcmenu->isEnable()) { _fcmenu->setEnable(true); }
				break;
			default:
				if(_igfr->isEnable()) { _igfr->setEnable(false); }
				if(_chrfrnt->isEnable()) { _chrfrnt->setEnable(false); }
				if(_mvcsr->isEnable(MoveCursor::EDCH_GUI)) { _mvcsr->setEnable(MoveCursor::EDCH_GUI, false); }
				if(_fccsr->isEnable()) { _fccsr->setEnable(false); }
				if(_fcmenu->isEnable()) { _fcmenu->setEnable(false); }
				break;
			}

			_igfr->notifyChangeMode(mode);
		}
	}
}

/*---------------------------------------------------------------------*//**
	言語変更通知
**//*---------------------------------------------------------------------*/
void GameGui::notifyChangeLanguage()
{
	// 親へ通知
	EtkGui::notifyChangeLanguage();

	// GUI テクスチャの再読み込み
	if(_texGui != 0L)
	{
		_texGui->destroy();
		if(!_texGui->create(FILEID_CONV_LANG( FILEID_CONV_RD( SYSTEM_GUI01_SD_EN_PNG ) ), Texture::FMT_PNG))
		{
			ASSERTM(false, "{notifyChangeLanguage} GameGui Texture - create failed.");
		}
	}
}

/*---------------------------------------------------------------------*//**
	スクリーンリサイズ通知
**//*---------------------------------------------------------------------*/
void GameGui::notifyResizeView()
{
	// 親へ通知
	EtkGui::notifyResizeView();

	_igfr->notifyResizeView();
}

/*---------------------------------------------------------------------*//**
	ゲーム開始通知
**//*---------------------------------------------------------------------*/
void GameGui::notifyEnterGame()
{
	_fcmenu->notifyEnterGame();
	_fccsr->notifyEnterGame();
}

/*---------------------------------------------------------------------*//**
	マップ変更前通知
**//*---------------------------------------------------------------------*/
void GameGui::notifyChangeMapBefore()
{
	_chrfrnt->notifyChangeMapBefore();
	_fccsr->notifyChangeMapBefore();
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void GameGui::notifyChangedMap()
{
	_mvcsr->notifyChangedMap();
	_fcmenu->notifyChangedMap();
}

/*---------------------------------------------------------------------*//**
	リーダー変更通知
**//*---------------------------------------------------------------------*/
void GameGui::notifyChangeLeader()
{
	_mvcsr->notifyChangeLeader();
	_igfr->notifyChangeLeader();
}

/*---------------------------------------------------------------------*//**
	デシリアライズ（セーブデータ読み込み）通知
**//*---------------------------------------------------------------------*/
void GameGui::notifyDeserialize()
{
	_fccsr->notifyDeserialize();
}

/*---------------------------------------------------------------------*//**
	システムメッセージウインドウの作成実装
**//*---------------------------------------------------------------------*/
SysMsgWindow* GameGui::makeSysMsgWindow()
{
	// システムメッセージウインドウを作成する
	GameSysMsgWindow* wndSysMsg = new GameSysMsgWindow();
	if(!wndSysMsg->create(_fontsetRef->getFont(GameFontSet::JP), _texGui))
	{
		delete wndSysMsg;
		ASSERTM(false, "GameSysMsgWindow::create failed.");
		return 0L;
	}
	return wndSysMsg;
}

/*---------------------------------------------------------------------*//**
	アクションメッセージウインドウの作成実装
**//*---------------------------------------------------------------------*/
ActionMsgWindow* GameGui::makeActionMsgWindow()
{
	// アクションメッセージウインドウを作成する
	GameActMsgWindow* wndActMsg = new GameActMsgWindow();
	if(!wndActMsg->create(_fontsetRef->getFont(GameFontSet::JP), _texGui))
	{
		delete wndActMsg;
		ASSERTM(false, "GameActMsgWindow::create failed.");
		return 0L;
	}
	return wndActMsg;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
