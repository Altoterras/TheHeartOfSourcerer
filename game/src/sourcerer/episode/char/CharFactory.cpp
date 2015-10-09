/***********************************************************************//**
 *	CharFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/29.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CharFactory.h"

// Friends
#include "CharIdDef.h"
#include "enemy/Char1401_Ungetsuiwa_EnemySpirit.h"
#include "enemy/Char1403_HighSaru_EnemySpirit.h"
#include "enemy/Char1404_BattleTutorial_EnemySpirit.h"
#include "enemy/Char1406_HighKabutomushi_EnemySpirit.h"
#include "enemy/Char1502_Usagi_EnemySpirit.h"
#include "enemy/Char1504_Shika_EnemySpirit.h"
#include "enemy/Char1505_Kuma_EnemySpirit.h"
#include "../../unit/char/npc/EnemyUnit.h"
#include "../../spirit/char/npc/EnemySpirit.h"

// External
#include "../../../tfw/string/StringUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CharFactory メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	敵キャラクタの作成
**//*---------------------------------------------------------------------*/
bool CharFactory::newEnemy(EnemyUnit** unit, EnemySpirit** sprt, u16 charid)
{
	switch(charid)
	{
	default:
		*unit = new EnemyUnit();
		*sprt = new EnemySpirit();
		break;
	case CharIdDef::CHARID_ENEMY_UNGETSUIWA:
		*unit = new EnemyUnit();
		*sprt = new Char1401_Ungetsuiwa_EnemySpirit();
		break;
	case CharIdDef::CHARID_ENEMY_H_SARU:
		*unit = new EnemyUnit();
		*sprt = new Char1403_HighSaru_EnemySpirit();
		break;
	case CharIdDef::CHARID_ENEMY_BATTLETUTO:
		*unit = new EnemyUnit();
		*sprt = new Char1404_BattleTutorial_EnemySpirit();
		break;
	case CharIdDef::CHARID_ENEMY_H_KABUTOMUSHI:
		*unit = new EnemyUnit();
		*sprt = new Char1406_HighKabutomushi_EnemySpirit();
		break;
	case CharIdDef::CHARID_ENEMY_USAGI:
		*unit = new EnemyUnit();
		*sprt = new Char1502_Usagi_EnemySpirit();
		break;
	case CharIdDef::CHARID_ENEMY_SHIKA:
		*unit = new EnemyUnit();
		*sprt = new Char1504_Shika_EnemySpirit();
		break;
	case CharIdDef::CHARID_ENEMY_KUMA:
		*unit = new EnemyUnit();
		*sprt = new Char1505_Kuma_EnemySpirit();
		break;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
