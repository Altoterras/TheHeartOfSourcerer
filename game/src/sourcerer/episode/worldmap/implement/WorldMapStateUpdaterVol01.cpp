/***********************************************************************//**
 *	WorldMapStateUpdaterVol01.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/14.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "WorldMapStateUpdaterVol01.h"

// Friends
#include "../WorldMap.h"
#include "../../MapIdDef.h"
#include "../../StDepEvDef.h"
#include "../../StoryManager.h"
#include "../../party/MyParty.h"
#include "../../save/SaveIdDef.h"
#include "../../../body/Game.h"
#include "../../../save/SaveData.h"
#include "../../../save/SaveSys.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// WorldMapStateUpdaterVol01 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	ワールドマップ開始通知
**//*---------------------------------------------------------------------*/
void WorldMapStateUpdaterVol01::notifyBegin(WorldMap* worldmap)
{
	StoryManager* strymng = Game::getGame()->getStoryManager(); ASSERT(strymng != 0L);
	SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
	SaveData* svdat = (svsys != 0L) ? svsys->getSaveData() : 0L; ASSERT(svdat != 0L);
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);

	if((strymng == 0L) || (svdat == 0L)) { return; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// ストーリーによるスポット更新

	// ストーリースポットを一度クリアする
	worldmap->clearSpot(WorldMap::SPOTKIND_MAINEV);
	worldmap->clearSpot(WorldMap::SPOTKIND_SUBEV);

	s32 stryid = strymng->getCurrentStoryId();
	switch(stryid)
	{
	case 10101:
		// ◆ メインイベント
		if(svdat->getSaveF(SaveIdDef::SFID_ST10101_MEET_CORPO))
		{
			// シネンの森の入口へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_FOREST4_ENTRANCE_SQUARE, WorldMap::SPOTKIND_MAINEV, true);
		}
		else	// コルポと会ってない
		{
			// 寺院入口の橋へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_TEMPLE_BRIDGE, WorldMap::SPOTKIND_MAINEV, true);
		}
		break;
	case 10201:
		// ◆ メインイベント
		// シネンの森の池へ
		worldmap->markSpot(MapIdDef::MAPID_EP01_FOREST4_ROCK_POND, WorldMap::SPOTKIND_MAINEV, true);
		break;
	case 10202:
		// ◆ メインイベント
		ASSERT(party != 0L);
		if(	party->isEnablePc(MyParty::PC_MY) &&
			!party->isEnablePc(MyParty::PC_PTN) &&
			!party->isEnablePc(MyParty::PC_HLP) )
		{
			// シネンの森の池へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_FOREST4_ROCK_POND, WorldMap::SPOTKIND_MAINEV, true);
		}
		else	// 自宅で休む必要あり
		{
			// コメサット村の水の区へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_WATER_WARD, WorldMap::SPOTKIND_MAINEV, true);
		}
		break;
	case 10203:
		// ◆ メインイベント
		// シネンの森の池へ
		worldmap->markSpot(MapIdDef::MAPID_EP01_FOREST4_ROCK_POND, WorldMap::SPOTKIND_MAINEV, true);
		// ◆ サブイベント
		if(!svdat->getSaveF(SaveIdDef::SFID_ST10203_DONE_KABAL_TALK))
		{
			// コメサット村の風の区へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_WIND_WARD, WorldMap::SPOTKIND_SUBEV, true);
		}
		break;
	case 10204:
		// ◆ メインイベント
		if(svdat->getSaveF(SaveIdDef::SFID_ST10204_GET_LUMIN_SOUMA_FROM_HERMIT))
		{
			// シネンの森の池へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_FOREST4_ROCK_POND, WorldMap::SPOTKIND_MAINEV, true);
		}
		else	// 発光ソウマを得ていない
		{
			// 寺院の回廊へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_TEMPLE_CORRIDOR, WorldMap::SPOTKIND_MAINEV, true);
		}
		// ◆ サブイベント
		if(!svdat->getSaveF(SaveIdDef::SFID_ST10204_DONE_KABAL_TALK))
		{
			// コメサット村の風の区へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_WIND_WARD, WorldMap::SPOTKIND_SUBEV, true);
		}
		break;
	case 10301:
		// ◆ メインイベント
		if(svdat->getSaveF(SaveIdDef::SFID_ST10301_READ_CERJA_LETTER))
		{
			// シネンの森の桜へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_FOREST4_BLOSSOMS, WorldMap::SPOTKIND_MAINEV, true);
		}
		else	// セルジャの手紙を読んでいない
		{
			// コメサット村の火の区へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_FIRE_WARD, WorldMap::SPOTKIND_MAINEV, true);
		}
		break;
	case 10302:
		// ◆ メインイベント
		// 寺院の回廊へ
		worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_TEMPLE_CORRIDOR, WorldMap::SPOTKIND_MAINEV, true);
		break;
	case 10303:
		// ◆ メインイベント
		// シネンの森の桜へ
		worldmap->markSpot(MapIdDef::MAPID_EP01_FOREST4_BLOSSOMS, WorldMap::SPOTKIND_MAINEV, true);
		break;
	case 10304:
		// ◆ メインイベント
		// コメサット村の水の区へ
		worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_WATER_WARD, WorldMap::SPOTKIND_MAINEV, true);
		// ◆ サブイベント
		if(!svdat->getSaveF(SaveIdDef::SFID_ST10304_TALK_KABAL_BEFORE_REBECCA))
		{
			// コメサット村の風の区へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_WIND_WARD, WorldMap::SPOTKIND_SUBEV, true);
		}
		break;
	case 10305:
		// ◆ メインイベント
		// シネンの森の桜へ
		worldmap->markSpot(MapIdDef::MAPID_EP01_FOREST4_BLOSSOMS, WorldMap::SPOTKIND_MAINEV, true);
		break;
	case 10306:
		// ◆ メインイベント
		if(svdat->getSaveF(SaveIdDef::SFID_ST10306_TOOK_CERJA_LETTER))
		{
			// シネンの森の桜へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_FOREST4_BLOSSOMS, WorldMap::SPOTKIND_MAINEV, true);
		}
		else	// セルジャの手紙を取得していない
		{
			// コメサット村の火の区へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_FIRE_WARD, WorldMap::SPOTKIND_MAINEV, true);
		}
		break;
	case 10401:
		// ◆ メインイベント
		if(svdat->getSaveF(SaveIdDef::SFID_ST10401_DONE_YASAN_THANK))
		{
			// シネンの森の出口広場へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_FOREST4_EXIT_SQUARE, WorldMap::SPOTKIND_MAINEV, true);
		}
		else	// ヤサンからソースを得ていない
		{
			// コメサット村の水の区へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_WATER_WARD, WorldMap::SPOTKIND_MAINEV, true);
		}
		// ◆ サブイベント
		if(!svdat->getSaveF(SaveIdDef::SFID_ST10401_DONE_KABAL_TALK))
		{
			// コメサット村の風の区へ
			worldmap->markSpot(MapIdDef::MAPID_EP01_BASE_VILLAGE_WIND_WARD, WorldMap::SPOTKIND_SUBEV, true);
		}
		break;
	}

	WorldMapStateUpdaterDefault::notifyBegin(worldmap);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
