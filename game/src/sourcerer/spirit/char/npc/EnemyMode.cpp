/***********************************************************************//**
 *	EnemyMode.h
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
#include "EnemyMode.h"

// Friends
#include "EnemyRole.h"
#include "EnemySpirit.h"
#include "../../../body/Game.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EnemyMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	歩行モードへ移行
**//*---------------------------------------------------------------------*/
void EnemyMode::toWalking()
{
	changeMode((s16)ACT_WALKING, false);
}

/*---------------------------------------------------------------------*//**
	対決モードへ移行
**//*---------------------------------------------------------------------*/
void EnemyMode::toConfront(Confront* confrontRef)
{
	_role->setConfrontModule(confrontRef);	// 対決モジュールをロールに設定する

	action();
}

/*---------------------------------------------------------------------*//**
	スタンバイモードへ移行
**//*---------------------------------------------------------------------*/
void EnemyMode::toStanby()
{
	changeMode((s16)ACT_STANDBY, false);
}

/*---------------------------------------------------------------------*//**
	アクション実行
**//*---------------------------------------------------------------------*/
bool EnemyMode::action()
{
	if(_sprtRef->isConfront())
	{
		changeMode(_sprtRef->decideNextConfrontAction(), false);
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	アクション中かどうか
**//*---------------------------------------------------------------------*/
bool EnemyMode::isActing()
{
	switch(getSelfRole()->getCurCommandId())
	{
	case 0:	// コマンド実行なし
	case EnemyRole::WALK_STANDBY:
	case EnemyRole::WALK_NORMAL:
	case EnemyRole::WALK_CF_DEFENSE:
	case EnemyRole::MOVE_AFTER_ATTACK:
		// 定常アクションのロールコマンド実行中
		return false;
	default:
		// アクション待ちのロールコマンド実行中
		return true;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EnemyMode::EnemyMode()
	: _role(0L)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EnemyMode::create(EnemySpirit* sprtRef, EnemyRole* roleEntr)
{
	// ロールを受け取る
	_role = roleEntr;

	// 親クラスによる作成
	if(!Mode::create(1, _role, true))
	{
		delete _role;
		_role = 0L;
		return false;
	}

	// 値の初期化
	_sprtRef = sprtRef;

	// モード登録
	registerMode((s16)ACT_STANDBY, new Mode());
	registerMode((s16)ACT_WALKING, new Mode());
	registerMode((s16)ACT_ATTACK, new Mode());
	registerMode((s16)ACT_DEFENSE, new Mode());
	registerMode((s16)ACT_SKILL, new Mode());
	registerMode((s16)ACT_MOVETOTRG, new Mode());
	changeMode((s16)ACT_STANDBY, false);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EnemyMode::destroy()
{
	_role = 0L;
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool EnemyMode::notifyBegin(void* objParamRef)
{
	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void EnemyMode::notifyEnd(void* objParamRef)
{
}

/*---------------------------------------------------------------------*//**
	子モードの開始通知
**//*---------------------------------------------------------------------*/
void EnemyMode::notifyChildModeBegin(short modeid, void* objParamRef)
{
	getSelfRole()->reset();
	switch(modeid)
	{
	case ACT_STANDBY:
		getSelfRole()->stackCommand(EnemyRole::WALK_STANDBY, 0L);
		getSelfRole()->begin();
		break;
	case ACT_WALKING:
		getSelfRole()->stackCommand(EnemyRole::WALK_NORMAL, 0L);
		getSelfRole()->begin();
		break;
	case ACT_ATTACK:
		getSelfRole()->stackCommand(EnemyRole::WALK_CF_ATTACK, 0L);
		getSelfRole()->stackCommand(EnemyRole::ATTACK, 0L);
		getSelfRole()->stackCommand(EnemyRole::MOVE_TO_HOME, 0L);
		getSelfRole()->stackCommand(EnemyRole::MOVE_AFTER_ATTACK, 0L);
		getSelfRole()->begin();
		break;
	case ACT_DEFENSE:
		getSelfRole()->stackCommand(EnemyRole::WALK_CF_DEFENSE, 0L);
		getSelfRole()->begin();
		break;
	case ACT_SKILL:
		getSelfRole()->stackCommand(EnemyRole::SKILL, 0L);
		getSelfRole()->stackCommand(EnemyRole::MOVE_AFTER_ATTACK, 0L);
		getSelfRole()->begin();
		break;
	case ACT_MOVETOTRG:
		getSelfRole()->stackCommand(EnemyRole::MOVE_TO_TRG, 0L);
		getSelfRole()->stackCommand(EnemyRole::WALK_NORMAL, 0L);
		getSelfRole()->begin();
		break;
	}
}

/*---------------------------------------------------------------------*//**
	子モードの終了通知
**//*---------------------------------------------------------------------*/
void EnemyMode::notifyChildModeEnd(short modeid, void* objParamRef)
{
	getSelfRole()->end();
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
