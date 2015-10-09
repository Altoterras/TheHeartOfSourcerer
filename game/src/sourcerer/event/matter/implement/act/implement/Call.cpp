/***********************************************************************//**
 *	Call.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/08.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Call.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../EventData.h"
#include "../../../../EventSys.h"

// External
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Call メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool Call::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// ［必須］ジャンプ先イベント ID
	s32 evidJump;
	if(!XmlParserUtils::analyzeAtrb(&evidJump, node, "evid"))
	{
		return false;
	}
	_evidJump = evidJump;
	if(emcp != 0L) { _evidJump = emcp->offsetEvid(_evidJump); }	// イベント ID のオフセット処理

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void Call::play(ActMatter* owner)
{
	EventData* evdat = owner->getEventSystemRefer()->getEventData();	ASSERT(evdat != 0L);

	// イベント開始
	if(_evidJump != 0)
	{
		EvCause evcause(EventCause::CID_ACT);
		owner->getEventSystemRefer()->begin(_evidJump, owner->getBeginParam(), &evcause);
	}

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
