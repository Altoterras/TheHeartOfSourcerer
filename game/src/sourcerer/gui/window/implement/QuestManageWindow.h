/***********************************************************************//**
 *	QuestManageWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_QUEST_MANAGE_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_QUEST_MANAGE_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../etk/gui/window/Window.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class QuestListWindow;

/*---------------------------------------------------------------------*//**
 *	クエスト管理ウインドウ
 *	
**//*---------------------------------------------------------------------*/
class QuestManageWindow : public Window
{
	//======================================================================
	// 定数
private:
	enum Button
	{
		BTN__DUMMY_,
		NUM_BTN
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QuestManageWindow();
	~QuestManageWindow();
	bool create(Font* fontRef, Texture* texRef);
	void destroy();
	void execContents(ExecRes* res, const ExecCtx* ec);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);

protected:
	virtual void onShowWindow(bool isShow);

	//======================================================================
	// 変数
private:
	QuestListWindow* _wndQstList;
	Panel* _pnlBtn[NUM_BTN];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_QUEST_MANAGE_WINDOW_H_
