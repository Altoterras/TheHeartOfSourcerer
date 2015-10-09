/***********************************************************************//**
 *	Char1404_BattleTutorial_EnemySpirit.cpp
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
#include "Char1404_BattleTutorial_EnemySpirit.h"

// Friends
#include "Char1502_Usagi_EnemySpirit.h"
#include "../../../episode/char/pc/MyUnit.h"
#include "../../../episode/party/MyParty.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../unit/Unit.h"
#include "../../../unit/UnitManager.h"
#include "../../../unit/char/npc/EnemyUnit.h"
#include "../../../spirit/char/npc/EnemyMode.h"
#include "../../../spirit/char/npc/EnemySpirit.h"
#include "../../../status/ActStat.h"
#include "../../../status/CharStat.h"
#include "../../../status/TransStat.h"

// External
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Char1404_BattleTutorial_EnemySpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ステップ変更
**//*---------------------------------------------------------------------*/
void Char1404_BattleTutorial_EnemySpirit::setStep(TutoStep tstep)
{
	_tstep = tstep;

	switch(_tstep)
	{
	case TSTEP_ATTACK_1:
		// 攻撃へ
		_mode->action();
		break;
	case TSTEP_ATTACK_2:
		// 対象をコルポへ
		setConfrontUnit( Game::getGame()->getMyParty()->pcUnit(MyParty::PC_PTN) );
		// 攻撃へ
		_mode->action();
		break;
	}
}

/*---------------------------------------------------------------------*//**
	次の対決時行動の取得
**//*---------------------------------------------------------------------*/
s16 Char1404_BattleTutorial_EnemySpirit::decideNextConfrontAction()
{
	switch(_tstep)
	{
	case TSTEP_SKILL_1:
		return EnemyMode::ACT_SKILL;
	default:
		return EnemySpirit::decideNextConfrontAction();
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Char1404_BattleTutorial_EnemySpirit::Char1404_BattleTutorial_EnemySpirit()
	: _tstep(TSTEP_FIRST)
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Char1404_BattleTutorial_EnemySpirit::exec(ExecRes* res, const ExecCtx* ec)
{
	switch(_tstep)
	{
	case TSTEP_FIRST:
	case TSTEP_WAIT_1:
		// 何もしない
		Spirit::exec(res, ec);
		return;
	default:
		EnemySpirit::exec(res, ec);
		return;
	}
}

/*---------------------------------------------------------------------*//**
	対決ゲームモード変更通知
**//*---------------------------------------------------------------------*/
void Char1404_BattleTutorial_EnemySpirit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
	if(isConfront)
	{
		// 戦闘モードへ
		((EnemyMode*)_mode)->toConfront(confront);
	}
	else
	{
		// 歩行モードへ
		((EnemyMode*)_mode)->toWalking();
	}

	// EnemySpirit の notifyChangedConfront 処理をスキップ
	Spirit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);
}

/*---------------------------------------------------------------------*//**
	ステータス影響発生通知
**//*---------------------------------------------------------------------*/
void Char1404_BattleTutorial_EnemySpirit::notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat)
{
	// 戦闘に入らない（EnemySpirit の notifyInfluenceStat 処理をスキップ）
	Spirit::notifyInfluenceStat(scres, tstat);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
