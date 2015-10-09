/***********************************************************************//**
 *	EventData.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/28.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EventData.h"

// Friends
#include "EventIdDef.h"
#include "MsgDataKind.h"
#include "matter/EventMatter.h"
#include "matter/EventMatterCreateParam.h"
#include "matter/EventMatterFactory.h"
#include "matter/OverrideEventMatter.h"
#include "../file/GameFileIdDef.h"
#include "../quest/QuestArticle.h"
#include "../quest/QuestDef.h"

// External
#include "../../tfw/collection/List.h"
#include "../../tfw/evsys/MsgDataSet.h"
#include "../../tfw/file/IdFile.h"
#include "../../tfw/lib/XmlParser.h"
#include "../../tfw/string/VcString.h"
#include "../../tfw/string/VcStringArray.h"
#include "../../tfw/string/StringUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス
	
/*---------------------------------------------------------------------*//**
	システム固定イベントの読み込み
**//*---------------------------------------------------------------------*/
bool EventData::loadSystemFixedEvent()
{
	// イベントデータ読み込み
	if(!createMatterListFromXml(SYSTEM_SYSTEMFIXED_EV_XML, 0, 0L))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	システム固定イベントの破棄
**//*---------------------------------------------------------------------*/
void EventData::unloadSystemFixedEvent()
{
	TRACE("{EventData::unloadSystemFixedEvent}\n");
	unloadMatter(EventIdDef::EVID_FIXED__START_, EventIdDef::EVID_FIXED__END_, 0);
}

/*---------------------------------------------------------------------*//**
	マップイベントの読み込み

	@param	mapid		マップ ID
**//*---------------------------------------------------------------------*/
bool EventData::loadMapEvent(s32 mapid)
{
	// メッセージデータ読み込み
	if(!loadMessageData(MsgDataKind::MSGDKIND_MAP, FILEID_FROM_MAP_EVENT_MSGD(mapid), 0))
	{
		//return false;
	}

	// イベントデータ読み込み
	if(!createMatterListFromXml(FILEID_FROM_MAP_EVENT_XML(mapid), 0, 0L))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	マップイベントの破棄
**//*---------------------------------------------------------------------*/
void EventData::unloadMapEvent()
{
	TRACE("{EventData::unloadMapEvent}\n");
	unloadMatter(EventIdDef::EVID_MAP__START_, EventIdDef::EVID_MAP__END_, 0);
	_msgdatset->removeMsgDataRange(MsgDataKind::MSGDKIND_MAP);
}

/*---------------------------------------------------------------------*//**
	ストーリーイベントの読み込み

	@param	storyid		ストーリー ID
**//*---------------------------------------------------------------------*/
bool EventData::loadStoryEvent(s32 storyid)
{
	if(storyid <= 0)	{	return false;	}

	// メッセージデータ読み込み
	if(!loadMessageData(MsgDataKind::MSGDKIND_STORY, FILEID_FROM_STORY_EVENT_MSGD(storyid), 0))
	{
		return false;
	}

	// イベントデータ読み込み
	if(!createMatterListFromXml(FILEID_FROM_STORY_EVENT_XML(storyid), 0, 0L))
	{
		return false;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	ストーリーイベントの破棄
**//*---------------------------------------------------------------------*/
void EventData::unloadStoryEvent()
{
	TRACE("{EventData::unloadStoryEvent}\n");
	unloadMatter(EventIdDef::EVID_STORY__START_, EventIdDef::EVID_STORY__END_, 0);
	_msgdatset->removeMsgDataRange(MsgDataKind::MSGDKIND_STORY);
}

/*---------------------------------------------------------------------*//**
	クエストイベントの読み込み

	@param	questid		クエスト ID
**//*---------------------------------------------------------------------*/
bool EventData::loadQuestEvent(const QuestArticle* qsta)
{
	if(qsta->getDef()->getQuestId() <= 0)	{	return false;	}

	s32 msgkind = MsgDataKind::MSGDKIND_QUEST__START_ + qsta->getQuestSlot() - 1;
	u32 caseid = EventIdDef::CASEID_QUEST__START_ + qsta->getQuestSlot();
	u16 ofsMsgid = (u16)(qsta->getQuestSlot() * EventIdDef::MSGID_QUEST_SLOT_MULT);
	u16 ofsEvid = (u16)(qsta->getQuestSlot() * EventIdDef::EVID_QUEST_SLOT_MULT);

	// メッセージデータ読み込み
	if(!loadMessageData(msgkind, FILEID_FROM_QUEST_EVENT_MSGD(qsta->getDef()->getQuestId()), ofsMsgid))
	{
		return false;
	}

	// イベントデータ読み込み
	EventMatterCreateParam cparam;
	cparam._ofsMsgid = ofsMsgid;
	cparam._ofsEvid = ofsEvid;
	cparam._evidOfsRangeStart = EventIdDef::EVID_QUEST__START_;
	cparam._evidOfsRangeEnd = EventIdDef::EVID_QUEST__END_ + 1;	// End は含まれないため +1 する
	cparam._questid = qsta->getDef()->getQuestId();
	if(!createMatterListFromXml(
		FILEID_FROM_QUEST_EVENT_XML(qsta->getDef()->getQuestId()),
		reinterpret_cast<addr_size>(qsta->getEventMatterPresvParam()),
		&cparam)	)
	{
		return false;
	}

	return false;

}

/*---------------------------------------------------------------------*//**
	クエストイベントの破棄

	@param	questid		クエスト ID
**//*---------------------------------------------------------------------*/
void EventData::unloadQuestEvent(const QuestArticle* qsta)
{
	TRACE("{EventData::unloadQuestEvent}\n");

	s32 msgkind = MsgDataKind::MSGDKIND_QUEST__START_ + qsta->getQuestSlot() - 1;
	u16 evidQuestStart = EventIdDef::EVID_QUEST__START_ + (u16)(qsta->getQuestSlot() * EventIdDef::EVID_QUEST_SLOT_MULT);
	u16 evidQuestEnd = evidQuestStart + 99;

	unloadMatter(evidQuestStart, evidQuestEnd, 0);
	_msgdatset->removeMsgDataRange(msgkind);
}

/*---------------------------------------------------------------------*//**
	テンポラリイベントの読み込み

	@param	tmpevid		テンポラリイベント ID
**//*---------------------------------------------------------------------*/
bool EventData::loadTmpEvent(s32 tmpevid)
{
	// メッセージデータ読み込み
	if(!loadMessageData(MsgDataKind::MSGDKIND_TMP, FILEID_FROM_TMP_EVENT_MSGD(tmpevid), 0L))
	{
		return false;
	}

	// イベントデータ読み込み
	if(!createMatterListFromXml(FILEID_FROM_TMP_EVENT_XML(tmpevid), 0, 0L))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	テンポラリイベントの破棄
**//*---------------------------------------------------------------------*/
void EventData::unloadTmpEvent()
{
	TRACE("{EventData::unloadTmpEvent}\n");
	unloadMatter(EventIdDef::EVID_TMP__START_, EventIdDef::EVID_TMP__END_, 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	マター読み込み
**//*---------------------------------------------------------------------*/
EvMatter* EventData::createMatter(const XmlNode* nodeEventMatter, const EvMatterCreateParam* cparam)
{
	EventMatter* matter = (EventMatter*)EvData::createMatter(nodeEventMatter, cparam);
	if(matter == 0L)	{	return 0L;	}

	const VcString* strwk;
	VcStringArray strarrwk;

	// 条件
	int fidSaveData = 0;
	int fidPlayData = 0;
	int nidSaveData = 0;
	int sdnKey = 0;
	int flags = 0;
	const XmlNode* nodeCondition = nodeEventMatter->findChildNode("Condition");
	if(nodeCondition != 0L)
	{
		if(getMatterAttribute(&strwk, nodeCondition, "SaveDataFlagNegaID"))			{	fidSaveData = (int)StringUtils::toInteger(strwk);																		}
		if(getMatterAttribute(&strwk, nodeCondition, "SaveDataFlagPosiID"))			{	fidSaveData = (int)StringUtils::toInteger(strwk);					flags |= EventMatter::CondFlags::FLG_SDF_POSITIVE;	}
		if(getMatterAttribute(&strwk, nodeCondition, "PlayDataFlagNegaID"))			{	fidPlayData = (int)StringUtils::toInteger(strwk);																		}
		if(getMatterAttribute(&strwk, nodeCondition, "PlayDataFlagPosiID"))			{	fidPlayData = (int)StringUtils::toInteger(strwk);					flags |= EventMatter::CondFlags::FLG_PDF_POSITIVE;	}
		if(getMatterAttribute(&strwk, nodeCondition, "SaveDataFlagTurn"))			{	flags |= (StringUtils::toBoolean(strwk) ? EventMatter::CondFlags::FLG_SDF_TURN : 0);									}
		if(getMatterAttribute(&strwk, nodeCondition, "PlayDataFlagTurn"))			{	flags |= (StringUtils::toBoolean(strwk) ? EventMatter::CondFlags::FLG_PDF_TURN : 0);									}
		if(getMatterAttribute(&strwk, nodeCondition, "SaveDataNumberID"))			{	nidSaveData = (int)StringUtils::toInteger(strwk);																		}
		if(getMatterAttribute(&strwk, nodeCondition, "BeginMapStart"))				{	flags |= (StringUtils::toBoolean(strwk) ? EventMatter::CondFlags::FLG_AUTOBGN_MAPSTART : 0);							}
		if(getMatterAttributeArray(&strarrwk, 2, nodeCondition, "SaveDataNumber"))	{	nidSaveData = (int)StringUtils::toInteger(strarrwk.get(0));	sdnKey = (int)StringUtils::toInteger(strarrwk.get(1));		}
	}

	// 追加条件設定
	EventMatter::CondFlags condf(fidSaveData, fidPlayData, nidSaveData, sdnKey, flags);
	matter->setCondFlags(&condf);

	// イベント ID のオフセット処理
	matter->offsetEventId((const EventMatterCreateParam*)cparam);
	
	return matter;
}

/*---------------------------------------------------------------------*//**
	自動割り当てのイベント ID を取得する

	@return	イベント ID（0 は不正な ID）
**//*---------------------------------------------------------------------*/
u16 EventData::getUnusedAutoEventId()
{
	u16 evidAuto = EventIdDef::EVID_AUTO__START_;
	for(u32 i = 0; i < _listMatter->getCount(); i++)
	{
		EventMatter* matter = (EventMatter*)_listMatter->getObject(i);
		u16 evid = matter->getEvId();
		if(evid >= evidAuto)
		{
			if(evid >= EventIdDef::EVID_FIXED__START_)
			{
				// システム固定イベントは無視
				continue;
			}
			else if(evid >= EventIdDef::EVID_AUTO__END_)
			{
				ASSERT(evid < EventIdDef::EVID_AUTO__END_);
				return 0;	// 自動割当失敗
			}
			
			evidAuto = (u16)(evid + 1);
		}
	}
	ASSERT(evidAuto <= EventIdDef::EVID_AUTO__END_);
	return evidAuto;
}

/*---------------------------------------------------------------------*//**
	メッセージデータを読み込む
**//*---------------------------------------------------------------------*/
bool EventData::loadMessageData(u16 msgkind, u32 fileid, u16 ofsMsgid)
{
	IdFile file;
	if(!file.open(fileid))
	{
		return false;
	}

	// メッセージデータ読み込み
	if(!_msgdatset->addMsgData(msgkind, &file, ofsMsgid))
	{
		return false;
	}

	file.close();

	return true;
}


////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
