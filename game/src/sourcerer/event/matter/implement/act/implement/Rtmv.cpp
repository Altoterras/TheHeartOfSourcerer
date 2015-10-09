/***********************************************************************//**
 *	Rtmv.cpp
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
#include "Rtmv.h"

// Friends
#include "../../ActMatter.h"
#include "../../RealtimeMovieMatter.h"
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
// Rtmv メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool Rtmv::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// 動作設定解析
	if(!RealtimeMovieMatter::analyzeSettingFromXml(&_fileid, node))
	{
		return false;
	}

	// 継続フレームを強制設定
	_duration = -1;

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void Rtmv::play(ActMatter* owner)
{
	EventData* evdat = owner->getEventSystemRefer()->getEventData(); ASSERT(evdat != 0L);

	// リアルタイムムービーを再生する
	_evid = evdat->addChildMatter(owner->getEvId(), "RTMOVIE"); ASSERT(_evid != 0);
	RealtimeMovieMatter* matter = (RealtimeMovieMatter*)evdat->getMatterFromEvid(_evid); ASSERT(matter != 0L);
	matter->setting(_fileid);	// 動作設定
	// イベント開始
	EvCause evcause(EventCause::CID_ACT);
	owner->getEventSystemRefer()->begin(_evid, owner->getBeginParam(), &evcause);

	Game::getGame()->setWaitFrame();	// ゲームへ読み込み負荷のためのウェイトフレーム設定

	Act::play(owner);
}

/*---------------------------------------------------------------------*//**
	動作停止
**//*---------------------------------------------------------------------*/
void Rtmv::stop(ActMatter* owner)
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
