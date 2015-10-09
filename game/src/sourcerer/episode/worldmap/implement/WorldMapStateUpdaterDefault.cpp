/***********************************************************************//**
 *	WorldMapStateUpdaterDefault.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/19.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "WorldMapStateUpdaterDefault.h"

// Friends
#include "../WorldMap.h"
#include "../../../body/Game.h"
#include "../../../quest/QuestArticle.h"
#include "../../../quest/QuestDef.h"
#include "../../../quest/QuestManager.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// WorldMapStateUpdaterDefault メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	ワールドマップ開始通知
**//*---------------------------------------------------------------------*/
void WorldMapStateUpdaterDefault::notifyBegin(WorldMap* worldmap)
{
	QuestManager* qstmng = Game::getGame()->getQuestManager(); ASSERT(qstmng != 0L);

	if(qstmng == 0L)	{	return;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// クエストのスポット更新

	// クエストの可変達成状態の更新
	qstmng->updateVariableAchievementState();

	// クエストスポットを一度クリアする
	worldmap->clearSpot(WorldMap::SPOTKIND_QSTEV_START);
	worldmap->clearSpot(WorldMap::SPOTKIND_QSTEV_END);

	// クエスト
	for(int i = 0; i < qstmng->getQuestNum(); i++)
	{
		const QuestArticle* qsta = qstmng->getQuestByIndex(i);
		if(!qsta->isValid())
		{
			continue;
		}
		ASSERT(qsta->getDef() != 0L);

		switch(qsta->getStat())
		{
		case QuestArticle::STAT_BOOT:
			worldmap->markSpot(qsta->getDef()->getBaseMapId(), WorldMap::SPOTKIND_QSTEV_START, true);
			break;
		case QuestArticle::STAT_ACHVED:
			worldmap->markSpot(qsta->getDef()->getBaseMapId(), WorldMap::SPOTKIND_QSTEV_END, true);
			break;
		}

	}

}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
