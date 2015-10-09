/***********************************************************************//**
 *	QuestArticle.cpp
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
#include "QuestArticle.h"

// Friends
#include "QuestDef.h"
#include "qsmod/QsMod.h"
#include "qsmod/QsModFactory.h"
#include "../event/EventIdDef.h"
#include "../event/matter/EventMatterPresvParam.h"
#include "../body/Conductor.h"
#include "../body/Game.h"
#include "../body/GameFixedStrTbl.h"
#include "../body/Party.h"
#include "../item/ItemDef.h"
#include "../item/ItemDefTbl.h"
#include "../item/Item.h"
#include "../item/ItemManager.h"

// External
#include "../../tfw/lib/Color.h"
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	起動状態へ
**//*---------------------------------------------------------------------*/
void QuestArticle::boot(u8 qstslot)
{
	_stat = STAT_BOOT;	// 起動へ
	_qstslot = qstslot;
	_cntAutoCtrlEvid = 0;

	// イベント保持パラメータの作成
	if(_evprsvp == 0L)
	{
		_evprsvp = new EventMatterPresvParam();
	}
	if(_evprsvp != 0L)
	{
		_evprsvp->_kind = EventMatterPresvParam::KIND_QUEST;
		_evprsvp->_d = this;
	}
}

/*---------------------------------------------------------------------*//**
	受託状態へ
**//*---------------------------------------------------------------------*/
void QuestArticle::accept()
{
	_stat = STAT_ACCEPT;	// 受託へ
	TFW_SET_FLAG(_flags, F_LISTED, true);	// リストに追加

	// クエストモジュールの作成
	_qsmod = QsModFactory::makeQsMod(_def->getQuestId(), this);
}

/*---------------------------------------------------------------------*//**
	条件達成（報酬はまだもらっていない）状態へ
**//*---------------------------------------------------------------------*/
void QuestArticle::achieve()
{
	if(_stat != STAT_ACCEPT)	// 受託以外はエラー
	{
		ASSERT(false);
		return;
	}

	_stat = STAT_ACHVED;	// 達成へ
}

/*---------------------------------------------------------------------*//**
	条件達成状態のキャンセル（条件のアイテムを失ったなど）
**//*---------------------------------------------------------------------*/
void QuestArticle::unachieve()
{
	if(_stat != STAT_ACHVED)	// 達成時以外はエラー
	{
		ASSERT(false);
		return;
	}

	_stat = STAT_ACCEPT;
}

/*---------------------------------------------------------------------*//**
	報酬処理
**//*---------------------------------------------------------------------*/
void QuestArticle::reward()
{
	ASSERT(_def != 0L);
	switch(_def->getRewardKind())
	{
	case QuestDef::REWKIND_CUSTOM:
		if(_qsmod != 0L)	{	return _qsmod->rewardCustom();	}
		break;

		// 経験値取得
	case QuestDef::REWKIND_EXP:
		reward_experience();
		break;

		// アイテム取得
	case QuestDef::REWKIND_ITEM:
		reward_obtainItem();
		break;
	}
}

/*---------------------------------------------------------------------*//**
	クリア状態へ
**//*---------------------------------------------------------------------*/
void QuestArticle::finish()
{
	TFW_SET_FLAG(_flags, F_FINISHED, true);	// クリア済みフラグを上げる

	// クリアカウント増算
	if(_cntCleared < U8_MAX)
	{
		_cntCleared++;
	}

	cancel();
}

