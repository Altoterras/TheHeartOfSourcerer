/***********************************************************************//**
 *	StEmp_10201_1001_GetShield.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StEmp_10201_1001_GetShield.h"

// Friends
#include "../StEmpType.h"
#include "../../../episode/char/pc/PartnerUnit.h"
#include "../../../episode/party/MyParty.h"
#include "../../../episode/item/ItemIdDef.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../item/Item.h"
#include "../../../item/ItemManager.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StEmp_10201_1001_GetShield メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void StEmp_10201_1001_GetShield::begin(u32 param)
{
	MyParty* party = (MyParty*)Game::getGame()->getMyParty(); ASSERT(party != 0L);
	ItemManager* itmmng = party->getItemManager(); ASSERT(itmmng != 0L);

	// 盾入手
	ItemArray itmarr = itmmng->findByItemDefId(ItemIdDef::ITEMDID_DUMMY, true);	// 予約用ダミーの検索
	if(itmarr.getCount() > 0)
	{
		// 盾入手場所確保用のダミーアイテムを削除
		Item* item = itmarr.item(0);
		itmmng->removeItem(item->getItemGenId());
		// 盾の追加
		itmmng->newItem(ItemIdDef::ITEMDID_SHLD_FIRST, 0L, Item::F_NOEDIT, true);	// 編集不可を末尾に
		// 盾の装備
		PartnerUnit* unitPc2 = (PartnerUnit*)party->pcUnit(MyParty::PC_PTN);
		unitPc2->equipBest();
		unitPc2->updateEquipModelObjVisibility();
	}

	StEmp::begin(param);
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void StEmp_10201_1001_GetShield::end()
{
	StEmp::end();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10201_1001_GetShield::StEmp_10201_1001_GetShield()
	: StEmp(STEMPTYPE_10201_1001)
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
