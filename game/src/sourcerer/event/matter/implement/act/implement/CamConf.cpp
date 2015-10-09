/***********************************************************************//**
 *	CamConf.cpp
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
#include "CamConf.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../../body/Game.h"

// External
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CamConf メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool CamConf::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// ［オプション］補間するかどうか
	bool isIntp;
	if(XmlParserUtils::analyzeAtrb(&isIntp, node, "intp"))
	{
		_isIntp = isIntp;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void CamConf::play(ActMatter* owner)
{
	ASSERT(false);	// 未実装

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
