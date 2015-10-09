/***********************************************************************//**
 *	IntervalSave.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/04/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "IntervalSave.h"

// Friends
#include "../../ActMatter.h"
#include "../../../EventMatterType.h"
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
// IntervalSave メソッド

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
IntervalSave::~IntervalSave()
{
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool IntervalSave::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}
	
	// 動作設定解析
	if(!IntvlSaveMatter::analyzeSettingFromXml(&_setting, node))
	{
		return false;
	}

	// 一時停止およびスキップ強制停止設定
	TFW_SET_FLAG(_flags, F_PAUSE | F_SKIPSTOP, true);
	_duration = -1;

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void IntervalSave::play(ActMatter* owner)
{
	EventData* evdat = owner->getEventSystemRefer()->getEventData();			ASSERT(evdat != 0L);

	// 途中セーブを再生する
	_evid = evdat->addChildMatter(owner->getEvId(), "INTVLSAVE");	ASSERT(_evid != 0);
	EvMatter* matter = evdat->getMatterFromEvid(_evid);
	if(matter != 0L)
	{
		matter->setPreserveParam(owner->getPreserveParam());	// プログラムでの取得・解析用に ActMatter の保持パラメータを渡す
		if(matter->getMatterType() == EMTYPE_INTVLSAVE)
		{
			((IntvlSaveMatter*)matter)->setting(&_setting);	// 動作設定
		}
	}
	// イベント開始
	EvCause evcause(EventCause::CID_ACT);
	owner->getEventSystemRefer()->begin(_evid, owner->getBeginParam(), &evcause);

	Act::play(owner);
}

/*---------------------------------------------------------------------*//**
	動作停止
**//*---------------------------------------------------------------------*/
void IntervalSave::stop(ActMatter* owner)
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
