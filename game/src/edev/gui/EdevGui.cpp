/***********************************************************************//**
 *	EdevGui.cpp
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
#include "EdevGui.h"

// Friends
#include "EdevFontSet.h"
#include "../body/EdevFixedStrTbl.h"
#include "../file/EdevFileIdDef.h"

// External
#include "../../etk/body/EtkBody.h"
#include "../../etk/gui/window/implement/ActionMsgWindow.h"
#include "../../etk/gui/window/implement/SysMsgWindow.h"
#include "../../tfw/framemod/FedTaskSys.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/lib/Rect.h"

// Library

////////////////////////////////////////////////////////////////////////////

EDEV_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EdevGui メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// プロパティ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EdevGui::EdevGui()
	: _texGui(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EdevGui::~EdevGui()
{
	ASSERT(_texGui == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EdevGui::create(FontSet* fontsetRef, FedTaskSys* tasksys)
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

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EdevGui::destroy()
{
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
void EdevGui::notifyChangeLanguage()
{
	// 親へ通知
	EtkGui::notifyChangeLanguage();

	// GUI テクスチャの再読み込み
	if(_texGui != 0L)
	{
		_texGui->destroy();
		if(!_texGui->create(FILEID_CONV_LANG( FILEID_CONV_RD( SYSTEM_GUI01_SD_EN_PNG ) ), Texture::FMT_PNG))
		{
			ASSERTM(false, "{notifyChangeLanguage} EdevGui Texture - create failed.");
		}
	}
}

/*---------------------------------------------------------------------*//**
	システムメッセージウインドウの作成実装
**//*---------------------------------------------------------------------*/
SysMsgWindow* EdevGui::makeSysMsgWindow()
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
		EdevFixedStrTbl::getString(EdevFixedStrTbl::SID_YES),
		EdevFixedStrTbl::getString(EdevFixedStrTbl::SID_NO)	))
	{
		delete wndSysMsg;
		ASSERTM(false, "GameSysMsgWindow::create failed.");
		return false;
	}
	return wndSysMsg;
}

/*---------------------------------------------------------------------*//**
	アクションメッセージウインドウの作成実装
**//*---------------------------------------------------------------------*/
ActionMsgWindow* EdevGui::makeActionMsgWindow()
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
		ASSERTM(false, "GameActMsgWindow::create failed.");
		return false;
	}
	return wndActMsg;
}

////////////////////////////////////////////////////////////////////////////

EDEV_END_NS
