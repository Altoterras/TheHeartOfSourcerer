/***********************************************************************//**
 *	SkilledEnemySpiritBase.cpp
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
#include "SkilledEnemySpiritBase.h"

// Friends
#include "../../../char/CharLogicData.h"
#include "../../../effect/StatusChangeEffect.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../unit/char/npc/EnemyUnit.h"
#include "../../../spirit/char/npc/EnemyMode.h"
#include "../../../status/ActStat.h"
#include "../../../status/CharStat.h"
#include "../../../status/TransStat.h"

// External
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// +++ Adjustable Parameter >>>
#define DIST_SKILL_REACH				(320.0f)
#define DIST_SKILL_REACH_SQ				(DIST_SKILL_REACH * DIST_SKILL_REACH)
// +++ Adjustable Parameter <<<

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SkilledEnemySpiritBase メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	次の対決時行動の取得
**//*---------------------------------------------------------------------*/
s16 SkilledEnemySpiritBase::decideNextConfrontAction()
{
	const CharLogicData* cldat = getCharLogicData(); ASSERT(cldat != 0L);
	s32 r = rand() & 127;
	if(r < cldat->getSkillProbability())
	{
		return EnemyMode::ACT_SKILL;
	}
	else
	{
		return EnemyMode::ACT_ATTACK;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SkilledEnemySpiritBase::SkilledEnemySpiritBase()
	: _reach(0.0f)
	, _reachSq(0.0f)
	, _animidSkillStart(0)
	, _seidSkillStart(0)
	, _ptcldidSkillStart(0)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SkilledEnemySpiritBase::create()
{
	// モードとロールの作成
	EnemyMode* mode = new EnemyMode();
	EnemyRole* role = new SkilledEnemyRole(this);
	if(!mode->create(this, role))
	{
		delete mode;
		return false;
	}

	// 変数準備
	_reachSq = _reach * _reach;

	return EnemySpirit::create(mode);
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 距離チェック
**//*---------------------------------------------------------------------*/
bool SkilledEnemySpiritBase::execCheckReach(ExecRes* res, const ExecCtx* ec, s32 step, s32 cntStep, f32 frmcntStep)
{
	if(_reachSq > 0.0f)
	{
		const Unit* unitTrg = getFocusUnit();
		if(unitTrg == 0L) { return true; }
		Vector3F posTrg = *unitTrg->getPosition();
		Vector3F diffSelfToEne = posTrg - *getThisUnit()->getPosition();
		if(diffSelfToEne.lengthSq() > _reachSq)
		{
			return true;
		}
	}

	return false;	// 継続
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 準備
**//*---------------------------------------------------------------------*/
bool SkilledEnemySpiritBase::execSkillPrepare(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	return true;
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 開始
**//*---------------------------------------------------------------------*/
bool SkilledEnemySpiritBase::execSkillStart(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	// アニメーション変更
	if(_animidSkillStart != 0)
	{
		getThisUnit()->changeAnim(_animidSkillStart, false);
	}
	// メッセージを表示する
	VcString msg;
	makeSkillStartMessage(&msg);
	if(msg.getLength() > 0)
	{
		Game::getGame()->getConductor()->on(&msg, getThisUnit(), 0);
	}
	// SE
	if(_seidSkillStart != 0)
	{
		Game::getGame()->getSoundManager()->playSe(_seidSkillStart, false, 1.0f, getThisUnit()->getCenterPos(), 0L);
	}
	// パーティクル
	if(_ptcldidSkillStart != 0)
	{
		ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
		ptclsys->appearParticle(_ptcldidSkillStart, getThisUnit()->getCenterPos(), 0);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 実行中
**//*---------------------------------------------------------------------*/
bool SkilledEnemySpiritBase::execSkillAct(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	return true;
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 終了待ち
**//*---------------------------------------------------------------------*/
bool SkilledEnemySpiritBase::execSkillWait(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	if(frmcntStep >= 60.0f)
	{
		return true;
	}
	return false;
}

//==========================================================================
// SkilledEnemySpiritBase:: SkilledEnemyRole メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - スキルを発動
**//*---------------------------------------------------------------------*/
int SkilledEnemySpiritBase::SkilledEnemyRole::execCommand_skill(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_PREPARE		= 1;
	const int	STEP_START			= 2;
	const int	STEP_ACT			= 3;
	const int	STEP_WAIT			= 4;
	const int	END					= 99;
	
	int step = param->_step;
	
	// 対象との距離を確認
	if(((SkilledEnemySpiritBase*)_sprtRef)->execCheckReach(res, ec, step, param->_ecntStep, param->_frmcntStep))
	{
		res->setDone();
		return END;
	}
	
	// スキルステップ処理
	switch(step)
	{
	case STEP_PREPARE:
		if(((SkilledEnemySpiritBase*)_sprtRef)->execSkillPrepare(res, ec, param->_ecntStep, param->_frmcntStep))
		{
			step++;
		}
		break;

	case STEP_START:
		if(((SkilledEnemySpiritBase*)_sprtRef)->execSkillStart(res, ec, param->_ecntStep, param->_frmcntStep))
		{
			step++;
		}
		break;

	case STEP_ACT:
		if(((SkilledEnemySpiritBase*)_sprtRef)->execSkillAct(res, ec, param->_ecntStep, param->_frmcntStep))
		{
			step++;
		}
		break;

	case STEP_WAIT:
		if(((SkilledEnemySpiritBase*)_sprtRef)->execSkillWait(res, ec, param->_ecntStep, param->_frmcntStep))
		{
			res->setDone();
		}
		break;
	}

	//STRACE("... skill ...\n");
	return step;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
