/***********************************************************************//**
 *	Char1505_Kuma_EnemySpirit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/04/03.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Char1505_Kuma_EnemySpirit.h"

// Friends
#include "../CharIdDef.h"
#include "../../StDepEvDef.h"
#include "../../../char/CharDef.h"
#include "../../../common/CalcUtils.h"
#include "../../../body/Game.h"
#include "../../../unit/UnitManager.h"
#include "../../../unit/char/CharUnit.h"

// External
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Char1505_Kuma_EnemySpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	対決状態変更通知
**//*---------------------------------------------------------------------*/
void Char1505_Kuma_EnemySpirit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
	if(isConfront)	// 戦闘開始時
	{
		if(getUnit()->getCtrlEvid() == StDepEvDef::CTRLEVID_EP01_KUMA_ENEMY)	// シネンの森出口のコグナである
		{
			ASSERT(unitEntrant != 0L);
			if(unitEntrant->getCtrlEvid() == StDepEvDef::CTRLEVID_EP01_KUMA_ENEMY)	// コグナ戦闘参加時
			{
				const Vector3F* posTrg = unitTrg->getPosition();
				UnitManager* unitmng = Game::getGame()->getUnitManager();
				const s32 NUM_ABU = 12;
				const f32 distRand = 100.0f;
				const f32 dirBasis = TFW_PI;
				const f32 angleOne = TFW_PI / (f32)NUM_ABU;
				const f32 thetaAbu[NUM_ABU] =
				{
					dirBasis - (2.0f * angleOne),
					dirBasis - (1.0f * angleOne),
					dirBasis,
					dirBasis + (1.0f * angleOne),
					dirBasis + (2.0f * angleOne),
					dirBasis - (1.5f * angleOne),
					dirBasis - (0.5f * angleOne),
					dirBasis + (0.5f * angleOne),
					dirBasis + (1.5f * angleOne),
					dirBasis - (1.0f * angleOne),
					dirBasis,
					dirBasis + (1.0f * angleOne),
				};
				const f32 distAbu[NUM_ABU] =
				{
					1000.0f,
					1000.0f,
					1000.0f,
					1000.0f,
					1000.0f,
					1500.0f,
					1500.0f,
					1500.0f,
					1500.0f,
					2000.0f,
					2000.0f,
					2000.0f,
				};
				s32 cntAbu = 0;
				for(int i = 0; i < unitmng->getUnitNum(); i++)
				{
					Unit* unit = unitmng->getUnitFromIndex(i);
					if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_CHAR))
					{
						CharUnit* cunit = (CharUnit*)unit;
						if((cunit->getCharDef() != 0L) && (cunit->getCharDef()->getCharDefId() == CharIdDef::CHARID_ENEMY_ABU))	// アブナである
						{
							Spirit* sprt = cunit->spirit();
							if(sprt->getType() == SPRTTYPE_ENEMY)
							{
								EnemySpirit* esprt = (EnemySpirit*)sprt;
								// アブナを起こす
								cunit->setEnable(true);
								// 対象をロックオン
								esprt->setControlFlags(EnemySpirit::CTRLF_LOCKON_TRG | EnemySpirit::CTRLF_UNLOCKON_IN_DETECT_DIST, true);
								esprt->setFocusUnit(unitTrg, false);
								// 周囲に配置
								Vector3F pos(*posTrg);
								pos.x() += (distAbu[cntAbu] * ::cosf(thetaAbu[cntAbu])) + (CalcUtils::randPlusMinusOneF() * distRand);
								pos.z() += (distAbu[cntAbu] * ::sinf(thetaAbu[cntAbu])) + (CalcUtils::randPlusMinusOneF() * distRand);
								cunit->setPosition(&pos);
								cntAbu++;
								if(cntAbu >= NUM_ABU)	{	break;	}
							}
						}
					}
				}
			}
		}
	}

	return EnemySpirit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
