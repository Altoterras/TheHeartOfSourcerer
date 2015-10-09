/***********************************************************************//**
 *	UnitManager.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "UnitManager.h"

// Friends
#include "Unit.h"
#include "char/CharUnit.h"
#include "char/CharUnitSaveStructure.h"
#include "item/FieldItemUnit.h"
#include "../char/CharDef.h"
#include "../episode/save/SaveStructure.h"

// External
#include "../../tfw/collection/Array.h"
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// UnitManager メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	ユニット数を得る
**//*---------------------------------------------------------------------*/
s32 UnitManager::getUnitNum() const
{
	return _arrUnit->getCount();
}

/*---------------------------------------------------------------------*//**
	ユニットを得る
**//*---------------------------------------------------------------------*/
Unit* UnitManager::getUnitFromId(u16 unitid) const
{
	for(u32 i = 0; i < _arrUnit->getCount(); i++)
	{
		Unit* unitWk = _arrUnit->getAt(i);
		if(unitWk->getUnitId() == unitid)
		{
			return unitWk;
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	ユニットをインデックスから得る
**//*---------------------------------------------------------------------*/
Unit* UnitManager::getUnitFromIndex(s32 index) const
{
	ASSERT((0 <= index) && ((u32)index < _arrUnit->getCount()));
	return _arrUnit->getAt(index);
}

/*---------------------------------------------------------------------*//**
	自キャラユニットを得る
**//*---------------------------------------------------------------------*/
Unit* UnitManager::getMyCharUnit() const
{
	if(_idxMyChar < 0)	{	return 0L;	}
	ASSERT((0 <= _idxMyChar) && ((u32)_idxMyChar < _arrUnit->getCount()));
	return _arrUnit->getAt(_idxMyChar);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ユニットの追加
	
	@return	ユニット ID
**//*---------------------------------------------------------------------*/
bool UnitManager::addUnit(Unit* unitEntr)
{
	u32 index = _arrUnit->getCount();
	if(_arrUnit->add(unitEntr))
	{
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	メインキャラユニットの追加
	
	@return	ユニット ID
**//*---------------------------------------------------------------------*/
bool UnitManager::addMyCharUnit(Unit* unitEntr)
{
	int index = _arrUnit->getCount();
	if(_arrUnit->add(unitEntr))
	{
		_idxMyChar = index;
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	ユニットの削除
**//*---------------------------------------------------------------------*/
void UnitManager::removeUnit(Unit* unit)
{
	Unit* unitMyChar = getMyCharUnit();

	for(u32 i = 0; i < _arrUnit->getCount(); i++)
	{
		Unit* unitWk = _arrUnit->getAt(i);
		if(unitWk == unit)
		{
			unitWk->destroy();
			_arrUnit->remove(i);
			break;
		}
	}

	// _idxMyChar の更新
	if(unitMyChar != 0L)
	{
		for(u32 i = 0; i < _arrUnit->getCount(); i++)
		{
			if(_arrUnit->getAt(i) == unitMyChar)
			{
				_idxMyChar = i;
				break;
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	全てのユニットの削除
**//*---------------------------------------------------------------------*/
void UnitManager::removeAllUnit()
{
	for(u32 i = 0; i < _arrUnit->getCount(); i++)
	{
		Unit* unitWk = _arrUnit->getAt(i);
		unitWk->destroy();
	}
	_arrUnit->removeAll(false);
	_idxMyChar = -1;
}

/*---------------------------------------------------------------------*//**
	制御用イベント ID からキャラユニットを探す
**//*---------------------------------------------------------------------*/
Unit* UnitManager::findUnitFromCtrlEvid(u16 evidCtrl) const
{
	if(evidCtrl == 0)	{	return 0L;	}
	for(u32 i = 0; i < _arrUnit->getCount(); i++)
	{
		Unit* unitWk = _arrUnit->getAt(i);
		if(unitWk->getCtrlEvid() == evidCtrl)
		{
			return (CharUnit*)unitWk;
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	キャラクター ID からキャラユニットを探す
**//*---------------------------------------------------------------------*/
CharUnit* UnitManager::findCharUnitFromCharId(u16 chrdid) const
{
	if(chrdid == 0)		{	return 0L;	}
	for(u32 i = 0; i < _arrUnit->getCount(); i++)
	{
		Unit* unitWk = _arrUnit->getAt(i);
		if(unitWk->isCategory(Unit::UNITCAT_CHAR))
		{
			const CharDef* chrdef = ((CharUnit*)unitWk)->getCharDef();
			if((chrdef != 0L) && (chrdef->getCharDefId() == chrdid))
			{
				return (CharUnit*)unitWk;
			}
		}
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	制御用イベント ID もしくはキャラクター ID からキャラユニットを探す
	※ 両方指定の場合は制御用イベント ID を優先する
**//*---------------------------------------------------------------------*/
CharUnit* UnitManager::findCharUnitFromCtrlEvidOrCharId(u16 evidCtrl, u16 chrdid) const
{
	Unit* unitFind = findUnitFromCtrlEvid(evidCtrl);
	if((unitFind != 0L) && unitFind->isCategory(Unit::UNITCAT_CHAR))
	{
		return (CharUnit*)unitFind;
	}
	return findCharUnitFromCharId(chrdid);
}

/*---------------------------------------------------------------------*//**
	現在のマップに有効な敵がいるかどうかチェックする

	@retval true 有効な敵がいる
	@retval false 有効な敵がいない
**//*---------------------------------------------------------------------*/
bool UnitManager::checkEnableEnemy() const
{
	for(u32 iunit = 0; iunit < _arrUnit->getCount(); iunit++)
	{
		const Unit* unit = _arrUnit->getAt(iunit);

		// NPC の状態保存
		if(unit->getUnitType() == UNITTYPE_ENEMY_CHAR)
		{
			if(unit->isEnable())	{	return true;	}
		}
	}

	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
UnitManager::UnitManager()
	: _arrUnit(0L)
	, _idxMyChar(-1)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
UnitManager::~UnitManager()
{
	delete _arrUnit;
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool UnitManager::create()
{
	_arrUnit = new Array<Unit*>(true, true);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void UnitManager::destroy()
{
	if(_arrUnit != 0L)
	{
		for(u32 i = 0; i < _arrUnit->getCount(); i++)
		{
			_arrUnit->getAt(i)->destroy();
		}
		delete _arrUnit;
		_arrUnit = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void UnitManager::exec(ExecRes* res, const ExecCtx* ec)
{
	for(u32 i = 0; i < _arrUnit->getCount(); i++)
	{
		_arrUnit->getAt(i)->exec(res, ec);
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void UnitManager::render(const RenderCtx* rc)
{
	for(u32 i = 0; i < _arrUnit->getCount(); i++)
	{
		_arrUnit->getAt(i)->render(rc);
	}
}

/*---------------------------------------------------------------------*//**
	マップ変更前通知
**//*---------------------------------------------------------------------*/
void UnitManager::notifyChangeMapBefore()
{
	for(u32 i = 0; i < _arrUnit->getCount(); i++)
	{
		_arrUnit->getAt(i)->notifyChangeMapBefore();
	}
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void UnitManager::notifyChangedMap()
{
	for(u32 i = 0; i < _arrUnit->getCount(); i++)
	{
		_arrUnit->getAt(i)->notifyChangedMap();
	}
}

/*---------------------------------------------------------------------*//**
	イベントモード変更通知
**//*---------------------------------------------------------------------*/
void UnitManager::notifyChangedEventMode(bool isEventMode)
{
	for(u32 i = 0; i < _arrUnit->getCount(); i++)
	{
		_arrUnit->getAt(i)->notifyChangedEventMode(isEventMode);
	}
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool UnitManager::serialize(Serializer* ser) const
{
	s32 idxSvst = 0;
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		for(s32 ist = 0; ist < SaveStructure::NUM_NPC_MAX; ist++)
		{
			if(svst->_cusvstNpc[ist]._ctrlevid == 0)	// 空きを発見
			{
				idxSvst = ist;
				break;
			}
		}
	}
	else						// ダイレクトモード
	{
		// ダイレクトモード時は NPC の数をまず保存する
		s32 numNpc = 0;
		s32 numFitm = 0;
		for(u32 iunit = 0; iunit < _arrUnit->getCount(); iunit++)
		{
			if(TFW_IS_FLAG(_arrUnit->getAt(iunit)->getUnitFlags(), Unit::UNITFLAG_NPC))
			{
				numNpc++;
			}
			if(_arrUnit->getAt(iunit)->getUnitType() == UNITTYPE_FIELD_ITEM)
			{
				numFitm++;
			}
		}
		ser->store(&numNpc);
		ser->store(&numFitm);
	}

	for(u32 iunit = 0; iunit < _arrUnit->getCount(); iunit++)
	{
		const Unit* unit = _arrUnit->getAt(iunit);

		// NPC の状態保存
		if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_NPC))
		{
			const CharUnit* cunit = (const CharUnit*)unit;
			u16 ctrlevid = cunit->getCtrlEvid();
			if(ctrlevid != 0L)
			{
				if(ser->isStructureMode())	// ストラクチャモード
				{
					SaveStructure* svst = (SaveStructure*)ser->getStructure();
					cunit->storeToStructure(&svst->_cusvstNpc[idxSvst]);
					ASSERT(svst->_cusvstNpc[idxSvst]._ctrlevid != 0);
					idxSvst++;
				}
				else						// ダイレクトモード
				{
					CharUnitSaveStructure cusvst;
					cunit->storeToStructure(&cusvst);
					ASSERT(cusvst._ctrlevid != 0);
					ser->store(&cusvst, sizeof(CharUnitSaveStructure));
				}
			}
		}
		// フィールドアイテムの状態保存
		else if(_arrUnit->getAt(iunit)->getUnitType() == UNITTYPE_FIELD_ITEM)
		{
			const FieldItemUnit* fiunit = (const FieldItemUnit*)unit;
			u16 ctrlevid = fiunit->getCtrlEvid();
			if(ctrlevid != 0L)
			{
				if(ser->isStructureMode())	// ストラクチャモード
				{
					SaveStructure* svst = (SaveStructure*)ser->getStructure();
					fiunit->storeToStructure(&svst->_cusvstNpc[idxSvst]);
					ASSERT(svst->_cusvstNpc[idxSvst]._ctrlevid != 0);
					idxSvst++;
				}
				else						// ダイレクトモード
				{
					CharUnitSaveStructure cusvst;
					fiunit->storeToStructure(&cusvst);
					ASSERT(cusvst._ctrlevid != 0);
					ser->store(&cusvst, sizeof(CharUnitSaveStructure));
				}
			}
		}
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool UnitManager::deserialize(Serializer* ser)
{
	s32 numNpc = 0;
	s32 numFitm = 0;
	if(!ser->isStructureMode())	// ダイレクトモード
	{
		ser->restore(&numNpc);
		ser->restore(&numFitm);
	}

	for(s32 ist = 0; ist < SaveStructure::NUM_NPC_MAX; ist++)
	{
		// NPC の状態セーブストラクチャを読み込む
		CharUnitSaveStructure cusvst;
		if(ser->isStructureMode())	// ストラクチャモード
		{
			const SaveStructure* svst = (const SaveStructure*)ser->getStructure();
			TFW_COPY_MEMORY(&cusvst, &svst->_cusvstNpc[ist], sizeof(CharUnitSaveStructure));
		}
		else						// ダイレクトモード
		{
			if(numNpc > 0)			{	numNpc--;	}
			else if(numFitm > 0)	{	numFitm--;	}
			else					{	break;		}
			ser->restore(&cusvst, sizeof(CharUnitSaveStructure));
			ASSERT(cusvst._ctrlevid != 0);
		}

		if(cusvst._ctrlevid != 0)
		{
			for(u32 iunit = 0; iunit < _arrUnit->getCount(); iunit++)
			{
				Unit* unit = _arrUnit->getAt(iunit);
				u16 ctrlevidUnit = unit->getCtrlEvid();
				if((ctrlevidUnit != 0L) && (ctrlevidUnit == cusvst._ctrlevid))
				{
					// NPC の状態復元
					if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_NPC))
					{
						CharUnit* cunit = (CharUnit*)unit;
						cunit->restoreFromStructure(&cusvst);

						// バージョン間互換性調整
						if(	(ser->getMajorVersionNumber() <= sourcerer::SaveStructure::VER_MAJOR_SAVE_STRUCTURE)
							&& (ser->getMinorVersionNumber() <= sourcerer::SaveStructure::VER_MINOR_SAVE_STRUCTURE_VER131)	)
						{
							// 最新パラメータを反映する
							cunit->reflectParameterFromDefData(cunit->getUnitFlags());
						}

						break;
					}
					// フィールドアイテムの状態復元
					else if(_arrUnit->getAt(iunit)->getUnitType() == UNITTYPE_FIELD_ITEM)
					{
						FieldItemUnit* fiunit = (FieldItemUnit*)unit;
						fiunit->restoreFromStructure(&cusvst);

						// バージョン間互換性調整
						if(	(ser->getMajorVersionNumber() <= sourcerer::SaveStructure::VER_MAJOR_SAVE_STRUCTURE)
							&& (ser->getMinorVersionNumber() <= sourcerer::SaveStructure::VER_MINOR_SAVE_STRUCTURE_VER131)	)
						{
							// 最新パラメータを反映する
							fiunit->reflectParameterFromDefData(true);
						}

						break;
					}
				}
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
