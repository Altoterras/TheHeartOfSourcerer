/***********************************************************************//**
 *	PrpzlMenuRole.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_PUREPUZZLE_MENU_PRPZL_MENU_ROLE_H_
#define _SRCR_MODE_PUREPUZZLE_MENU_PRPZL_MENU_ROLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"

#include "../../../../tfw/modemng/Role.h"
#include "../../../../tfw/smenu/MenuFuncTable.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Panel;
class Texture;
namespace smenu
{
	class Menu;
}

TFW_END_NS
using namespace smenu;

SRCR_BEGIN_NS

class Game;
class GameMenuShowParam;
class PrpzlOperationData;

/*---------------------------------------------------------------------*//**
 *	パズル メニュー ロール
 *
**//*---------------------------------------------------------------------*/
class PrpzlMenuRole : public Role
{
	//======================================================================
	// 定数
public:
	enum CommandType
	{
		STABLE = 1,
	};

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	メニュー機能テーブル
	 *	
	**//*-----------------------------------------------------------------*/
	class MenuFuncTable_PrpzlMainMenu : public smenu::MenuFuncTable
	{
		//==================================================================
		// メソッド
	public:
		void onShow(Menu* menu, MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam);
		void onHide(Menu* menu, void* objCreateParam, void* objShowParam);
		bool onCall(Menu* menu, const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);
	};

	//======================================================================
	// メソッド
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PrpzlMenuRole(PrpzlOperationData* opdatRef);
	~PrpzlMenuRole();
	bool onClickNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);

protected:
	void notifyBegin();
	void notifyEnd();
	int execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	void drawCommand(const RenderCtx* rc, const CmdParam* param);

private:
	bool createMenu();
	void destroyMenu();
	int execCommand_stable(ExecRes* res, const ExecCtx* ec, const CmdParam* param);

	//======================================================================
	// 変数
private:
	PrpzlOperationData*		_opdatRef;
	Texture*				_texBg;	
	Menu*					_menu;
	GameMenuShowParam*		_msprm;
	bool					_isRecreateMenuReserved;

};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_PUREPUZZLE_MENU_PRPZL_MENU_ROLE_H_
