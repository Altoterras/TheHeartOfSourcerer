/***********************************************************************//**
 *	MapConfig.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/04.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAP_MAP_CONFIG_H_
#define _SRCR_MAP_MAP_CONFIG_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../status/EleneStat.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

class Game;
class Map;
class PlacementObj;

/*---------------------------------------------------------------------*//**
 *	マップ
 *
**//*---------------------------------------------------------------------*/
class MapConfig
{
	//======================================================================
	// 定数
public:
	static const s32 NUM_MOVE_MAPID	= 8;

	static const u8 MAPKIND_NULL = 0;
	static const u8 MAPKIND_FIELD = 1;		// 屋外
	static const u8 MAPKIND_INDOOR = 2;		// 屋内

	//======================================================================
	// メソッド
public:
	MapConfig();
	~MapConfig();
	void clear();
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);

	void addPlacementObjSetting(PlacementObj* pobjEntr);
	void placePlacementObjToMap(Map* map, bool isDeserializeCase);

	//======================================================================
	// 変数
public:
	u8 _mapkind;						// マップ種別
	s16 _bgmid;							// BGM ID
	f32 _volumeBgm;						// BGM ボリューム
	s32 _evidStartup;					// 開始イベント ID
	s32 _mapidMove[NUM_MOVE_MAPID];		// マップ移動設定
	f32 _xCamMoveMin;					// カメラ最小移動 X 座標
	f32 _xCamMoveMax;					// カメラ最大移動 X 座標
	f32 _zCamMoveMin;					// カメラ最小移動 Z 座標
	f32 _zCamMoveMax;					// カメラ最大移動 Z 座標
	s32	_fogmode;						// フォグモード
	f32 _fogdensity;					// フォグ強さ
	f32 _fogstart;						// フォグ開始距離
	f32 _fogend;						// フォグ終了距離
	f32 _fogcolor[4];					// フォグ色
	EleneStat _eestatEff;				// 影響する元素エネルギー

private:
	List<PlacementObj*>* _listPlaceObj;	// 配置オブジェクトリスト
	u16 _ctrlevidAutoCnt;				// 配置オブジェクト用自動割当制御イベント ID カウンタ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAP_MAP_CONFIG_H_
