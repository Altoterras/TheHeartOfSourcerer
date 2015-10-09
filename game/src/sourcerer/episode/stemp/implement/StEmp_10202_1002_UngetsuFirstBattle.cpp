/***********************************************************************//**
 *	StEmp_10202_1002_UngetsuFirstBattle.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/06.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StEmp_10202_1002_UngetsuFirstBattle.h"

// Friends
#include "../StEmpType.h"
#include "../../char/pc/MyUnit.h"
#include "../../party/MyParty.h"
#include "../../../confront/Confront.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../status/CharStat.h"
#include "../../../unit/UnitManager.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define CTRLEVID_CF_UNGETSU		(51001)		// 雲月岩の制御用イベント ID

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StEmp_10202_1002_UngetsuFirstBattle メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void StEmp_10202_1002_UngetsuFirstBattle::begin(u32 param)
{
	// カウンタリセット
	_fcntBattle = 0.0f;

	// ロックを解除し、ユーザー操作可能に
	setLockWithParent(false);

	StEmp::begin(param);
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void StEmp_10202_1002_UngetsuFirstBattle::end()
{
	StEmp::end();

	// 雲月岩を消す
	UnitManager* unitmng = Game::getGame()->getUnitManager();
	CharUnit* cunit = (CharUnit*)unitmng->findUnitFromCtrlEvid(CTRLEVID_CF_UNGETSU);
	ASSERT(cunit != 0L);
	if(cunit != 0L)	// 念のため
	{
		cunit->setEnable(false);
	}

	// 戦闘を終了させる
	if(Game::getGame()->isConfront())	// 戦闘中
	{
		Game::getGame()->getConfront()->end(0L);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StEmp_10202_1002_UngetsuFirstBattle::StEmp_10202_1002_UngetsuFirstBattle()
	: StEmp(STEMPTYPE_10202_1002)
	, _fcntBattle(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御実装
**//*---------------------------------------------------------------------*/
void StEmp_10202_1002_UngetsuFirstBattle::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	if(Game::getGame()->isConfront())	// 戦闘中
	{
		// フレームをカウント
		_fcntBattle += ec->getDeltaFrame();

		// 時間で終了
		if(_fcntBattle >= (1.0f * 60.0f * FRAMERATE))	// 1 分以上
		{
			end();	// 強制終了
		}

		// 残生命エネルギーで終了
		if(_fcntBattle >= (10.0f * FRAMERATE))	// 最低でも 10 秒以上
		{
			MyUnit* unit = (MyUnit*)Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
			const CharStat* cstat = unit->getCharStat();
			if(cstat->getEnergy() < (cstat->getMaxEnergy() / 2))
			{
				end();	// 強制終了
			}
		}
	}
	else if(_fcntBattle > 0.0f)	// 一度戦闘になったが終了した場合
	{
		end();	// 終了
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
