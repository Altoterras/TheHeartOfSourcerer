/***********************************************************************//**
 *	ChangeStory.cpp
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
#include "ChangeStory.h"

// Friends
#include "../../ActMatter.h"
#include "../../../../../body/Game.h"
#include "../../../../../episode/StoryManager.h"

// External
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ChangeStory メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ChangeStory::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	// ［必須］ストーリー ID
	if(!XmlParserUtils::analyzeAtrb(&_storyid, node, "story_id"))
	{
		return false;
	}

	// スキップ確認
	//	※	この Act はスキップすると致命的なバグになりかねないので、
	//		デフォルトでは、スキップを防止すべきと考える
	ASSERTM(TFW_IS_FLAG(_flags, F_NOSKIP | F_SKIPONLY | F_SKIPSTOP), "{ChangeStory::analyze} Illegal skip-flag setting of ChangeStory-Act.");
	if(!TFW_IS_FLAG(_flags, F_NOSKIP | F_SKIPONLY | F_SKIPSTOP))
	{
		TFW_SET_FLAG(_flags, F_NOSKIP, true);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void ChangeStory::play(ActMatter* owner)
{
	StoryManager* strymng = Game::getGame()->getStoryManager();				ASSERT(strymng != 0L);
	if(strymng->getCurrentStoryId() != _storyid)
	{
		strymng->reserveNextStory(_storyid);
	}

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
