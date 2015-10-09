/***********************************************************************//**
 *	Flag.cpp
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
#include "Flag.h"

// Friends
#include "../../ActMatter.h"
#include "../../../EventMatterPresvParam.h"
#include "../../../../../body/Game.h"
#include "../../../../../quest/QuestArticle.h"
#include "../../../../../quest/QuestManager.h"
#include "../../../../../save/SaveData.h"
#include "../../../../../save/SaveSys.h"

// External
#include "../../../../../../tfw/lib/XmlParser.h"
#include "../../../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Flag メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool Flag::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	s32 iwk;

	// ［オプション（いずれか必須）］表示か非表示か
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "sfid"))
	{
		_sfid = iwk;
	}

	// ［オプション（いずれか必須）］クエスト汎用値識別番号（クエスト用）
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "quest_gval"))
	{
		_questgval = iwk;
	}

	// ［オプション］新しい値
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "value"))
	{
		_value = iwk;
	}

	// スキップ確認
	//	※	この Act はスキップすると致命的なバグになりかねないので、
	//		デフォルトでは、スキップを防止すべきと考える
	ASSERTM(TFW_IS_FLAG(_flags, F_NOSKIP | F_SKIPONLY | F_SKIPSTOP), "{Flag::analyze} Illegal skip-flag setting of Flag-Act.");
	if(!TFW_IS_FLAG(_flags, F_NOSKIP | F_SKIPONLY | F_SKIPSTOP))
	{
		TFW_SET_FLAG(_flags, F_NOSKIP, true);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void Flag::play(ActMatter* owner)
{
	SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);

	// フラグセット
	if(_sfid != 0L)
	{
		svd->setSaveF(_sfid, _value != 0);
	}
	else if(_questgval != 0)
	{
		do
		{
			QuestManager* qstmng = Game::getGame()->getQuestManager();
			if(qstmng == 0L)										{	break;	}
			EventMatterPresvParam* empp = reinterpret_cast<EventMatterPresvParam*>(owner->getPreserveParam());
			if(empp == 0L)											{	break;	}
			if(empp->_kind != EventMatterPresvParam::KIND_QUEST)	{	break;	}
			QuestArticle* qsta = (QuestArticle*)empp->_d;
			if(qsta == 0L)											{	break;	}
			qsta->setGeneralValue(_questgval - 1, (f32)_value);
		}
		while(false);
	}
	else
	{
		ASSERT(false);
	}

	Act::play(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
