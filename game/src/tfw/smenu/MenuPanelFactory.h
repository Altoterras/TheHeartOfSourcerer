/***********************************************************************//**
 *  MenuPanelFactory.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SMENU_MENU_PANEL_FACTORY_H_
#define _TFW_SMENU_MENU_PANEL_FACTORY_H_

#include "../Base.h"
#include "Base.h"

#include "../framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS
template<class TYPE> class TreeNode;
class PanelBase;

SMENU_BEGIN_NS
class MenuTreeNode;

/*---------------------------------------------------------------------*//**
	メニューパネル ファクトリ インターフェイス
	
**//*---------------------------------------------------------------------*/
TFW_INTERFACE MenuPanelFactory
{
	//======================================================================
	// メソッド
public:
	virtual ~MenuPanelFactory() {}
	virtual PanelBase* makePanel(TreeNode<MenuTreeNode>* tnodeSrc, int depth, bool isCustomChild, void* objShowParam, void* objCreateParam) = 0;
};

SMENU_END_NS
TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_SMENU_MENU_PANEL_FACTORY_H_
