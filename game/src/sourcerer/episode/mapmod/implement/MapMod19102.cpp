/***********************************************************************//**
 *	MapMod19102.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MapMod19102.h"

// Friends
#include "../../char/pc/HelperUnit.h"
#include "../../char/pc/MyUnit.h"
#include "../../char/pc/PartnerUnit.h"
#include "../../party/MyParty.h"
#include "../../../event/EventSys.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../item/ItemEquip.h"
#include "../../../status/CharStat.h"
#include "../../../status/EleneStat.h"
#include "../../../status/EquipStat.h"
#include "../../../status/StatusIfEx.h"
#include "../../../status/TransStat.h"
#include "../../../unit/char/CharUnit.h"
#include "../../../unit/item/EquipmentItemUnit.h"

// External
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapMod19102 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MapMod19102::MapMod19102()
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool MapMod19102::begin()
{
	Party* party = Game::getGame()->getMyParty();

	// コツメ無効
	party->setEnablePcChar(MyParty::PC_HLP, false);
	// コツメがリーダーの場合は他のメンバーをリーダーに
	if(party->getLeaderPcUnit() == party->pcUnit(MyParty::PC_HLP))
	{
		if(!party->setLeader(MyParty::PC_MY))
		{
			party->setLeader(MyParty::PC_PTN);
		}
	}

	// アサの LFENE を 1 に
	#if defined(_DEBUG) && 0
	{
		MyUnit* unitMy = (MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY); ASSERT(unitMy != 0L);
		unitMy->charStat()->debug_damageEnergyDirect(unitMy->getCharStat()->getEnergy() - 1);
	}
	#endif

	#if defined(_DEBUG) && 1
		PartnerUnit* unitPtn = (PartnerUnit*)party->pcUnit(MyParty::PC_PTN); ASSERT(unitPtn != 0L);
		if((unitPtn != 0L) && unitPtn->isEnable())
		{
			#if 0	// 雲月テスト
			{
				// コルポの状態を錯乱に
				CharStat* cstatPtn = unitPtn->charStat();
				const EleneStat* eestatPtn = cstatPtn->getEleneStat();
				const Elene* eePtnNql = eestatPtn->getNql();
				Elene eeImp(
					eePtnNql->getSoil() / 2048,
					eePtnNql->getWater() / 1024,
					eePtnNql->getFire() * 5 / 2,
					eePtnNql->getWind() * 3 / 2,
					eePtnNql->getAethel()	);
				TransStat tstat;
				EleneStat eestat(&eeImp);
				tstat.setupForDirectFunc(0L, &eestat, EleneStat::FF_OP_EXCEED);
				StatusChangeResult scres;
				StatusInfluencer::influence(&scres, cstatPtn, &tstat);
				Game::getGame()->getConductor()->on(&scres, 0);			// 結果を表示
			}
			#endif

			#if 1	// 大猿などのボステスト
			{
				// 右手武器に最大のベールを付加
				EquipmentItemUnit* unitWep = (EquipmentItemUnit*)unitPtn->getItemEquip()->getEquipUnit(EquipPlace::EQP_RHAND);
				if(unitWep != 0L)
				{
					Elene ee(0, Elene::ELEN_MAX, Elene::ELEN_MAX, Elene::ELEN_MAX, 0);
					EleneStat eestat(&ee, &ee);
					TransStat tstat;
					tstat.setupForDirectFunc(0L, &eestat, EleneStat::FF_OP_WRAP);
#if 1
					StatusIfEx::influence(unitWep->charStat(), &tstat, true, true);
#else
					StatusChangeResult scres;
					StatusInfluencer::influence(&scres, unitWep->charStat(), &tstat);
					Game::getGame()->getConductor()->on(&scres, 0);			// 結果を表示
#endif
				}
			}
			#endif
		}
	#endif

	return MapMod10000::begin();
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
bool MapMod19102::end()
{
	return MapMod10000::end();
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void MapMod19102::exec(ExecRes* res, const ExecCtx* ec)
{
	MapMod10000::exec(res, ec);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
