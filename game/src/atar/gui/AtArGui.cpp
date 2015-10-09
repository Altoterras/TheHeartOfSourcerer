/***********************************************************************//**
 *	AtArGui.cpp
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "AtArGui.h"

// Friends
#include "AtArFontSet.h"
#include "IngameFront.h"
#include "window/implement/HelpWindow.h"
#include "../body/AtArFixedStrTbl.h"
#include "../body/AtArTaskModDef.h"
#include "../file/AtArFileIdDef.h"

// External
#include "../../etk/body/EtkBody.h"
#include "../../etk/gui/window/implement/ActionMsgWindow.h"
#include "../../etk/gui/window/implement/SysMsgWindow.h"
#include "../../tfw/framemod/FedTaskSys.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/lib/Rect.h"

// Library

////////////////////////////////////////////////////////////////////////////

ATAR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define ENABLE_GUI_TEXTURE	1

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// AtArGui メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// プロパティ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	何か操作をロックする GUI を開いているかどうか
**//*---------------------------------------------------------------------*/
bool AtArGui::isOpenedLockGui() const
{
	if(EtkGui::isOpenedLockGui())		{	return true;	}
	if(_wndHelp->isShowWindow())		{	return true;	}
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AtArGui::AtArGui()
	: _texGui(0L)
	, _igfr(0L)
	, _wndHelp(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
AtArGui::~AtArGui()
{
	ASSERT(_texGui == 0L);
	ASSERT(_igfr == 0L);
	ASSERT(_wndHelp == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool AtArGui::create(FontSet* fontsetRef, FedTaskSys* tasksys)
{
#if ENABLE_GUI_TEXTURE
	// GUI テクスチャの読み込み
	Texture* texGui = new Texture();
	//if(!texGui->create(FILEID_CONV_LANG( FILEID_CONV_RD( SYSTEM_GUI01_SD_EN_PNG ) ), Texture::FMT_PNG))
	if(!texGui->create(FILEID_CONV_RD( SYSTEM_GUI01_SD_JP_PNG ), Texture::FMT_PNG))
	{
		delete texGui;
		ASSERTM(false, "gui Texture::create failed.");
		return false;
	}
	_texGui = texGui;
#endif

	// 親の作成
	if(!EtkGui::create(fontsetRef, tasksys))
	{
		return false;
	}

	// インゲームフロントの作成
	IngameFront* igfr = new IngameFront();
	if(!igfr->create(_texGui))
	{
		delete igfr;
		ASSERTM(false, "IngameFront::create failed.");
		return false;
	}
	_igfr = igfr;
	_igfr->setEnable(false);

	// ヘルプウインドウを作成する
	HelpWindow* wndHelp = new HelpWindow();
	if(!wndHelp->create(fontsetRef->getFont(AtArFontSet::JP), _texGui))
	{
		delete wndHelp;
		ASSERTM(false, "HelpWindow::create failed.");
		return false;
	}
	_wndHelp = wndHelp;

	// タスクシステムに各モジュールを登録する（描画順をそのままヒント ID に流用する）
	if(!tasksys->registerExecModule(_igfr,			AtArTaskModDef::calcExecOrder(AtArTaskModDef::MOD_INGAMEFRONT),		AtArTaskModDef::MOD_INGAMEFRONT				))	{	ASSERT(false);	}
	if(!tasksys->registerExecModule(_wndHelp,		AtArTaskModDef::calcExecOrder(AtArTaskModDef::MOD_HELP_WINDOW),		AtArTaskModDef::MOD_HELP_WINDOW				))	{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_igfr,			AtArTaskModDef::calcDrawOrder(AtArTaskModDef::MOD_INGAMEFRONT),		false, AtArTaskModDef::MOD_INGAMEFRONT		))	{	ASSERT(false);	}
	if(!tasksys->registerDrawModule(_wndHelp,		AtArTaskModDef::calcDrawOrder(AtArTaskModDef::MOD_HELP_WINDOW),		false, AtArTaskModDef::MOD_HELP_WINDOW		))	{	ASSERT(false);	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void AtArGui::destroy()
{
	// ヘルプウインドウを削除する
	if(_wndHelp != 0L)
	{
		_wndHelp->destroy();
		delete _wndHelp;
		_wndHelp = 0L;
	}

	// インゲームフロントを削除する
	if(_igfr != 0L)
	{
		_igfr->destroy();
		delete _igfr;
		_igfr = 0L;
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
	言語変更通知
**//*---------------------------------------------------------------------*/
void AtArGui::notifyChangeLanguage()
{
	// 親へ通知
	EtkGui::notifyChangeLanguage();

#if ENABLE_GUI_TEXTURE
	// GUI テクスチャの再読み込み
	if(_texGui != 0L)
	{
		_texGui->destroy();
		//if(!_texGui->create(FILEID_CONV_LANG( FILEID_CONV_RD( SYSTEM_GUI01_SD_EN_PNG ) ), Texture::FMT_PNG))
		if(!_texGui->create(FILEID_CONV_RD( SYSTEM_GUI01_SD_JP_PNG ), Texture::FMT_PNG))
		{
			ASSERTM(false, "{notifyChangeLanguage} AtArGui Texture - create failed.");
		}
	}
#endif
}

/*---------------------------------------------------------------------*//**
	システムメッセージウインドウの作成実装
**//*---------------------------------------------------------------------*/
SysMsgWindow* AtArGui::makeSysMsgWindow()
{
	#define W_SYSMSGWND					(443)
	#define H_SYSMSGWND					(106)

	// システムメッセージウインドウを作成する
	SysMsgWindow* wndSysMsg = new SysMsgWindow();
	RectS32 rectWnd((EtkBody::getBody()->getLogicalWidth() - W_SYSMSGWND) / 2, EtkBody::getBody()->getLogicalHeight() - H_SYSMSGWND, W_SYSMSGWND, H_SYSMSGWND);
	RectS32 rectScreen(0, 0, EtkBody::getBody()->getLogicalWidth(), EtkBody::getBody()->getLogicalHeight());
	if(!wndSysMsg->create(
		&rectWnd,
		&rectScreen,
		_fontsetRef->getMainGuiFont(),
		_texGui,
		AtArFixedStrTbl::getString(AtArFixedStrTbl::SID_YES),
		AtArFixedStrTbl::getString(AtArFixedStrTbl::SID_NO)	))
	{
		delete wndSysMsg;
		ASSERTM(false, "SysMsgWindow::create failed.");
		return 0L;
	}
	return wndSysMsg;
}

/*---------------------------------------------------------------------*//**
	アクションメッセージウインドウの作成実装
**//*---------------------------------------------------------------------*/
ActionMsgWindow* AtArGui::makeActionMsgWindow()
{
	#define H_ACTMSGWND					(160)

	// アクションメッセージウインドウを作成する
	RectS32 rectWnd(0, EtkBody::getBody()->getLogicalHeight() - H_ACTMSGWND, EtkBody::getBody()->getLogicalWidth(), H_ACTMSGWND);
	RectS32 rectScreen(0, 0, EtkBody::getBody()->getLogicalWidth(), EtkBody::getBody()->getLogicalHeight());
	ActionMsgWindow* wndActMsg = new ActionMsgWindow();
	if(!wndActMsg->create(
		&rectWnd,
		&rectScreen,
		_fontsetRef->getMainGuiFont(),
		_texGui))
	{
		delete wndActMsg;
		ASSERTM(false, "ActionMsgWindow::create failed.");
		return 0L;
	}
	return wndActMsg;
}

////////////////////////////////////////////////////////////////////////////

ATAR_END_NS
