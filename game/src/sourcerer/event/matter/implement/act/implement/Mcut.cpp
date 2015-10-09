/***********************************************************************//**
 *	Mcut.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Mcut.h"

// Friends
#include "../../ActMatter.h"
#include "../../MovieCutMatter.h"
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
// Mcut メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool Mcut::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// 動作設定解析
	if(!MovieCutMatter::analyzeSettingFromXml(&_setting, node))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void Mcut::play(ActMatter* owner)
{
	EventData* evdat = owner->getEventSystemRefer()->getEventData(); ASSERT(evdat != 0L);

	// ムービーカット定義開始
	_evid = evdat->addChildMatter(owner->getEvId(), "MOVIECUT"); ASSERT(_evid != 0);
	MovieCutMatter* vfmatter = (MovieCutMatter*)evdat->getMatterFromEvid(_evid); ASSERT(vfmatter != 0L);
	vfmatter->setting(&_setting);	// 動作設定
	// イベント開始
	EvCause evcause(EventCause::CID_ACT);
	owner->getEventSystemRefer()->begin(_evid, owner->getBeginParam(), &evcause);

	Act::play(owner);
}

/*---------------------------------------------------------------------*//**
	動作停止
**//*---------------------------------------------------------------------*/
void Mcut::stop(ActMatter* owner)
{
	EventData* evdat = owner->getEventSystemRefer()->getEventData(); ASSERT(evdat != 0L);

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
