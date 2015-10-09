/***********************************************************************//**
 *	StEmp_10202_1001_AsaOnlyEvent.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/29.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StEmp_10202_1001_AsaOnlyEvent.h"

// Friends
#include "../StEmpType.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../episode/party/MyParty.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define EVID_ASA_ONLY_EVENT		10003	// アサが独りのときに発動するイベント

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StEmp_10202_1001_AsaOnlyEvent メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void StEmp_10202_1001_AsaOnlyEvent::begin(u32 param)
{
	Party* party = Game::getGame()->getMyParty();

	// アサのみが有効な場合、次のイベントを起動する
	if(	party->isEnablePc(MyParty::PC_MY) &&
		!party->isEnablePc(MyParty::PC_PTN) &&
		!party->isEnablePc(MyParty::PC_HLP) )
	{
		this->setOption(EVID_ASA_ONLY_EVENT, 0);	// 次のイベントを設定する
	}
	else
	{
		// Nothing メッセージ表示
		GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow();
		VcString str;
		wndSysMsg->showStdMsg(VcString::format(&str, GameFixedStrTbl::getString(GameFixedStrTbl::SID_NOT_FOUND)->getRaw(), ""));
	}

	StEmp::begin(param);
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void StEmp_10202_1001_AsaOnlyEvent::end()
{
	StEmp::end();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10202_1001_AsaOnlyEvent::StEmp_10202_1001_AsaOnlyEvent()
	: StEmp(STEMPTYPE_10202_1001)
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
