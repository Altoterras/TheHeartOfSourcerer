/***********************************************************************//**
 *	MapModFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

#include "MapModFactory.h"
// Self

// Friends
#include "implement/MapMod10181.h"
#include "implement/MapMod10221.h"
#include "implement/MapMod11104.h"
#include "implement/MapMod19102.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapModFactory メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ストーリー作成
**//*---------------------------------------------------------------------*/
MapMod* MapModFactory::makeMapMod(s32 mapid)
{
	MapMod* mapmod = 0L;

	switch(mapid)
	{
	case 10181:	mapmod = new MapMod10181();	break;
	case 10221:	mapmod = new MapMod10221();	break;
	case 11104:	mapmod = new MapMod11104();	break;
	case 19102:	mapmod = new MapMod19102(); break;
	default:	mapmod = new MapMod10000();	break;
	}

	if(!mapmod->create())
	{
		delete mapmod;
		return 0L;
	}

	return mapmod;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
