/***********************************************************************//**
 *	Char1406_HighKabutomushi_EnemySpirit.cpp
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
#include "Char1406_HighKabutomushi_EnemySpirit.h"

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
// 定数

// +++ Adjustable Parameter >>>
#define DIST_SKILL_REACH				(320.0f)
// +++ Adjustable Parameter <<<

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Char1406_HighKabutomushi_EnemySpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Char1406_HighKabutomushi_EnemySpirit::Char1406_HighKabutomushi_EnemySpirit()
	: _cntCharge(0)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Char1406_HighKabutomushi_EnemySpirit::create()
{
	// 変数設定
	_cntCharge = 0;
	_reach = DIST_SKILL_REACH;
	_animidSkillStart = CharUnit::ANIMID_SKILL_1;
	_seidSkillStart = GameSoundDef::SE_ENE_SHAKE_STICK;

	return SkilledEnemySpiritBase::create();
}

/*---------------------------------------------------------------------*//**
	スキル発動メッセージ作成
**//*---------------------------------------------------------------------*/
void Char1406_HighKabutomushi_EnemySpirit::makeSkillStartMessage(VcString* msg)
{
	// メッセージ設定
	msg->set(
		VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_SKILL_FIRST)->getRaw(),
			getThisUnit()->getMainName()->getRaw(),
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_HORN)->getRaw(),
			"\n")	);
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 準備
**//*---------------------------------------------------------------------*/
bool Char1406_HighKabutomushi_EnemySpirit::execSkillPrepare(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	const f32 FRAME_STEP = 2.0f * FRAMERATE;
	const s32 NUM_STEP = 3;

	if(cntStep == 1)
	{
		_cntCharge = 0;
		// アニメーション変更
		getThisUnit()->changeAnim(CharUnit::ANIMID_STOP, false);
		// エフェクト表示
		ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
		ptclsys->appearParticle(GameParticleDef::PTCLK_CHARGE, getThisUnit()->getCenterPos(), 0);
	
		setBefaviorMaxFrame(FRAME_STEP);	// 最大の行動フレーム数設定
	}

	setBefaviorCurrentFrame((_cntCharge * NUM_STEP) + frmcntStep);	// 現在の行動フレーム数設定

	if(frmcntStep >= FRAME_STEP)
	{
		_cntCharge++;
		if(_cntCharge >= NUM_STEP)
		{
			resetBefaviorFrame();		// 行動フレームリセット
			return true;	// 次のステップへ
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 実行中
**//*---------------------------------------------------------------------*/
bool Char1406_HighKabutomushi_EnemySpirit::execSkillAct(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	if(frmcntStep >= 15.0f)	// 攻撃フレーム
	{
		CharUnit* cunitTrg = (CharUnit*)getFocusUnit();
		if(cunitTrg == 0L) { return false; }
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

		return true;
	}

	return false;	// 継続する
}

#if 0
//==========================================================================
// Char1406_HighKabutomushi_EnemySpirit:: Char1406_EnemyRole メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理 - 技を発動
**//*---------------------------------------------------------------------*/
int Char1406_HighKabutomushi_EnemySpirit::Char1406_EnemyRole::execCommand_skill(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_ANIM_CHARGE_1	= 1;
	const int	STEP_ANIM_CHARGE_2	= 2;
	const int	STEP_ANIM_CHARGE_3	= 3;
	const int	STEP_ANIM_WAIT		= 4;
	const int	END					= 999;
	
	// 敵との距離を確認
	const Unit* unitTrg = _sprtRef->getFocusUnit();
	if(cunitTrg == 0L) { return END; }
	Vector3F posTrg = *unitTrg->getPosition();
	Vector3F diffSelfToEne = posTrg - *_sprtRef->getThisUnit()->getPosition();
	if(diffSelfToEne.lengthSq() > DIST_SKILL_REACH_SQ)
	{
		res->setDone();
		return END;
	}
	
	int step = param->_step;
	
	// 角攻撃
	switch(step)
	{
	case STEP_ANIM_CHARGE_1:
	case STEP_ANIM_CHARGE_2:
	case STEP_ANIM_CHARGE_3:
		if(param->_ecntStep == 1)
		{
			// アニメーション変更
			_sprtRef->getThisUnit()->changeAnim(CharUnit::ANIMID_STOP, false);
			// エフェクト表示
			ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
			ptclsys->appearParticle(GameParticleDef::PTCLK_CHARGE, _sprtRef->getThisUnit()->getCenterPos(), 0);
		}
		else if(param->_frmcntStep >= 60.0f)
		{
			step++;
		}
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
				GameFixedStrTbl::getString(GameFixedStrTbl::SID_HORN)->getRaw(),
				"\n"));
			Game::getGame()->getConductor()->on(&msg, _sprtRef->getThisUnit(), 0);
			// SE
			Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_ENE_SHAKE_STICK, false, 1.0f, _sprtRef->getThisUnit()->getCenterPos(), 0L);
		}
		else if(param->_frmcntStep >= 15.0f)	// 攻撃フレーム
		{
			CharUnit* cunitTrg = (CharUnit*)_sprtRef->getFocusUnit();
			if(cunitTrg == 0L) { return END; }
			CharStat* cstatTrg = cunitTrg->charStat(); ASSERT(cstatTrg != 0L);
			StatusChangeResult scres(cunitTrg);
			TransStat tstat;
			Elene eeOql(150000, 50000, 100000, 100000, 100000);
			Elene eeCql(150000, 50000, 160000, 140000, 100000);
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
