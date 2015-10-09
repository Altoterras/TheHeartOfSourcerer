/***********************************************************************//**
 *	EquipStat.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/06/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_EQUIP_STAT_H_
#define _SRCR_STATUS_EQUIP_STAT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "EquipPlace.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

class Item;
class ItemManager;

/*---------------------------------------------------------------------*//**
 *	装備状態
 *		※	現状ではアイテムだけを対象としているが、
 *			このモジュール自体ではアイテムのみと決定的に言及はしない。
 *			高度な処理は、ItemEquip などに委譲し、
 *			このクラスは状態保持のみを目的とするものである。
 *
**//*---------------------------------------------------------------------*/
class EquipStat
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	装備状態のセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class SaveStructure
	{
	public:
		// [  0]
		u16 _itmgid[EquipPlace::NUM_EQP];	// [  6] アイテム生成 ID
		u8 __pad_0010[ 2 ];					// [  2]
		// [  8]
		static const s32 SIZE_THIS = 8;
		SaveStructure() { TFW_ZERO_MEMORY(this, sizeof(SaveStructure)); }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 装備アイテムを得る
	inline const Item* getEquipItem(s32 ieqp) const { ASSERT((0 <= ieqp) && (ieqp < EquipPlace::NUM_EQP)); return _itemEq[ieqp]; }
	inline Item* equipItem(s32 ieqp) { ASSERT((0 <= ieqp) && (ieqp < EquipPlace::NUM_EQP)); return _itemEq[ieqp]; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void equipItem(s32 ieqp, Item* item);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EquipStat()						{	clear();	}
	EquipStat(const EquipStat& src)	{	copy(&src);	}

	void copy(const EquipStat* src)
	{
		for(int i = 0; i < EquipPlace::NUM_EQP; i++)
		{
			_itemEq[i] = src->_itemEq[i];
		}
	}
	EquipStat& operator =(const EquipStat& src) { copy(&src); return *this; }

	void clear()
	{
		for(int i = 0; i < EquipPlace::NUM_EQP; i++)
		{
			_itemEq[i] = 0L;
		}
	}

	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser, ItemManager* itmmng);
	bool storeToStructure(SaveStructure* svst) const;
	bool restoreFromStructure(const SaveStructure* svst, ItemManager* itmmng);

	//======================================================================
	// 変数
private:
	Item* _itemEq[EquipPlace::NUM_EQP];	// 装備アイテム
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_EQUIP_STAT_H_
