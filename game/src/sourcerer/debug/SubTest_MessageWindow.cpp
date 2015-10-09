/***********************************************************************//**
 *	SubTest_MessageWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SubTest_MessageWindow.h"

// Friends
#include "../body/Game.h"
#include "../gui/GameGui.h"
#include "../gui/window/implement/GameActMsgWindow.h"

// External
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラスメソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SubTest_MessageWindow::SubTest_MessageWindow()
	: _wndMsg(0L)
	, _cntFrame(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SubTest_MessageWindow::~SubTest_MessageWindow()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SubTest_MessageWindow::create()
{
	_wndMsg = Game::getGame()->getGui()->getActionMsgWindow();
	_wndMsg->showWindow(true);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SubTest_MessageWindow::destroy()
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SubTest_MessageWindow::exec(ExecRes* res, const ExecCtx* ec)
{
	if((_cntFrame & 0x3f) == 0)
	{
		ASSERT(_wndMsg != 0L);
		VcString str;
		_wndMsg->addWindowText(VcString::format(&str, "あああ %d\n", _cntFrame));
	}

	_cntFrame++;
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void SubTest_MessageWindow::render(const RenderCtx* rc)
{
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void SubTest_MessageWindow::draw(const RenderCtx* rc)
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
