/***********************************************************************//**
 *	ItemManager.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ItemManager.h"

// Friends
#include "Item.h"
#include "ItemDef.h"
#include "ItemDefTbl.h"
#include "../episode/save/SaveStructure.h"
#include "../body/Game.h"
#include "../body/MoveMap.h"
#include "../map/Map.h"
#include "../map/PlacementObj.h"
#include "../unit/Unit.h"

// External
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ItemManager メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	アイテムを得る
**//*---------------------------------------------------------------------*/
const Item* ItemManager::getItem(s32 index) const
{
	return &_itmarr[index];
}

/*---------------------------------------------------------------------*//**
	アイテムを得る
**//*---------------------------------------------------------------------*/
Item* ItemManager::item(s32 index) const
{
	return &_itmarr[index];
}

/*---------------------------------------------------------------------*//**
	生成 ID からアイテムを得る
**//*---------------------------------------------------------------------*/
const Item* ItemManager::getItemFromGenId(u16 itmgid) const
{
	return itemFromGenId(itmgid);
}

/*---------------------------------------------------------------------*//**
	生成 ID からアイテムを得る
**//*---------------------------------------------------------------------*/
Item* ItemManager::itemFromGenId(u16 itmgid) const
{
	for(s32 i = 0; i < _num; i++)
	{
		if(!_itmarr[i].isEmpty())
		{
			if(_itmarr[i].getItemGenId() == itmgid)
			{
				return &_itmarr[i];
			}
		}
	}

	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アイテムの新規追加
	
	@return	インデックス（-1 は失敗）
**//*---------------------------------------------------------------------*/
s32 ItemManager::newItem(u16 itmdid, const EleneStat* eestat, u32 flags, bool isReverse)
{
	// 空きインデックスを得る
	s32 idx = getEmptyItemIndex(isReverse);
	if(idx == -1)		{	return -1;	}

	// アイテム定義を得る
	const ItemDef* itmdef = _itmdftblRef->getDef(itmdid);
	if(itmdef == 0L)	{	return -1;	}

	// 空き生成 ID を得る
	u16 gid = getFreeGenId();
	if(gid == 0)		{	return -1;	}

	// アイテム作成
	Item* item = &_itmarr[idx];
	item->clear();
	item->setup(gid, itmdef, flags);

	// 五元素初期値がある場合は上書き設定する
	if((eestat != 0L) && !eestat->isInitialValue())
	{
		item->eleneStat()->copy(eestat);
	}

	return idx;
}

/*---------------------------------------------------------------------*//**
	アイテムを削除する
**//*---------------------------------------------------------------------*/
bool ItemManager::removeItem(u16 itmgid)
{
	s32 index = getItemIndexFromGenId(itmgid);
	if(index < 0)	{	return false;	}
	Item* item = &_itmarr[index];
	if(item == 0L)	{	return false;	}
	item->clear();

	return true;
}

/*---------------------------------------------------------------------*//**
	アイテムを投げ捨てる
**//*---------------------------------------------------------------------*/
bool ItemManager::throwItem(u16 itmgid, Map* map, const Unit* unitBy)
{
	Item* item = itemFromGenId(itmgid);
	if(item == 0L)					{	return false;	}
	if(item->getItemDef() == 0L)	{	return false;	}

	// 装備済みや編集不可は捨てられない
	if(item->isNoEdit() || item->isEquiped())
	{
		return false;
	}

	// 最大一時配置オブジェクト数チェック
	if(map->getTemporaryPlacementObjCount() >= NUM_THROW_ITEM_MAX)
	{
		return false;
	}

	// 配置情報を作成する
	PlacementObj* pobj = new PlacementObj();
	if(pobj == 0L)	{	return false;	}
	pobj->setup(
		PlacementObj::KIND_DROPITEM,
		PlacementObj::UCASE_MAP_TMP,
		(u8)(map->getAutoCountCtrlEvid() & 0xff),
		0,
		item->getItemDef()->getItemDefId(),
		map->getCurrentMapId(),
		0,
		100,
		0L,
		unitBy->getPosition()	);
	pobj->eleneStat()->copy(item->getEleneStat());

	// マップに配置する
	if(!map->addTemporaryPlacementObjSetting(pobj, true))
	{
		return false;
	}

	// 所持品からアイテムを削除
	return removeItem(itmgid);
}

/*---------------------------------------------------------------------*//**
	アイテムをマップに置く
**//*---------------------------------------------------------------------*/
bool ItemManager::dropItem(u16 itmdid, Map* map, const Vector3F* pos, const EleneStat* eestatDefault)
{
	// アイテム定義を得る
	const ItemDef* itmdef = _itmdftblRef->getDef(itmdid);
	if(itmdef == 0L)	{	return false;	}

	// 配置情報を作成する
	PlacementObj* pobj = new PlacementObj();
	if(pobj == 0L)		{	return false;	}
	pobj->setup(
		PlacementObj::KIND_DROPITEM,
		PlacementObj::UCASE_MAP_TMP,
		(u8)(map->getAutoCountCtrlEvid() & 0xff),
		0,
		itmdid,
		Game::getGame()->getMoveMap()->getCurMapId(),
		0,
		100,
		0L,
		pos	);
	if(eestatDefault != 0L)
	{
		pobj->eleneStat()->copy(eestatDefault);
	}
	else
	{
		pobj->eleneStat()->setup(itmdef->getOqlElene(), itmdef->getOqlElene());
	}

	// マップに配置する
	map->addTemporaryPlacementObjSetting(pobj, true);

	return true;
}

/*---------------------------------------------------------------------*//**
	アイテムをマップから拾う
**//*---------------------------------------------------------------------*/
bool ItemManager::pickupItem(Map* map, PlacementObj* pobj, const EleneStat* eestatDefault)
{
	ASSERT(map != 0L);	if(map == 0L)	{	return false;	}
	ASSERT(pobj != 0L);	if(pobj == 0L)	{	return false;	}

	if((eestatDefault != 0L) && eestatDefault->isInitialValue())	{ eestatDefault = 0L;	}

	// アイテム化
	if(newItem(pobj->getItemDefId(), eestatDefault, 0, false) == -1)
	{
		// 失敗
		return false;
	}

	// 成功
	map->removeTemporaryPlacementObjSetting(pobj);
	return true;
}

/*---------------------------------------------------------------------*//**
	生成 ID からアイテムインデックスを得る
	
	@return	インデックス（-1 は失敗）
**//*---------------------------------------------------------------------*/
s32 ItemManager::getItemIndexFromGenId(u16 itmgid) const
{
	for(s32 i = 0; i < _num; i++)
	{
		if(!_itmarr[i].isEmpty())
		{
			if(_itmarr[i].getItemGenId() == itmgid)
			{
				return i;
			}
		}
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	空きアイテムのインデックスを得る
	
	@return	インデックス（-1 は失敗）
**//*---------------------------------------------------------------------*/
s32 ItemManager::getEmptyItemIndex(bool isReverse)
{
	if(isReverse)	// 逆順
	{
		for(s32 i = _num - 1; i >= 0; i--)
		{
			if(_itmarr[i].isEmpty())
			{
				return i;
			}
		}
	}
	else
	{
		for(s32 i = 0; i < _num; i++)
		{
			if(_itmarr[i].isEmpty())
			{
				return i;
			}
		}
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	アイテムを検索する
	
	@return	アイテム配列
**//*---------------------------------------------------------------------*/
ItemArray ItemManager::findByItemDefId(u16 itmdid, bool isIncludeEquiped) const
{
	ItemArray iarr;

	for(s32 i = 0; i < _num; i++)
	{
		if(!_itmarr[i].isEmpty())
		{
			if(!_itmarr[i].isEquiped() || isIncludeEquiped)
			{
				if(_itmarr[i].getItemDef()->getItemDefId() == itmdid)
				{
					iarr.addItem(&_itmarr[i]);
				}
			}
		}
	}

	return iarr;
}

/*---------------------------------------------------------------------*//**
	カテゴリからアイテムを得る
**//*---------------------------------------------------------------------*/
ItemArray ItemManager::findByCategory(ItemDef::Category cat, bool isIncludeEquiped) const
{
	for(s32 i = 0; i < _num; i++)
	{
		if(!_itmarr[i].isEmpty())
		{
			if(!_itmarr[i].isEquiped() || isIncludeEquiped)
			{
				if(_itmarr[i].getItemDef()->getCategory() == cat)
				{
					return &_itmarr[i];
				}
			}
		}
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	アイテムを使用した
**//*---------------------------------------------------------------------*/
bool ItemManager::used(Item* item)
{
	if(item == 0L)	{	return false;	}
	const ItemDef* itmdf = item->getItemDef();
	if(itmdf == 0L)	{	return false;	}

	if(itmdf->getCategory() == ItemDef::CAT_STONE)	// 石
	{
		#if 1		// 初期状態（原質）の状態に戻す仕様
			const ItemDef* itmdfDefaultStone = item->getItemDef();
			if(itmdfDefaultStone != 0L)
			{
				item->setup(item->getItemGenId(), itmdfDefaultStone, 0);
				return true;
			}
		#else 		// 使用後はデフォルトの石に変化する仕様
			const ItemDef* itmdfDefaultStone = _itmdftblRef->getDef(ITEMDID_STONE_DEFAULT);
			if(itmdfDefaultStone != 0L)
			{
				item->setup(item->getItemGenId(), itmdfDefaultStone, 0);
				return true;
			}
		#endif
	}

	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ItemManager::ItemManager()
	: _itmarr(0L)
	, _num(0)
	, _itmdftblRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ItemManager::~ItemManager()
{
	ASSERT(_itmarr == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ItemManager::create(const ItemDefTbl* itmdftblRef)
{
	// アイテム配列の作成
	_num = NUM_ITEM_MAX;
	_itmarr = new Item[_num];

	// アイテム定義テーブルへの参照を保存
	_itmdftblRef = itmdftblRef;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ItemManager::destroy()
{
	if(_itmarr != 0L)
	{
		delete[] _itmarr;
		_itmarr = 0;
	}
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
u16 ItemManager::getFreeGenId() const
{
	bool isUsed;
	for(u16 gid = 1; gid <= (NUM_ITEM_MAX + 1); gid++)
	{
		isUsed = false;
		for(s32 i = 0; i < _num; i++)
		{
			if(_itmarr[i].getItemGenId() == gid)
			{
				isUsed = true;
				break;
			}
		}
		if(!isUsed)
		{
			return gid;
		}
	}
	ASSERT(false);
	return 0;
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool ItemManager::serialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		ASSERT(SaveStructure::NUM_ITEM_MAX >= _num);
		for(int i = 0; i < _num; i++)
		{
			const Item* item = &_itmarr[i];
			item->storeToStructure(&svst->_itmsvst[i]);
		}
	}
	else						// ダイレクトモード
	{
		for(int i = 0; i < _num; i++)
		{
			const Item* item = &_itmarr[i];
			item->serialize(ser);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool ItemManager::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		const SaveStructure* svst = (SaveStructure*)ser->getStructure();
		ASSERT(SaveStructure::NUM_ITEM_MAX >= _num);
		for(int i = 0; i < _num; i++)
		{
			Item* item = &_itmarr[i];
			item->restoreFromStructure(&svst->_itmsvst[i], _itmdftblRef);
		}
	}
	else						// ダイレクトモード
	{
		for(int i = 0; i < _num; i++)
		{
			Item* item = &_itmarr[i];
			item->deserialize(ser, _itmdftblRef);
		}
	}

	// バージョン間互換性調整
	if(	(ser->getMajorVersionNumber() <= sourcerer::SaveStructure::VER_MAJOR_SAVE_STRUCTURE)
		&& (ser->getMinorVersionNumber() <= sourcerer::SaveStructure::VER_MINOR_SAVE_STRUCTURE_VER131)	)
	{
		// 最新パラメータを反映する
		for(int i = 0; i < _num; i++)
		{
			_itmarr[i].reflectParameterFromDefData(true);
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
