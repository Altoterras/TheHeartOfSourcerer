/***********************************************************************//**
 *  GameMenuPanelFactory.cpp
 *  Enlight Game Application
 *
 *  Created by Ryoutarou Kishi on 2009/06/11.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GameMenuPanelFactory.h"

// Friends
#include "GameMenuCreateParam.h"
#include "../window/menu/MenuWindow.h"
#include "../window/menu/implement/DebugMenuWindow.h"
#include "../window/menu/implement/FocusMenuWindow.h"
#include "../window/menu/implement/MainMenuWindow.h"
#include "../window/menu/implement/SourceListMenuWindow.h"
#include "../window/menu/implement/ItemListMenuWindow.h"
#include "../window/menu/implement/QuestListMenuWindow.h"
#include "../window/menu/implement/SubMenuWindow.h"
#include "../window/menu/implement/TitleMenuWindow.h"
#include "../../body/Game.h"
#include "../../gui/GameFontSet.h"
#include "../../gui/GameGui.h"

// External
#include "../../../tfw/collection/Tree.h"
#include "../../../tfw/smenu/MenuTreeNode.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
	パネル作成（Menu クラスからメニューパネルを作成するときに呼ばれる）
**//*---------------------------------------------------------------------*/
PanelBase* GameMenuPanelFactory::makePanel(TreeNode<MenuTreeNode>* tnodeSrc, int depth, bool isCustomChild, void* objShowParam, void* objCreateParam)
{
	GameMenuCreateParam* gmcprm = (GameMenuCreateParam*)objCreateParam;
	
	// テクスチャを得る
	Texture* tex = 0L;
	switch(gmcprm->_case)
	{
	///case GameMenuCreateParam::CASE_DEBUG:
	case GameMenuCreateParam::CASE_TITLE:
		break;
	default:
		tex = Game::getGame()->getGui()->getGuiTexture();
		break;
	}

	// ウインドウを作成する
	MenuWindow* wnd = 0L;
	if(isCustomChild)
	{
		VcString nameChild;
		if(tnodeSrc->getChild() != 0L)	{	nameChild = *tnodeSrc->getChild()->getObject()->getName();	}

		if(	false )
		{
			///wnd = new CharStatusMenuWindow();
		}
		else
		{
			wnd = new MainMenuWindow();
		}
	}
	else
	{
		switch(gmcprm->_case)
		{
		case GameMenuCreateParam::CASE_DEBUG:			wnd = new DebugMenuWindow();		break;
		case GameMenuCreateParam::CASE_INGAME_FOCUS:	wnd = new FocusMenuWindow();		break;
		case GameMenuCreateParam::CASE_INGAME_MAIN:		wnd = new MainMenuWindow();			break;
		case GameMenuCreateParam::CASE_INGAME_SRCL:		wnd = new SourceListMenuWindow();	break;
		case GameMenuCreateParam::CASE_INGAME_ITEML:	wnd = new ItemListMenuWindow();		break;
		case GameMenuCreateParam::CASE_INGAME_QSTL:		wnd = new QuestListMenuWindow();	break;
		case GameMenuCreateParam::CASE_INGAME_SUB:		wnd = new SubMenuWindow();			break;
		case GameMenuCreateParam::CASE_TITLE:			wnd = new TitleMenuWindow();		break;
		default:										wnd = new MenuWindow();				break;
		}
	}
	ASSERT(wnd != 0L);
	if(!wnd->create(tnodeSrc, depth, Game::getGame()->getFontSet()->getFont(GameFontSet::JP), 16, tex, objShowParam, objCreateParam))
	{
		return 0L;
	}
	wnd->showWindow(true);
	return wnd;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