/*---------------------------------------------------------------------*//**
	キャンセル状態へ
**//*---------------------------------------------------------------------*/
void QuestArticle::cancel()
{
	_stat = STAT_SLEEP;		// 未起動へ
	_qstslot = 0;
	for(int i = 0; i < NUM_GVAL; i++)	{	_gval[i] = 0.0f;	}

	// クエストモジュールの削除
	if(_qsmod != 0L)
	{
		_qsmod->destroy();
		delete _qsmod;
		_qsmod = 0L;
	}

	// イベント保持パラメータの削除
	if(_evprsvp != 0L)
	{
		delete _evprsvp;
		_evprsvp = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	クエスト達成チェック
**//*---------------------------------------------------------------------*/
bool QuestArticle::checkAchievementImmediately() const
{
	ASSERT(_def != 0L);
	switch(_def->getAchievementKind())
	{
	case QuestDef::ACHVKIND_CUSTOM:
		if(_qsmod != 0L)	{	return _qsmod->checkAchievementCustom();	}
		break;

		// アイテム達成チェック
	case QuestDef::ACHVKIND_HAND_OVER_ITEM:
		return checkAchievement_handOverItem();
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	クエスト条件の品を手渡す（失う）
**//*---------------------------------------------------------------------*/
bool QuestArticle::handoverRequisites()
{
	ASSERT(_def != 0L);
	switch(_def->getAchievementKind())
	{
	case QuestDef::ACHVKIND_CUSTOM:
		if(_qsmod != 0L)	{	return _qsmod->handoverRequisitesCustom();	}
		break;

		// アイテムを渡す
	case QuestDef::ACHVKIND_HAND_OVER_ITEM:
		return handoverRequisites_handOverItem();
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	メニュー表示順を設定する
**//*---------------------------------------------------------------------*/
void QuestArticle::setMenuOrder(u8 order)
{
	_orderMenu = order;
}

/*---------------------------------------------------------------------*//**
	汎用値を得る
**//*---------------------------------------------------------------------*/
f32 QuestArticle::getGeneralValue(s32 igval) const
{
	if((igval < 0) || (NUM_GVAL <= igval))	{	return 0.0f;	}
	return _gval[igval];
}

/*---------------------------------------------------------------------*//**
	汎用値を設定する
**//*---------------------------------------------------------------------*/
void QuestArticle::setGeneralValue(s32 igval, f32 val)
{
	if((igval < 0) || (NUM_GVAL <= igval))	{	return;	}
	_gval[igval] = val;
}

/*---------------------------------------------------------------------*//**
	制御イベント ID を取得し、次に送る
**//*---------------------------------------------------------------------*/
u16 QuestArticle::nextCtrlEvid()
{
	_cntAutoCtrlEvid++;
	u16 ctrlevid = EventIdDef::CTRLEVID_QUEST__START_ + (EventIdDef::CTRLEVID_QUEST_SLOT_MULT * _qstslot) + _cntAutoCtrlEvid;
	return ctrlevid;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QuestArticle::QuestArticle()
	: _def(0L)
	, _qsmod(0L)
	, _evprsvp(0L)
	, _stat(STAT_NULL)
	, _statTemp(STAT_NULL)
	, _flags(0)
	, _qstslot(0)
	, _cntCleared(0)
	, _orderMenu(0)
	, _cntAutoCtrlEvid(0)
{
	for(int i = 0; i < NUM_GVAL; i++)	{	_gval[i] = 0.0f;	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
QuestArticle::~QuestArticle()
{
	ASSERT(_qsmod == 0L);
	ASSERT(_evprsvp == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool QuestArticle::create(const QuestDef* def, u8 stat)
{
	_def = def;
	_stat = stat;
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void QuestArticle::destroy()
{
	// クエストモジュールの削除
	if(_qsmod != 0L)
	{
		_qsmod->destroy();
		delete _qsmod;
		_qsmod = 0L;
	}

	// イベント保持パラメータの削除
	if(_evprsvp != 0L)
	{
		delete _evprsvp;
		_evprsvp = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void QuestArticle::exec(ExecRes* res, const ExecCtx* ec)
{
	if(_qsmod != 0L)
	{
		_qsmod->exec(res, ec);
	}
}

/*---------------------------------------------------------------------*//**
	マップ変更後通知
**//*---------------------------------------------------------------------*/
void QuestArticle::notifyChangedMap(bool isDeserializeCase)
{
	if(_qsmod != 0L)
	{
		_qsmod->notifyChangedMap(isDeserializeCase);
	}
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void QuestArticle::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	if(_qsmod != 0L)
	{
		_qsmod->notifyUnitInfluenceStat(unitRecv, scres, tstat);
	}
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool QuestArticle::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!storeToStructure(&svst)) { return false; }
		if(!ser->store(&svst, sizeof(SaveStructure))) { return false; }
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool QuestArticle::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!ser->restore(&svst, sizeof(SaveStructure))) { return false; }
		return restoreFromStructure(&svst);
	}
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャに保存する
**//*---------------------------------------------------------------------*/
bool QuestArticle::storeToStructure(SaveStructure* svst) const
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	svst->_stat = _stat;
	svst->_flags = _flags;
	svst->_cntCleared = _cntCleared;
	svst->_qstslot = _qstslot;
	svst->_cntAutoCtrlEvid = _cntAutoCtrlEvid;
	svst->_orderMenu = _orderMenu;
	for(int i = 0; i < NUM_GVAL; i++)
	{
		svst->_gval[i] = _gval[i];
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool QuestArticle::restoreFromStructure(const SaveStructure* svst)
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	_stat = svst->_stat;
	_flags = svst->_flags;
	_cntCleared = svst->_cntCleared;
	_qstslot = svst->_qstslot;
	_cntAutoCtrlEvid = svst->_cntAutoCtrlEvid;
	_orderMenu = svst->_orderMenu;
	for(int i = 0; i < NUM_GVAL; i++)
	{
		_gval[i] = svst->_gval[i];
	}

	// 古いセーブデータ対応	【2010/12/28】Ver1.2 未満対応
	if((_stat == STAT_NULL) && (_def != 0L))
	{
		_stat = STAT_SLEEP;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool QuestArticle::checkAchievement_handOverItem() const
{
	ASSERT(_def != 0L);
	u16 itemid = _def->getAchievementTargetId();
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	ItemManager* itmmng = party->getItemManager(); ASSERT(itmmng != 0L);
	ItemArray itmarr = itmmng->findByItemDefId(itemid, false);	// アイテムの検索
	return itmarr.getCount() >= 1;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool QuestArticle::handoverRequisites_handOverItem()
{
	ASSERT(_def != 0L);
	u16 itemid = _def->getAchievementTargetId();
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	ItemManager* itmmng = party->getItemManager(); ASSERT(itmmng != 0L);
	ItemArray itmarr = itmmng->findByItemDefId(itemid, false);	// アイテムの検索
	if(itmarr.getCount() <= 0)
	{
		return false;
	}
	itmmng->removeItem(itmarr.getItem(0)->getItemGenId());	// アイテムを失う
	return true;
}

/*---------------------------------------------------------------------*//**
	経験値取得
**//*---------------------------------------------------------------------*/
void QuestArticle::reward_experience()
{
	ASSERT(_def != 0L);
	Conductor* conductor = Game::getGame()->getConductor(); ASSERT(conductor != 0L);
	conductor->onExperienceNow(_def->getRewardValue());
}

/*---------------------------------------------------------------------*//**
	アイテム取得
**//*---------------------------------------------------------------------*/
void QuestArticle::reward_obtainItem()
{
	ASSERT(_def != 0L);
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	ItemManager* itmmng = party->getItemManager(); ASSERT(itmmng != 0L);
	Conductor* conductor = Game::getGame()->getConductor(); ASSERT(conductor != 0L);
	u16 itmdid = _def->getRewardValue();
	ColorU8 colorMsg;
	Conductor::getSysRewardColor(&colorMsg);
	VcString msg;
	if(itmmng->newItem(itmdid, 0L, 0, false) != -1)
	{
		// アイテム取得メッセージ表示
		const ItemDefTbl* itmdftbl = itmmng->getItemDefTableRef();
		const ItemDef* itmdf = itmdftbl->getDef(itmdid);
		conductor->on(VcString::format(&msg, GameFixedStrTbl::getString(GameFixedStrTbl::SID_GET_SOMETHING)->getRaw(), itmdf->getName(Env_getLangId())->getRaw(), ""), &colorMsg, 0);
	}
	else
	{
		// Nothing メッセージ表示
		conductor->on(VcString::format(&msg, GameFixedStrTbl::getString(GameFixedStrTbl::SID_NOT_FOUND)->getRaw(), ""), &colorMsg, 0);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
