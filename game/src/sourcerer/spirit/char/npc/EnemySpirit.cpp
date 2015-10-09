/***********************************************************************//**
 *	EnemySpirit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EnemySpirit.h"

// Friends
#include "EnemyMode.h"
#include "EnemyRole.h"
#include "../../../char/CharLogicData.h"
#include "../../../confront/Confront.h"
#include "../../../event/EventIdInfoArray.h"
#include "../../../event/EventSys.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/Party.h"
#include "../../../map/PlacementObj.h"
#include "../../../unit/UnitManager.h"
#include "../../../unit/char/npc/EnemyUnit.h"
#include "../../../unit/char/pc/PcUnit.h"
#include "../../../status/CharStat.h"
#include "../../../status/TransStat.h"

// External
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EnemySpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	行動の中断

	@param behv 中断する行動（BEHV_NULL は全てを対象とした中断）
**//*---------------------------------------------------------------------*/
void EnemySpirit::breakBehavior(Behavior behv, bool isByGui, bool duetoConfu)
{
	if((_behvCur == behv) || (behv == BEHV_NULL))
	{
		if(isBehaviorActing())
		{
			((EnemyMode*)_mode)->toStanby();
		}

		Spirit::breakBehavior(behv, isByGui, duetoConfu);
	}
}

/*---------------------------------------------------------------------*//**
	対決相手を検索する
**//*---------------------------------------------------------------------*/
void EnemySpirit::searchConfrontTarget()
{
	// +++ Adjustable Parameter >>>
	#define DISTSQ_FIGHTER_DEFENSE_SOURCERER	(300.0f * 300.0f)	// 3m
	// +++ Adjustable Parameter <<<

	// 最も近いプレイヤーキャラクタを得る（MyParty からとることもできるが、汎用性を考えてユニットマネージャからとることにする）
	f32 distNearestSq = F32_PMAX;
	f32 distNearestFighterSq = F32_PMAX;
	CharUnit* cunitNearest = 0L;
	CharUnit* cunitNearestFighter = 0L;
	Party* party = Game::getGame()->getGameMyParty();
	for(s32 ipc = 0; ipc < party->getPcNum(); ipc++)
	{
		Unit* unitWk = party->pcUnit(ipc);
		if(unitWk == 0L)			{	continue;	}
		if(!unitWk->isEnable())		{	continue;	}	// 無効なユニットは除外
		CharUnit* cunitWk = (CharUnit*)unitWk;

		// 活動停止は除外
		if(cunitWk->getCharStat()->getStat() == CharStat::STAT_DOWN)
		{
			continue;
		}

		f32 distSq = (*getThisUnit()->getPosition() - *cunitWk->getPosition()).lengthSq();

		// 最も近いプレイヤーキャラクタユニットを保存
		if(distNearestSq > distSq)
		{
			distNearestSq = distSq;
			cunitNearest = cunitWk;
		}

		// 戦士の情報を保存
		if(cunitWk->getCharClass() == CharUnit::CHARCLS_FIGHTER)
		{
			if(distNearestFighterSq > distSq)
			{
				distNearestFighterSq = distSq;
				cunitNearestFighter = cunitWk;
			}
		}
	}
	if(cunitNearest == 0L)	{	return;	}

	// 発見距離に入ったらロックオンを解除
	if(distNearestSq <= _lenDetectThrsldParamSq)	// 発見範囲に入ったとき
	{
		if(TFW_IS_FLAG(_ctrlflags, CTRLF_UNLOCKON_IN_DETECT_DIST))	// 発見距離に入ったらロックオン解除
		{
			if(TFW_IS_FLAG(_ctrlflags, CTRLF_LOCKON_TRG))	// ロックオン時
			{
				TFW_SET_FLAG(_ctrlflags, CTRLF_LOCKON_TRG, false);	// ロックオン解除
			}
		}
	}
	// ロックオン時は終了
	if(TFW_IS_FLAG(_ctrlflags, CTRLF_LOCKON_TRG))
	{
		return;
	}

	// 戦士は無条件でターゲット
	if(cunitNearest->getCharClass() == CharUnit::CHARCLS_FIGHTER)
	{
		if(getFocusUnit() != cunitNearest)
		{
			setFocusUnit(cunitNearest, false);
		}
		return;
	}
	// ソウマ使い
	else if(cunitNearest->getCharClass() == CharUnit::CHARCLS_SOURCERER)
	{
		// 戦士が近くにいる場合は戦士にターゲット
		if(cunitNearestFighter != 0L)
		{
			f32 distdistStoF = (*cunitNearest->getPosition() - *cunitNearestFighter->getPosition()).lengthSq();
			if(distdistStoF <= DISTSQ_FIGHTER_DEFENSE_SOURCERER)
			{
				if(getFocusUnit() != cunitNearestFighter)
				{
					setFocusUnit(cunitNearestFighter, false);
				}
				return;
			}
		}

		if(getFocusUnit() != cunitNearest)
		{
			setFocusUnit(cunitNearest, false);
		}
	}
	// エンハンサー
	else if(cunitNearest->getCharClass() == CharUnit::CHARCLS_ENHANCER)
	{
		if(getFocusUnit() != cunitNearest)
		{
			setFocusUnit(cunitNearest, false);
		}
		return;
	}
}

