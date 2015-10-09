/***********************************************************************//**
 *	Char1502_Usagi_EnemySpirit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/07.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Char1502_Usagi_EnemySpirit.h"

// Friends
#include "../../../effect/StatusChangeEffect.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../particle/GameParticleDef.h"
#include "../../../sound/GameSoundDef.h"
#include "../../../spirit/char/npc/EnemyMode.h"
#include "../../../status/ActStat.h"
#include "../../../status/CharStat.h"
#include "../../../status/StatusIfEx.h"
#include "../../../status/TransStat.h"
#include "../../../unit/char/npc/EnemyUnit.h"

// External
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Char1502_Usagi_EnemySpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Char1502_Usagi_EnemySpirit::create()
{
	// 変数設定
	_animidSkillStart = CharUnit::ANIMID_SKILL_1;
	_seidSkillStart = GameSoundDef::SE_STREAM_WIND;
	_ptcldidSkillStart = GameParticleDef::PTCLK_SOIL;

	return SkilledEnemySpiritBase::create();
}

/*---------------------------------------------------------------------*//**
	スキル発動メッセージ作成
**//*---------------------------------------------------------------------*/
void Char1502_Usagi_EnemySpirit::makeSkillStartMessage(VcString* msg)
{
	// メッセージ設定
	msg->set(
		VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_SKILL_FIRST)->getRaw(),
			getThisUnit()->getMainName()->getRaw(),
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_ISITSUBUTE)->getRaw(),
			"\n")	);
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 実行中
**//*---------------------------------------------------------------------*/
bool Char1502_Usagi_EnemySpirit::execSkillAct(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	const f32 FRAME_STEP = 0.5f * FRAMERATE;

	if(cntStep == 1)
	{
		setBefaviorMaxFrame(FRAME_STEP);	// 最大の行動フレーム数設定
	}

	setBefaviorCurrentFrame(frmcntStep);	// 現在の行動フレーム数設定

	if(frmcntStep >= FRAME_STEP)	// 攻撃フレーム
	{
		CharUnit* cunitTrg = (CharUnit*)getFocusUnit();
		if(cunitTrg == 0L) { return true; }
		CharStat* cstatTrg = cunitTrg->charStat(); ASSERT(cstatTrg != 0L);
		const CharLogicData* cldat = getCharLogicData(); ASSERT(cldat != 0L);

		TransStat tstat;
		EleneStat eestatSkill(cldat->getSkillOqlElene(), cldat->getSkillCurElene());
		tstat.setupForSkill(getThisUnit(), &eestatSkill);
#if 1
		StatusIfEx::influence(cstatTrg, &tstat, true, true);
#else
		StatusChangeResult scres(cunitTrg);
		StatusInfluencer::influence(&scres, cstatTrg, &tstat);	// ステータス影響
		StatusChangeEffect::on(&scres);							// エフェクト表示
		Game::getGame()->getConductor()->on(&scres, 0);			// 結果を処理
#endif

		resetBefaviorFrame();		// 行動フレームリセット
		return true;
	}

	return false;	// 継続する
}

#if 0
//==========================================================================
// Char1502_Usagi_EnemySpirit:: Char1502_EnemyRole メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 技を発動
**//*---------------------------------------------------------------------*/
int Char1502_Usagi_EnemySpirit::Char1502_EnemyRole::execCommand_skill(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_ANIM_START		= 1;
	const int	STEP_ANIM_WAIT		= 2;
	const int	END					= 999;
	
	int step = param->_step;
	
	// スキル - 石つぶて
	switch(step)
	{
	case STEP_ANIM_START:
		step = STEP_ANIM_WAIT;
		break;

	case STEP_ANIM_WAIT:
		if(param->_ecntStep == 1)
		{
			// アニメーション変更
			_sprtRef->getThisUnit()->changeAnim(CharUnit::ANIMID_SKILL_1, false);
			// メッセージを表示する
			ASSERT(_confrontRef != 0L);
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_SKILL_FIRST)->getRaw(),
				_sprtRef->getThisUnit()->getMainName()->getRaw(),
				GameFixedStrTbl::getString(GameFixedStrTbl::SID_ISITSUBUTE)->getRaw(),
				"\n"));
			Game::getGame()->getConductor()->on(&msg, _sprtRef->getThisUnit(), 0);
			// SE
			Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_STREAM_WIND, false, 1.0f, _sprtRef->getThisUnit()->getCenterPos(), 0L);
			// パーティクル
			ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
			ptclsys->appearParticle(GameParticleDef::PTCLK_SOIL, _sprtRef->getThisUnit()->getCenterPos(), 0);
		}
		else if(param->_frmcntStep >= 15.0f)	// 攻撃フレーム
		{
			CharUnit* cunitTrg = (CharUnit*)_sprtRef->getFocusUnit();
			if(cunitTrg == 0L) { return END; }
			CharStat* cstatTrg = cunitTrg->charStat(); ASSERT(cstatTrg != 0L);
			StatusChangeResult scres(cunitTrg);
			TransStat tstat;
			Elene eeOql(40000, 15000, 5500, 5500, 20500);
			Elene eeCql(40000, 15000, 6000, 17000, 20500);
			EleneStat eestatSkill(&eeOql, &eeCql);
			tstat.setupForSkill(_sprtRef->getThisUnit(), &eestatSkill);
#if 1
			StatusIfEx::influence(cstatTrg, &tstat, true, true);
#else
			StatusInfluencer::influence(&scres, cstatTrg, &tstat);	// ステータス影響
			StatusChangeEffect::on(&scres);							// エフェクト表示
			Game::getGame()->getConductor()->on(&scres, 0);			// 結果を処理
#endif

			step = END;
		}
		break;
		
	case END:
		if(param->_frmcntStep >= 60.0f)
		{
			res->setDone();
		}
		break;
	}

	//STRACE("... skill ...\n");

	return step;
}
#endif

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
