/***********************************************************************//**
 *	TexFacial.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/18.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TexFacial.h"

// Friends
#include "../../ActMatter.h"
#include "../../TexFacialMatter.h"
#include "../../../../EventData.h"
#include "../../../../EventSys.h"
#include "../../../../../body/Game.h"

// External
#include "../../../../../../tfw/lib/Rect.h"
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TexFacial メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool TexFacial::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}
	
	// 動作設定解析
	if(!TexFacialMatter::analyzeSettingFromXml(&_setting, node))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void TexFacial::play(ActMatter* owner)
{
	EventData* evdat = owner->getEventSystemRefer()->getEventData(); ASSERT(evdat != 0L);

	// スプライト表示
	_evid = evdat->addChildMatter(owner->getEvId(), "TEXFACIAL"); ASSERT(_evid != 0);
	TexFacialMatter* matter = (TexFacialMatter*)evdat->getMatterFromEvid(_evid); ASSERT(matter != 0L);
	matter->setting(&_setting);	// 動作設定
	// イベント開始
	EvCause evcause(EventCause::CID_ACT);
	owner->getEventSystemRefer()->begin(_evid, owner->getBeginParam(), &evcause);

	Game::getGame()->setWaitFrame();	// ゲームへ読み込み負荷のためのウェイトフレーム設定

	Act::play(owner);
}

/*---------------------------------------------------------------------*//**
	動作停止
**//*---------------------------------------------------------------------*/
void TexFacial::stop(ActMatter* owner)
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
