/***********************************************************************//**
 *	QsMod10103.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/07.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QsMod10103.h"

// Friends
#include "../../MapIdDef.h"
#include "../../../episode/party/MyParty.h"
#include "../../../episode/item/ItemIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/MoveMap.h"
#include "../../../item/Item.h"
#include "../../../item/ItemManager.h"
#include "../../../quest/QuestArticle.h"
#include "../../../quest/QuestDef.h"
#include "../../../quest/QuestManager.h"
#include "../../../unit/Unit.h"
#include "../../../unit/UnitManager.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QsMod10103 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void QsMod10103::exec(ExecRes* res, const ExecCtx* ec)
{
	QsMod::exec(res, ec);
	
	if(Game::getGame()->getMoveMap()->getCurMapId() == MapIdDef::MAPID_EP01_FOREST4_HERB_GARDEN)	// ハーブ園マップ
	{
		if(_qstaRef->getStat() == QuestArticle::STAT_ACCEPT)	// 受諾中のみ
		{
			UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);

			// ハーブの有効・無効化
			Unit* unitHerb = unitmng->findUnitFromCtrlEvid(CTRLEVID_HERB_03);
			if(unitHerb != 0L)
			{
				bool isEnebleHerb = !unitmng->checkEnableEnemy();	// 有効な敵がいない場合にハーブ有効
				if(unitHerb->isEnable() != isEnebleHerb)
				{
					unitHerb->setEnable(isEnebleHerb);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
