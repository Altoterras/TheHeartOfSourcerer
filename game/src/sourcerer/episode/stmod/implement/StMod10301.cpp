/***********************************************************************//**
 *	StMod10301.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StMod10301.h"

// Friends
#include "../../MapIdDef.h"
#include "../../StoryManager.h"
#include "../../party/MyParty.h"
#include "../../save/SaveIdDef.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../body/MoveMap.h"
#include "../../../mode/GameMode.h"
#include "../../../unit/UnitManager.h"
#include "../../../unit/char/pc/PcUnit.h"
#include "../../../save/SaveData.h"
#include "../../../save/SaveSys.h"
#include "../../../status/CharStat.h"

// External
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define EVID_CORPO_SAY_OUTHOUSE	(10001)		// 納屋に寄ってくれイベント
#define EVID_WATER_POOL			(10006)		// 水溜りイベント

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StMod10301 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StMod10301::StMod10301()
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StMod10301::exec(ExecRes* res, const ExecCtx* ec)
{
	if(Game::getGame()->getMoveMap()->getCurMapId() == MapIdDef::MAPID_EP01_FOREST4_BLOSSOMS)	// セルジャの桜マップ
	{
		// キャラクタの位置によってイベント起動
		Unit* unitMy = Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
		if(unitMy != 0L)
		{
			const Vector3F* posMy = unitMy->getPosition();
			if((posMy->z() <= -3000.0f) && ((1900.0f <= posMy->x()) && (posMy->x() <= 3000.0f)))	// 水溜りに近づいた
			{
				// 次のイベント起動確認
				do
				{
					// セルジャの手紙を読んでいるか確認
					SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
					if(!svd->getSaveF(SaveIdDef::SFID_ST10301_READ_CERJA_LETTER))	{	break;	}

					// イベントを安全に実行できるか？
					if(!isSafeNextEventConditionNow())								{	break;	}

					// イベント起動
					EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
					EvCause evcause(EventCause::CID_NULL);
					evsys->begin(EVID_WATER_POOL, 0, &evcause);
				}
				while(false);
			}
		}
	}

	StMod10300::exec(res, ec);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
