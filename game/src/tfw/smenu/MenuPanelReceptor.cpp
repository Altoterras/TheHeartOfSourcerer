/***********************************************************************//**
 *  MenuPanelReceptor.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MenuPanelReceptor.h"

// Friends
#include "MenuExecRes.h"

// External
#include "../gui/PanelBase.h"
#include "../string/VcString.h"
#include "../lib/XmlParser.h"

// Library
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS
SMENU_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラスメソッド

//==========================================================================
// MenuPanelReceptor メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MenuPanelReceptor::MenuPanelReceptor() : _panelCur(0L), _isAvailable(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MenuPanelReceptor::~MenuPanelReceptor()
{
	ASSERT(_panelCur == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MenuPanelReceptor::create(PanelBase* panel)
{
	_panelCur = panel;
	_panelCur->onCreatePanel();
	
	_isAvailable = true;
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MenuPanelReceptor::destroy()
{
	ASSERT(_isAvailable);
	_isAvailable = false;
	
	_panelCur->onDestroyPanel();
	delete _panelCur;
	_panelCur = 0L;
}
	
/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void MenuPanelReceptor::exec(ExecRes* res, const ExecCtx* ec)
{
	// 制御
	MenuExecRes* cres = (MenuExecRes*)res;
	_panelCur->exec(cres, ec);
	
	// 終了通知
	if((res != 0L) && res->isDone())
	{
		res->setCancel();
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void MenuPanelReceptor::draw(const RenderCtx* rc, bool isCurrent)
{
	if(_panelCur != 0L)
	{
		_panelCur->setFront(isCurrent);
		_panelCur->draw(rc);
	}
}

////////////////////////////////////////////////////////////////////////////

SMENU_END_NS
TFW_END_NS