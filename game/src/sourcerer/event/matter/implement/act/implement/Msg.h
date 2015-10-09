/***********************************************************************//**
 *	Msg.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_MSG_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_MSG_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"
#include "../../TalkMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	メッセージ表示 Act
 *	
**//*---------------------------------------------------------------------*/
class Msg : public Act
{
public:
	TalkMatter::Setting _setting;	// 動作設定

public:
	Msg() : Act(ACT_MSG) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
	void stop(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_IF_H_
