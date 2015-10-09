/***********************************************************************//**
 *	MapEmp_10221_1001_LostCheck.cpp
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
#include "MapEmp_10221_1001_LostCheck.h"

// Friends
#include "../MapEmpType.h"
#include "../../StDepEvDef.h"
#include "../../../episode/party/MyParty.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../unit/char/pc/PcUnit.h"
#include "../../../status/CharStat.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// アサが独りのときに発動するイベント
#define EVID_ASA_ONLY_EVENT		10003

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapEmp_10221_1001_LostCheck メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MapEmp_10221_1001_LostCheck::MapEmp_10221_1001_LostCheck()
	: MapEmp(MAPEMPTYPE_10221_1001)
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void MapEmp_10221_1001_LostCheck::begin(u32 param)
{
	MapEmp::begin(param);

	MyParty* party = (MyParty*)Game::getGame()->getMyParty();
	for(int i = 0; i < party->getPcNum(); i++)
	{
		const CharUnit* cunit = party->pcUnit(i);
		const CharStat* cstat = cunit->getCharStat();
		if(cstat->getStat() == CharStat::STAT_DOWN)
		{
			// エネルギーが無い場合は 1 だけ回復する
			for(int j = 0; j < party->getPcNum(); j++)
			{
				CharStat* cstatWk = party->pcUnit(j)->charStat();
				if(cstatWk->getEnergy() <= 0)
				{
					cstatWk->revive(true);	// 1 だけ回復
				}
			}

			// 対決敗北時は続けてメッセージイベント起動
			EvCause evcause(EventCause::CID_EVENT);
			getOwner()->begin(StDepEvDef::EVID_EP01_MAP_MESSAGE_CONFRONT_LOSE, 0, &evcause);
			break;
		}
	}

	/*
	CharUnit* cunitMy = (CharUnit*)party->pcUnit(MyParty::PC_MY);
	if(cunitMy != 0L)
	{
		const CharStat* cstatMy = cunitMy->getCharStat();
		if(cstatMy->getStat() == CharStat::STAT_DOWN)
		{
			// アサ・コルポのエネルギーが無い場合は 1 だけ回復する
			for(int i = MyParty::PC_MY; i <= MyParty::PC_PTN; i++)
			{
				CharStat* cstat = party->pcUnit(i)->charStat();
				if(cstat->getEnergy() <= 0)
				{
					cstat->revive(true);	// 1 だけ回復
				}
			}

			// 対決敗北時は続けてメッセージイベント起動
			EvCause evcause(EventCause::CID_EVENT);
			getOwner()->begin(StDepEvDef::EVID_EP01_MAP_MESSAGE_CONFRONT_LOSE, 0, &evcause);
		}
	}
	*/
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
