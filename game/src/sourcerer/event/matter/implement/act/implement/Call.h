/***********************************************************************//**
 *	Call.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/08.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CALL_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CALL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	イベント呼び出し Act
 *	
**//*---------------------------------------------------------------------*/
class Call : public Act
{
public:
	u16				_evidJump;		// ［必須］ジャンプ先イベント ID

public:
	Call() : Act(ACT_IF), _evidJump(0) {}
	bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_CALL_H_
