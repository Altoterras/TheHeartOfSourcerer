/***********************************************************************//**
 *	StMod10101.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/14.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StMod10101.h"

// Friends
#include "../../MapIdDef.h"
#include "../../StDepEvDef.h"
#include "../../StoryManager.h"
#include "../../party/MyParty.h"
#include "../../save/SaveIdDef.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../body/MoveMap.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/IngameFront.h"
#include "../../../save/SaveData.h"
#include "../../../save/SaveSys.h"
#include "../../../unit/UnitManager.h"
#include "../../../unit/char/pc/PcUnit.h"

// External
#include "../../../../tfw/lib/Color.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS


////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StMod10101 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void StMod10101::exec(ExecRes* res, const ExecCtx* ec)
{
	// (1) コメサット村入口マップで門を出たらストーリー変更
	if(Game::getGame()->getMoveMap()->getCurMapId() == MapIdDef::MAPID_EP01_BASE_VILLAGE_GATE)
	{
		// キャラクタの位置でストーリー変更イベント発生
		Unit* unitMy = Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
		if(unitMy != 0L)
		{
			const Vector3F* posMy = unitMy->getPosition();
			if(posMy->z() > 0.0f)
			{
				// 門を出た
				EventSys* evsys = Game::getGame()->getEventSys();
				EvCause evcause(EventCause::CID_EVENT);
				evsys->begin(StoryIdDef::EVID_STEV10101_NEXT_STORY, 0, &evcause);
			}
		}
	}
	// 座標でアイテム拾いチュートリアル
	else if(Game::getGame()->getMoveMap()->getCurMapId() == MapIdDef::MAPID_EP01_BASE_VILLAGE_WATER_MAIN_STREET)
	{
		Unit* unitMy = Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
		if(unitMy != 0L)
		{
			const Vector3F* posMy = unitMy->getPosition();
			if(posMy->z() > 1500.0f)
			{
				// 水の区主参道を出そうなところまで来た
				SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
				if(!svd->getSaveF(SaveIdDef::SFID_ST10101_PICKUP_TUTORIAL_STONE))
				{
					EventSys* evsys = Game::getGame()->getEventSys();
					EvCause evcause(EventCause::CID_EVENT);
					evsys->begin(StoryIdDef::EVID_STEV10101_ITEM_PICK_TUTO, 0, &evcause);
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	マップ変更前通知
**//*---------------------------------------------------------------------*/
bool StMod10101::notifyChangeMapBefore(s32* mapidNew, s32 mapidOld)
{
	// （(1) に対して、）またはコメサット村マップから出たところでストーリー変更（念のため）
	if((!MapIdDef::isBaseVillage(*mapidNew)) && (*mapidNew != MapIdDef::MAPID_EVENT_DYMMY))
	{
		_strymngRef->reserveNextStory(StoryIdDef::STID_EP01_OP_BATTLE_TUTO);	// 次のストーリーへ
		return false;
	}

	// オープニング後の寺院を出るシーンでイベント発生
	SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
	if(	!svd->getSaveF(SaveIdDef::SFID_ST10101_MEET_CORPO) &&				// コルポにまだ会っていない
		MapIdDef::isBaseVillageTemple(mapidOld) &&							// 移動前のマップは寺院
		(*mapidNew == MapIdDef::MAPID_EP01_BASE_VILLAGE_TEMPLE_BRIDGE)	)	// 移動後のマップは寺院前の橋
	{
		EventSys* evsys = Game::getGame()->getEventSys();
		EvCause evcause(EventCause::CID_EVENT);
		evsys->begin(StoryIdDef::EVID_STEV10101_MEET_CORPO, 0, &evcause);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	マップ変更後通知
**//*---------------------------------------------------------------------*/
void StMod10101::notifyChangedMap(s32 mapidNew, s32 mapidOld)
{
	// ソウマアイコンの有効化・無効化
	bool enableIgfr = true;
	SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
	if(!svd->getSaveF(SaveIdDef::SFID_ST10101_MEET_CORPO))				// コルポにまだ会っていない
	{
		if(	(mapidNew >= MapIdDef::MAPID_EP01_BASE_VILLAGE_TEMPLE__START_) &&
			(mapidNew <= MapIdDef::MAPID_EP01_BASE_VILLAGE_TEMPLE__END_) )
		{
			enableIgfr = false;
		}
	}
	// インゲームフロントのアイコンを無効化
	IngameFront* igfr = Game::getGame()->getGui()->getIngameFront();
	igfr->setEneblePanel(IngameFront::BTN_SOUMA, enableIgfr);
	igfr->setEneblePanel(IngameFront::BTN_MENU, enableIgfr);

	// アイテム拾いチュートリアルを表示していいたらフィールドアイテムを無効化する
	if(mapidNew == MapIdDef::MAPID_EP01_BASE_VILLAGE_WATER_MAIN_STREET)
	{
		UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
		Unit* unitStone = unitmng->findUnitFromCtrlEvid(StDepEvDef::CTRLEVID_EP01_TUTORIAL_STONE);
		if(unitStone != 0L)
		{
			SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
			if(svd->getSaveF(SaveIdDef::SFID_ST10101_PICKUP_TUTORIAL_STONE))
			{
				unitStone->setEnable(false);
			}
		}
	}
	else if(
		(mapidNew == MapIdDef::MAPID_EP01_BASE_VILLAGE_TEMPLE_CORRIDOR) ||
		(mapidNew == MapIdDef::MAPID_EP01_BASE_VILLAGE_WATER_WARD)	)
	{
		// 寺院回廊、水の区のフィールドアイテムを無効化する（水の区主参道でチュートリアルがあるため）
		UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
		for(s32 i = 0; i < unitmng->getUnitNum(); i++)
		{
			Unit* unit = unitmng->getUnitFromIndex(i);
			if(unit->getUnitType() == UNITTYPE_FIELD_ITEM)
			{
				if(unit->isEnable())
				{
					unit->setEnable(false);
				}
			}
		}
	}

	StMod10000::notifyChangedMap(mapidNew, mapidOld);
}

/*---------------------------------------------------------------------*//**
	アイテム拾い通知
**//*---------------------------------------------------------------------*/
void StMod10101::notifyPickupItem(const Unit* unitActor, const Unit* unitItem)
{
	// アイテム拾いチュートリアル
	if(unitItem->getCtrlEvid() == StDepEvDef::CTRLEVID_EP01_TUTORIAL_STONE)
	{
		SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
		if(!svd->getSaveF(SaveIdDef::SFID_ST10101_PICKUP_TUTORIAL_STONE))
		{
			EventSys* evsys = Game::getGame()->getEventSys();
			EvCause evcause(EventCause::CID_EVENT);
			evsys->begin(StoryIdDef::EVID_STEV10101_ITEM_PICK_TUTO, 0, &evcause);
		}
	}

	StMod10000::notifyPickupItem(unitActor, unitItem);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
