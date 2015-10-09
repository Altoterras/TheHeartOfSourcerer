/***********************************************************************//**
 *	ScrMode.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2015/05/20.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ScrMode.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../EventData.h"
#include "../../../../EventSys.h"
#include "../../../../../body/Game.h"

// External
#include "../../../../../../etk/effect/ScreenEffect.h"
#include "../../../../../../tfw/lib/XmlParser.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ScrMode メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ScrMode::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// 動作設定解析
	if(!ScrModeMatter::analyzeSettingFromXml(&_setting, node))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void ScrMode::play(ActMatter* owner)
{
	EventData* evdat = owner->getEventSystemRefer()->getEventData(); ASSERT(evdat != 0L);

	// スクリーンエフェクト開始
	_evid = evdat->addChildMatter(owner->getEvId(), "SCRMODE"); ASSERT(_evid != 0);
	ScrModeMatter* matter = (ScrModeMatter*)evdat->getMatterFromEvid(_evid); ASSERT(matter != 0L);
	matter->setting(&_setting);	// 動作設定
	// イベント開始
	EvCause evcause(EventCause::CID_ACT);
	owner->getEventSystemRefer()->begin(_evid, owner->getBeginParam(), &evcause);

	Act::play(owner);
}

/*---------------------------------------------------------------------*//**
	動作停止
**//*---------------------------------------------------------------------*/
void ScrMode::stop(ActMatter* owner)
{
	EventData* evdat = owner->getEventSystemRefer()->getEventData();	ASSERT(evdat != 0L);

	// マターを削除する
	if(_evid != 0)
	{
		evdat->removeMatter(_evid);
		_evid = 0;
	}

	Act::stop(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
