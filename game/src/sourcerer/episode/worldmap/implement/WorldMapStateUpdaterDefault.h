/***********************************************************************//**
 *	WorldMapStateUpdaterDefault.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/19.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_WORLDMAP_IMPLEMENT_WORLD_MAP_SATE_UPDATER_DEFAULT_H_
#define _SRCR_EPISODE_WORLDMAP_IMPLEMENT_WORLD_MAP_SATE_UPDATER_DEFAULT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../WorldMapStateUpdater.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	Vol.1 世界地図の状態更新モジュール
 *		※	Vol.1 は途中から実装の関係上、
 *			XML イベントスクリプトによる実装がベストではなかった
 *			従って Vol.1 に限り、ハードコードプログラムによる実装を行う．
 *			以降のバージョンでは XML イベントスクリプトによる実装を行う．
 *
**//*---------------------------------------------------------------------*/
class WorldMapStateUpdaterDefault : public WorldMapStateUpdater
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	virtual void notifyBegin(WorldMap* worldmap);
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_WORLDMAP_IMPLEMENT_WORLD_MAP_SATE_UPDATER_DEFAULT_H_
