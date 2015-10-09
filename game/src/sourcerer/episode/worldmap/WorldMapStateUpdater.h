/***********************************************************************//**
 *	WorldMapStateUpdater.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/10/20.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_WORLDMAP_WORLD_MAP_STATE_UPDATER_H_
#define _SRCR_EPISODE_WORLDMAP_WORLD_MAP_STATE_UPDATER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class Game;
class WorldMap;

/*---------------------------------------------------------------------*//**
 *	世界地図の状態更新モジュール
 *
**//*---------------------------------------------------------------------*/
class WorldMapStateUpdater
{
	//======================================================================
	// 定数

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	WorldMapStateUpdater() {}
	virtual ~WorldMapStateUpdater() {}
	virtual bool create() { return true; }
	virtual void destroy() {}
	virtual void notifyBegin(WorldMap* worldmap) {}

	//======================================================================
	// 変数
private:

};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_WORLDMAP_WORLD_MAP_STATE_UPDATER_H_
