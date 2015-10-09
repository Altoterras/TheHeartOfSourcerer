/*
 *  MenuTreeNode.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/06/02.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
 */

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MenuTreeNode.h"

// Friends
#include "MenuPanelReceptor.h"

// External
#include "../string/VcString.h"
#include "../string/StringUtils.h"
#include "../lib/XmlParser.h"

// Library
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS
SMENU_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MenuTreeNode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	名前の設定
**//*---------------------------------------------------------------------*/
void MenuTreeNode::setName(const VcString* name)
{
	_name->set(name);
}

/*---------------------------------------------------------------------*//**
	汎用値の設定
**//*---------------------------------------------------------------------*/
void MenuTreeNode::setValue(const VcString* valu)
{
	_value->set(valu);
}

/*---------------------------------------------------------------------*//**
	説明文の設定
**//*---------------------------------------------------------------------*/
void MenuTreeNode::setExplain(const VcString* str)
{
	_explain->set(str);
}

/*---------------------------------------------------------------------*//**
	現在のパネルの取得
**//*---------------------------------------------------------------------*/
PanelBase* MenuTreeNode::getPanel() const
{
	if(_pnlrcp == 0L)		{	return 0L;	}
	return _pnlrcp->getPanel();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MenuTreeNode::MenuTreeNode()
	: _id(0)
	, _name(0L)
	, _value(0L)
	, _explain(0L)
	, _pnlrcp(0L)
	, _flagsSys(0)
	, _flagsGeneral(0)
	, _paramGeneral(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MenuTreeNode::~MenuTreeNode()
{
	ASSERT(_name == 0L);
	ASSERT(_value == 0L);
	ASSERT(_explain == 0L);
	ASSERT(_pnlrcp == 0L);
}
	
/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MenuTreeNode::create(const XmlNode* node)
{
	// 動的変数作成
	_name = new VcString();
	_value = new VcString();
	_explain = new VcString();

	// 汎用パラメータ取得
	{
		// ID
		const VcString* strAtrbValue = node->findAttribute("ID");
		if((strAtrbValue != 0L) && (strAtrbValue->getLength() > 0))
		{
			_id = StringUtils::toInteger(strAtrbValue);
		}
		// Value
		strAtrbValue = node->findAttribute("Value");
		if((strAtrbValue != 0L) && (strAtrbValue->getLength() > 0))
		{
			_value->set(strAtrbValue);
		}
		// Expain
		strAtrbValue = node->findAttribute("Explain");
		if((strAtrbValue != 0L) && (strAtrbValue->getLength() > 0))
		{
			_explain->set(strAtrbValue);
		}
		// Debug
		strAtrbValue = node->findAttribute("Debug");
		if((strAtrbValue != 0L) && (strAtrbValue->getLength() > 0))
		{
			bool isDebug = StringUtils::toBoolean(strAtrbValue);
			TFW_SET_FLAG(_flagsSys, SF_DEBUG, isDebug);
		}
		// Enabled
		strAtrbValue = node->findAttribute("Enabled");
		if((strAtrbValue != 0L) && (strAtrbValue->getLength() > 0))
		{
			bool isEnable = StringUtils::toBoolean(strAtrbValue);
			setDisabled(!isEnable);
		}
		// Checked
		strAtrbValue = node->findAttribute("Checked");
		if((strAtrbValue != 0L) && (strAtrbValue->getLength() > 0))
		{
			bool isChecked = StringUtils::toBoolean(strAtrbValue);
			setChecked(isChecked);
		}
		// Hidden
		strAtrbValue = node->findAttribute("Hidden");
		if((strAtrbValue != 0L) && (strAtrbValue->getLength() > 0))
		{
			bool isHidden = StringUtils::toBoolean(strAtrbValue);
			setHidden(isHidden);
		}
	}
	
	// タグ別パラメータ取得
	if(node->getName()->equals("Function"))
	{
		TFW_SET_FLAG(_flagsSys, SF_FUNCTION, true);
		
		const VcString* strAtrbValue = node->findAttribute("MenuClose");
		if((strAtrbValue != 0L) && strAtrbValue->equals("yes"))
		{
			TFW_SET_FLAG(_flagsSys, SF_MENU_CLOSE, true);
		}
	}
	else if((node->getChildNode() != 0L) && (node->getChildNode()->getType() == XmlParser::XMLNODETYPE_PCDATA) && node->getName()->equals("CustomChild"))
	{
		TFW_SET_FLAG(_flagsSys, SF_CUSTOM_CHILD, true);
	}
	
	_pnlrcp = new MenuPanelReceptor();
	
	return true;
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MenuTreeNode::create(const CStringBase* name, bool isFunction, bool isMenuClose, bool isCustomChild, u32 flagsGeneral, u64 paramGeneral)
{
	// 動的変数作成
	_name = new VcString();
	_value = new VcString();
	_explain = new VcString();

	_name->set(name);
	_flagsGeneral = flagsGeneral;
	_paramGeneral = paramGeneral;
	TFW_SET_FLAG(_flagsSys, SF_FUNCTION, isFunction);
	TFW_SET_FLAG(_flagsSys, SF_MENU_CLOSE, isMenuClose);
	TFW_SET_FLAG(_flagsSys, SF_CUSTOM_CHILD, isCustomChild);
	_pnlrcp = new MenuPanelReceptor();
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MenuTreeNode::destroy()
{
	delete _name;
	_name = 0L;
	delete _value;
	_value = 0L;
	delete _explain;
	_explain = 0L;
	delete _pnlrcp;
	_pnlrcp = 0L;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void MenuTreeNode::copy(const MenuTreeNode* src)
{
	_id = src->_id;
	_name->set(src->_name);
	_value->set(src->_value);
	_explain->set(src->_explain);
	_flagsSys = src->_flagsSys;
	_flagsGeneral = src->_flagsGeneral;
	_paramGeneral = src->_paramGeneral;
}

////////////////////////////////////////////////////////////////////////////

SMENU_END_NS
TFW_END_NS