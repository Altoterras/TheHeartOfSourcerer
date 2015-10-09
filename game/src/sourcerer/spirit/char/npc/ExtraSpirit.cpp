/***********************************************************************//**
 *	ExtraSpirit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/06/24.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ExtraSpirit.h"

// Friends
#include "EnemyMode.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../unit/Unit.h"
#include "../../../unit/UnitManager.h"
#include "../../../unit/char/npc/ExtraUnit.h"
#include "../../../episode/char/pc/MyUnit.h"

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
// ExtraSpirit 静的メンバ変数

//==========================================================================
// ExtraSpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ExtraSpirit::ExtraSpirit()
	: Spirit(SPRTTYPE_EXTRA)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ExtraSpirit::~ExtraSpirit()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ExtraSpirit::create()
{
	return Spirit::create(0L);
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void ExtraSpirit::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;
	if(gec->isLockOperating())	// 操作ロック時
	{
		Spirit::exec(res, ec);
		return;
	}

	// 歩行フレーム処理
	if(getFocusUnit() == 0L)	// 注視がないときのみ
	{
		getThisUnit()->execWalkingRandom(ec);
	}

	Spirit::exec(res, ec);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
