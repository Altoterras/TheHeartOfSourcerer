/***********************************************************************//**
 *	ScoreUp.cpp
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
#include "ScoreUp.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../EventSys.h"
#include "../../../../../save/ScoreBase.h"
#include "../../../../../body/Game.h"

// External
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ScoreUp メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ScoreUp::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	s32 iwk;

	// ［必須］表示か非表示か
	if(!XmlParserUtils::analyzeAtrb(&iwk, node, "score_count"))
	{
		return false;
	}
	_sc = iwk;

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void ScoreUp::play(ActMatter* owner)
{
	// スコアアップ
	ScoreBase* score = Game::getGame()->getScore(); ASSERT(score != 0L);
	score->upScore(_sc);

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
