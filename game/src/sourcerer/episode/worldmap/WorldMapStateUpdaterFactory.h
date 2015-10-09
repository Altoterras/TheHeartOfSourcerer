/***********************************************************************//**
 *	WorldMapStateUpdaterFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/19.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_WORLDMAP_WORLD_MAP_SATE_UPDATER_FACTORY_H_
#define _SRCR_EPISODE_WORLDMAP_WORLD_MAP_SATE_UPDATER_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class WorldMapStateUpdater;
class Game;

/*---------------------------------------------------------------------*//**
 *	ストーリー専用モジュール ファクトリー
 *
**//*---------------------------------------------------------------------*/
class WorldMapStateUpdaterFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static WorldMapStateUpdater* makeUpdater();
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_WORLDMAP_WORLD_MAP_SATE_UPDATER_FACTORY_H_
