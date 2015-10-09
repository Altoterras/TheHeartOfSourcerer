/***********************************************************************//**
 *  GameMenuCreateParam.h
 *  Enlight Game Application
 *
 *  Created by Ryoutarou Kishi on 2009/11/21.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_MENU_GAME_MENU_CREATE_PARAM_H_
#define _SRCR_GUI_MENU_GAME_MENU_CREATE_PARAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{
	namespace smenu
	{
	}
}
using namespace smenu;

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ゲームメニュー作成パラメータ
 *	
**//*---------------------------------------------------------------------*/
class GameMenuCreateParam
{
	//======================================================================
	// 定数
public:
	enum Case
	{
		CASE_NULL,
		CASE_DEBUG,
		CASE_INGAME_FOCUS,
		CASE_INGAME_MAIN,
		CASE_INGAME_SRCL,
		CASE_INGAME_ITEML,
		CASE_INGAME_QSTL,
		CASE_INGAME_SUB,
		CASE_TITLE,
	};

	//======================================================================
	// メソッド
public:
	GameMenuCreateParam()
		: _case(CASE_NULL)
		, _owner(0L)
	{
	}

	//======================================================================
	// 変数
public:
	Case		_case;
	void*		_owner;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_MENU_GAME_MENU_CREATE_PARAM_H_
