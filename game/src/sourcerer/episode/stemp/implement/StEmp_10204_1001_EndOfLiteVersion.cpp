/***********************************************************************//**
 *	StEmp_10204_1001_EndOfLiteVersion.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/10/29.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StEmp_10204_1001_EndOfLiteVersion.h"

// Friends
#include "../StEmpType.h"
#include "../../char/pc/MyUnit.h"
#include "../../char/pc/PartnerUnit.h"
#include "../../party/MyParty.h"
#include "../../../confront/Confront.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../status/CharStat.h"
#include "../../../status/EleneStat.h"
#include "../../../unit/UnitManager.h"

// External
#include "../../../../etk/effect/ScreenEffect.h"
#include "../../../../tfw/lib/Color.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define EVID_CLEAR_V01_LITE		10008			// Volume1 Lite クリアイベント ID

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StEmp_10204_1001_EndOfLiteVersion メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void StEmp_10204_1001_EndOfLiteVersion::begin(u32 param)
{
#if defined(_ENLIGHT_V01_LITE)
	EvCause evcause;
	Game::getGame()->getEventSys()->begin(EVID_CLEAR_V01_LITE, 0, &evcause);	// コルポ混乱イベント

	ScreenEffect* screff = Game::getGame()->getSceneScreenEffect(); ASSERT(screff != 0L);
	ColorU8 col1(0, 0, 0, 255);
	screff->setColor1(&col1);
	screff->setMaxFrame(60);
	screff->start(ScreenEffect::MODE_FILL);
#endif

	StEmp::begin(param);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10204_1001_EndOfLiteVersion::StEmp_10204_1001_EndOfLiteVersion()
	: StEmp(STEMPTYPE_10204_1001)
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
