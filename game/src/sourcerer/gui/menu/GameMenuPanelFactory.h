/***********************************************************************//**
 *  GameMenuPanelFactory.h
 *  Enlight Game Application
 *
 *  Created by Ryoutarou Kishi on 2009/06/11.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_MENU_GAME_MENU_PANEL_FACTORY_H_
#define _SRCR_GUI_MENU_GAME_MENU_PANEL_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
#include "../../../tfw/smenu/MenuPanelFactory.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{
	template<class TYPE> class TreeNode;
	class PanelBase;
	namespace smenu
	{
		class MenuTreeNode;
	}
}
using namespace smenu;

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	メニューパネル ファクトリ
 *	
**//*---------------------------------------------------------------------*/
class GameMenuPanelFactory : public MenuPanelFactory
{
	//======================================================================
	// メソッド
public:
	virtual PanelBase* makePanel(TreeNode<MenuTreeNode>* tnodeSrc, int depth, bool isCustomChild, void* objShowParam, void* objCreateParam);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_MENU_GAME_MENU_PANEL_FACTORY_H_
