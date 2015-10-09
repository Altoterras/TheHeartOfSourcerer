/***********************************************************************//**
 *	PcSpirit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/09.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PcSpirit.h"

// Friends
#include "PcMode.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/Party.h"
#include "../../../item/ItemDef.h"
#include "../../../unit/UnitManager.h"
#include "../../../unit/char/pc/PcUnit.h"

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
// PcSpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	フォーカス相手を得る
**//*---------------------------------------------------------------------*/
Unit* PcSpirit::focusUnit() const
{
	if(_unitFocusRefGui != 0L)
	{
		return _unitFocusRefGui;
	}
	return Spirit::focusUnit();
}

/*---------------------------------------------------------------------*//**
	リーダーかどうかを得る
**//*---------------------------------------------------------------------*/
bool PcSpirit::isLeader() const
{
	if(getUnit() == 0L)	{	return false;	}
	return getPcUnit()->isLeader();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	フォーカス設定
**//*---------------------------------------------------------------------*/
void PcSpirit::setFocusUnit(Unit* unit, bool isByGui)
{
#if 1
	if(isByGui)
	{
		_unitFocusRefGui = unit;
	}

	Spirit::setFocusUnit(unit, isByGui);
#else
	if(isLeader())
	{
		if(isByGui || (unit == 0L) || (getFocusUnit() == 0L))
		{
			// GUI によるものとフォーカス外しは反映
			Spirit::setFocusUnit(unit, isByGui);
		}
	}
	else
	{
		Spirit::setFocusUnit(unit, isByGui);
	}
#endif
}

/*---------------------------------------------------------------------*//**
	行動を開始する
**//*---------------------------------------------------------------------*/
void PcSpirit::startBehavior(Behavior behv, const BehaviorParam* bparam)
{
	Spirit::startBehavior(behv, bparam);

	if(behv == BEHV_USE_ITEM)
	{
		const UseItemBehaviorParam* ibp = (const UseItemBehaviorParam*)bparam;
		_paramReservedBehavior = ibp->_itmgidRsvUsing;
	}
	else if(behv == BEHV_PICKUP_ITEM)
	{
		const PickupItemBehaviorParam* ibp = (const PickupItemBehaviorParam*)bparam;
		_paramReservedBehavior = ibp->_unitidRsvPickup;
	}
	else
	{
		_paramReservedBehavior = 0;
	}
}

/*---------------------------------------------------------------------*//**
	行動の中断

	@param behv 中断する行動（BEHV_NULL は全てを対象とした中断）
**//*---------------------------------------------------------------------*/
void PcSpirit::breakBehavior(Behavior behv, bool isByGui, bool duetoConfu)
{
	if((_behvCur == behv) || (behv == BEHV_NULL))
	{
		if(isBehaviorActing())
		{
			pcMode()->toNull();
		}

		Spirit::breakBehavior(behv, isByGui, duetoConfu);
	}
}

/*---------------------------------------------------------------------*//**
	イベントモードを開始
**//*---------------------------------------------------------------------*/
void PcSpirit::beginEventMode()
{
	// イベントモードへ
	pcMode()->toEvent();
}

/*---------------------------------------------------------------------*//**
	イベントモードを終了
**//*---------------------------------------------------------------------*/
void PcSpirit::endEventMode()
{
	// 歩行モードへ
	pcMode()->toWalking(false);
}

/*---------------------------------------------------------------------*//**
	策敵
**//*---------------------------------------------------------------------*/
void PcSpirit::searchEnemy()
{
	if((getFocusUnit() == 0L) || !getFocusUnit()->isEnable())	// フォーカスが外れた、もしくは対象が無効化された（倒された）場合
	{
		// Spirit::execSearchConfrontTarget で探してきた対象に自動フォーカス
		setFocusUnit(confrontUnit(), false);
	}
	else if(!getPcUnit()->isLeader())	// リーダー時以外
	{
		// リーダーと同じ対象をフォーカスする
		const Unit* unitLeader = Game::getGame()->getMyParty()->getLeaderPcUnit();
		const Spirit* sprtLeader = (unitLeader != 0L) ? unitLeader->getSpirit() : 0L;
		if((sprtLeader != 0L) && (getFocusUnit() != sprtLeader->getConfrontUnit()))
		{
			setFocusUnit(sprtLeader->confrontUnit(), false);
		}
	}
}

/*---------------------------------------------------------------------*//**
	フォールドアイテムを拾う
**//*---------------------------------------------------------------------*/
u16 PcSpirit::searchFieldItem(f32 rangeSq)
{
	UnitManager* unitmng = Game::getGame()->getUnitManager();
	for(int i = 0; i < unitmng->getUnitNum(); i++)
	{
		const Unit* unit = unitmng->getUnitFromIndex(i);
		if(!unit->isEnable())							{	continue;	}
		if(unit->getUnitType() != UNITTYPE_FIELD_ITEM)	{	continue;	}

		Vector3F diffItemToSelf = *getUnit()->getPosition() - *unit->getPosition();
		f32 lenItemToSelfSq = diffItemToSelf.lengthSq();
		if(lenItemToSelfSq <= rangeSq)
		{
			return unit->getUnitId();
		}
	}

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PcSpirit::PcSpirit(SpiritType sprttype)
	: Spirit(sprttype)
	, _paramReservedBehavior(0)
	, _unitFocusRefGui(0L)
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void PcSpirit::exec(ExecRes* res, const ExecCtx* ec)
{
	Spirit::exec(res, ec);

	// 現在参照している相手ユニットが無効の場合は、参照を外す
	if((_unitFocusRefGui != 0L) && !_unitFocusRefGui->isEnable())
	{
		_unitFocusRefGui = 0L;
	}
}
/*---------------------------------------------------------------------*//**
	マップ変更前通知
**//*---------------------------------------------------------------------*/
void PcSpirit::notifyChangeMapBefore()
{
	_unitFocusRefGui = 0L;
	Spirit::notifyChangeMapBefore();
}

/*---------------------------------------------------------------------*//**
	対決ゲームモード変更通知
**//*---------------------------------------------------------------------*/
void PcSpirit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
	if(isConfront)
	{
		if(!this->isConfront())
		{
			// 戦闘モードへ
			pcMode()->toConfront(confront);
		}
	}
	else
	{
		// 歩行モードへ
		pcMode()->toWalking(true);
	}

	Spirit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);
}

