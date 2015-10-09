/***********************************************************************//**
 *	ItemManager.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_ITEM_ITEM_MANAGER_H_
#define _SRCR_ITEM_ITEM_MANAGER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "ItemDef.h"
#include "ItemArray.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

class EleneStat;
class Item;
class ItemDefTbl;
class Map;
class PlacementObj;
class Unit;

/*---------------------------------------------------------------------*//**
 *	アイテム マネージャ
 *		※	固定長のアイテムリスト
 *			固定長なのはメモリ使用量による動作不確定要素を緩和するため．
 *			（アイテムなどは論理的要素のため、表示物などに比べて、
 *			　メモリ確保できなかったときのダメージがでかい．）
 *			同様のリストに SourceManager がある．
 *
**//*---------------------------------------------------------------------*/
class ItemManager
{
	//======================================================================
	// 定数
protected:
	static const	s32		NUM_ITEM_MAX			= 100;
	static const	s32		NUM_THROW_ITEM_MAX		= 4;

	static const	u16		ITEMDID_STONE_DEFAULT	= 2001;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// アイテムの数を得る
	inline s32 getItemNum() const		{	return _num;	}
	// アイテムを得る
	const Item* getItem(s32 index) const;
	Item* item(s32 index) const;
	const Item* getItemFromGenId(u16 itmgid) const;
	Item* itemFromGenId(u16 itmgid) const;
	// アイテム定義テーブルを得る
	inline const ItemDefTbl* getItemDefTableRef() const { return _itmdftblRef; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	s32 newItem(u16 itmdid) { return newItem(itmdid, 0L, 0, false); }
	s32 newItem(u16 itmdid, const EleneStat* eestat, u32 flags, bool isReverse);
	bool removeItem(u16 itmgid);
	bool throwItem(u16 itmgid, Map* map, const Unit* unitBy);
	bool dropItem(u16 itmdid, Map* map, const Vector3F* pos, const EleneStat* eestatDefault);
	bool pickupItem(Map* map, PlacementObj* pobj, const EleneStat* eestatDefault);

	s32 getItemIndexFromGenId(u16 itmgid) const;
	s32 getEmptyItemIndex() { return getEmptyItemIndex(false); }
	s32 getEmptyItemIndex(bool isReverse);

	ItemArray findByItemDefId(u16 itmdid, bool isIncludeEquiped) const;
	ItemArray findByCategory(ItemDef::Category cat, bool isIncludeEquiped) const;

	bool used(Item* item);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ItemManager();
	~ItemManager();
	bool create(const ItemDefTbl* itmdftblRef);
	void destroy();

	bool serialize(Serializer* ser);
	bool deserialize(Serializer* ser);

private:
	u16 getFreeGenId() const;

	//======================================================================
	// 変数
private:
	Item* _itmarr;
	s32	_num;

	const ItemDefTbl* _itmdftblRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_ITEM_ITEM_MANAGER_H_
