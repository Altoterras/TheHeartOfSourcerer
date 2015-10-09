/***********************************************************************//**
 *	StoryconfMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_STORYCONF_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_STORYCONF_MATTER_H_

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
 *	ストーリー設定マター
 *	
**//*---------------------------------------------------------------------*/
class StoryconfMatter : public EventMatter
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StoryconfMatter();
	~StoryconfMatter();
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void destroy();

private:
	void analyzePlaceObj(const XmlNode* node, u8 subid, const EventMatterCreateParam* emcp);
	void analyzeOverrideEvent(const XmlNode* node, const EventMatterCreateParam* emcp);
	void analyzeAutoBeginEvent(const XmlNode* node, const EventMatterCreateParam* emcp);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_STORYCONF_MATTER_H_
