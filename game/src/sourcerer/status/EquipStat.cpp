/***********************************************************************//**
 *	EquipStat.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/06/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EquipStat.h"

// Friends
#include "../body/Game.h"
#include "../item/Item.h"
#include "../item/ItemManager.h"

// External
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EquipStat メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アイテムを装備
**//*---------------------------------------------------------------------*/
void EquipStat::equipItem(s32 ieqp, Item* item)
{
	ASSERT((0 <= ieqp) && (ieqp < EquipPlace::NUM_EQP));
	Item* itemOld = _itemEq[ieqp];
	_itemEq[ieqp] = item;

	if(item != 0L)		// アイテム外しではない場合
	{
		item->setEquiped(true);
	}
	if(itemOld != 0L)	// 以前の装備アイテムがある場合
	{
		itemOld->setEquiped(false);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool EquipStat::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!storeToStructure(&svst)) { return false; }
		if(!ser->store(&svst, sizeof(SaveStructure))) { return false; }
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	デシリアイズ
**//*---------------------------------------------------------------------*/
bool EquipStat::deserialize(Serializer* ser, ItemManager* itmmng)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!ser->restore(&svst, sizeof(SaveStructure))) { return false; }
		return restoreFromStructure(&svst, itmmng);
	}
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャに保存する
**//*---------------------------------------------------------------------*/
bool EquipStat::storeToStructure(SaveStructure* svst) const
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	for(int i = 0; i < EquipPlace::NUM_EQP; i++)
	{
		u16 gid = 0;
		if(_itemEq[i] != 0L)
		{
			gid = _itemEq[i]->getItemGenId();
		}
		svst->_itmgid[i] = gid;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool EquipStat::restoreFromStructure(const SaveStructure* svst, ItemManager* itmmng)
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	ASSERT(itmmng != 0L);
	for(int i = 0; i < EquipPlace::NUM_EQP; i++)
	{
		u16 gid = svst->_itmgid[i];
		if(gid == 0)
		{
			equipItem(i, 0L);
		}
		else
		{
			equipItem(i, itmmng->itemFromGenId(gid));
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
