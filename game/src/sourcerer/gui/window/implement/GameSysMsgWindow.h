/***********************************************************************//**
 *	GameSysMsgWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_GAME_SYS_MSG_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_GAME_SYS_MSG_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../etk/gui/window/implement/SysMsgWindow.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	メッセージウインドウ
 *
**//*---------------------------------------------------------------------*/
class GameSysMsgWindow : public SysMsgWindow
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	bool create(Font* fontRef, Texture* texRef);

protected:
	void onShowWindow(bool isShow);
	void notifyChangeLanguage();
	void notifyChangeLogicalScreenSize();

private:
	void complyChangeScreenSize();
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_GAME_SYS_MSG_WINDOW_H_