/*---------------------------------------------------------------------*//**
	プレイヤーキャラクタの検索
**//*---------------------------------------------------------------------*/
void EnemySpirit::searchConfrontTargetWhenNoConfront()
{
	const Vector3F* posSelf = getUnit()->getPosition();
	if(posSelf == 0L)	{	return;	}

	s32 ipcDetectMin = -1;
	f32 distDetectSqMin = F32_PMAX;
	Party* party = Game::getGame()->getGameMyParty();
	for(s32 ipc = 0; ipc < party->getPcNum(); ipc++)
	{
		Unit* unitPc = party->pcUnit(ipc);
		if(unitPc == 0L)				{	continue;	}
		const Vector3F* posPc = unitPc->getPosition();
		f32 distSq = (*posPc - *posSelf).lengthSq();
		if((distSq <= _lenBattleThrsldParamSq) && (!isConfront()))	// 戦闘開始距離内に入ったとき
		{
			if(!_isBaganPobjEvent)	// 配置オブジェクトのイベントを起動していないときのみ
			{
				// 配置オブジェクトのイベント起動
				const PlacementObj* pobj = getThisUnit()->getPlacementObj();
				if(pobj != 0L)
				{
					const EventIdInfoArray::EventIdInfo* cii = pobj->getCallEvi()->getInfo(pobj->getActiveCallEviIndex());
					if(cii != 0L)
					{
						EventSys* evsys = Game::getGame()->getEventSys();
						EvCause evcause(EventCause::CID_FOCUSMENU);
						if(evsys->begin(cii->getEvid(), cii->getEventParam(), &evcause) != 0L)
						{
							_isBaganPobjEvent = true;	// 配置オブジェクトのイベントを起動済み
							return;
						}
					}
				}

				// ゲーム本体に対決通知を送り、モードを変更する
				Game::getGame()->getConfront()->start(unit(), 0, getThisUnit());
				return;
			}
		}
		else if(distSq <= _lenDetectThrsldParamSq)	// 発見範囲に入ったとき
		{
			if(distDetectSqMin > distSq)
			{
				distDetectSqMin = distSq;
				ipcDetectMin = ipc;
			}

			// 発見距離に入ったらロックオンを解除
			if(TFW_IS_FLAG(_ctrlflags, CTRLF_UNLOCKON_IN_DETECT_DIST))	// 発見距離に入ったらロックオン解除
			{
				if(TFW_IS_FLAG(_ctrlflags, CTRLF_LOCKON_TRG))	// ロックオン時
				{
					TFW_SET_FLAG(_ctrlflags, CTRLF_LOCKON_TRG, false);	// ロックオン解除
				}
			}
		}
	}

	// 注視設定
	if(ipcDetectMin != -1)	// 発見したが戦闘やイベントには入らなかった
	{
		Unit* unitPc = party->pcUnit(ipcDetectMin);
		if(getFocusUnit() != unitPc)	// 注視先が違うか未設定
		{
			if(!TFW_IS_FLAG(_ctrlflags, CTRLF_LOCKON_TRG))	// ロックオン時は除く
			{
				// 探知対象の設定
				getThisUnit()->setDetectTarget(unitPc);

				// 歩行モードへ移行
				((EnemyMode*)_mode)->toWalking();
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	次の対決時行動の取得
**//*---------------------------------------------------------------------*/
s16 EnemySpirit::decideNextConfrontAction()
{
	return EnemyMode::ACT_ATTACK;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EnemySpirit::EnemySpirit()
	: Spirit(SPRTTYPE_ENEMY)
	, _ctrlflags(0)
	, _isBaganPobjEvent(false)
	, _distReach(0.0f)
	, _lenBattleThrsldParamSq(0.0f)
	, _lenDetectThrsldParamSq(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EnemySpirit::create(SpiritMode* modeEntr)
{
	// デフォルトのモードとロールの作成
	if(modeEntr == 0L)	// 指定がない場合
	{
		EnemyMode* mode = new EnemyMode();
		EnemyRole* role = new EnemyRole(this);
		if(!mode->create(this, role))
		{
			delete mode;
			return false;
		}
		modeEntr = mode;
	}

	return Spirit::create(modeEntr);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EnemySpirit::create()
{
	return EnemySpirit::create(0L);
}

/*---------------------------------------------------------------------*//**
	キャラクタパラメータを設定する
**//*---------------------------------------------------------------------*/
bool EnemySpirit::bind(CharUnit* cunitSelfRef, const CharDef* chrdef, const CharLogicData* chrlgd)
{
	// パラメータによる距離計算
	_distReach = chrlgd->getParam(CharLogicData::PARAM_NORMAL_REACH);
	f32 lenBattleThrsldParamSq = chrlgd->getParam(CharLogicData::PARAM_BATTLE_RANGE);
	_lenBattleThrsldParamSq = lenBattleThrsldParamSq * lenBattleThrsldParamSq;
	f32 lenDetectThrsldParamSq = chrlgd->getParam(CharLogicData::PARAM_DETECT_RANGE);
	_lenDetectThrsldParamSq = lenDetectThrsldParamSq * lenDetectThrsldParamSq;

	return Spirit::bind(cunitSelfRef, chrdef, chrlgd);
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void EnemySpirit::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	if(gec->isLockOperating())	// 操作ロック時
	{
		Spirit::exec(res, ec);
		return;
	}

	if(isConfront())
	{
	}
	else if(getUnit()->isEnable() && (_cstatRef->getStat() == CharStat::STAT_ALIVE))	// 有効なときのみ
	{
		searchConfrontTargetWhenNoConfront();	// 自動策敵
	}

	Spirit::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	対決ゲームモード変更通知
**//*---------------------------------------------------------------------*/
void EnemySpirit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
	if(isConfront)
	{
		if(!this->isConfront())
		{
			// 戦闘モードへ
			((EnemyMode*)_mode)->toConfront(confront);
		}
	}
	else
	{
		// 歩行モードへ
		((EnemyMode*)_mode)->toWalking();
	}

	Spirit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);
}

/*---------------------------------------------------------------------*//**
	ステータス影響発生通知
**//*---------------------------------------------------------------------*/
void EnemySpirit::notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat)
{
	if(!isConfront())	// 非対決時
	{
		// 対決へ
		if(tstat->getOwnerUnit() != 0L)	// とりあえずどんなユニットでも対決を挑んでみる…
		{
			Game::getGame()->getGame()->getConfront()->start(unit(), 0, getUnit());
		}
	}

	Spirit::notifyInfluenceStat(scres, tstat);
}

/*---------------------------------------------------------------------*//**
	行動変更通知
**//*---------------------------------------------------------------------*/
void EnemySpirit::onChangeBehavior(Spirit::Behavior behvNew, Spirit::Behavior behvOld, bool isBreak, bool duetoConfu)
{
	if(isBreak)
	{
		VcString msg(VcString::format(GameFixedStrTbl::getString(duetoConfu ? GameFixedStrTbl::SID_CONFU_CANCEL_BEHV : GameFixedStrTbl::SID_CANCEL_BEHV)->getRaw(),
			getThisUnit()->getMainName()->getRaw(),
			"\n"));
		Game::getGame()->getConductor()->on(&msg, getUnit(), 0);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
