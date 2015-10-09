/***********************************************************************//**
 *	QuestconfMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/26.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_QUESTCONF_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_QUESTCONF_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../EventMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class XmlNode;

TFW_END_NS

SRCR_BEGIN_NS

class EventMatterCreateParam;

/*---------------------------------------------------------------------*//**
 *	クエスト設定マター
 *	
**//*---------------------------------------------------------------------*/
class QuestconfMatter : public EventMatter
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QuestconfMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);

private:
	void analyzePlaceObj(const XmlNode* node, u8 subid, const EventMatterCreateParam* emcp);
	void analyzeOverrideEvent(const XmlNode* node, const EventMatterCreateParam* emcp);
	void analyzeAutoBeginEvent(const XmlNode* node, const EventMatterCreateParam* emcp);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_QUESTCONF_MATTER_H_
