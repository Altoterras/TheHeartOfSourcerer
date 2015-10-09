/***********************************************************************//**
 *	ChangeMap.cpp
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
#include "ChangeMap.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../../body/Game.h"
#include "../../../../../body/MoveMap.h"

// External
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ChangeMap メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ChangeMap::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// ［必須］マップ ID
	if(!XmlParserUtils::analyzeAtrb(&_mapid, node, "map_id"))
	{
		return false;
	}

	// ［オプション］次のフレームで変更
	XmlParserUtils::analyzeAtrb(&_isRsvChange, node, "next_frame_change");

	// ［オプション］同じマップ番号などでも強制的にマップ読込処理を行う
	XmlParserUtils::analyzeAtrb(&_isForceChange, node, "force_change");

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void ChangeMap::play(ActMatter* owner)
{
	MoveMap* mvmap = Game::getGame()->getMoveMap();
	if((mvmap->getCurMapId() != _mapid) || _isForceChange)
	{
		if(_isRsvChange)
		{
			mvmap->reserveChangeMap(_mapid);
		}
		else
		{
			mvmap->changeMap(_mapid, 0);
		}
	}

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
