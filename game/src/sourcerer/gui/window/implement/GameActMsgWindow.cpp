/***********************************************************************//**
 *	GameActMsgWindow.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GameActMsgWindow.h"

// Friends
#include "../../../body/Game.h"
#include "../../../unit/Unit.h"
#include "../../../unit/char/CharUnit.h"

// External

// Library
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/Rect.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 表示位置
#define X_THIS			(0)
#define Y_THIS			(Game::getGame()->getLogicalHeight() - H_THIS)
#define W_THIS			(Game::getGame()->getLogicalWidth())
#define H_THIS			(160)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GameActMsgWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	メッセージ表示
**//*---------------------------------------------------------------------*/
void GameActMsgWindow::showMessage(const Unit* unitActorRef, const VcString* text)
{
	// 色の設定
	ColorU8 color(255, 255, 255, 255);
	getUnitColor(&color, unitActorRef);
	showMessageCol(&color, text);
}

/*---------------------------------------------------------------------*//**
	ユニットの色を得る
**//*---------------------------------------------------------------------*/
bool GameActMsgWindow::getUnitColor(ColorU8* color, const Unit* unit)
{
	if(unit == 0L)
	{
		// システム等
		color->set(0xff, 0xff, 0xff, 0xff);
	}
	else if(unit->getUnitType() == UNITTYPE_ENEMY_CHAR)
	{
		// 敵
		color->set(0xff, 0x77, 0x00, 0xff);
	}
	else if(unit->getUnitType() == UNITTYPE_MY_CHAR)
	{
		// PC 1
		color->set(0xff, 0xee, 0xcc, 0xff);
	}
	else if(unit->getUnitType() == UNITTYPE_PARTNER_CHAR)
	{
		// PC 2
		color->set(0xcc, 0xff, 0xff, 0xff);
	}
	else if(unit->getUnitType() == UNITTYPE_HELPER_CHAR)
	{
		// PC 3
		color->set(0xff, 0xcc, 0xff, 0xff);
	}
	else if(unit->getUnitType() == UNITTYPE_EXTRA_CHAR)
	{
		// 脇役
		color->set(0xff, 0xff, 0xff, 0xff);
	}
	else
	{
		return false;
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool GameActMsgWindow::create(Font* fontRef, Texture* texRef)
{
	RectF32 rectWnd(X_THIS, Y_THIS, W_THIS, H_THIS);
	RectF32 rectScreen(0, 0, Game::getGame()->getLogicalWidth(), Game::getGame()->getLogicalHeight());
	return ActionMsgWindow::create(
		&rectWnd,
		&rectScreen,
		fontRef,
		texRef );
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
