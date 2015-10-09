/***********************************************************************//**
 *	MenuMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_INGAME_MENU_MENU_MODE_H_
#define _SRCR_MODE_INGAME_MENU_MENU_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../GameMode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

namespace smenu
{
	class Menu;
}

TFW_END_NS
using namespace smenu;

ETK_BEGIN_NS

class Window;

ETK_END_NS

SRCR_BEGIN_NS

class GameMenuShowParam;
class HeaderPanel;

/*---------------------------------------------------------------------*//**
 *	インゲーム メニュー モード
 *
**//*---------------------------------------------------------------------*/
class MenuMode : public GameModeBase
{
	//======================================================================
	// 定数
private:
	enum SmallMode
	{
		SMODE_TOP,
		SMODE_SOURCE,
		SMODE_ITEM,
		SMODE_STATUS,
		SMODE_ABILITY,
		SMODE_EQUIP,
		SMODE_QUEST,
		SMODE_SAVE,
		SMODE_OPTIONS,
	};

	enum ChildWnd
	{
		CWND_CHARSTAT,
		CWND_EQUIP,
		CWND_ABILITY,
		CWND_QUEST,
		CWND_SAVE,
		CWND_OPTIONS,
		NUM_CWND
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void onCallMenuSource();
	void onCallMenuItem();
	void onCallMenuStatus();
	void onCallMenuAbility();
	void onCallMenuEquip();
	void onCallMenuQuest();
	void onCallMenuSave();
	void onCallMenuOptions();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MenuMode();
	~MenuMode();
	bool create();
	void exec(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);

protected:
	bool notifyBegin(void* objParamRef);
	void notifyEnd(void* objParamRef);

private:
	bool appearChidWindow(ChildWnd cwnd);
	void disappearChildWindow(ChildWnd cwnd);

	//======================================================================
	// メンバ変数
private:
	SmallMode				_smode;
	Menu*					_menu;
	GameMenuShowParam*		_showprm;
	HeaderPanel*			_hpnlHbar;
	Window*					_cwnd[NUM_CWND];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_INGAME_MENU_MENU_MODE_H_
