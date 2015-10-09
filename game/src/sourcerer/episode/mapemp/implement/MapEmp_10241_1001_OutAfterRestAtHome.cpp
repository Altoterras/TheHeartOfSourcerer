/***********************************************************************//**
 *	MapEmp_10241_1001_OutAfterRestAtHome.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/01.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MapEmp_10241_1001_OutAfterRestAtHome.h"

// Friends
#include "../MapEmpType.h"
#include "../../StDepEvDef.h"
#include "../../../body/Game.h"
#include "../../../episode/party/MyParty.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define MSGID_INQURE		21
#define MSGWND_CTRL_HINT	675432

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapEmp_10241_1001_OutAfterRestAtHome メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MapEmp_10241_1001_OutAfterRestAtHome::MapEmp_10241_1001_OutAfterRestAtHome()
	: MapEmp(MAPEMPTYPE_10241_1702)
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void MapEmp_10241_1001_OutAfterRestAtHome::begin(u32 param)
{
	MapEmp::begin(param);

	MyParty* party = (MyParty*)Game::getGame()->getMyParty();

	// アサのみが有効な場合、次のイベントを起動する
	if(	party->isEnablePc(MyParty::PC_MY) &&
		!party->isEnablePc(MyParty::PC_PTN) &&
		!party->isEnablePc(MyParty::PC_HLP) )
	{
		// コルポをパーティに自動追加する
		party->setEnablePcChar(MyParty::PC_PTN, true);

		// コルポが合流するイベントを次に設定
		this->setOption(StDepEvDef::EVID_EP01_MAP_MEETING_BY_CORPO, 0);	// 次のイベントを設定する
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
