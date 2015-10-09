/***********************************************************************//**
 *	Char1401_Ungetsuiwa_EnemySpirit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/02.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Char1401_Ungetsuiwa_EnemySpirit.h"

// Friends
#include "../../../effect/StatusChangeEffect.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/Party.h"
#include "../../../unit/char/npc/EnemyUnit.h"
#include "../../../spirit/char/npc/EnemyMode.h"
#include "../../../status/ActStat.h"
#include "../../../status/CharStat.h"
#include "../../../status/TransStat.h"
#include "../../../unit/char/pc/PcUnit.h"

// External
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Char1401_Ungetsuiwa_EnemySpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	次の対決時行動の取得
**//*---------------------------------------------------------------------*/
s16 Char1401_Ungetsuiwa_EnemySpirit::decideNextConfrontAction()
{
	// 倦怠時は攻撃のみ
	if(TFW_IS_FLAG(getCharStat()->getSpStatFlags(), CharStat::SPSTAT_HEAVY))
	{
		return EnemyMode::ACT_ATTACK;
	}
	else
	{
		// 相手が一人のときは攻撃のみ
		Party* party = Game::getGame()->getMyParty();
		if(party != 0L)
		{
			int cnt = 0;
			for(int i = 0; i < party->getPcNum(); i++)
			{
				if((party->getPcUnit(i) != 0L) && party->getPcUnit(i)->isEnable())
				{
					cnt++;
				}
			}
			if(cnt <= 1)
			{
				return EnemyMode::ACT_ATTACK;
			}
		}

		// 通常判定
		return SkilledEnemySpiritBase::decideNextConfrontAction();
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Char1401_Ungetsuiwa_EnemySpirit::create()
{
	// 変数設定
	_animidSkillStart = CharUnit::ANIMID_GUARD_2;

	return SkilledEnemySpiritBase::create();
}

/*---------------------------------------------------------------------*//**
	スキル発動メッセージ作成
**//*---------------------------------------------------------------------*/
void Char1401_Ungetsuiwa_EnemySpirit::makeSkillStartMessage(VcString* msg)
{
	// メッセージ設定
	msg->set(
		VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_START_GUARD)->getRaw(),
			getThisUnit()->getMainName()->getRaw(),
			"\n")	);
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 実行中
**//*---------------------------------------------------------------------*/
bool Char1401_Ungetsuiwa_EnemySpirit::execSkillAct(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	const f32 FRAME_STEP = 10.0f * FRAMERATE;

	if(cntStep == 1)
	{
		setBefaviorMaxFrame(FRAME_STEP);	// 最大の行動フレーム数設定
	}

	setBefaviorCurrentFrame(frmcntStep);	// 現在の行動フレーム数設定

	// ガード処理
	CharStat* cstatSelf = charStat();
	cstatSelf->guardingFrame();	// 防御

	// 終了フレームまで実行
	if(frmcntStep >= FRAME_STEP)
	{
		resetBefaviorFrame();	// 行動フレームリセット
		return true;	// 次のステップへ
	}

	return false;	// 継続する
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
