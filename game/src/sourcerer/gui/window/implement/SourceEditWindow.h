/***********************************************************************//**
 *	SourceEditWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_SOURCE_EDIT_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_SOURCE_EDIT_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../etk/gui/window/Window.h"
#include "../../../../tfw/smenu/MenuFuncTable.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Font;
class Keyboard;
class Texture;
class TxtBox;
namespace smenu
{
	class Menu;
}

TFW_END_NS
using namespace smenu;

SRCR_BEGIN_NS

class GameMenuShowParam;
class HeaderPanel;
class ScrollBar;
class SoftKeyboardWindow;

/*---------------------------------------------------------------------*//**
 *	メッセージウインドウ
 *
**//*---------------------------------------------------------------------*/
class SourceEditWindow : public Window
{
	//======================================================================
	// 定数
public:
	enum ActMode
	{
		ACTMODE_NULL,
		ACTMODE_LIVE,
		ACTMODE_EDIT,
		ACTMODE_LOOK,
	};

private:
	enum Button
	{
		BTN_CLOSE,
		BTN_SLIST,
		NUM_BTN
	};

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	メニュー機能テーブル
	 *	
	**//*-----------------------------------------------------------------*/
	class MenuFuncTable_SourceEdit : public smenu::MenuFuncTable
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
	// アクセサ
public:
	// モードを得る
	inline ActMode getActMode() const { return _actmode; }
	// 終了済みかを得る
	inline bool isDone() const { return _isDone; }
	// 入力完了したかを得る
	inline bool isInputDone() const { return _isInputDone; }
	// リストクリックによる完了かどうかを得る
	inline bool isListClickDone() const { return _isListClickDone; }
	// ヘッダパネルを得る
	inline HeaderPanel*	headerPanel() { return _hpnlHbar; }
	// ソフトウェアキーボードを得る
	inline SoftKeyboardWindow* softKeyboardWindow() { return _wndSoftkb; }
	// ソースを変更したかどうかを得る
	bool isChanged() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void takeSourceText(VcString* src) const;
	bool setup(ActMode actmode, const VcString* src);
	void trimSourceText();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SourceEditWindow();
	~SourceEditWindow();
	bool create(Font* fontRef, Texture* texRef);
	void destroy();
	void execContents(ExecRes* res, const ExecCtx* ec);
	bool execKeyboard(ExecRes* res, const Keyboard* kbd);
	void drawContents(const RenderCtx* rc, const RectF32* rectBase);
	void notifyChangeLanguage();

private:
	void copyText();
	void pasteText();
	void showOsTextField();
	void hideOsTextField();
	bool createSubMenu();
	void destroySubMenu();

private:
	void onShowWindow(bool isShow);
	bool onClickSubMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);

	//======================================================================
	// メンバ変数
private:
	Texture* _texRef;
	HeaderPanel* _hpnlHbar;
	SoftKeyboardWindow* _wndSoftkb;
	TxtBox* _txtbox;
	ScrollBar* _scrlbar;
	Menu* _menuSub;
	GameMenuShowParam* _msprmSub;
	bool _isDone;
	bool _isInputDone;
	bool _isListClickDone;
	u8 _skfmmodeLast;
	ActMode _actmode;
	bool _isLastSentReturnKey;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_SOURCE_EDIT_WINDOW_H_
