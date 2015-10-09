/***********************************************************************//**
 *	MapEmp_10243_1001_DoorControl.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/18.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MapEmp_10243_1001_DoorControl.h"

// Friends
#include "../MapEmpType.h"
#include "../../MapIdDef.h"
#include "../../QuestIdDef.h"
#include "../../char/CharIdDef.h"
#include "../../../body/Game.h"
#include "../../../map/Map.h"
#include "../../../quest/QuestArticle.h"
#include "../../../quest/QuestManager.h"
#include "../../../unit/UnitManager.h"
#include "../../../unit/char/CharUnit.h"
	
// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapEmp_10243_1001_DoorControl メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MapEmp_10243_1001_DoorControl::MapEmp_10243_1001_DoorControl()
	: MapEmp(MAPEMPTYPE_10243_1001)
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void MapEmp_10243_1001_DoorControl::begin(u32 param)
{
	MapEmp::begin(param);

	QuestManager* qsmng = Game::getGame()->getQuestManager();
	const QuestArticle* qsta = qsmng->getQuestFromId(QuestIdDef::QUESTID_OPENTHEDOOR_01);
	if(qsta != 0L)
	{
		// 井戸舎を開けるクエストをまだ終えてない場合のマップ変化処理
		if(!TFW_IS_FLAG(qsta->getFlags(), QuestArticle::F_FINISHED))
		{
			// 井戸舎内への移動設定を無効化する
			Map* map = Game::getGame()->getMap(); ASSERT(map != 0L);
			map->changeMoveMapId(MapIdDef::MAPID_EP01_BASE_VILLAGE_IDO_HUT, 0);

			// 開いている扉を無効化し、閉じている扉を有効化する
			UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
			CharUnit* cunit = unitmng->findCharUnitFromCharId(CharIdDef::CHARID_IDO_DOOR_OPENED);
			if(cunit != 0L)	{	cunit->setEnable(false);	}
			cunit = unitmng->findCharUnitFromCharId(CharIdDef::CHARID_IDO_DOOR_CLOSED);
			if(cunit != 0L)	{	cunit->setEnable(true);		}
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
