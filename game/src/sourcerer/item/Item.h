/***********************************************************************//**
 *	Item.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_ITEM_ITEM_H_
#define _SRCR_ITEM_ITEM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "ItemDef.h"

#include "../status/EleneStat.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Serializer;
class VcString;

TFW_END_NS

SRCR_BEGIN_NS

class ItemDef;
class ItemDefTbl;

/*---------------------------------------------------------------------*//**
 *	アイテム
 *		※	アイテムオブジェクトは、Unit 等とは違って動的に生成されない．
 *			アイテムマネージャによって予め個定数用意され、
 *			その生成済みオブジェクトを使いまわす．
 *			そのため、アイテムマネージャの範囲を超えては存在しずらい．
 *			将来的にはこの仕様は改められるかもしれない．
 *
**//*---------------------------------------------------------------------*/
class Item
{
	//======================================================================
	// 定数
public:
	// フラグ
	static const u32	F_NOEDIT	= 0x00000001;		// 編集不可
	static const u32	F_EQUIPED	= 0x00000002;		// 装備済み

	// 石アイテムに許される元素オーバー率
	static const s32	RATE_STONE_ELENE_OVER_MAX	= 10;

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	アイテムのセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class SaveStructure
	{
	public:
		// [  0]
		u16 _itmdid;							// [  2] アイテム定義 ID
		u16 _itmgid;							// [  2] アイテム生成 ID
		u16 _flags;								// [  2] フラグ
		u8 __pad_0010[ 2];						// [  2]
		// [  8]
		EleneStat::SaveStructure _eestat;		// [ 96] 元素エネルギー状態
		// [104]
		static const s32 SIZE_THIS = 104;
		SaveStructure() { TFW_ZERO_MEMORY(this, sizeof(SaveStructure)); }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 生成 ID を得る
	inline u16 getItemGenId() const { return _itmgid; }
	// 元素エネルギーを得る
	virtual const EleneStat* getEleneStat() const {	return &_eestat; }
	virtual EleneStat* eleneStat() { return &_eestat; }
	// アイテム定義を得る
	inline const ItemDef* getItemDef() const { return _itmdf; }
	// 編集不可かを得る
	inline bool isNoEdit() const { return TFW_IS_FLAG(_flags, F_NOEDIT); }
	// 装備済みかを得る
	inline bool isEquiped() const { return TFW_IS_FLAG(_flags, F_EQUIPED); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool isEmpty() const;

	void setEquiped(bool isEquiped) { TFW_SET_FLAG(_flags, F_EQUIPED, isEquiped);	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Item();
	~Item();
	void clear();
	void copy(const Item* src);
	void setup(u16 itmgid, const ItemDef* itmdf, u16 flags);
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser, const ItemDefTbl* itmdftbl);
	bool storeToStructure(SaveStructure* svst) const;
	bool restoreFromStructure(const SaveStructure* svst, const ItemDefTbl* itmdftbl);
	void reflectParameterFromDefData(bool isKeepVeil);

	//======================================================================
	// 変数
private:
	u16 _itmgid;			// アイテム生成 ID
	u16 _flags;				// フラグ
	EleneStat _eestat;		// 元素エネルギー状態
	const ItemDef* _itmdf;	// アイテム定義
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_ITEM_ITEM_H_
