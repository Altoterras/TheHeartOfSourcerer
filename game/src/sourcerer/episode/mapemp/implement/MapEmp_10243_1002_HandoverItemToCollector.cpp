/***********************************************************************//**
 *	MapEmp_10243_1002_HandoverItemToCollector.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/21.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MapEmp_10243_1002_HandoverItemToCollector.h"

// Friends
#include "../MapEmpType.h"
#include "../../save/Trophy.h"
#include "../../../event/EventSys.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/Party.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/ItemListWindow.h"
#include "../../../item/Item.h"
#include "../../../item/ItemDef.h"
#include "../../../item/ItemManager.h"
	
// External
#include "../../../../etk/sg/SorSgm.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define EVID_SUCCEEDED_GOOD		(114)
#define EVID_SUCCEEDED_BAD		(115)
#define EVID_CANCELED			(116)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapEmp_10243_1002_HandoverItemToCollector メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MapEmp_10243_1002_HandoverItemToCollector::MapEmp_10243_1002_HandoverItemToCollector()
	: MapEmp(MAPEMPTYPE_10243_1002)
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void MapEmp_10243_1002_HandoverItemToCollector::begin(u32 param)
{
	MapEmp::begin(param);

	///
}

/*---------------------------------------------------------------------*//**
	フレーム処理実装
**//*---------------------------------------------------------------------*/
void MapEmp_10243_1002_HandoverItemToCollector::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	enum Step
	{
		STEP_NULL,
		STEP_WAIT_BEGIN,
		STEP_SHOW_ITEM_LIST,
		STEP_WAIT_ITEM_SELECT,
	};

	switch(getStep())
	{
	case STEP_WAIT_BEGIN:		// 継続監視
		if(!getOwner()->checkBegunOtherMatter(this, true))
		{
			// 自身以外のイベントが起動していないならば次のステップへ
			advanceStep();
		}
		break;
	case STEP_SHOW_ITEM_LIST:
		{
			// 背景を消す
			SorSgm* sgm = Game::getGame()->getSceneGraph();
			sgm->showBg(false);

			// アイテムリストウインドウを開く
			ItemListWindow* wndItemList = Game::getGame()->getGui()->getItemListWindow(); ASSERT(wndItemList != 0L);
			wndItemList->setCase(ItemListWindow::ILWCASE_SELECT, ItemListWindow::AF_STONE_ONLY);
			wndItemList->showWindow(true);
			wndItemList->setSelfClose();

			advanceStep();
		}
		break;
	case STEP_WAIT_ITEM_SELECT:
		{
			ItemListWindow* wndItemList = Game::getGame()->getGui()->getItemListWindow(); ASSERT(wndItemList != 0L);
			if(wndItemList->isDone())	// 完了検知
			{
				// アイテムを選択した
				if(wndItemList->getResult()->getSelectedIndex() != -1)
				{
					// アイテムを渡す
					const Item* item = wndItemList->getSelectedItem();
					handoverItem(item);
				}
				// キャンセル
				else
				{
					// アイテムを渡す
					handoverItem(0L);
				}

				// アイテムリストウインドウを閉じる
				if(wndItemList->isShowWindow())
				{
					wndItemList->showWindow(false);
				}

				// 背景表示を戻す
				SorSgm* sgm = Game::getGame()->getSceneGraph();
				sgm->showBg(true);

				end();	// イベント終了
			}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	アイテム譲渡処理
**//*---------------------------------------------------------------------*/
void MapEmp_10243_1002_HandoverItemToCollector::handoverItem(const Item* item)
{
	const ItemDef* itmdef = (item != 0L) ? item->getItemDef() : 0L;
	if(itmdef == 0L)
	{
		// キャンセルイベントへ
		EvCause evcause(EventCause::CID_EVENT);
		getOwner()->begin(EVID_CANCELED, 0, &evcause);
		return;
	}

	// 石以外は拒否
	if(itmdef->getCategory() != ItemDef::CAT_STONE)
	{
		EvCause evcause(EventCause::CID_EVENT);
		getOwner()->begin(EVID_CANCELED, 0, &evcause);
		return;
	}

	// 経験値取得
	u16 exp = itmdef->getPrice() / 20;
	if(exp < 3)	{	exp = 3;	}
	Conductor* conductor = Game::getGame()->getConductor(); ASSERT(conductor != 0L);
	conductor->onExperienceNow(exp);

	// トロフィー処理
	#if defined(_ENLIGHT_V01) && ENABLE_PSNS
		Game::getGame()->getTrophy()->achievementTrophy(Trophy::TI_ITEM_ADEPT_3_EXCHANGED_FOR_EXP, 100);
	#endif

	// アイテム消滅
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	ItemManager* itmmng = party->getItemManager(); ASSERT(itmmng != 0L);
	itmmng->removeItem(item->getItemGenId());	// アイテムを失う

	// 返答イベントへ
	EvCause evcause(EventCause::CID_EVENT);
	getOwner()->begin(exp >= 20 ? EVID_SUCCEEDED_GOOD : EVID_SUCCEEDED_BAD, 0, &evcause);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
