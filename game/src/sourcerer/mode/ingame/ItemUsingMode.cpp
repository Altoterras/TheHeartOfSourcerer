/***********************************************************************//**
 *	ItemUsingMode.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/08.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ItemUsingMode.h"

// Friends
#include "../../episode/char/pc/MySpirit.h"
#include "../../episode/party/MyParty.h"
#include "../../body/Game.h"
#include "../../gui/GameGui.h"
#include "../../gui/window/implement/ItemListWindow.h"
#include "../../item/Item.h"
#include "../../item/ItemManager.h"
#include "../../unit/char/pc/PcUnit.h"
#include "../../spirit/char/pc/PcSpirit.h"

// External
#include "../../../es/Es.h"
#include "../../../etk/sg/SorSgm.h"
#include "../../../tfw/lib/Matrix.h"
#include "../../../tfw/lib/Vector.h"
#include "../../../tfw/g3d/sg/node/SgNodeChain.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ItemUsingMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ItemUsingMode::ItemUsingMode()
	: _unitRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ItemUsingMode::create()
{
	if(!GameModeBase::create(0, 0, 0L))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool ItemUsingMode::notifyBegin(void* objParamRef)
{
	// 背景を消す
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	sgm->showBg(false);

	// アイテムリストウインドウを開く
	ItemListWindow* wndItemList = Game::getGame()->getGui()->getItemListWindow(); ASSERT(wndItemList != 0L);
	wndItemList->setCase(ItemListWindow::ILWCASE_SELECT, ItemListWindow::AF_STONE_ONLY);
	wndItemList->showWindow(true);
	wndItemList->setSelfClose();

	// 対象ユニットを得る
	_unitRef = ((GameModeParam*)objParamRef)->getFocusMenuUnit();
	ASSERT(_unitRef != 0L);

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void ItemUsingMode::notifyEnd(void* objParamRef)
{
	// まだアイテム選択中だったら強制終了
	ItemListWindow* wndItemList = Game::getGame()->getGui()->getItemListWindow(); ASSERT(wndItemList != 0L);
	if(wndItemList->isShowWindow())
	{
		wndItemList->showWindow(false);
	}

	// 背景表示を戻す
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	sgm->showBg(true);
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void ItemUsingMode::exec(ExecRes* res, const ExecCtx* ec)
{
	GameModeBase::exec(res, ec);

	ItemListWindow* wndItemList = Game::getGame()->getGui()->getItemListWindow(); ASSERT(wndItemList != 0L);
	if(wndItemList->isDone())	// 完了検知
	{
		// アイテムを使う
		if(wndItemList->getResult()->getSelectedIndex() != -1)
		{
			ASSERT(_unitRef != 0L);
			if((_unitRef != 0L) && TFW_IS_FLAG(_unitRef->getUnitFlags(), Unit::UNITFLAG_PC))
			{
				// キャラを得る
				PcSpirit* sprtPc = (PcSpirit*)_unitRef->getSpirit();
				ASSERT(sprtPc != 0L);

				// アイテムを使う
				Item* item = wndItemList->getSelectedItem();
				ASSERT(item != 0L);
				if(item != 0L)
				{
					Spirit::UseItemBehaviorParam ibp(
						item->getItemGenId()	);
					sprtPc->startBehavior(Spirit::BEHV_USE_ITEM, &ibp);
				}
			}

			// 以前のモードへ戻る
///			Mode* modeParent = getParentMode();
///			modeParent->reserveMode(modeParent->getPrevMode());	// モードを戻す
///			getParentMode()->setPrevMode();
			Game::getGame()->getGameMode()->changePrevEndMode(GameMode::MODE_NULL);
		}
		// キャンセル
		else
		{
			// 以前のモードへ戻る
///			Mode* modeParent = getParentMode();
///			modeParent->reserveMode(modeParent->getPrevMode());	// モードを戻す
///			getParentMode()->setPrevMode();
			Game::getGame()->getGameMode()->changePrevEndMode(GameMode::MODE_NULL);
		}
	}
}


////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
