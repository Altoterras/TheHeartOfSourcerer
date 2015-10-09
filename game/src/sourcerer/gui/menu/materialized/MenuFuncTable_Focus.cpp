/***********************************************************************//**
 *  MenuFuncTable_Focus.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/22.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MenuFuncTable_Focus.h"

// Friends
#include "../GameMenuShowParam.h"
#include "../../../confront/Confront.h"
#include "../../../episode/char/pc/MyUnit.h"
#include "../../../episode/char/pc/PartnerUnit.h"
#include "../../../episode/char/pc/PartnerSpirit.h"
#include "../../../event/EventIdInfoArray.h"
#include "../../../event/EventSys.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/GameNotifier.h"
#include "../../../body/Party.h"
#include "../../../gui/FocusCursor.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../item/ItemEquip.h"
#include "../../../item/ItemManager.h"
#include "../../../map/Map.h"
#include "../../../map/PlacementObj.h"
#include "../../../mode/GameMode.h"
#include "../../../mode/GameModeParam.h"
#include "../../../quest/QuestManager.h"
#include "../../../spirit/Spirit.h"
#include "../../../status/EquipStat.h"
#include "../../../unit/char/npc/NpcCharUnit.h"
#include "../../../unit/item/PutItemUnit.h"

// External
#include "../../../../tfw/smenu/MenuTreeNode.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

/*---------------------------------------------------------------------*//**
	表示通知
**//*---------------------------------------------------------------------*/
void MenuFuncTable_Focus::onShow(smenu::Menu* menu, smenu::MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	非表示通知
**//*---------------------------------------------------------------------*/
void MenuFuncTable_Focus::onHide(smenu::Menu* menu, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	呼び出し通知
**//*---------------------------------------------------------------------*/
bool MenuFuncTable_Focus::onCall(smenu::Menu* menu, const ExecCtx* ec, smenu::MenuTreeNode* mtnodeCall, smenu::MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	const GameExecCtx* gec = (const GameExecCtx*)ec;
	const VcString* cmd = mtnodeCall->getName();
	GameMode* gmode = Game::getGame()->getGameMode();
	GameMenuShowParam* showprm = (GameMenuShowParam*)objShowParam;

	if( cmd->equals("Focus") )
	{
		if(showprm->_unitRef != 0L)
		{
			// フォーカスをセットする
			GameGui* gui = Game::getGame()->getGui();	ASSERT(gui != 0L);
			FocusCursor* fccsr = gui->getFocusCursor();
			fccsr->setFocus(0, showprm->_unitRef);
		}
	}
	else if( cmd->equals("FocusWeapon") )
	{
		if(showprm->_unitRef != 0L)
		{
			ASSERT(TFW_IS_FLAG(showprm->_unitRef->getUnitFlags(), Unit::UNITFLAG_PC));	// 現状では PcUnit のはず
			if(TFW_IS_FLAG(showprm->_unitRef->getUnitFlags(), Unit::UNITFLAG_PC))
			{
				PcUnit* unitPc = (PcUnit*)showprm->_unitRef;
				s32 ieqp = EquipPlace::EQP_RHAND;	// 現状では右手装備のみ対応
				const ItemEquip* itmeq = unitPc->getItemEquip();
				if(itmeq != 0L)
				{
					const Item* item = itmeq->getEquipStat()->getEquipItem(ieqp);
					if(item != 0L)
					{
						// 装備品は存在する
						Unit* unitWep = itmeq->getEquipUnit(ieqp);
						// フォーカスをセットする
						GameGui* gui = Game::getGame()->getGui();	ASSERT(gui != 0L);
						FocusCursor* fccsr = gui->getFocusCursor();
						fccsr->setFocus(0, unitWep);
					}
				}
			}
		}
	}
	else if( cmd->equals("Fight") )
	{
		if(showprm->_unitRef != 0L)
		{
			Confront* cf = Game::getGame()->getConfront(); ASSERT(cf != 0L);
			bool isFound = false;
			const UnitArray* uaTrg = cf->getTargets();
			for(int i = 0; i < uaTrg->getCount(); i++)
			{
				if(uaTrg->getUnit(i) == showprm->_unitRef)
				{
					isFound = true;
				}
			}
			// ゲーム本体に対決通知を送り、モードを変更する
			if(!isFound)
			{
				Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
				Game::getGame()->getConfront()->start(showprm->_unitRef, 0, party->getLeaderPcUnit());
			}
		}
	}
	else if( cmd->equals("Talk") || cmd->equals("Check") )
	{
		Unit* unitRef = showprm->_unitRef;
		if(unitRef != 0L)
		{
			if(TFW_IS_FLAG(unitRef->getUnitFlags(), Unit::UNITFLAG_NPC))
			{
				NpcCharUnit* npc = (NpcCharUnit*)unitRef;
				// こちらに注目させる
				if(cmd->equals("Talk") && (npc->getSpirit() != 0))
				{
					npc->spirit()->setFocusUnit(Game::getGame()->getMyParty()->leaderPcUnit(), false);
				}
				// 話す、もしくは拾う
				PlacementObj* pobj = npc->getPlacementObj();
				const EventIdInfoArray::EventIdInfo* cii = pobj->getCallEvi()->getInfo(pobj->getActiveCallEviIndex());
				if(cii != 0L)
				{
					EventSys* evsys = Game::getGame()->getEventSys();
					EvCause evcause(EventCause::CID_FOCUSMENU);
					evcause.setParameter(npc);
					Game::getGame()->getMyParty()->animUnitOnEvent(npc, cii->getEvid());	// ユニットを注視
					evsys->begin(cii->getEvid(), cii->getEventParam(), &evcause);
					pobj->nextCallEvi(false);	// イベントを次へ
				}
			}
		}
	}
	else if( cmd->equals("Leader") )
	{
		// リーダーを設定する
		Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
		s32 ipc = party->findPcUnit(showprm->_unitRef);
		if(ipc >= 0)
		{
			party->setLeader(ipc);
		}
	}
	else if( cmd->equals("Souma") )
	{
		// ソウマ入力へモード変更
		GameModeParam* gmparam = (GameModeParam*)gmode->getParameter();
		gmparam->setFocusMenuUnit(showprm->_unitRef);
		if(Game::getGame()->isConfront())
		{
			gec->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_CONFRONT_MAGICTYPING);
		}
		else
		{
			gec->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_WALKING_MAGICTYPING);
		}
	}
	else if( cmd->equals("Meditation") )
	{
		Spirit* sprt = showprm->_unitRef->spirit();
		sprt->startBehavior(Spirit::BEHV_MEDITATION, 0L);
	}
	else if( cmd->equals("GuardCharge") )
	{
		Spirit* sprt = showprm->_unitRef->spirit();
		sprt->startBehavior(Spirit::BEHV_GUARD_CHARGE, 0L);
	}
	else if( cmd->equals("Attract") )
	{
		Spirit* sprt = showprm->_unitRef->spirit();
		sprt->startBehavior(Spirit::BEHV_ATTRACT, 0L);
	}
	else if( cmd->equals("CureParty") )
	{
		Spirit* sprt = showprm->_unitRef->spirit();
		sprt->startBehavior(Spirit::BEHV_CURE, 0L);
	}
	else if( cmd->equals("Item") )
	{
		// アイテム使用モードへ変更
		GameModeParam* gmparam = (GameModeParam*)gmode->getParameter();
		gmparam->setFocusMenuUnit(showprm->_unitRef);
		if(Game::getGame()->isConfront())
		{
			gec->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_CONFRONT_ITEMUSING);
		}
		else
		{
			gec->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_WALKING_ITEMUSING);
		}
	}
	else if( cmd->equals("StopBehavior") )
	{
		Spirit* sprt = showprm->_unitRef->spirit();
		sprt->breakBehavior(Spirit::BEHV_NULL, true, false);	// BEHV_NULL は全てを対象とした汎用中断
	}
	else if( cmd->equals("Pick") )
	{
		// アイテムを拾う
		Unit* unitRef = showprm->_unitRef;
		if(unitRef != 0L)
		{
			if(unitRef->getUnitType() == UNITTYPE_PUT_ITEM)
			{
				Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
				const Unit* unitActor = party->getPcUnit(0);
				ItemManager* itmmng = party->getItemManager(); ASSERT(itmmng != 0L);
				PutItemUnit* ditmu = (PutItemUnit*)unitRef;
				if(itmmng->pickupItem(Game::getGame()->getMap(), ditmu->getPlacementObj(), ditmu->getEleneStat()))
				{
					// 成功
					ditmu->setPlacementObj(0L);
					ditmu->setEnable(false);

					// メッセージを表示する
					const ItemDef* itmdf = ditmu->getItemDef();	ASSERT(itmdf != 0L);
					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GET_SOMETHING)->getRaw(), itmdf->getName(Env_getLangId())->getRaw(), "\n"));
					Game::getGame()->getConductor()->on(&msg, unitActor, 0);

					// 通知発行
					Game::getGame()->getGameNotifier()->notifyPickupItem(unitActor, ditmu);
					// クエストの可変達成状態の更新
					QuestManager* qsmng = Game::getGame()->getQuestManager();
					qsmng->updateVariableAchievementState();
				}
				else
				{
					// 失敗
					GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
					VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_ERROR_ITEM_FULL)->getRaw(), ""));
					wndSysMsg->showStdMsg(&msg);	// メッセージ表示
				}
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
