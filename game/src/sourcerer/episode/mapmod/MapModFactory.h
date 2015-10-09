/***********************************************************************//**
 *	MapModFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_MAPMOD_MAP_MOD_FACTORY_H_
#define _SRCR_EPISODE_MAPMOD_MAP_MOD_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class MapMod;
class StoryManager;
class Game;

/*---------------------------------------------------------------------*//**
 *	ストーリー
 *
**//*---------------------------------------------------------------------*/
class MapModFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static MapMod* makeMapMod(s32 mapid);
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_EPISODE_MAPMOD_MAP_MOD_FACTORY_H_
