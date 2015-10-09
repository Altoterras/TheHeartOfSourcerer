/***********************************************************************//**
 *	MapconfMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_MAPCONF_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_MAPCONF_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../EventMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;
class XmlNode;

TFW_END_NS

ETK_BEGIN_NS

class MessageWindow;

ETK_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	マップ設定マター
 *	
**//*---------------------------------------------------------------------*/
class MapconfMatter : public EventMatter
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部制御
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MapconfMatter();
	~MapconfMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void destroy();

	//======================================================================
	// 変数
	
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_MAPCONF_MATTER_H_
