/***********************************************************************//**
 *	QsMod10102.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QsMod10102.h"

// Friends
#include "../../MapIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/MoveMap.h"
#include "../../../quest/QuestArticle.h"
#include "../../../quest/QuestDef.h"
#include "../../../quest/QuestManager.h"
#include "../../../unit/UnitManager.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QsMod10102 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void QsMod10102::exec(ExecRes* res, const ExecCtx* ec)
{
	QsMod::exec(res, ec);
	
	if(Game::getGame()->getMoveMap()->getCurMapId() == MapIdDef::MAPID_EP01_FOREST4_HERB_GARDEN)	// ハーブ園マップ
	{
		if(_qstaRef->getStat() == QuestArticle::STAT_ACCEPT)	// 受諾中のみ
		{
			QuestManager* qsmng = Game::getGame()->getQuestManager(); ASSERT(qsmng != 0L);
			if(!qsmng->isReservedAchieveQuest(_qstaRef->getDef()->getQuestId()))
			{
				// 有効な敵がいない場合に条件達成
				UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
				if(!unitmng->checkEnableEnemy())
				{
					qsmng->reserveAchieveQuest(_qstaRef->getDef()->getQuestId());
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
