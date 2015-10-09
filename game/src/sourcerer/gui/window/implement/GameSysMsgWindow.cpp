/***********************************************************************//**
 *	GameSysMsgWindow.cpp
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
#include "GameSysMsgWindow.h"

// Friends
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"

// External
#include "../../../../tfw/lib/Rect.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS
	
////////////////////////////////////////////////////////////////////////////
// 定数

// 表示位置
#define W_THIS						(443)
#define H_THIS						(106)
#define X_THIS						((Game::getGame()->getLogicalWidth() - W_THIS) / 2)
#define Y_THIS						(Game::getGame()->getLogicalHeight() - H_THIS)
	
////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GameSysMsgWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool GameSysMsgWindow::create(Font* fontRef, Texture* texRef)
{
	RectF32 rectWnd(X_THIS, Y_THIS, W_THIS, H_THIS);
	RectF32 rectScreen(0, 0, Game::getGame()->getLogicalWidth(), Game::getGame()->getLogicalHeight());
	return SysMsgWindow::create(
		&rectWnd,
		&rectScreen,
		fontRef,
		texRef,
		GameFixedStrTbl::getString(GameFixedStrTbl::SID_YES),
		GameFixedStrTbl::getString(GameFixedStrTbl::SID_NO) );
}


/*---------------------------------------------------------------------*//**
	ウインドウ表示通知
**//*---------------------------------------------------------------------*/
void GameSysMsgWindow::onShowWindow(bool isShow)
{
	if(isShow)
	{
		complyChangeScreenSize(); // 位置調整
	}

	SysMsgWindow::onShowWindow(isShow);
}

/*---------------------------------------------------------------------*//**
	言語変更通知
**//*---------------------------------------------------------------------*/
void GameSysMsgWindow::notifyChangeLanguage()
{
	SysMsgWindow::notifyChangeLanguage();

	// Yes, No ボタンの言語を変更する
	changeLanguage(
		GameFixedStrTbl::getString(GameFixedStrTbl::SID_YES),
		GameFixedStrTbl::getString(GameFixedStrTbl::SID_NO));
}

/*---------------------------------------------------------------------*//**
	論理画面解像度変更通知
**//*---------------------------------------------------------------------*/
void GameSysMsgWindow::notifyChangeLogicalScreenSize()
{
	complyChangeScreenSize(); // 位置調整
}

/*---------------------------------------------------------------------*//**
	画面解像度変更に対応する
**//*---------------------------------------------------------------------*/
void GameSysMsgWindow::complyChangeScreenSize()
{
	// 現在の画面解像度における位置調整
	RectF32 rectWnd(*getRectangle());
	rectWnd.left() = X_THIS;
	rectWnd.top() = Y_THIS;
	RectF32 rectScreen(0, 0, Game::getGame()->getLogicalWidth(), Game::getGame()->getLogicalHeight());
	setRectangle(&rectWnd, &rectScreen);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
