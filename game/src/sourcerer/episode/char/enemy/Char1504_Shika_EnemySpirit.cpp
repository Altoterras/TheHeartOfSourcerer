/***********************************************************************//**
 *	Char1504_Shika_EnemySpirit.cpp
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
#include "Char1504_Shika_EnemySpirit.h"

// Friends
#include "../../../common/CalcUtils.h"
#include "../../../effect/StatusChangeEffect.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../magic/MagicInfluencer.h"
#include "../../../particle/GameParticleDef.h"
#include "../../../sound/GameSoundDef.h"
#include "../../../spirit/char/npc/EnemyMode.h"
#include "../../../status/ActStat.h"
#include "../../../status/CharStat.h"
#include "../../../status/TransStat.h"
#include "../../../unit/char/npc/EnemyUnit.h"

// External
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/lib/Matrix.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Char1504_Shika_EnemySpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Char1504_Shika_EnemySpirit::create()
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
void Char1504_Shika_EnemySpirit::makeSkillStartMessage(VcString* msg)
{
	// メッセージ設定
	msg->set(
		VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_SKILL_FIRST)->getRaw(),
			getThisUnit()->getMainName()->getRaw(),
			GameFixedStrTbl::getString(GameFixedStrTbl::SID_SHOCKWAVE)->getRaw(),
			"\n")	);
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 実行中
**//*---------------------------------------------------------------------*/
bool Char1504_Shika_EnemySpirit::execSkillAct(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	const f32 FRAME_STEP = 0.76667f * FRAMERATE;

	if(cntStep == 1)
	{
		setBefaviorMaxFrame(FRAME_STEP);	// 最大の行動フレーム数設定
	}

	setBefaviorCurrentFrame(frmcntStep);	// 現在の行動フレーム数設定

	if(frmcntStep >= FRAME_STEP)	// 攻撃フレーム
	{
		const CharLogicData* cldat = getCharLogicData(); ASSERT(cldat != 0L);
		// 範囲内の対象をリストアップ
		f32 lenEffctvSq = cldat->getSkillRange() * cldat->getSkillRange();
		List<Unit*> listUnits;
		CalcUtils::collectUnitFanRange(
			&listUnits,
			getThisUnit()->getPosition(),
			getThisUnit()->getDirection()->y(),
			Unit::UNITFLAG_PC,
			lenEffctvSq,
			TFW_COS45	);	// ±45°以内
		// リストアップしたユニットにクラスタを投げる
		Unit* unitTrg = 0L;
		for(ListIterator<Unit*> it = listUnits.iterator(); it.has(); it.next())
		{
			unitTrg = it.object();
			break;
		}

#if 1
		TransStat tstat;
		EleneStat eestatSkill(cldat->getSkillOqlElene(), cldat->getSkillCurElene());
		tstat.setupForSkill(getThisUnit(), &eestatSkill);
		MagicInfluencer::serveGeneralMc(getThisUnit(), unitTrg, &tstat, GameParticleDef::PTCLK_SKILL_SHOCK_WAVE);
#else
		MagicSys* mgcsys = Game::getGame()->getMagicSys(); ASSERT(mgcsys != 0L);
		MagicClusterCreateParam mccprm(MagicClusterType::SERVED, GameParticleDef::PTCLK_SKILL_SHOCK_WAVE);
		EleneStat eestatSkill(cldat->getSkillOqlElene(), cldat->getSkillCurElene());
		Vector3F pos(*getThisUnit()->getCenterPos());
		mccprm._unitOwnrRef = getThisUnit();
		mccprm._unitMainTrgRef = unitTrg;
		mccprm._pos = &pos;
		mccprm._tstat.setupForSkill(getThisUnit(), &eestatSkill);
		mccprm._lifeframe = cldat->getSkillCurElene()->getWater() / Elene::ELENE_VALID_FIGURE;
		mccprm._isNoOnlyTrgColl = true;
		u16 mcidNew = mgcsys->appearCluster(&mccprm);
		if(mcidNew == 0)		{	return true;	}	// 成功ではないが終えるために次のステップへ
		MagicCluster* mcNew = mgcsys->getCluster(mcidNew);
		if(mcNew == 0L)			{	return true;	}	// 成功ではないが終えるために次のステップへ
		TFW_SET_FLAG(*mcNew->ctrlFlags(), MagicCluster::CTRLF_DIR_TO_ROTZ, true);	// 方向を Z 軸回転に反映させる

		// クラスタ発射
		/*
		f32 acc = cldat->getSkillCurElene()->getWind() * (0.1f / (f32)Elene::ELENE_VALID_FIGURE);
		if(acc < 0.033f)	{	acc = 0.033f;	}
		Vector3F accMc(0.0, 0.0, acc);
		Matrix4F mtxRot;
		mtxRot.rotateY(getThisUnit()->getDirection()->y());
		mtxRot.mult(&accMc, &accMc);
		mcNew->accelerate(&accMc);
		*/
		f32 speed = cldat->getSkillCurElene()->getWind() * (0.1f / (f32)Elene::ELENE_VALID_FIGURE);
		if(speed < 0.033f)	{ speed = 0.033f; }
		Vector3F velMc(0.0f, 0.0f, speed);
		Matrix4F mtxRot;
		mtxRot.rotateY(getThisUnit()->getDirection()->y());
		mtxRot.mult(&velMc, &velMc);
		mcNew->setInitialVelocity(&velMc);
		mcNew->reflectPosture();
#endif

		resetBefaviorFrame();		// 行動フレームリセット
		return true;
	}

	return false;	// 継続する
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
