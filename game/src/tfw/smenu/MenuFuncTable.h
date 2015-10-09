/***********************************************************************//**
 *  MenuFuncTable.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SMENU_MENU_FUNC_TABLE_H_
#define _TFW_SMENU_MENU_FUNC_TABLE_H_


////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "Base.h"
#include "../framemod/FrameExecDraw.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS
class ExecCtx;

SMENU_BEGIN_NS
class Menu;
class MenuTreeNode;

TFW_INTERFACE MenuFuncTable
{
	//======================================================================
	// メソッド
public:
	virtual void onShow(Menu* menu, MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam) {}
	virtual void onHide(Menu* menu, void* objCreateParam, void* objShowParam) {}
	virtual bool onCall(Menu* menu, const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam) { return false; }
	virtual void onClickOutside(Menu* menu, const ExecCtx* ec, void* objCreateParam, void* objShowParam) {}
};

SMENU_END_NS
TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_SMENU_MENU_FUNC_TABLE_H_
