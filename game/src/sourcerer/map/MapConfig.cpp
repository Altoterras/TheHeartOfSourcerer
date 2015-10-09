/***********************************************************************//**
 *	MapConfig.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/04.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MapConfig.h"

// Friends
#include "Map.h"
#include "PlacementObj.h"
#include "../episode/save/SaveStructure.h"
#include "../event/EventIdDef.h"

// External
#include "../../tfw/collection/List.h"
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapConfig 静的メンバ変数

//==========================================================================
// MapConfig メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MapConfig::MapConfig()
{
	_listPlaceObj = new List<PlacementObj*>(true);
	clear();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MapConfig::~MapConfig()
{
	delete _listPlaceObj;
}

/*---------------------------------------------------------------------*//**
	設定のクリア
**//*---------------------------------------------------------------------*/
void MapConfig::clear()
{
	_mapkind = MAPKIND_NULL;
	_bgmid = -1;
	_volumeBgm = 0.0f;
	_evidStartup = 0L;
	_xCamMoveMin = 0.0f;
	_xCamMoveMax = 0.0f;
	_zCamMoveMin = 0.0f;
	_zCamMoveMax = 0.0f;
	_fogmode = 0;
	_fogdensity = 0.0f;
	_fogstart = 0.0f;
	_fogend = 0.0f;
	_eestatEff.clear();

	for(int i = 0; i < NUM_MOVE_MAPID; i++)
	{
		_mapidMove[i] = 0;
	}

	TFW_ZERO_MEMORY(_fogcolor, sizeof(f32) * 4);

	_listPlaceObj->removeAll();
	_ctrlevidAutoCnt = EventIdDef::CTRLEVID_MAPCONF__START_;	// 自動割当カウンタリセット
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool MapConfig::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		// 配置オブジェクトの状態を保存する
		if(!svst->storePlacementObjData(_listPlaceObj))
		{
			return false;
		}
	}
	else						// ダイレクトモード
	{
		// 配置オブジェクトの状態を保存する
		s32 numPobj = _listPlaceObj->getCount();
		ser->store(&numPobj);
		for(ListIterator<PlacementObj*> it = _listPlaceObj->iterator(); it.has(); it.next())
		{
			const PlacementObj* pobj = it.object();
			pobj->serialize(ser);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool MapConfig::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		// 配置オブジェクトの状態を読み込む
		if(!svst->restorePlacementObjData(_listPlaceObj))
		{
			return false;
		}
	}
	else						// ダイレクトモード
	{
		// 配置オブジェクトの状態を読み込む
		s32 numPobj = 0;
		ser->restore(&numPobj);
		ASSERT(_listPlaceObj->getCount() == numPobj);
		ListIterator<PlacementObj*> it = _listPlaceObj->iterator();
		for(int i = 0; i < numPobj; i++)
		{
			if(!it.has())
			{
				ASSERT(false);
				return false;
			}
			PlacementObj* pobj = it.object();
			pobj->deserialize(ser);
			it.next();
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	配置オブジェクトの配置
**//*---------------------------------------------------------------------*/
void MapConfig::placePlacementObjToMap(Map* map, bool isDeserializeCase)
{
	// 配置オブジェクトの配置
	for(ListIterator<PlacementObj*> it = _listPlaceObj->iterator(); it.has(); it.next())
	{
		PlacementObj* pobj = it.object();
		if(isDeserializeCase)	// デシリアライズ（セーブデータ読み込み）時
		{
			if(pobj->isPlaced())	// 配置されていた
			{
				map->placePlacementObj(pobj, true);	// 強制配置
			}
		}
		else
		{
			if(pobj->isNewPlacedCondition())
			{
				map->placePlacementObj(pobj, false);	// マップに配置
			}
			else
			{
				map->unplacePlacementObj(pobj);			// 配置解除
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	配置オブジェクト設定の追加
**//*---------------------------------------------------------------------*/
void MapConfig::addPlacementObjSetting(PlacementObj* pobjEntr)
{
	// 自動割当制御イベント ID
	if(pobjEntr->getCtrlEvid() == 0)
	{
		pobjEntr->setCtrlEvid(_ctrlevidAutoCnt);
		_ctrlevidAutoCnt++;
		ASSERT(_ctrlevidAutoCnt <= EventIdDef::CTRLEVID_MAPCONF__END_);
	}

	// リストに追加
	ASSERT(_listPlaceObj != 0L);
	_listPlaceObj->addTail(pobjEntr);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
