/***********************************************************************//**
 *	WalkingRole.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "WalkingRole.h"

// Friends
#include "../../../event/EventIdDef.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/Party.h"
#include "../../../status/CharStat.h"
#include "../../../unit/char/pc/PcUnit.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// WalkingRole メソッド

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理
**//*---------------------------------------------------------------------*/
int WalkingRole::execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	if(!gec->isLockOperating() && !gec->isOpenedLockGui())
	{
		PcUnit* cunitMy = (PcUnit*)Game::getGame()->getGameMyParty()->leaderPcUnit();
		if(cunitMy != 0L)
		{
			const CharStat* cstatMy = cunitMy->getCharStat();
			if(cstatMy->getStat() == CharStat::STAT_DOWN)
			{
				if(gec->isPreventDown())	// ダウン防止中
				{
					cunitMy->charStat()->revive(true);	// 1 だけ回復
				}
				else
				{
					// ダウンイベント起動
					EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
					EvCause evcause(EventCause::CID_NULL);
					evsys->begin(EventIdDef::EVID_FIXED_DOWN_ME, 0, &evcause);
				}
			}
		}
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
