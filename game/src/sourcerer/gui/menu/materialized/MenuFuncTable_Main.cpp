/***********************************************************************//**
 *  MenuFuncTable_Main.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/23.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MenuFuncTable_Main.h"

// Friends
#include "../GameMenuShowParam.h"
#include "../../../body/Game.h"
#include "../../../mode/GameMode.h"
#include "../../../mode/ingame/menu/MenuMode.h"

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
void MenuFuncTable_Main::onShow(smenu::Menu* menu, smenu::MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	非表示通知
**//*---------------------------------------------------------------------*/
void MenuFuncTable_Main::onHide(smenu::Menu* menu, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	呼び出し通知
**//*---------------------------------------------------------------------*/
bool MenuFuncTable_Main::onCall(smenu::Menu* menu, const ExecCtx* ec, smenu::MenuTreeNode* mtnodeCall, smenu::MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	const VcString* cmd = mtnodeCall->getName();
	GameMenuShowParam* showprm = (GameMenuShowParam*)objShowParam;
	MenuMode* modeOwnr = (MenuMode*)showprm->_ownerRef;
	if(cmd->equals("Source"))
	{
		modeOwnr->onCallMenuSource();
	}
	else if(cmd->equals("Item"))
	{
		modeOwnr->onCallMenuItem();
	}
	else if(cmd->equals("Status"))
	{
		modeOwnr->onCallMenuStatus();
	}
	else if(cmd->equals("Ability"))
	{
		modeOwnr->onCallMenuAbility();
	}
	else if(cmd->equals("Equip"))
	{
		modeOwnr->onCallMenuEquip();
	}
	else if(cmd->equals("Quest"))
	{
		modeOwnr->onCallMenuQuest();
	}
	else if(cmd->equals("Save"))
	{
		modeOwnr->onCallMenuSave();
	}
	else if(cmd->equals("Options"))
	{
		modeOwnr->onCallMenuOptions();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
