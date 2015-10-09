/***********************************************************************//**
 *	StoryconfMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StoryconfMatter.h"

// Friends
#include "../EventMatterCreateParam.h"
#include "../EventMatterType.h"
#include "../OverrideEventMatter.h"
#include "../../EventSys.h"
#include "../../AutoBeginEventInfo.h"
#include "../../../episode/StoryManager.h"
#include "../../../body/Game.h"
#include "../../../map/PlacementObj.h"
#include "../../../save/SaveSys.h"

// External
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StoryconfMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StoryconfMatter::StoryconfMatter()
	: EventMatter(EMTYPE_STORYCONF)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StoryconfMatter::~StoryconfMatter()
{
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void StoryconfMatter::destroy()
{
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool StoryconfMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
{
	if(!EventMatter::setupFromXmlNode(node, cparam))	{	return false;	}

	StoryManager* strymng = Game::getGame()->getStoryManager();	ASSERT(strymng != 0L);

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// ストーリーコンフィグ

	s32 iwk;

	s32 evidStartup = 0;
	if(XmlParserUtils::analyzeChildData(&iwk, node, "StartupEvent") != 0L)	{	evidStartup = iwk;	}
	strymng->setup(evidStartup);

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
void StoryconfMatter::analyzePlaceObj(const XmlNode* node, u8 subid, const EventMatterCreateParam* emcp)
{
	StoryManager* strymng = Game::getGame()->getStoryManager();	ASSERT(strymng != 0L);

	// 配置情報を作成する
	PlacementObj* pobj = new PlacementObj();
	if(pobj == 0L)
	{
		// 失敗
		return;
	}
	pobj->setupFromXmlNode(node, PlacementObj::UCASE_STORY, subid, emcp);	// XML ノードからセットアップ

	// ストーリーに設定追加
	strymng->addPlacementObjSetting(pobj);
}

/*---------------------------------------------------------------------*//**
	上書きイベントの解析
**//*---------------------------------------------------------------------*/
void StoryconfMatter::analyzeOverrideEvent(const XmlNode* node, const EventMatterCreateParam* emcp)
{
	StoryManager* strymng = Game::getGame()->getStoryManager();	ASSERT(strymng != 0L);
	
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

	// ストーリーに設定追加
	strymng->addOverrideEvent(oem);
}

/*---------------------------------------------------------------------*//**
	自動起動イベントの解析
**//*---------------------------------------------------------------------*/
void StoryconfMatter::analyzeAutoBeginEvent(const XmlNode* node, const EventMatterCreateParam* emcp)
{
	StoryManager* strymng = Game::getGame()->getStoryManager();	ASSERT(strymng != 0L);
	
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

	// ストーリーに設定追加
	strymng->addAutoBeginEvent(abevi);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
