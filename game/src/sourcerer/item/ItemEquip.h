/***********************************************************************//**
 *	ItemEquip.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_ITEM_ITEM_EQUIP_H_
#define _SRCR_ITEM_ITEM_EQUIP_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "ItemDef.h"

#include "../status/EleneStat.h"
#include "../status/EquipPlace.h"
#include "../status/EquipStat.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class ExecCtx;
class VcString;
class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

class EquipStat;
class Item;
class EquipmentItemUnit;
class Unit;

/*---------------------------------------------------------------------*//**
 *	アイテム装備機能
 *		※	単純な状態保持のみを行う EquipStat を補って、
 *			キャラクタユニットに必要なアイテム装備処理を行う。
 *			ユニットが、このクラスのインスタンスを持つと、
 *			アイテム装備が可能になる。
 *
**//*---------------------------------------------------------------------*/
class ItemEquip
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	アイテム装備機能のセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class SaveStructure
	{
	public:
		// [  0]
		u16 _spid[EquipPlace::NUM_EQP];		// [  6] 装備箇所 SPID 配列
		u8 __pad_0010[ 2 ];					// [  2]
		// [  8]
		EquipStat::SaveStructure _eqssvst;	// [  8] 装備状態のセーブストラクチャ
		// [ 16]
		static const s32 SIZE_THIS = 16;
		SaveStructure() { TFW_ZERO_MEMORY(this, sizeof(SaveStructure)); }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 装備状態を得る
	inline const EquipStat* getEquipStat() const { return _eqstat; }
	inline EquipStat* equipStat() { return _eqstat; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void equip(s32 ieqp, Item* item, u16 spid);
	Unit* getEquipUnit(s32 ieqp) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ItemEquip();
	~ItemEquip();
	bool create(Unit* unitOwner);
	void destroy();
	void exec(const ExecCtx* ec);
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);
	bool storeToStructure(SaveStructure* svst) const;
	bool restoreFromStructure(const SaveStructure* svst);

private:
	void unitization(s32 ieqp, Item* item, u16 spid);

	//======================================================================
	// 変数
private:
	EquipStat* _eqstat;						// 装備状態
	Unit* _unitOwner;						// オーナー ユニット
	u16 _uidarrEq[EquipPlace::NUM_EQP];		// 各装備箇所ユニット化用ユニット ID 配列
	u16	_spidarrEq[EquipPlace::NUM_EQP];	// 各装備箇所 SPID 配列
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_ITEM_ITEM_H_
