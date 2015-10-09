/***********************************************************************//**
 *	BasicModelGui.cpp
 *	Enlight BasicModel Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "BasicModelGui.h"

// Friends
#include "BasicModelFontSet.h"
#include "../body/BasicModelFixedStrTbl.h"
#include "../file/BasicModelFileIdDef.h"

// External
#include "../../etk/body/EtkBody.h"
#include "../../etk/gui/window/implement/ActionMsgWindow.h"
#include "../../etk/gui/window/implement/SysMsgWindow.h"
#include "../../tfw/framemod/FedTaskSys.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/lib/Rect.h"

// Library

////////////////////////////////////////////////////////////////////////////

BASICMODEL_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BasicModelGui メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// プロパティ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BasicModelGui::BasicModelGui()
	: _texGui(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BasicModelGui::~BasicModelGui()
{
	ASSERT(_texGui == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool BasicModelGui::create(FontSet* fontsetRef, FedTaskSys* tasksys)
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
void BasicModelGui::destroy()
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
void BasicModelGui::notifyChangeLanguage()
{
	// 親へ通知
	EtkGui::notifyChangeLanguage();

	// GUI テクスチャの再読み込み
	if(_texGui != 0L)
	{
		_texGui->destroy();
		if(!_texGui->create(FILEID_CONV_LANG( FILEID_CONV_RD( SYSTEM_GUI01_SD_EN_PNG ) ), Texture::FMT_PNG))
		{
			ASSERTM(false, "{notifyChangeLanguage} BasicModelGui Texture - create failed.");
		}
	}
}

/*---------------------------------------------------------------------*//**
	システムメッセージウインドウの作成実装
**//*---------------------------------------------------------------------*/
SysMsgWindow* BasicModelGui::makeSysMsgWindow()
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
		BasicModelFixedStrTbl::getString(BasicModelFixedStrTbl::SID_YES),
		BasicModelFixedStrTbl::getString(BasicModelFixedStrTbl::SID_NO)	))
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
ActionMsgWindow* BasicModelGui::makeActionMsgWindow()
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

BASICMODEL_END_NS
