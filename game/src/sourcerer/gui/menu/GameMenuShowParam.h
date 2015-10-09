/***********************************************************************//**
 *  GameMenuShowParam.h
 *  Enlight Game Application
 *
 *  Created by Ryoutarou Kishi on 2009/11/21.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_MENU_GAME_MENU_SHOW_PARAM_H_
#define _SRCR_GUI_MENU_GAME_MENU_SHOW_PARAM_H_

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

class Unit;

/*---------------------------------------------------------------------*//**
 *	ゲームメニュー作成パラメータ
 *	
**//*---------------------------------------------------------------------*/
class GameMenuShowParam
{
	//======================================================================
	// メソッド
public:
	GameMenuShowParam()
		: _xBase(0.0f)
		, _yBase(0.0f)
		, _wMax(0.0f)
		, _hMax(0.0f)
		, _unitRef(0L)
		, _ownerRef(0L)
	{
	}

	//======================================================================
	// 変数
public:
	f32			_xBase;
	f32			_yBase;
	f32			_wMax;
	f32			_hMax;
	Unit*		_unitRef;
	void*		_ownerRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_MENU_GAME_MENU_SHOW_PARAM_H_
