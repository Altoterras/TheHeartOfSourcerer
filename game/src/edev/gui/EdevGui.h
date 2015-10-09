/***********************************************************************//**
 *	EdevGui.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _EDEV_GUI_EDEV_GUI_H_
#define _EDEV_GUI_EDEV_GUI_H_

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

EDEV_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	GUI（2D 表示物）
 *
**//*---------------------------------------------------------------------*/
class EdevGui : public EtkGui
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EdevGui();
	~EdevGui();
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
};

EDEV_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _EDEV_GUI_EDEV_GUI_H_
