/***********************************************************************//**
 *	ItemEquip.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ItemEquip.h"

// Friends
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../item/Item.h"
#include "../status/EquipStat.h"
#include "../unit/UnitManager.h"
#include "../unit/item/EquipmentItemUnit.h"
#include "../episode/party/MyParty.h"

// External
#include "../../tfw/serialize/Serializer.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ItemEquip メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アイテム装備
**//*---------------------------------------------------------------------*/
void ItemEquip::equip(s32 ieqp, Item* item, u16 spid)
{
	ASSERT((0 <= ieqp) && (ieqp < EquipPlace::NUM_EQP));
	_eqstat->equipItem(ieqp, item);

	_spidarrEq[ieqp] = spid;

	// ユニット化
	unitization(ieqp, item, spid);
}

/*---------------------------------------------------------------------*//**
	ユニット化した装備品を得る
**//*---------------------------------------------------------------------*/
Unit* ItemEquip::getEquipUnit(s32 ieqp) const
{
	ASSERT((0 <= ieqp) && (ieqp < EquipPlace::NUM_EQP));
	if(_uidarrEq[ieqp] == 0)
	{
		return 0L;
	}

	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
	Unit* unit = unitmng->getUnitFromId(_uidarrEq[ieqp]);
	return unit;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ItemEquip::ItemEquip()
	: _eqstat(0L)
	, _unitOwner(0L)
{
	for(int i = 0; i < EquipPlace::NUM_EQP; i++)
	{
		_uidarrEq[i] = 0;
		_spidarrEq[i] = 0;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ItemEquip::~ItemEquip()
{
	ASSERT(_eqstat == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ItemEquip::create(Unit* unitOwner)
{
	_unitOwner = unitOwner;

	// 装備状態の作成
	_eqstat = new EquipStat();
	if(_eqstat == 0L)
	{
		return false;
	}

	// 配列の初期化
	for(int i = 0; i < EquipPlace::NUM_EQP; i++)
	{
		_uidarrEq[i] = 0;
		_spidarrEq[i] = 0;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ItemEquip::destroy()
{
	// 装備状態の削除
	if(_eqstat != 0L)
	{
		delete _eqstat;
		_eqstat = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void ItemEquip::exec(const ExecCtx* ec)
{
	// エネルギー状態の更新
	if(!((GameExecCtx*)ec)->isLockOperating())	// 操作ロック（イベント等）以外
	{
		for(int ieqp = 0; ieqp < EquipPlace::NUM_EQP; ieqp++)
		{
			Item* item = _eqstat->equipItem(ieqp);
			if(item != 0L)
			{
				item->eleneStat()->exec(ec);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool ItemEquip::serialize(Serializer* ser) const
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
bool ItemEquip::deserialize(Serializer* ser)
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
		return restoreFromStructure(&svst);
	}
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャーに保存する
**//*---------------------------------------------------------------------*/
bool ItemEquip::storeToStructure(SaveStructure* svst) const
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	_eqstat->storeToStructure(&svst->_eqssvst);
	for(int ieqp = 0; ieqp < EquipPlace::NUM_EQP; ieqp++)
	{
		svst->_spid[ieqp] = _spidarrEq[ieqp];
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャーから読み込む
**//*---------------------------------------------------------------------*/
bool ItemEquip::restoreFromStructure(const SaveStructure* svst)
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	Party* party = Game::getGame()->getMyParty();
	if(party == 0L)	{	ASSERT(party != 0L);	return false;	}
	_eqstat->restoreFromStructure(&svst->_eqssvst, party->getItemManager());

	// ユニット化
	for(int ieqp = 0; ieqp < EquipPlace::NUM_EQP; ieqp++)
	{
		_spidarrEq[ieqp] = svst->_spid[ieqp];

		Item* item = _eqstat->equipItem(ieqp);
		if(item != 0)
		{
			unitization(ieqp, item, _spidarrEq[ieqp]);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	アイテムをユニット化
	※	ソウマから装備アイテムをフォーカスできるようにユニット化する
**//*---------------------------------------------------------------------*/
void ItemEquip::unitization(s32 ieqp, Item* item, u16 spid)
{
	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);

	// 既にあるユニットを一度削除
	if(_uidarrEq[ieqp] != 0)
	{
		Unit* unit = unitmng->getUnitFromId(_uidarrEq[ieqp]);
		if(unit != 0)
		{
			ASSERT(unit->getUnitType() == UNITTYPE_EQUIPMENT_ITEM);
			// 一度削除する
			unitmng->removeUnit(unit);
		}
		_uidarrEq[ieqp] = 0;
	}

	// アイテムを外した場合はここで抜ける
	if(item == 0L)	{	return;	}

	// アイテムユニットを作成
	EquipmentItemUnit* itmu = new EquipmentItemUnit();
	ASSERT(itmu != 0L);
	itmu->create(item, _unitOwner, spid);

	// ユニットマネージャに追加
	if(unitmng->addUnit(itmu))
	{
		_uidarrEq[ieqp] = itmu->getUnitId();
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
