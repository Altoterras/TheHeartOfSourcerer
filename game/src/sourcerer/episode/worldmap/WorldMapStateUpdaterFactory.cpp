/***********************************************************************//**
 *	WorldMapStateUpdaterFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/19.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

#include "WorldMapStateUpdaterFactory.h"
// Self

// Friends
#include "implement/WorldMapStateUpdaterDefault.h"
#include "implement/WorldMapStateUpdaterVol01.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// WorldMapStateUpdaterFactory メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	アップデーター作成
**//*---------------------------------------------------------------------*/
WorldMapStateUpdater* WorldMapStateUpdaterFactory::makeUpdater()
{
	WorldMapStateUpdater* wmstupdr = 0L;

	#if defined(_ENLIGHT_V01)
	{
		// Vol.1 専用の世界地図状態アップデータを作成する
		wmstupdr = new WorldMapStateUpdaterVol01();
	}
	#else
		wmstupdr = new WorldMapStateUpdaterDefault();
	#endif

	if(wmstupdr == 0L)
	{
		return 0L;
	}

	if(!wmstupdr->create())
	{
		delete wmstupdr;
		wmstupdr = 0L;
	}

	return wmstupdr;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
