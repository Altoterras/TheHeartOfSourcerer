/***********************************************************************//**
 *	DimzGui.cpp
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
#include "DimzGui.h"

// Friends
#include "DimzFontSet.h"
#include "../body/DimzFixedStrTbl.h"
#include "../file/DimzFileIdDef.h"

// External
#include "../../etk/gui/window/implement/ActionMsgWindow.h"
#include "../../etk/gui/window/implement/SysMsgWindow.h"
#include "../../tfw/framemod/FedTaskSys.h"
#include "../../tfw/gcmn/Texture.h"

// Library

////////////////////////////////////////////////////////////////////////////

DIMZ_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// DimzGui メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// プロパティ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御
	
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
DimzGui::DimzGui()
	: _texGui(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
DimzGui::~DimzGui()
{
	ASSERT(_texGui == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool DimzGui::create(FontSet* fontsetRef, FedTaskSys* tasksys)
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
void DimzGui::destroy()
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
void DimzGui::notifyChangeLanguage()
{
	// 親へ通知
	EtkGui::notifyChangeLanguage();

	// GUI テクスチャの再読み込み
	if(_texGui != 0L)
	{
		_texGui->destroy();
		if(!_texGui->create(FILEID_CONV_LANG( FILEID_CONV_RD( SYSTEM_GUI01_SD_EN_PNG ) ), Texture::FMT_PNG))
		{
			ASSERTM(false, "{notifyChangeLanguage} DimzGui Texture - create failed.");
		}
	}
}

/*---------------------------------------------------------------------*//**
	システムメッセージウインドウの作成実装
**//*---------------------------------------------------------------------*/
SysMsgWindow* DimzGui::makeSysMsgWindow()
{
	// システムメッセージウインドウを作成する
	SysMsgWindow* wndSysMsg = new SysMsgWindow();
	if(!wndSysMsg->create(
		_fontsetRef->getMainGuiFont(),
		_texGui,
		DimzFixedStrTbl::getString(DimzFixedStrTbl::SID_YES),
		DimzFixedStrTbl::getString(DimzFixedStrTbl::SID_NO)	))
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
ActionMsgWindow* DimzGui::makeActionMsgWindow()
{
	// アクションメッセージウインドウを作成する
	ActionMsgWindow* wndActMsg = new ActionMsgWindow();
	if(!wndActMsg->create(_fontsetRef->getMainGuiFont(), _texGui))
	{
		delete wndActMsg;
		ASSERTM(false, "GameActMsgWindow::create failed.");
		return false;
	}
	return wndActMsg;
}

////////////////////////////////////////////////////////////////////////////

DIMZ_END_NS
