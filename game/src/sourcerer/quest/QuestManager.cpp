/***********************************************************************//**
 *	QuestManager.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/14.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QuestManager.h"

// Friends
#include "QuestArticle.h"
#include "QuestDef.h"
#include "../body/DescKeyDef.h"
#include "../body/Game.h"
#include "../body/GameFixedStrTbl.h"
#include "../body/MoveMap.h"
#include "../common/CalcUtils.h"
#include "../episode/Situation.h"
#include "../episode/StoryManager.h"
#include "../episode/data/FixedData.h"
#include "../episode/save/SaveStructure.h"
#include "../event/AutoBeginEventInfo.h"
#include "../event/EventIdDef.h"
#include "../event/EventIdInfoArray.h"
#include "../event/EventData.h"
#include "../event/EventSys.h"
#include "../event/matter/EventMatter.h"
#include "../event/matter/OverrideEventMatter.h"
#include "../map/Map.h"
#include "../map/PlacementObj.h"
#include "../mode/GameMode.h"
#include "../unit/UnitManager.h"
#include "../unit/char/CharUnit.h"

// External
#include "../../tfw/collection/List.h"
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QuestManager メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	クエストオブジェクトを ID から得る
**//*---------------------------------------------------------------------*/
const QuestArticle* QuestManager::getQuestFromId(u16 questid) const
{
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		if(_qstaarr[i]->isValid())
		{
			if(_qstaarr[i]->getDef()->getQuestId() == questid)
			{
				return _qstaarr[i];
			}
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	クエストオブジェクトを ID から得る
**//*---------------------------------------------------------------------*/
QuestArticle* QuestManager::questFromId(u16 questid)
{
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		if(_qstaarr[i]->isValid())
		{
			if(_qstaarr[i]->getDef()->getQuestId() == questid)
			{
				return _qstaarr[i];
			}
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	クエストオブジェクトを識別キーから得る
**//*---------------------------------------------------------------------*/
const QuestArticle* QuestManager::getQuestFromDescKey(u32 desckey) const
{
	u16 questid = convDescKeyToQuestId(desckey);
	if(questid == 0)
	{
		return 0L;
	}
	const QuestArticle* qsta = getQuestFromId(questid);
	if(qsta == 0L)
	{
		return 0L;
	}
	if(!qsta->isValid())
	{
		return 0L;
	}
	return qsta;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	条件を満たしているクエストを起動させる
**//*---------------------------------------------------------------------*/
void QuestManager::bootQuestToMeet()
{
	#if !ENABLE_QUEST
		return;
	#endif

	// 現在受諾しているクエスト数をカウントする
	s32 cntAccept = 0;
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		if(_qstaarr[i]->getStat() == QuestArticle::STAT_ACCEPT)
		{
			cntAccept++;
		}
	}

	// 最大数受諾済みの場合は抜ける
	if(cntAccept >= NUM_QUEST_SLOT_MAX)
	{
		return;
	}

	// それぞれのクエストの状態を一時保存状態にコピーする
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		if(_qstaarr[i]->getStat() == QuestArticle::STAT_BOOT)
		{
			_qstaarr[i]->setTempStat(QuestArticle::STAT_SLEEP);	// 起動中のものは STAT_SLEEP とする
		}
		else
		{
			_qstaarr[i]->setTempStat(_qstaarr[i]->getStat());
		}
	}

	// 条件を満たしているか
	for(int icnt = 0; icnt < 2; icnt++)	// 最初の起動判定、２回目以降の起動判定
	{
		for(int i = 0; i < NUM_QUEST_MAX; i++)
		{
			if(_qstaarr[i]->getTempStat() == QuestArticle::STAT_SLEEP)	// 一時保存状態を参照
			{
				if(checkQuestBootCondition(_qstaarr[i], icnt == 0, true, false))	// 条件判定
				{
					if(_qstaarr[i]->getStat() == QuestArticle::STAT_BOOT)	// 実際は起動している
					{
						// 起動済み．最大数起動済みの場合は抜ける
						cntAccept++;
						if(cntAccept >= NUM_QUEST_SLOT_MAX)
						{
							break;
						}
					}
					else	// 実際も未起動
					{
						if(checkQuestBootCondition(_qstaarr[i], icnt == 0, false, true))	// 実状態で再条件判定
						{
							if(bootQuest(_qstaarr[i]))	// クエスト起動
							{
								// 最大数起動済みの場合は抜ける
								cntAccept++;
								if(cntAccept >= NUM_QUEST_SLOT_MAX)
								{
									break;
								}
							}
						}
					}
				}
				else // 条件 NG
				{
					if(_qstaarr[i]->getStat() == QuestArticle::STAT_BOOT)	// 実際は起動している
					{
						// クエストキャンセル
						cancelQuest(_qstaarr[i]->getDef()->getQuestId());
					}
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	クエスト受託
**//*---------------------------------------------------------------------*/
void QuestManager::contractQuest(u16 questid)
{
	QuestArticle* qsta = questFromId(questid);
	if(qsta == 0L)	{	return;	}

	// 受託処理
	qsta->accept();

	// メニュー表示順の決定
	if(qsta->getMenuOrder() == 0)	// 未決定の場合のみ
	{
		u32 orderMenuMax = 0;
		for(int i = 0; i < NUM_QUEST_MAX; i++)
		{
			if((_qstaarr[i] != qsta) && (_qstaarr[i]->getMenuOrder() > orderMenuMax))
			{
				orderMenuMax = _qstaarr[i]->getMenuOrder();
			}
		}
		qsta->setMenuOrder(orderMenuMax + 1);
	}

	updateVariableAchievementState();	// 可変達成状態の更新
	updateMapDeploymentObjects(false);	// 配置オブジェクト更新
}

/*---------------------------------------------------------------------*//**
	クエスト条件達成
**//*---------------------------------------------------------------------*/
void QuestManager::achieveQuest(u16 questid)
{
	QuestArticle* qsta = questFromId(questid);
	if(qsta == 0L)	{	return;	}

	// 達成処理
	qsta->achieve();

	updateMapDeploymentObjects(false);	// 配置オブジェクト更新
}

/*---------------------------------------------------------------------*//**
	クエスト クリアー
**//*---------------------------------------------------------------------*/
void QuestManager::finishQuest(u16 questid)
{
	QuestArticle* qsta = questFromId(questid);
	if(qsta == 0L)	{	return;	}
	
	// 報酬処理
	qsta->reward();

	releaseQuest(qsta);

	// 終了処理
	qsta->finish();

	updateMapDeploymentObjects(false);	// 配置オブジェクト更新
}

/*---------------------------------------------------------------------*//**
	クエスト キャンセル
**//*---------------------------------------------------------------------*/
void QuestManager::cancelQuest(u16 questid)
{
	QuestArticle* qsta = questFromId(questid);
	if(qsta == 0L)	{	return;	}

	releaseQuest(qsta);

	// キャンセル処理
	qsta->cancel();

	updateMapDeploymentObjects(false);	// 配置オブジェクト更新
}

/*---------------------------------------------------------------------*//**
	クエスト条件の品を手渡す（失う）
**//*---------------------------------------------------------------------*/
bool QuestManager::handoverThing(u16 questid)
{
	QuestArticle* qsta = questFromId(questid);
	if(qsta == 0L)	{	return false;	}

	return qsta->handoverRequisites();
}

/*---------------------------------------------------------------------*//**
	クエスト条件達成
**//*---------------------------------------------------------------------*/
void QuestManager::reserveAchieveQuest(u16 questid)
{
	reserveEvent(ReserveEvent::KIND_ACHVED, questid);
}

/*---------------------------------------------------------------------*//**
	クエスト クリアーの予約
**//*---------------------------------------------------------------------*/
void QuestManager::reserveFinishQuest(u16 questid)
{
	reserveEvent(ReserveEvent::KIND_FINISH, questid);
}

/*---------------------------------------------------------------------*//**
	クエスト キャンセルの予約
**//*---------------------------------------------------------------------*/
void QuestManager::reserveCancelQuest(u16 questid)
{
	reserveEvent(ReserveEvent::KIND_CANCEL, questid);
}

/*---------------------------------------------------------------------*//**
	クエスト条件達成を予約しているかどうかを得る
**//*---------------------------------------------------------------------*/
bool QuestManager::isReservedAchieveQuest(u16 questid) const
{
	return isReservedEvent(questid);
}

/*---------------------------------------------------------------------*//**
	クエスト条件達成が可変なもの（アイテム取得など）の状態更新
**//*---------------------------------------------------------------------*/
void QuestManager::updateVariableAchievementState()
{
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		if(_qstaarr[i]->isValid())
		{
			u8 stat = _qstaarr[i]->getStat();
			switch(stat)
			{
			case QuestArticle::STAT_ACCEPT:
			case QuestArticle::STAT_ACHVED:
				if(_qstaarr[i]->checkAchievementImmediately())	// 条件チェック
				{
					if(stat == QuestArticle::STAT_ACCEPT)
					{
						_qstaarr[i]->achieve();		// 達成へ
					}
				}
				else
				{
					if(stat == QuestArticle::STAT_ACHVED)
					{
						_qstaarr[i]->unachieve();	// 非達成へ
					}
				}
				break;
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	ニューゲーム
**//*---------------------------------------------------------------------*/
bool QuestManager::newGame()
{
	// クエスト起動
	bootQuestToMeet();

	return true;
}

/*---------------------------------------------------------------------*//**
	マップに直接入る（デバッグ用）
**//*---------------------------------------------------------------------*/
void QuestManager::debug_enterMap()
{
#if defined(_DEBUG)
	// クエスト起動
	bootQuestToMeet();
#endif
}

/*---------------------------------------------------------------------*//**
	クエストを起動する（デバッグ用）
**//*---------------------------------------------------------------------*/
const QuestArticle* QuestManager::debug_bootQuestByIndex(s32 index)
{
#if defined(_DEBUG)
	if((index < 0) || (NUM_QUEST_MAX <= index))	{	return 0L;	}
	QuestArticle* qsta = _qstaarr[index];
	if(!qsta->isValid())	{	return 0L;		}
	// クエスト起動
	if(bootQuest(qsta))		{	return qsta;	}
#endif
	return 0L;
}

/*---------------------------------------------------------------------*//**
	配置オブジェクト設定の追加
**//*---------------------------------------------------------------------*/
void QuestManager::addPlacementObjSetting(PlacementObj* pobjEntr, u16 questid)
{
	// 識別キーを設定する
	pobjEntr->setDescKey(convQuestIdToDescKey(questid));

	// 自動割当制御イベント ID を付加
	if(pobjEntr->getCtrlEvid() == 0)
	{
		QuestArticle* qsta = questFromId(questid);
		if(qsta != 0L)
		{
			pobjEntr->setCtrlEvid(qsta->nextCtrlEvid());
		}
	}

	// リストに追加
	ASSERT(_listPlaceObj != 0L);
	_listPlaceObj->addTail(pobjEntr);
}

/*---------------------------------------------------------------------*//**
	置換イベント設定の追加
**//*---------------------------------------------------------------------*/
void QuestManager::addOverrideEvent(OverrideEventMatter* oemEntr)
{
	ASSERT(_listOvrdEvm != 0L);
	_listOvrdEvm->addTail(oemEntr);
}

/*---------------------------------------------------------------------*//**
	自動起動イベントの追加
**//*---------------------------------------------------------------------*/
void QuestManager::addAutoBeginEvent(AutoBeginEventInfo* abeviEntr)
{
	ASSERT(_listAutoBeginEvi != 0L);
	_listAutoBeginEvi->addTail(abeviEntr);
}

/*---------------------------------------------------------------------*//**
	クエスト ID から識別キーに変換する
**//*---------------------------------------------------------------------*/
u32 QuestManager::convQuestIdToDescKey(u16 questid) const
{
	u32 desckey = DescKeyDef::DESCKEY_QUEST_HBIT | questid;
	return desckey;
}

/*---------------------------------------------------------------------*//**
	識別キーからクエスト ID に変換する
**//*---------------------------------------------------------------------*/
u16 QuestManager::convDescKeyToQuestId(u32 desckey) const
{
	u32 questid = (u16)(DescKeyDef::DESCKEY_LBIT_MASK & desckey);
	return questid;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QuestManager::QuestManager()
	: _evsysRef(0L)
	, _listPlaceObj(0L)
	, _listOvrdEvm(0L)
	, _listAutoBeginEvi(0L)
{
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		_qstaarr[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
QuestManager::~QuestManager()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_QUEST_MAX; i++)
		{
			ASSERT(_qstaarr[i] == 0L);
		}
	#endif
	ASSERT(_listPlaceObj == 0L);
	ASSERT(_listOvrdEvm == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool QuestManager::create(EventSys* evsysRef)
{
	// 参照を保存
	_evsysRef = evsysRef;

	// クエストデータを作成
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		_qstaarr[i] = new QuestArticle();
	}

	// クエスト定義データを設定とクエストの作成
	 const QuestDefTbl* qsdftbl = FixedData::getQuestDefTable();
	 for(int i = 0; (i < NUM_QUEST_MAX) && (i < qsdftbl->getDefNum()); i++)
	{
		const QuestDef* qstdef = qsdftbl->getDefFromIndex(i);
		if(qstdef->getQuestId() != 0)
		{
			_qstaarr[i]->create(qstdef, QuestArticle::STAT_SLEEP);
		}
	}

	// 配置リストを作成
	_listPlaceObj = new List<PlacementObj*>(true);
	if(_listPlaceObj == 0L)	{	goto FAILED_END;	}

	// 置換イベントリストを作成
	_listOvrdEvm = new List<OverrideEventMatter*>(true);
	if(_listOvrdEvm == 0L)	{	goto FAILED_END;	}

	// 自動起動イベントリストを作成
	_listAutoBeginEvi = new List<AutoBeginEventInfo*>(true);
	if(_listAutoBeginEvi == 0L)	{	goto FAILED_END;	}

	// 変数初期化

	return true;

FAILED_END:
	delete _listAutoBeginEvi;
	_listAutoBeginEvi = 0L;

	delete _listOvrdEvm;
	_listOvrdEvm = 0L;

	delete _listPlaceObj;
	_listPlaceObj = 0L;

	return false;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void QuestManager::destroy()
{
	// 自動起動イベントリストを削除
	if(_listAutoBeginEvi != 0L)
	{
		delete _listAutoBeginEvi;
		_listAutoBeginEvi = 0L;
	}

	// 置換イベントリストを削除
	if(_listOvrdEvm != 0L)
	{
		delete _listOvrdEvm;
		_listOvrdEvm = 0L;
	}
	
	// 配置リストを削除
	if(_listPlaceObj != 0L)
	{
		delete _listPlaceObj;
		_listPlaceObj = 0L;
	}

	// クエストデータを削除
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		if(_qstaarr[i] != 0L)
		{
			_qstaarr[i]->destroy();
			delete _qstaarr[i];
			_qstaarr[i] = 0L;
		}
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void QuestManager::exec(ExecRes* res, const ExecCtx* ec)
{
	// 個別のクエストに伝達
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		if((_qstaarr[i] != 0L) && (_qstaarr[i]->getStat() == QuestArticle::STAT_ACCEPT))
		{
			_qstaarr[i]->exec(res, ec);
		}
	}

	// 予約イベント処理
	EventSys* evsys = Game::getGame()->getEventSys();
	if((evsys != 0L) && !evsys->isBegunEvent())	// イベントを実行していない／イベントが実行終了した
	{
		for(int slotidx = 0; slotidx < NUM_QUEST_SLOT_MAX; slotidx++)
		{
			switch(_resvev[slotidx]._stat)
			{
			case ReserveEvent::STAT_WAIT_BEGIN:	// 予約イベントがある
			case ReserveEvent::STAT_RUN:		// イベント実行中
				if(!evsys->isBegunEvent())	// イベントを実行していない／イベントが実行終了した再判定
				{
					ASSERT(_resvev[slotidx]._kind != ReserveEvent::KIND_NULL);
					ASSERT(_resvev[slotidx]._questid != 0);

					// イベント ID を得る
					u16 evid = 0;
					switch(_resvev[slotidx]._kind)
					{
					case ReserveEvent::KIND_ACHVED: evid = EventIdDef::EVID_FIXED_QUEST_ACHVED__START_ + slotidx; break;
					case ReserveEvent::KIND_CANCEL: evid = EventIdDef::EVID_FIXED_QUEST_CANCEL__START_ + slotidx; break;
					case ReserveEvent::KIND_FINISH: evid = EventIdDef::EVID_FIXED_QUEST_FINISH__START_ + slotidx; break;
					}
					// イベント起動および消去
					if(evid != 0)
					{
						EventData* evdat = (EventData*)evsys->getEvData();
						switch(_resvev[slotidx]._stat)
						{
						case ReserveEvent::STAT_WAIT_BEGIN:	// 予約イベントがある
							do
							{
								VcString strMatterName;
								switch(_resvev[slotidx]._kind)
								{
								case ReserveEvent::KIND_ACHVED: strMatterName = "QsEmp_Fixed_AchievePerform"; break;
								case ReserveEvent::KIND_CANCEL: strMatterName = "QsEmp_Fixed_ActualCancel"; break;
								case ReserveEvent::KIND_FINISH: strMatterName = "QsEmp_Fixed_ActualFinish"; break;
								}
								evid = evdat->addFixedMatter(evid, &strMatterName);
								if(evid == 0) { break; }
								EventMatter* matter = evdat->getEventMatterFromEvid(evid);
								if(matter == 0L) { break; }
								const QuestArticle* qsta = getQuestFromId(_resvev[slotidx]._questid);
								if(qsta == 0L) { break; }
								// 保持パラメータ設定
								matter->setPreserveParam(reinterpret_cast<addr_size>(qsta->getEventMatterPresvParam()));
								// イベント起動
								EvCause evcause(EventCause::CID_NULL);
								evsys->begin(evid, 0, &evcause);					// 専用イベント起動
								_resvev[slotidx]._stat = ReserveEvent::STAT_RUN;	// 予約イベント情報の状態を実行中に
							}
							while(false);
							break;
						case ReserveEvent::STAT_RUN:		// イベント実行中
							// イベント消去
							evdat->removeMatter(evid);	// 専用イベント削除
							_resvev[slotidx].clear();	// 予約イベント情報の状態クリア
							break;
						}
					}
				}
				break;
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	ゲームスタートモード開始通知
**//*---------------------------------------------------------------------*/
void QuestManager::notifyBeginStartGameMode()
{
}

/*---------------------------------------------------------------------*//**
	インゲーム開始通知
**//*---------------------------------------------------------------------*/
void QuestManager::notifyBeginIngameMode()
{
}

/*---------------------------------------------------------------------*//**
	マップ変更後通知
**//*---------------------------------------------------------------------*/
void QuestManager::notifyChangedMap(bool isDeserializeCase)
{
	// 可変達成状態の更新
	updateVariableAchievementState();

	// イベント配置オブジェクトの更新
	updateMapDeploymentObjects(isDeserializeCase);

	// 個別のクエストに伝達
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		if((_qstaarr[i] != 0L) && (_qstaarr[i]->getStat() == QuestArticle::STAT_ACCEPT))
		{
			_qstaarr[i]->notifyChangedMap(isDeserializeCase);
		}
	}

	// マップ開始時イベントを起動
	beginAutoBeginEvent(AutoBeginEventInfo::KIND_MAP_START);
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void QuestManager::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	// 個別のクエストに伝達
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		if((_qstaarr[i] != 0L) && (_qstaarr[i]->getStat() == QuestArticle::STAT_ACCEPT))
		{
			_qstaarr[i]->notifyUnitInfluenceStat(unitRecv, scres, tstat);
		}
	}
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool QuestManager::serialize(Serializer* ser) const
{
	// クエストデータの保存
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		ASSERT(SaveStructure::NUM_QUEST_MAX == NUM_QUEST_MAX);
		for(int i = 0; i < NUM_QUEST_MAX; i++)
		{
			_qstaarr[i]->storeToStructure(&svst->_qssvst[i]);
		}
	}
	else						// ダイレクトモード
	{
		for(int i = 0; i < NUM_QUEST_MAX; i++)
		{
			_qstaarr[i]->serialize(ser);
		}
	}

	// 配置オブジェクトの状態を保存する
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		if(!svst->storePlacementObjData(_listPlaceObj))
		{
			return false;
		}

		return true;
	}
	else						// ダイレクトモード
	{
		s32 numPobj = _listPlaceObj->getCount();
		ser->store(&numPobj);
		for(ListIterator<PlacementObj*> it = _listPlaceObj->iterator(); it.has(); it.next())
		{
			const PlacementObj* pobj = it.object();
			pobj->serialize(ser);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool QuestManager::deserialize(Serializer* ser)
{
	// クエストデータの読み込み
	if(ser->isStructureMode())	// ストラクチャモード
	{
		const SaveStructure* svst = (SaveStructure*)ser->getStructure();
		ASSERT(SaveStructure::NUM_QUEST_MAX == NUM_QUEST_MAX);
		for(int i = 0; i < NUM_QUEST_MAX; i++)
		{
			_qstaarr[i]->restoreFromStructure(&svst->_qssvst[i]);
		}
	}
	else						// ダイレクトモード
	{
		for(int i = 0; i < NUM_QUEST_MAX; i++)
		{
			_qstaarr[i]->deserialize(ser);
		}
	}

	// クエストのロード
	for(int i = 0; i < NUM_QUEST_MAX; i++)
	{
		u8 stat = _qstaarr[i]->getStat();
		switch(stat)
		{
		case QuestArticle::STAT_BOOT:
		case QuestArticle::STAT_ACCEPT:
		case QuestArticle::STAT_ACHVED:
			{
				u8 qstslot = _qstaarr[i]->getQuestSlot();

				// 過去バージョンのバグで、達成クエストのクエストスロットが既存のものと被ることがあるのを回避
				bool isRetry;
				do
				{
					isRetry = false;
					for(int j = 0; j < NUM_QUEST_MAX; j++)
					{
						if(i == j)	{	continue;	}
						if(qstslot == _qstaarr[j]->getQuestSlot())
						{
							qstslot++;
							isRetry = true;
							break;
						}
					}
				}
				while(isRetry);

				_qstaarr[i]->boot(qstslot);
				loadQuest(_qstaarr[i]);

				switch(stat)
				{
				case QuestArticle::STAT_ACCEPT:
				case QuestArticle::STAT_ACHVED:
					_qstaarr[i]->accept();

					switch(stat)
					{
					case QuestArticle::STAT_ACHVED:
						_qstaarr[i]->achieve();

						break;
					}
					break;
				}
				break;
			}
		}
	}

	// 配置オブジェクトの状態を読み込む
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		if(!svst->restorePlacementObjData(_listPlaceObj))
		{
			return false;
		}
	}
	else						// ダイレクトモード
	{
		s32 numPobj = 0;
		ser->restore(&numPobj);
		ASSERT(_listPlaceObj->getCount() == numPobj);
		ListIterator<PlacementObj*> it = _listPlaceObj->iterator();
		for(int i = 0; i < numPobj; i++)
		{
			if(!it.has())
			{
				ASSERT(false);
				return false;
			}
			PlacementObj* pobj = it.object();
			pobj->deserialize(ser);
			it.next();
		}
	}

	// 可変達成状態の更新
	updateVariableAchievementState();

	// イベント配置オブジェクトの更新
	updateMapDeploymentObjects(true);

	return true;
}

/*---------------------------------------------------------------------*//**
	デバッグモード設定
**//*---------------------------------------------------------------------*/
void QuestManager::debug_onDebugMode(s32 lv)
{
}

/*---------------------------------------------------------------------*//**
	クエストを起動する
**//*---------------------------------------------------------------------*/
bool QuestManager::bootQuest(QuestArticle* qsta)
{
	// 空きスロットを得る
	u8 qstslot = getEmptySlot();	ASSERT(qstslot != 0);
	if(qstslot <= 0)	{	return false;	}	// 念のため

	// 起動
	ASSERT(qsta->getDef() != 0L);
	qsta->boot(qstslot);
	return loadQuest(qsta);
}

/*---------------------------------------------------------------------*//**
	クエストの起動条件確認
**//*---------------------------------------------------------------------*/
bool QuestManager::checkQuestBootCondition(const QuestArticle* qsta, bool isFirstBoot, bool isRefTempStat, bool ignoreProbCheck)
{
	if(qsta->getDef() == 0L)
	{
		return false;	// 条件を満たしていない
	}

	// 最小ストーリー ID 判定
	if(qsta->getDef()->getBootMinStoryId() != 0)
	{
		StoryManager* strymng = Game::getGame()->getStoryManager();
		if(qsta->getDef()->getBootMinStoryId() > strymng->getCurrentStoryId())
		{
			return false;	// 条件を満たしていない
		}
	}

	// 最大ストーリー ID 判定
	if(qsta->getDef()->getBootMaxStoryId() != 0)
	{
		StoryManager* strymng = Game::getGame()->getStoryManager();
		if(qsta->getDef()->getBootMaxStoryId() < strymng->getCurrentStoryId())
		{
			return false;	// 条件を満たしていない
		}
	}

	// 他クエストクリア判定
	if(qsta->getDef()->getBootClearedQuestId() != 0)
	{
		bool isCleared = false;
		for(int j = 0; j < NUM_QUEST_MAX; j++)
		{
			if((qsta != _qstaarr[j]) && (_qstaarr[j]->getDef() != 0L))
			{
				if(qsta->getDef()->getBootClearedQuestId() == _qstaarr[j]->getDef()->getQuestId())
				{
					if(TFW_IS_FLAG(_qstaarr[j]->getFlags(), QuestArticle::F_FINISHED))
					{
						isCleared = true;
						break;
					}
				}
			}
		}
		if(!isCleared)
		{
			return false;	// 条件を満たしていない
		}
	}

	// シチュエーション判定
	if(qsta->getDef()->getBootSituationFlags() != 0)
	{
		StoryManager* strymng = Game::getGame()->getStoryManager();
		if(TFW_IS_FLAG(strymng->situation()->getCurrentSituationFlags(1), qsta->getDef()->getBootSituationFlags()))
		{
			if(qsta->getDef()->isBootSituationFlagsNegative())
			{
				return false;	// 条件を満たしていない
			}
		}
		else
		{
			if(!qsta->getDef()->isBootSituationFlagsNegative())
			{
				return false;	// 条件を満たしていない
			}
		}
	}

	// 確率判定
	if(!ignoreProbCheck)
	{
		if(isFirstBoot)
		{
			if(qsta->getClearedCount() == 0)	// 初めての起動
			{
				// 初期起動確率
				s32 randnum = (s32)(100.0f * CalcUtils::randF());
				if(randnum >= qsta->getDef()->getBootProbabilityFirst())
				{
					return false;	// 条件を満たしていない
				}
			}
			else
			{
				return false;	// 条件を満たしていない
			}
		}
		else	// ２回目以降
		{
			if(qsta->getClearedCount() >= 1)	// ２回目以降の起動
			{
				// 繰り返し起動確率
				s32 randnum = (s32)(100.0f * CalcUtils::randF());
				if(randnum >= qsta->getDef()->getBootProbabilityRepeat())
				{
					return false;	// 条件を満たしていない
				}
			}
		}
	}

	// 排他起動グループ判定
	if(qsta->getDef()->getBootXorGroupId() != 0)
	{
		//bool isBootedSameGroupQuest = false;
		for(int j = 0; j < NUM_QUEST_MAX; j++)
		{
			if((qsta != _qstaarr[j]) && (_qstaarr[j]->getDef() != 0L))
			{
				if(qsta->getDef()->getBootXorGroupId() == _qstaarr[j]->getDef()->getBootXorGroupId())
				{
					if(isRefTempStat)	// 一時保存状態の方を参照する
					{
						if(_qstaarr[j]->getTempStat() != QuestArticle::STAT_SLEEP)
						{
							return false;	// 条件を満たしていない
						}
					}
					else
					{
						if(_qstaarr[j]->getStat() != QuestArticle::STAT_SLEEP)
						{
							return false;	// 条件を満たしていない
						}
					}
				}
			}
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	クエスト読み込み
**//*---------------------------------------------------------------------*/
bool QuestManager::loadQuest(const QuestArticle* qsta)
{
	#if !ENABLE_QUEST
		return true;
	#endif

	// クエストイベント読み込み
	_evsysRef->loadQuest(qsta);

	return true;
}

/*---------------------------------------------------------------------*//**
	クエストの解放
**//*---------------------------------------------------------------------*/
void QuestManager::releaseQuest(const QuestArticle* qsta)
{
	// 識別キーを得る
	u16 desckey = convQuestIdToDescKey(qsta->getDef()->getQuestId());

	// マップ置換イベントを戻す
	EventData* evdat = _evsysRef->getEventData();
	{
		ListIterator<OverrideEventMatter*> itCur, itNext;
		itCur = _listOvrdEvm->iterator();
		while(itCur.has())
		{
			// 削除に対応するため、次を得ておく
			itNext = itCur;
			itNext.next();

			const OverrideEventMatter* oem = itCur.object();
			if(oem->getDescKey() == desckey)	// 識別キーが合っているものを対象
			{
				if(oem->getKind() == OverrideEventMatter::KIND_MAP)	// タイプがマップのもののみ
				{
					evdat->unoverrideMatter(oem);
				}

				_listOvrdEvm->removeNode(itCur.node());		// 置換イベント情報の削除
			}

			// 次のリストノードへ
			itCur = itNext;
		}
	}

	// マップの脇役（配置オブジェクトを）を解除
	{
		ListIterator<PlacementObj*> itCur, itNext;
		itCur = _listPlaceObj->iterator();
		while(itCur.has())
		{
			// 削除に対応するため、次を得ておく
			itNext = itCur;
			itNext.next();

			PlacementObj* pobj = itCur.object();
			if(pobj->getDescKey() == desckey)	// 識別キーが合っているものを対象
			{
				if(pobj->getMapId() == Game::getGame()->getMoveMap()->getCurMapId())
				{
					Game::getGame()->getMap()->unplacePlacementObj(pobj);
				}

				_listPlaceObj->removeNode(itCur.node());	// 配置オブジェクト情報の削除
			}

			// 次のリストノードへ
			itCur = itNext;
		}
	}

	// 自動起動イベントを削除
	{
		ListIterator<AutoBeginEventInfo*> itCur, itNext;
		itCur = _listAutoBeginEvi->iterator();
		while(itCur.has())
		{
			// 削除に対応するため、次を得ておく
			itNext = itCur;
			itNext.next();

			AutoBeginEventInfo* abevi = itCur.object();
			if(abevi->getDescKey() == desckey)	// 識別キーが合っているものを対象
			{
				_listAutoBeginEvi->removeNode(itCur.node());	// 自動起動イベントの削除
			}

			// 次のリストノードへ
			itCur = itNext;
		}
	}

	// クエストイベント削除
	_evsysRef->unloadQuest(qsta);
}

/*---------------------------------------------------------------------*//**
	自動開始イベントの開始
**//*---------------------------------------------------------------------*/
bool QuestManager::beginAutoBeginEvent(s32 kind)
{
	bool isBegin = false;

	for(ListIterator<AutoBeginEventInfo*> it = _listAutoBeginEvi->iterator(); it.has(); it.next())
	{
		AutoBeginEventInfo* abevi = it.object();
		if(abevi->getKind() == kind)	// 同じ種別
		{
			// イベント起動原因と起動判定
			short cid = EventCause::CID_NULL;
			bool isCall = true;
			switch(kind)
			{
			case AutoBeginEventInfo::KIND_STORY_START:
				cid = EventCause::CID_STORYSTART;
				break;
			case AutoBeginEventInfo::KIND_MAP_START:
				cid = EventCause::CID_MAPSTART;
				if(Game::getGame()->getMoveMap()->getCurMapId() != abevi->getMapId())
				{
					isCall = false;
				}
				break;
			}

			// イベント起動
			if(isCall)
			{
				u16 evid = abevi->getCallEvi()->getInfo(abevi->getActiveCallEviIndex())->getEvid();
				if(evid != 0)	// わざわざ起動しない設定で上書きすることもありうる
				{
					EvCause evcause(cid);
					EventMatter* matter = (EventMatter*)_evsysRef->begin(evid, 0, &evcause);
					if(matter != 0L)
					{
						isBegin = true;

						// イベントが複数定義されている場合は、次に送る
						if(abevi->getCallEvi()->getInfoNum() >= 2)
						{
							abevi->nextCallEvi(false);
						}
					}
				}
			}
		}
	}

	return isBegin;
}

/*---------------------------------------------------------------------*//**
	イベント予約
**//*---------------------------------------------------------------------*/
bool QuestManager::reserveEvent(u8 kindResvKind, u16 questid)
{
	const QuestArticle* qsta = getQuestFromId(questid);
	if(qsta == 0L)	{	return false;	}

	s32 slotidx = qsta->getQuestSlot() - 1;
	ASSERTM((0 <= slotidx) && (slotidx < NUM_QUEST_SLOT_MAX), "Illegal Slot");
	if((slotidx < 0) || (NUM_QUEST_SLOT_MAX <= slotidx))	{	return false;	}	// 念のため

	ASSERTM(_resvev[slotidx]._stat == ReserveEvent::STAT_NULL, "Already Exists Event-Reservation");	// 予約イベントの予約・実行は同時に一つまで
	if(_resvev[slotidx]._stat != ReserveEvent::STAT_NULL)	{	return false;	}	// 念のため

	_resvev[slotidx]._kind = kindResvKind;
	_resvev[slotidx]._stat = ReserveEvent::STAT_WAIT_BEGIN;
	_resvev[slotidx]._questid = questid;

	return true;
}

/*---------------------------------------------------------------------*//**
	イベント予約がされているかを得る
**//*---------------------------------------------------------------------*/
bool QuestManager::isReservedEvent(u16 questid) const
{
	const QuestArticle* qsta = getQuestFromId(questid);
	if(qsta == 0L)	{	return false;	}

	s32 slotidx = qsta->getQuestSlot() - 1;
	ASSERT((0 <= slotidx) && (slotidx < NUM_QUEST_SLOT_MAX));
	if((slotidx < 0) || (NUM_QUEST_SLOT_MAX <= slotidx))	{	return false;	}	// 念のため

	return _resvev[slotidx]._stat != ReserveEvent::STAT_NULL;
}

/*---------------------------------------------------------------------*//**
	マップ配置オブジェクト／イベントの更新
**//*---------------------------------------------------------------------*/
void QuestManager::updateMapDeploymentObjects(bool isDeserializeCase)
{
	EventData* evdat = _evsysRef->getEventData();
	Map* map = Game::getGame()->getMap(); ASSERT(map != 0L);

	// マップ置換イベントを処理
	for(ListIterator<OverrideEventMatter*> it = _listOvrdEvm->iterator(); it.has(); it.next())
	{
		const OverrideEventMatter* oem = it.object();
		if(oem->getKind() == OverrideEventMatter::KIND_MAP)	// タイプがマップのもののみ
		{
			const QuestArticle* qsta = getQuestFromDescKey(oem->getDescKey());
			if(qsta != 0L)
			{
				switch(qsta->getStat())
				{
				case QuestArticle::STAT_NULL:
				case QuestArticle::STAT_SLEEP:
					if(evdat->isOverrideMatter(oem))
					{
						evdat->unoverrideMatter(oem);
					}
					break;
				case QuestArticle::STAT_BOOT:
				case QuestArticle::STAT_ACCEPT:
				case QuestArticle::STAT_ACHVED:
					if(oem->isCondition() && !evdat->isOverrideMatter(oem))
					{
						evdat->overrideMatter(oem);
					}
					break;
				}
			}
		}
	}

	// マップ脇役配置を更新
	for(ListIterator<PlacementObj*> it = _listPlaceObj->iterator(); it.has(); it.next())
	{
		PlacementObj* pobj = it.object();

#if 1
		bool isMapPlacedOld = map->isPlacePlacementObj(pobj);
		bool isMapPlacedNew = isMapPlacedOld;
		bool isForcePlace = false;

		// 以前のマップなどから残った配置済みのマップ脇役を一度配置解除
		if(!isDeserializeCase)	// デシリアライズ時は状態復帰のため、pobj->isPlaced() に従う
		{
			if(!isMapPlacedOld && pobj->isPlaced())	// 前のマップなどで配置済みだった場合
			{
				pobj->setPlaced(false);					// 未配置に
			}
		}

		// 配置状態変更判定
		if(pobj->isPlaced() && isDeserializeCase)			// 状態が配置済（デシリアライズ用）
		{
			if(!isMapPlacedOld)	// 状態が配置済だが、実際には配置されていない
			{
				isMapPlacedNew = true;
				isForcePlace = true;	// 強制配置
			}
		}
		else if(isNewPlacedConditionQuestState(pobj))		// 条件を満たしている
		{
			if(!isMapPlacedOld)	// 条件を満たしているが、実際には配置されていない
			{	
				isMapPlacedNew = true;
			}
		}
		else if(!pobj->isPlaced())							// 状態が未配置で条件も満たしていない（デシリアライズ用）
		{
			if(isMapPlacedOld)	// 条件は満たしておらず、状態も未配置だが、実際には配置されている
			{
				isMapPlacedNew = false;	// 配置排除
			}
		}

		// 配置状態変更
		if(isMapPlacedOld != isMapPlacedNew)
		{
			if(isMapPlacedNew)
			{
				map->placePlacementObj(pobj, isForcePlace);
			}
			else
			{
				map->unplacePlacementObj(pobj);
			}
		}
#else
		bool isMapPlaced = map->isPlacePlacementObj(pobj);

		// 以前のマップなどから残った配置済みのマップ脇役を一度配置解除
		if(!isDeserializeCase)	// デシリアライズ時は状態復帰のため、pobj->isPlaced() に従う
		{
			if(isMapPlaced)	// 配置済みの場合
			{
				map->unplacePlacementObj(pobj);			// 配置排除
				isMapPlaced = false;
			}
			else if(pobj->isPlaced())	// 前のマップなどで配置済みだった場合
			{
				pobj->setPlaced(false);					// 未配置に
			}
		}

		// 新規配置解除
		if(pobj->isPlaced() && isDeserializeCase)			// 状態が配置済（デシリアライズ用）
		{
			if(!isMapPlaced)	// 状態が配置済だが、実際には配置されていない
			{
				map->placePlacementObj(pobj, true);		// 強制配置
			}
		}
		else if(isNewPlacedConditionQuestState(pobj))		// 条件を満たしている
		{
			if(!isMapPlaced)	// 条件を満たしているが、実際には配置されていない
			{	
				map->placePlacementObj(pobj, false);	// 通常配置
			}
		}
		else if(!pobj->isPlaced() && isMapPlaced)			// 状態が未配置で条件も満たしていない（デシリアライズ用）
		{
			if(isMapPlaced)		// 条件は満たしておらず、状態も未配置だが、実際には配置されている
			{	
				map->unplacePlacementObj(pobj);			// 配置排除
			}
		}
#endif
	}

	// ユニットにクエストフラグを設定する
	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
	for(int iunit = 0; iunit < unitmng->getUnitNum(); iunit++)
	{
		Unit* unit = unitmng->getUnitFromIndex(iunit);
		if(unit->isCategory(Unit::UNITCAT_CHAR))
		{
			((CharUnit*)unit)->setQuestFlags(0);	// まずクエストフラグをクリア
		}
	}
	for(int iqs = 0; iqs < NUM_QUEST_MAX; iqs++)
	{
		if(_qstaarr[iqs]->isValid())
		{
			u16 qsflags = 0;
			switch(_qstaarr[iqs]->getStat())
			{
			case QuestArticle::STAT_BOOT:		qsflags = CharUnit::QSF_ACCEPTABLE;	break;
			case QuestArticle::STAT_ACCEPT:		qsflags = CharUnit::QSF_CANCELABLE;	break;
			case QuestArticle::STAT_ACHVED:		qsflags = CharUnit::QSF_FINISHABLE;	break;
			}
			if(qsflags != 0)
			{
				const QuestDef* qsdef = _qstaarr[iqs]->getDef();
				if(qsdef->getBaseCharId() == qsdef->getClearCharId())
				{
					if(qsdef->getBaseCharId() != 0)
					{
						updateCharUnitQuestFlags(unitmng, qsdef->getBaseCharId(), qsflags);
					}
				}
				else
				{
					if(qsdef->getBaseCharId() != 0)
					{
						updateCharUnitQuestFlags(unitmng, qsdef->getBaseCharId(), qsflags);
					}
					if(qsdef->getClearCharId() != 0)
					{
						updateCharUnitQuestFlags(unitmng, qsdef->getClearCharId(), qsflags);
					}
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	キャラクタユニットにクエストフラグを設定する
**//*---------------------------------------------------------------------*/
void QuestManager::updateCharUnitQuestFlags(UnitManager* unitmng, u16 chrdid, u16 qsflags)
{
	CharUnit* unit = unitmng->findCharUnitFromCharId(chrdid);
	if(unit == 0L)	{	return;	}
	unit->setQuestFlags(qsflags);
}

/*---------------------------------------------------------------------*//**
	新規配置条件にクエストの状態が合致するかを判定する
**//*---------------------------------------------------------------------*/
bool QuestManager::isNewPlacedConditionQuestState(const PlacementObj* pobj) const
{
	if(!pobj->isNewPlacedCondition())
	{
		return false;
	}

	const QuestArticle* qsta = getQuestFromDescKey(pobj->getDescKey());
	if(qsta == 0L)
	{
		return false;
	}

	switch(qsta->getStat())
	{
	case QuestArticle::STAT_NULL:
	case QuestArticle::STAT_SLEEP:
		return false;
	case QuestArticle::STAT_BOOT:
	case QuestArticle::STAT_ACHVED:
		if(pobj->isQuestAcceptingOnly())
		{
			return false;
		}
		break;
	case QuestArticle::STAT_ACCEPT:
		break;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	クエストスロットの空きを得る
**//*---------------------------------------------------------------------*/
u8 QuestManager::getEmptySlot() const
{
	for(u8 qstslot = 1; qstslot <= NUM_QUEST_SLOT_MAX; qstslot++)
	{
		bool isFound = false;
		for(int i = 0; i < NUM_QUEST_MAX; i++)
		{
			switch(_qstaarr[i]->getStat())
			{
			case QuestArticle::STAT_BOOT:
			case QuestArticle::STAT_ACCEPT:
			case QuestArticle::STAT_ACHVED:
				if(_qstaarr[i]->getQuestSlot() == qstslot)
				{
					isFound = true;
					break;
				}
			}
		}
		if(!isFound)
		{
			return qstslot;
		}
	}

	return 0;	// 空きなし
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
