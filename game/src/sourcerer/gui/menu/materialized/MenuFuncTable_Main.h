/***********************************************************************//**
 *  MenuFuncTable_Main.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/11/23.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_MENU_MATERIALIZED_MENU_FUNC_TABLE_MAIN_H_
#define _SRCR_GUI_MENU_MATERIALIZED_MENU_FUNC_TABLE_MAIN_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../tfw/smenu/MenuFuncTable.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	デバッグ メニュー機能テーブル
 *	
**//*---------------------------------------------------------------------*/
class MenuFuncTable_Main : public smenu::MenuFuncTable
{
	//======================================================================
	// メソッド
public:
	void onShow(smenu::Menu* menu, smenu::MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam);
	void onHide(smenu::Menu* menu, void* objCreateParam, void* objShowParam);
	bool onCall(smenu::Menu* menu, const ExecCtx* ec, smenu::MenuTreeNode* mtnodeCall, smenu::MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_GUI_MENU_MATERIALIZED_MENU_FUNC_TABLE_MAIN_H_