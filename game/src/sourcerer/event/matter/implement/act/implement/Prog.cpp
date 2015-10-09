/***********************************************************************//**
 *	Prog.cpp
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
#include "Prog.h"

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
// Prog メソッド

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Prog::~Prog()
{
	delete _name;
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool Prog::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// ［必須］プログラム名
	VcString name;
	if(!XmlParserUtils::analyzeAtrb(&name, node, "name"))
	{
		return false;
	}
	_name = new VcString(name);
	
	// 動作設定解析
	if(!ProgMatter::analyzeSettingFromXml(&_setting, node))
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
void Prog::play(ActMatter* owner)
{
	EventData* evdat = owner->getEventSystemRefer()->getEventData();	ASSERT(evdat != 0L);

	// カスタムプログラムを再生する
	ASSERT(_name != 0L);
	_evid = evdat->addChildMatter(owner->getEvId(), _name);				ASSERT(_evid != 0);
	EvMatter* matter = evdat->getMatterFromEvid(_evid);
	if(matter != 0L)
	{
		matter->setPreserveParam(owner->getPreserveParam());	// プログラムでの取得・解析用に ActMatter の保持パラメータを渡す
		if(matter->getMatterType() == EMTYPE_PROG)
		{
			((ProgMatter*)matter)->setting(&_setting);	// 動作設定
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
void Prog::stop(ActMatter* owner)
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
