/***********************************************************************//**
 *	PcMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/08.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PcMode.h"

// Friends
#include "PcRole.h"
#include "PcSpirit.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../status/CharStat.h"
#include "../../../unit/char/pc/PcUnit.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PcMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	歩行モードへ移行
**//*---------------------------------------------------------------------*/
void PcMode::toWalking(bool isAfterConfront)
{
	if(isAfterConfront)
	{
		changeMode((s16)ACT_LEAVE_CONFRONT, false);
	}
	else
	{
		changeMode((s16)ACT_WALKING, false);
	}
}

/*---------------------------------------------------------------------*//**
	対決モードへ移行
**//*---------------------------------------------------------------------*/
void PcMode::toConfront(Confront* confrontRef)
{
	_role->setConfrontModule(confrontRef);	// 対決モジュールをロールに設定する

	changeMode((s16)ACT_ENTER_CONFRONT, false);
}

/*---------------------------------------------------------------------*//**
	イベントモードへ移行
**//*---------------------------------------------------------------------*/
void PcMode::toEvent()
{
	changeMode((s16)ACT_EVENT, false);
}

/*---------------------------------------------------------------------*//**
	NULL モードへ移行
**//*---------------------------------------------------------------------*/
void PcMode::toNull()
{
	changeMode((s16)ACT_NULL, false);
}

/*---------------------------------------------------------------------*//**
	アクション リセット
**//*---------------------------------------------------------------------*/
void PcMode::resetAction()
{
	_sprtRef->startBehavior(Spirit::BEHV_DEFAULT, 0L);
	changeMode((s16)ACT_WALKING, false);	// 即座に移行
}

/*---------------------------------------------------------------------*//**
	アクション中かどうか
**//*---------------------------------------------------------------------*/
bool PcMode::isActing()
{
	short modeid = getCurrentModeId();
	switch(modeid)
	{
	case ACT_NULL:
	case ACT_WALKING:
	/*
	case ACT_EVENT:
	case ACT_ENTER_CONFRONT:
	case ACT_LEAVE_CONFRONT:
	*/
		return false;
	default:
		return getSelfRole()->isRunCommand();
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PcMode::PcMode()
	: _role(0L)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PcMode::create(PcSpirit* sprtRef, PcRole* roleEntr)
{
	// ロールを受け取る
	_role = roleEntr;

	// 親クラスによる作成
	if(!Mode::create(1, _role, true))
	{
		return false;
	}

	// 値の初期化
	_sprtRef = sprtRef;

	// モード登録と初期モード設定は派生クラスで行うこと
	// registerMode((s16)ACT_XXXXX, new Mode());
	// registerMode((s16)ACT_XXXXX, new Mode());
	// ...

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PcMode::destroy()
{
	_role = 0L;
}

/*---------------------------------------------------------------------*//**
	子モードの終了通知
**//*---------------------------------------------------------------------*/
void PcMode::notifyChildModeEnd(short modeid, void* objParamRef)
{
	getSelfRole()->end();
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
