/***********************************************************************//**
 *	If.cpp
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
#include "If.h"

// Friends
#include "../../ActMatter.h"
#include "../../../EventMatterPresvParam.h"
#include "../../../../EventSys.h"
#include "../../../../../body/Game.h"
#include "../../../../../episode/Situation.h"
#include "../../../../../episode/StoryManager.h"
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
// If メソッド

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool If::analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp)
{
	if(!Act::analyze(owner, node, emcp))
	{
		return false;
	}

	s32 iwk;
	bool bwk;
	VcString strwk;

	// ［必須］ジャンプ先イベント ID
	if(!XmlParserUtils::analyzeAtrb(&iwk, node, "jump_evid"))
	{
		return false;
	}
	_evidJump = (u16)iwk;
	if(emcp != 0L) { _evidJump = emcp->offsetEvid(_evidJump); }	// イベント ID のオフセット処理

	// ［オプション］シチュエーション ID カテゴリ 01
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "situ_flag_c01"))
	{
		_situfc01 = (u16)iwk;
	}

	// ［オプション］言語
	if(XmlParserUtils::analyzeAtrb(&strwk, node, "lang"))
	{
		if(strwk.equalsIgnoreCase("en"))
		{
			_lang = LANGID_EN;
		}
		else if(strwk.equalsIgnoreCase("jp"))
		{
			_lang = LANGID_JP;
		}
	}

	// ［オプション］セーブデータ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "sfid"))
	{
		_sfid = iwk;
	}

	// ［オプション］指定ストーリー ID 以上
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "story_above"))
	{
		_storyidAbove = (u16)iwk;
	}

	// ［オプション］指定ストーリー ID 以下
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "story_under"))
	{
		_storyidUnder = (u16)iwk;
	}

	// ［オプション］クエスト状態（クエスト用）
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "quest_stat"))
	{
		_queststat = (u8)iwk;
	}

	// ［オプション］クエスト汎用値識別番号（クエスト用；１以上の値の場合は IF 条件合格）
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "quest_gval"))
	{
		_questgval = (u8)iwk;
	}

	// ［オプション］条件判定を反転させる
	if(XmlParserUtils::analyzeAtrb(&bwk, node, "negative"))
	{
		_negative = bwk;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	動作開始
**//*---------------------------------------------------------------------*/
void If::play(ActMatter* owner)
{
	StoryManager* strymng = Game::getGame()->getStoryManager();			ASSERT(strymng != 0L);
	Situation* situ = strymng->situation();								ASSERT(situ != 0L);

	bool isCondTrue = true;

	// シチュエーション判定
	if(_situfc01 != 0)
	{
		u16 situfc01Now = situ->getCurrentSituationFlags(1);
		if(!TFW_IS_FLAG(_situfc01, situfc01Now))
		{
			isCondTrue = false;
		}
	}

	// 言語判定
	if(_lang != -1)
	{
		if(isCondTrue)
		{
			isCondTrue = (_lang == Env_getLangId());
		}
	}

	// セーブデータ ID 判定
	if(_sfid != 0)
	{
		SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
		if(!svd->getSaveF(_sfid))
		{
			isCondTrue = false;
		}
	}

	// ストーリー ID 判定
	if(_storyidAbove != 0)
	{
		if(strymng->getCurrentStoryId() < _storyidAbove)
		{
			isCondTrue = false;
		}
	}
	if(_storyidUnder != 0)
	{
		if(strymng->getCurrentStoryId() > _storyidUnder)
		{
			isCondTrue = false;
		}
	}

	// クエスト状態判定
	if(_queststat != 0)
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
			if(_queststat != qsta->getStat())
			{
				isCondTrue = false;
			}
		}
		while(false);
	}

	// クエスト汎用値判定
	if(_questgval != 0)
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
			if(qsta->getGeneralValue(_questgval - 1) == 0.0f)	// 0 は不合格
			{
				isCondTrue = false;
			}
		}
		while(false);
	}

	// 条件判定反転
	if(_negative)
	{
		isCondTrue = !isCondTrue;
	}

	// ● 条件が一致したジャンプしてこのイベントを中断
	if(isCondTrue)
	{
		// 分岐イベント開始
		if(_evidJump != 0)	// 0 は単なる強制終了として正当
		{
			EvCause evcause(EventCause::CID_ACT);
			owner->getEventSystemRefer()->begin(_evidJump, owner->getBeginParam(), &evcause);
		}
		// 強制終了フラグを立てる
		TFW_SET_FLAG(_flags, F_TERMINATE, true);
	}

	Act::play(owner);
}

/*---------------------------------------------------------------------*//**
	動作停止
**//*---------------------------------------------------------------------*/
void If::stop(ActMatter* owner)
{
	// 強制終了フラグを戻す
	TFW_SET_FLAG(_flags, F_TERMINATE, false);

	Act::stop(owner);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
