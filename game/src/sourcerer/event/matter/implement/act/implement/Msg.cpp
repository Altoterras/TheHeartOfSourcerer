/***********************************************************************//**
 *	Msg.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Msg.h"

// Friends
#include "../../ActMatter.h"
#include "../../TalkMatter.h"
#include "../../../../EventData.h"
#include "../../../../EventSys.h"
#include "../../../../../body/Game.h"

// External
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Msg メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool Msg::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// 動作設定解析
	if(!TalkMatter::analyzeSettingFromXml(&_setting, node, emcp))
	{
		return false;
	}

	// 一時停止設定
	if((_setting._kind == TalkMatter::KIND_TALK) || (_setting._kind == TalkMatter::KIND_YESNO))
	{
		TFW_SET_FLAG(_flags, F_PAUSE, true);
		_duration = -1;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void Msg::play(ActMatter* owner)
{
	EventData* evdat = owner->getEventSystemRefer()->getEventData();	ASSERT(evdat != 0L);

	// メッセージを表示する
	_evid = evdat->addChildMatter(owner->getEvId(), "TALK");	ASSERT(_evid != 0);
	TalkMatter* matter = (TalkMatter*)evdat->getMatterFromEvid(_evid); ASSERT(matter != 0L);
	matter->setting(&_setting, evdat->getMsgDataSet());	// 動作設定
	// イベント開始
	EvCause evcause(EventCause::CID_ACT);
	owner->getEventSystemRefer()->begin(_evid, owner->getBeginParam(), &evcause);

	Act::play(owner);
}

/*---------------------------------------------------------------------*//**
	動作停止
**//*---------------------------------------------------------------------*/
void Msg::stop(ActMatter* owner)
{
	EventData* evdat = owner->getEventSystemRefer()->getEventData(); ASSERT(evdat != 0L);

	// メッセージを終了する
	TalkMatter* matter = (TalkMatter*)evdat->getMatterFromEvid(_evid); ASSERT(matter != 0L);
	matter->terminate();
	// マターを削除する
//	evdat->removeMatter(_evid);

	Act::stop(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
