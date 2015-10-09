/***********************************************************************//**
 *	QuestconfMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/26.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QuestconfMatter.h"

// Friends
#include "../EventMatterCreateParam.h"
#include "../EventMatterType.h"
#include "../OverrideEventMatter.h"
#include "../../AutoBeginEventInfo.h"
#include "../../EventSys.h"
#include "../../../body/Game.h"
#include "../../../map/PlacementObj.h"
#include "../../../quest/QuestManager.h"
#include "../../../save/SaveSys.h"

// External
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QuestconfMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QuestconfMatter::QuestconfMatter()
	: EventMatter(EMTYPE_QUESTCONF)
{
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool QuestconfMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
{
	if(!EventMatter::setupFromXmlNode(node, cparam))	{	return false;	}

	QuestManager* qstmng = Game::getGame()->getQuestManager();	ASSERT(qstmng != 0L);

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// ストーリーコンフィグ

//	s32 iwk;

//	s32 evidStartup = 0;
//	if(XmlParserUtils::analyzeChildData(&iwk, node, "StartupEvent") != 0L)	{	evidStartup = iwk;	}
//	qstmng->setup(evidStartup);

	s32 cntPobj = 1;

	const XmlNode* nodeChild = node->getChildNode();
	while(nodeChild != 0L)
	{
		// 配置オブジェクト解析
		if(nodeChild->getName()->equals("PlaceObj"))
		{
			analyzePlaceObj(nodeChild, (u8)cntPobj++, (const EventMatterCreateParam*)cparam);
		}
		// オーバーライドイベント解析
		else if(nodeChild->getName()->equals("OverrideEvent"))
		{
			analyzeOverrideEvent(nodeChild, (const EventMatterCreateParam*)cparam);
		}
		// 自動起動イベント解析
		else if(nodeChild->getName()->equals("AutoBeginEvent"))
		{
			analyzeAutoBeginEvent(nodeChild, (const EventMatterCreateParam*)cparam);
		}
		
		nodeChild = nodeChild->getSiblingNode();
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	配置オブジェクトの解析
**//*---------------------------------------------------------------------*/
void QuestconfMatter::analyzePlaceObj(const XmlNode* node, u8 subid, const EventMatterCreateParam* emcp)
{
	QuestManager* qstmng = Game::getGame()->getQuestManager();	ASSERT(qstmng != 0L);

	// 配置情報を作成する
	PlacementObj* pobj = new PlacementObj();
	if(pobj == 0L)
	{
		// 失敗
		return;
	}
	pobj->setupFromXmlNode(node, PlacementObj::UCASE_QUEST, subid, emcp);	// XML ノードからセットアップ

	// ストーリーに設定追加
	qstmng->addPlacementObjSetting(pobj, emcp->_questid);
}

/*---------------------------------------------------------------------*//**
	上書きイベントの解析
**//*---------------------------------------------------------------------*/
void QuestconfMatter::analyzeOverrideEvent(const XmlNode* node, const EventMatterCreateParam* emcp)
{
	QuestManager* qstmng = Game::getGame()->getQuestManager();	ASSERT(qstmng != 0L);

	// 置換情報を作成する
	OverrideEventMatter* oem = new OverrideEventMatter();
	if(oem == 0L)
	{
		// 失敗
		return;
	}
	if(!oem->setupFromXmlNode(node, emcp))
	{
		// 失敗
		delete oem;
		return;
	}

	// 識別キーを設定する
	oem->setDescKey(qstmng->convQuestIdToDescKey(emcp->_questid));

	// ストーリーに設定追加
	qstmng->addOverrideEvent(oem);
}

/*---------------------------------------------------------------------*//**
	自動起動イベントの解析
**//*---------------------------------------------------------------------*/
void QuestconfMatter::analyzeAutoBeginEvent(const XmlNode* node, const EventMatterCreateParam* emcp)
{
	QuestManager* qstmng = Game::getGame()->getQuestManager();	ASSERT(qstmng != 0L);
	
	// 自動起動イベント情報を作成する
	AutoBeginEventInfo* abevi = new AutoBeginEventInfo();
	if(abevi == 0L)
	{
		// 失敗
		return;
	}
	if(!abevi->setupFromXmlNode(node, emcp))	// XML ノードからセットアップ
	{
		// 失敗
		delete abevi;
		return;
	}

	// 識別キーを設定する
	abevi->setDescKey(qstmng->convQuestIdToDescKey(emcp->_questid));

	// ストーリーに設定追加
	qstmng->addAutoBeginEvent(abevi);
}

#if 0
/*---------------------------------------------------------------------*//**
	達成情報の解析
**//*---------------------------------------------------------------------*/
void QuestconfMatter::analyzeAchievement(const XmlNode* node, const EventMatterCreateParam* emcp)
{
	QuestManager* qstmng = Game::getGame()->getQuestManager();	ASSERT(qstmng != 0L);

	VcString strwk;
//	bool bwk;
	s32 iwk;
//	Vector3F vwk;

	u8 achvkind = QuestArticle::ACHVKIND_CUSTOM;	// 達成種別（デフォルトはカスタム）
	u16 didAchv = 0;								// 達成対象定義 ID

	// ［必須］達成タイプ
	strwk.emptyEasy();
	if(!XmlParserUtils::analyzeAtrb(&strwk, node, "kind"))
	{
		// 失敗
		return;
	}
	if(strwk.equalsIgnoreCase("hand_over_item"))	{	achvkind = QuestArticle::ACHVKIND_HAND_OVER_ITEM;	}

	if(achvkind == QuestArticle::ACHVKIND_HAND_OVER_ITEM)
	{
		// ［オプション］アイテム ID
		if(XmlParserUtils::analyzeAtrb(&iwk, node, "item_id"))
		{
			didAchv = (u16)iwk;
		}
	}

	// クエストオブジェクトに設定する
	QuestArticle* qsta = qstmng->questFromId(emcp->_questid);
	if(qsta != 0L)
	{
		qsta->checkAchievementImmediately();
	}
}
#endif

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