/*---------------------------------------------------------------------*//**
	イベントゲームモード変更通知
**//*---------------------------------------------------------------------*/
void PcSpirit::notifyChangedEventMode(bool isEventMode)
{
	if(isEventMode)
	{
		// イベントモードへ
		pcMode()->toEvent();
	}
	else
	{
		// 歩行モードへ
		pcMode()->toWalking(false);
	}
}

/*---------------------------------------------------------------------*//**
	ゲームの終了通知（行動の強制停止）
**//*---------------------------------------------------------------------*/
void PcSpirit::notifyTerminateGame()
{
	_mode->end(0L);
}

/*---------------------------------------------------------------------*//**
	行動変更通知
**//*---------------------------------------------------------------------*/
void PcSpirit::onChangeBehavior(Spirit::Behavior behvNew, Spirit::Behavior behvOld, bool isBreak, bool duetoConfu)
{
	if(isBreak)
	{
		VcString msg(VcString::format(GameFixedStrTbl::getString(duetoConfu ? GameFixedStrTbl::SID_CONFU_CANCEL_BEHV : GameFixedStrTbl::SID_CANCEL_BEHV)->getRaw(),
			getPcUnit()->getMainName()->getRaw(),
			"\n"));
		Game::getGame()->getConductor()->on(&msg, getUnit(), 0);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
