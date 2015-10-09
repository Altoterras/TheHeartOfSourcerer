/***********************************************************************//**
 *	QuestManager.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/14.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_QUEST_QUEST_MANAGER_H_
#define _SRCR_QUEST_QUEST_MANAGER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/framemod/FrameExecDraw.h"
#include "../../tfw/serialize/SerializeBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;

TFW_END_NS

SRCR_BEGIN_NS

class AutoBeginEventInfo;
class StatusChangeResult;
class QuestArticle;
class QuestDef;
class Game;
class EventSys;
class EventMatter;
class CharUnit;
class OverrideEventMatter;
class PlacementObj;
class TransStat;
class Unit;
class UnitManager;

/*---------------------------------------------------------------------*//**
 *	クエスト管理
 *
**//*---------------------------------------------------------------------*/
class QuestManager : public FrameExec, SerializeBase
{
	//======================================================================
	// 定数
public:
	static const s32 NUM_QUEST_MAX		= 128;
	static const u32 NUM_QUEST_SLOT_MAX	= 10;

	/*-----------------------------------------------------------------*//**
	 *	※ クエストの ID 空間
	 *
	 *	クエストは平行起動するため，各種 ID の競合を避けるために
	 *	クエストスロットインデックス（slotidx）を ID の一部に埋め込む．
	 *
	 *	・イベント ID
	 *		EVID_QUEST__START_ + (EVID_QUEST_SLOT_MULT * slotidx)
	 *			EVID_QUEST__START_: 20001
	 *			EVID_QUEST__END_: 35000
	 *			EVID_QUEST_SLOT_MULT: 100
	 *			下２桁 1～100、つまり 100 個のイベントを定義できる
	 *	・メッセージ ID
	 *		MSGID_QUEST__START_ + (MSGID_QUEST_SLOT_MULT * slotidx)
	 *			MSGID_QUEST__START_: 10001
	 *			MSGID_QUEST__END_: 50000;
	 *			MSGID_QUEST_SLOT_MULT: 100
	 *			下２桁 1～100、つまり 100 個のメッセージを定義できる
	 *	・自動割り当て制御イベント ID
	 *		CTRLEVID_QUEST__START_ + (CTRLEVID_QUEST_SLOT_MULT * slotidx)
	 *			CTRLEVID_QUEST__START_: 55001
	 *			CTRLEVID_QUEST__END_: 59000
	 *			CTRLEVID_QUEST_SLOT_MULT: 25
	 *			下２桁 1～25、25 個の自動制御イベントを定義できる
	 *			（この個数を超える場合は自動ではなく手動で制御イベント ID を
	 *			　割り振る）
	 *
	**//*-----------------------------------------------------------------*/

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	クエスト イベントの予約情報
	 *		※	クエストはイベントによって起動したり終了したりするが，
	 *			終了クエストイベントは自らを解放するために
	 *			終了演出と処理自体は QuestManager 自身が起動したイベントに
	 *			委譲する．
	 *			クエスト固有イベントは QuestManager に終了イベントを予約し，
	 *			QuestManager は適切なタイミングで終了演出と処理イベントを実
	 *			行し，その中でクエスト固有イベントを解放する．
	 *
	**//*-----------------------------------------------------------------*/
	class ReserveEvent
	{
	public:
		static const u8 KIND_NULL = 0;
		static const u8 KIND_ACHVED = 1;
		static const u8 KIND_CANCEL = 2;
		static const u8 KIND_FINISH = 3;

		static const u8 STAT_NULL = 0;
		static const u8 STAT_WAIT_BEGIN = 1;
		static const u8 STAT_RUN = 2;

	public:
		ReserveEvent() { clear(); }
		inline void clear() { _questid = _kind = _stat = 0; }

	public:
		u16 _questid;
		u8 _kind;
		u8 _stat;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// クエストの数を得る
	inline s32 getQuestNum() const { return NUM_QUEST_MAX; }
	// クエストオブジェクトをインデックスから得る
	inline const QuestArticle* getQuestByIndex(s32 index) const { return _qstaarr[index]; }
	inline QuestArticle* questByIndex(s32 index) { return _qstaarr[index]; }
	// クエストオブジェクトを ID から得る
	const QuestArticle* getQuestFromId(u16 questid) const;
	QuestArticle* questFromId(u16 questid);
private:
	// クエストオブジェクトを識別キーから得る
	const QuestArticle* getQuestFromDescKey(u32 desckey) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void bootQuestToMeet();
	void contractQuest(u16 questid);
	void achieveQuest(u16 questid);
	void finishQuest(u16 questid);
	void cancelQuest(u16 questid);
	bool handoverThing(u16 questid);
	void reserveAchieveQuest(u16 questid);
	void reserveFinishQuest(u16 questid);
	void reserveCancelQuest(u16 questid);
	bool isReservedAchieveQuest(u16 questid) const;

	void updateVariableAchievementState();

	bool newGame();
	void debug_enterMap();
	const QuestArticle* debug_bootQuestByIndex(s32 index);

	void addPlacementObjSetting(PlacementObj* pobjEntr, u16 questid);
	void addOverrideEvent(OverrideEventMatter* oemEntr);
	void addAutoBeginEvent(AutoBeginEventInfo* abeviEntr);

	u32 convQuestIdToDescKey(u16 questid) const;
	u16 convDescKeyToQuestId(u32 desckey) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QuestManager();
	~QuestManager();
	bool create(EventSys* evsysRef);
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

	void notifyBeginStartGameMode();
	void notifyBeginIngameMode();
	void notifyChangedMap(bool isDeserializeCase);
	void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);
	void debug_onDebugMode(s32 lv);

private:
	bool bootQuest(QuestArticle* qsta);
	bool checkQuestBootCondition(const QuestArticle* qsta, bool isFirstBoot, bool isRefTempStat, bool ignoreProbCheck);
	bool loadQuest(const QuestArticle* qsta);
	void releaseQuest(const QuestArticle* qsta);
	bool beginAutoBeginEvent(s32 kind);
	bool reserveEvent(u8 kindResvKind, u16 questid);
	bool isReservedEvent(u16 questid) const;
	void updateMapDeploymentObjects(bool isDeserializeCase);
	void updateCharUnitQuestFlags(UnitManager* unitmng, u16 chrdid, u16 qsflags);
	bool isNewPlacedConditionQuestState(const PlacementObj* pobj) const;

	u8 getEmptySlot() const;

	//======================================================================
	// 変数
private:
	// 参照
	EventSys* _evsysRef;

	// 設定等
	QuestArticle* _qstaarr[NUM_QUEST_MAX];
	List<PlacementObj*>* _listPlaceObj;
	List<OverrideEventMatter*>* _listOvrdEvm;
	List<AutoBeginEventInfo*>* _listAutoBeginEvi;
	ReserveEvent _resvev[NUM_QUEST_SLOT_MAX];

	// 作業変数
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS

#endif	// _SRCR_QUEST_QUEST_MANAGER_H_
