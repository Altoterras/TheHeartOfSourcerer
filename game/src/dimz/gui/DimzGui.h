/***********************************************************************//**
 *	DimzGui.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _DIMZ_GUI_DIMZ_GUI_H_
#define _DIMZ_GUI_DIMZ_GUI_H_

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

DIMZ_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	GUI（2D 表示物）
 *
**//*---------------------------------------------------------------------*/
class DimzGui : public EtkGui
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
	DimzGui();
	~DimzGui();
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

DIMZ_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _DIMZ_GUI_DIMZ_GUI_H_
