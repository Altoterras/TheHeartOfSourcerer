/***********************************************************************//**
 *	AutoBeginEventInfo.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "AutoBeginEventInfo.h"

// Friends
#include "EventIdInfoArray.h"
#include "matter/EventMatterCreateParam.h"

// External
#include "../../tfw/lib/XmlParser.h"
#include "../../tfw/lib/XmlParserUtils.h"

// Library

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	XML からセットアップ
**//*---------------------------------------------------------------------*/
bool AutoBeginEventInfo::setupFromXmlNode(const XmlNode* node, const EventMatterCreateParam* emcp)
{
	VcString strwk;
	s32 iwk;
	
	// ［必須］タイプ
	if(!XmlParserUtils::analyzeAtrb(&strwk, node, "kind"))
	{
		return false;
	}
	if(strwk.equalsIgnoreCase("map_start"))
	{
		_kind = AutoBeginEventInfo::KIND_MAP_START;
	}

	// ［必須］起動イベント ID
	EventIdInfoArray* eiiarr = new EventIdInfoArray();
	CcString cstrCallEvid("call_evid");
	if(!eiiarr->setupFromXmlNode(node, &cstrCallEvid, emcp))
	{
		delete eiiarr;
		return false;
	}
	_eiiarr = eiiarr;

	// ［オプション］マップ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "map_id"))
	{
		_mapid = iwk;
	}

	// ［オプション］true のとき実行する条件セーブフラグ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "cond_sfid_p"))
	{
		_sfidCondP = iwk;
	}

	// ［オプション］false のとき実行する条件セーブフラグ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "cond_sfid_n"))
	{
		_sfidCondN = iwk;
	}

	// ［オプション］実行後に true を書き込むセーブフラグ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "write_sfid_p"))
	{
		_sfidWriteP = iwk;
	}

	// ［オプション］実行後に false を書き込むセーブフラグ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "write_sfid_n"))
	{
		_sfidWriteN = iwk;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ストーリー開始時の自動起動イベント情報としてからセットアップ
**//*---------------------------------------------------------------------*/
bool AutoBeginEventInfo::setupForStoryStart(u16 evid)
{
	_kind = KIND_STORY_START;
	_eiiarr = new EventIdInfoArray();
	if(_eiiarr == 0L)
	{
		return false;
	}
	_eiiarr->setupOneEvent(evid, 0, 0);
	return true;
}

/*---------------------------------------------------------------------*//**
	イベント途中データロード時の自動起動イベント情報としてからセットアップ
**//*---------------------------------------------------------------------*/
bool AutoBeginEventInfo::setupForLoadIntervalEventStart(u16 evid)
{
	_kind = KIND_LOAD_INTVEV_START;
	_eiiarr = new EventIdInfoArray();
	if(_eiiarr == 0L)
	{
		return false;
	}
	_eiiarr->setupOneEvent(evid, 0, 0);
	return true;
}

/*---------------------------------------------------------------------*//**
	呼び出しイベント情報を次に送る
**//*---------------------------------------------------------------------*/
void AutoBeginEventInfo::nextCallEvi(bool isAutoRewind)
{
	_idxCallEvi++;
	if(_eiiarr == 0L)
	{
		_idxCallEvi = 0;
	}
	else if(_idxCallEvi >= _eiiarr->getInfoNum())
	{
		_idxCallEvi = isAutoRewind ? 0 : _eiiarr->getInfoNum() - 1;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
AutoBeginEventInfo::~AutoBeginEventInfo()
{
	delete _eiiarr;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
