/***********************************************************************//**
 *	BasicModelGui.h
 *	Enlight BasicModel Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _BASICMODEL_GUI_BASICMODEL_GUI_H_
#define _BASICMODEL_GUI_BASICMODEL_GUI_H_

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

BASICMODEL_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	GUI（2D 表示物）
 *
**//*---------------------------------------------------------------------*/
class BasicModelGui : public EtkGui
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
	BasicModelGui();
	~BasicModelGui();
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

BASICMODEL_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _BASICMODEL_GUI_BASICMODEL_GUI_H_
