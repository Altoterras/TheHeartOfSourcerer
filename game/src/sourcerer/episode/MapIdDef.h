/***********************************************************************//**
 *	MapIdDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_MAP_ID_DEF_H_
#define _SRCR_EPISODE_MAP_ID_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	マップ ID の定義
 *
**//*---------------------------------------------------------------------*/
class MapIdDef
{
	//======================================================================
	// 定数
public:
	static const s32	MAPID_EP01_BASE_VILLAGE__START_				= 10100;
	static const s32	MAPID_EP01_BASE_VILLAGE__END_				= 10299;
	static const s32	MAPID_EP01_BASE_VILLAGE_GATE				= 10221;	// コメサット村 - 入口
	static const s32	MAPID_EP01_BASE_VILLAGE_WATER_WARD			= 10241;	// コメサット村 - 水の区（デフィン家外）
	static const s32	MAPID_EP01_BASE_VILLAGE_FIRE_WARD			= 10201;	// コメサット村 - 火の区（アジエル家外）
	static const s32	MAPID_EP01_BASE_VILLAGE_WIND_WARD			= 10223;	// コメサット村 - 風の区（フォグ家外）
	static const s32	MAPID_EP01_BASE_VILLAGE_HOME				= 10242;	// アサ自宅
	static const s32	MAPID_EP01_BASE_VILLAGE_WATER_MAIN_STREET	= 10243;	// 水の区主参道
	static const s32	MAPID_EP01_BASE_VILLAGE_IDO_HUT				= 10244;	// 井戸舎（アイテム合成場）

	static const s32	MAPID_EP01_BASE_VILLAGE_TEMPLE__START_		= 10101;
	static const s32	MAPID_EP01_BASE_VILLAGE_TEMPLE__END_		= 10105;
	static const s32	MAPID_EP01_BASE_VILLAGE_TEMPLE_HERMIT_ROOM	= 10101;	// ノバイ寺院 - ハーミットの部屋
	static const s32	MAPID_EP01_BASE_VILLAGE_TEMPLE_CORRIDOR		= 10102;	// ノバイ寺院 - 回廊
	static const s32	MAPID_EP01_BASE_VILLAGE_TEMPLE_BRIDGE		= 10181;	// ノバイ寺院前の橋

	static const s32	MAPID_EP01_FOREST4_ENTRANCE_SQUARE			= 11101;	// シネンの森 - 入口広場
	static const s32	MAPID_EP01_FOREST4_EXIT_SQUARE				= 11104;	// シネンの森 - 出口広場
	static const s32	MAPID_EP01_FOREST4_HERB_GARDEN				= 11111;	// シネンの森 - ハーブ畑
	static const s32	MAPID_EP01_FOREST4_BLOSSOMS					= 11112;	// シネンの森 - セルジャの桜
	static const s32	MAPID_EP01_FOREST4_ROCK_POND				= 11113;	// シネンの森 - 雲月岩の池
	static const s32	MAPID_EP01_FOREST4_SECRET					= 11114;	// シネンの森 - 闘いの洞窟入口

	static const s32	MAPID_EVENT_DYMMY							= 11891;	// イベント用ダミーマップ

	// マップ ID が拠点の村（コメサット村）かどうかを得る
	static inline bool isBaseVillage(s32 mapid)
	{
		return (MAPID_EP01_BASE_VILLAGE__START_ <= mapid) && (mapid <= MAPID_EP01_BASE_VILLAGE__END_);
	}

	// マップ ID が拠点の村（コメサット村）の寺院かどうかを得る
	static inline bool isBaseVillageTemple(s32 mapid)
	{
		return (MAPID_EP01_BASE_VILLAGE_TEMPLE__START_ <= mapid) && (mapid <= MAPID_EP01_BASE_VILLAGE_TEMPLE__END_);
	}
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_STORY_ID_DEF_H_
