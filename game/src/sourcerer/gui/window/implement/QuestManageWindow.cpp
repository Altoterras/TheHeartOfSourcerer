/***********************************************************************//**
 *	QuestManageWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QuestManageWindow.h"

// Friends
#include "../../../char/CharDef.h"
#include "../../../episode/party/MyParty.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/QuestListWindow.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../quest/QuestArticle.h"
#include "../../../quest/QuestDef.h"
#include "../../../quest/QuestManager.h"
#include "../../../status/CharStat.h"
#include "../../../status/CharParam.h"
#include "../../../status/EquipStat.h"
#include "../../../unit/char/pc/PcUnit.h"

// External
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gui/PanelText.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#if 1
#define X_BASE						((Game::getGame()->getLogicalWidth() - 400) / 2)
#define RECT_THIS					X_BASE + 40, 48, 400, 272
#define RECT_BTN_ITEM				X_BASE + 300, 108, 60, 24
#else
#define RECT_THIS					42, 48, 400, 272
#define RECT_BTN_ITEM				340, 108, 60, 24
#endif

#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ
#define UV_ITEMBTN_NRM				928.0f / W_TEX, 264.0f / H_TEX, 60.0f / W_TEX, 24.0f / H_TEX
#define UV_ITEMBTN_HVR				928.0f / W_TEX, 288.0f / H_TEX, 60.0f / W_TEX, 24.0f / H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QuestManageWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QuestManageWindow::QuestManageWindow()
	: _wndQstList(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
QuestManageWindow::~QuestManageWindow()
{
	ASSERT(_wndQstList == 0L);
}

/*---------------------------------------------------------------------*//**
	作成通知
**//*---------------------------------------------------------------------*/
bool QuestManageWindow::create(Font* fontRef, Texture* texRef)
{
	// ウインドウの作成
	RectF32 rectThis(RECT_THIS);
	if(!Window::create(&rectThis))
	{
		return false;
	}

	// クエストリストウインドウの作成
	QuestListWindow* wndQstList = new QuestListWindow();
	if(!wndQstList->create(fontRef, texRef))
	{
		delete wndQstList;
		return false;
	}
	_wndQstList = wndQstList;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void QuestManageWindow::destroy()
{
	// クエストリストウインドウの削除
	if(_wndQstList != 0L)
	{
		_wndQstList->destroy();
		delete _wndQstList;
		_wndQstList = 0L;
	}

	Window::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void QuestManageWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	GameGui* gui = Game::getGame()->getGui(); ASSERT(gui != 0L);

	// クエストリストウインドウの制御
	_wndQstList->exec(res, ec);

	// 親の制御
	Window::execContents(res, ec);
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void QuestManageWindow::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	Window::drawContents(rc, rectBase);

	// クエストリストウインドウの描画
	_wndQstList->draw(rc);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
void QuestManageWindow::onShowWindow(bool isShow)
{
	Window::onShowWindow(isShow);

	// リストウインドウを表示する
	if(_wndQstList != 0L)	{	_wndQstList->showWindow(isShow);	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
