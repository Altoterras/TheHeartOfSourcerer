/***********************************************************************//**
 *	OverrideEventMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/01.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "OverrideEventMatter.h"

// Friends
#include "EventMatterCreateParam.h"
#include "../../body/Game.h"
#include "../../body/MoveMap.h"

// External
#include "../../../tfw/lib/XmlParser.h"
#include "../../../tfw/lib/XmlParserUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// OverrideEventMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	XML からセットアップ
**//*---------------------------------------------------------------------*/
bool OverrideEventMatter::setupFromXmlNode(const XmlNode* node, const EventMatterCreateParam* emcp)
{
	VcString strwk;
	s32 iwk;

	// ［必須］タイプ
	OverrideEventMatter::Kind kind = OverrideEventMatter::KIND_NULL;
	if(!XmlParserUtils::analyzeAtrb(&strwk, node, "kind"))	{	return false;	}
	if(strwk.equalsIgnoreCase("map"))
	{
		kind = OverrideEventMatter::KIND_MAP;
	}

	// ［オプション］置換前のイベント ID
	s32 evidOld = 0;
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "old_evid"))
	{
		evidOld = iwk;
		if(emcp != 0L) { evidOld = emcp->offsetEvid(evidOld); }	// イベント ID のオフセット処理
	}

	// ［オプション］置換後のイベント ID
	s32 evidNew = 0;
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "new_evid"))
	{
		evidNew = iwk;
		if(emcp != 0L) { evidNew = emcp->offsetEvid(evidNew); }	// イベント ID のオフセット処理
	}

	// ［オプション］マップ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "map_id"))
	{
		_mapid = iwk;
	}

	// 置換情報を作成する
	if(!setup(kind, evidOld, evidNew, 0))	{	return false;	}

	return true;
}

/*---------------------------------------------------------------------*//**
	条件判定
**//*---------------------------------------------------------------------*/
bool OverrideEventMatter::isCondition() const
{
	// マップ ID を判定する
	if(_mapid != 0)
	{
		if(Game::getGame()->getMoveMap()->getCurMapId() != _mapid)
		{
			return false;
		}
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
OverrideEventMatter::OverrideEventMatter()
	: _mapid(0)
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
