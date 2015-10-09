/***********************************************************************//**
 *	AtArGui.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_GUI_AT_AR_GUI_H_
#define _ATAR_GUI_AT_AR_GUI_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../etk/gui/EtkGui.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Texture;
class FedTaskSys;

TFW_END_NS

ETK_BEGIN_NS

class FontSet;

ETK_END_NS

ATAR_BEGIN_NS

class HelpWindow;
class IngameFront;

/*---------------------------------------------------------------------*//**
 *	GUI（2D 表示物）
 *
**//*---------------------------------------------------------------------*/
class AtArGui : public EtkGui
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:
	inline Texture*				getGuiTexture() const		{	return _texGui;					}
	inline IngameFront*			getIngameFront() const		{	return _igfr;					}
	inline HelpWindow*			getHelpWindow() const		{	return _wndHelp;				}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool isOpenedLockGui() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	AtArGui();
	~AtArGui();
	bool create(FontSet* fontsetRef, FedTaskSys* tasksys);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec) {}

	void notifyChangeLanguage();

protected:
	SysMsgWindow* makeSysMsgWindow();
	ActionMsgWindow* makeActionMsgWindow();

	//======================================================================
	// 変数
private:
	Texture*			_texGui;				// GUI 用テクスチャ

	IngameFront*		_igfr;
	HelpWindow*			_wndHelp;
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_GUI_AT_AR_GUI_H_
