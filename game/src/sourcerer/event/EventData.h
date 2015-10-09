/***********************************************************************//**
 *	EventData.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/28.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_EVENT_DATA_H_
#define _SRCR_EVENT_EVENT_DATA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/evsys/EvData.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class XmlNode;
class EvMatterCreateParam;
class OverrideEventMatter;
class VcStringArray;

TFW_END_NS

SRCR_BEGIN_NS

class EventMatter;
class QuestArticle;

/*---------------------------------------------------------------------*//**
 *	イベント データ
 *
**//*---------------------------------------------------------------------*/
class EventData : public EvData
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// イベントマターの数を得る
	int getEventMatterNum() const { return getMatterNum(); }
	// イベントマターを得る
	EventMatter* getEventMatter(int index) const { return (EventMatter*)getMatter(index); }
	// イベントマターをイベント ID から得る
	EventMatter* getEventMatterFromEvid(u16 evid) const { return (EventMatter*)getMatterFromEvid(evid); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool loadSystemFixedEvent();
	void unloadSystemFixedEvent();
	bool loadMapEvent(s32 mapid);
	void unloadMapEvent();
	bool loadStoryEvent(s32 storyid);
	void unloadStoryEvent();
	bool loadQuestEvent(const QuestArticle* qstatc);
	void unloadQuestEvent(const QuestArticle* qstatc);
	bool loadTmpEvent(s32 tmpevid);
	void unloadTmpEvent();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:

private:
	bool loadMessageData(u16 msgkind, u32 fileid, u16 ofsMsgid);
	EvMatter* createMatter(const XmlNode* nodeEventMatter, const EvMatterCreateParam* cmparam);
	u16 getUnusedAutoEventId();

	//======================================================================
	// 変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_EVENT_DATA_H_
