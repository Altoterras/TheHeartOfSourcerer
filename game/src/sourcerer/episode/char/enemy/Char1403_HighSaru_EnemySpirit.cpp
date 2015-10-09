/***********************************************************************//**
 *	Char1403_HighSaru_EnemySpirit.cpp
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
#include "Char1403_HighSaru_EnemySpirit.h"

// Friends
#include "../../../common/CalcUtils.h"
#include "../../../effect/StatusChangeEffect.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../unit/char/npc/EnemyUnit.h"
#include "../../../spirit/char/npc/EnemyMode.h"
#include "../../../status/ActStat.h"
#include "../../../status/CharStat.h"
#include "../../../status/StatusIfEx.h"
#include "../../../status/TransStat.h"

// External
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Char1403_HighSaru_EnemySpirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Char1403_HighSaru_EnemySpirit::Char1403_HighSaru_EnemySpirit()
	: _phaseSkill(0)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Char1403_HighSaru_EnemySpirit::create()
{
	// 変数設定
	_phaseSkill = SP_NULL;

	return SkilledEnemySpiritBase::create();
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 準備
**//*---------------------------------------------------------------------*/
bool Char1403_HighSaru_EnemySpirit::execSkillPrepare(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	// フェイズ初期化
	_phaseSkill = SP_PREPARE;

	return true;
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 開始
**//*---------------------------------------------------------------------*/
bool Char1403_HighSaru_EnemySpirit::execSkillStart(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	const f32 FRAME_CHARGE_PHASE = 10.0f;
	const f32 FRAME_STEP = 5.0f * FRAMERATE;

	// 溜め開始
	switch(_phaseSkill)
	{
	case SP_PREPARE:
		{
			_phaseSkill = SP_CHARGE_START;	// 溜め開始へ
			// アニメーション変更
			getThisUnit()->changeAnim(CharUnit::ANIMID_CHARGE_START_1, false);
			// メッセージを表示する
			VcString msg(*VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_START_CHARGE)->getRaw(),
					getThisUnit()->getMainName()->getRaw(),
					"\n")	);
			Game::getGame()->getConductor()->on(&msg, getThisUnit(), 0);
			// SE
			///Game::getGame()->getSoundManager()->playSe(_seidSkillStart, false, 1.0f, getThisUnit()->getCenterPos(), 0L);
			// パーティクル
			///ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
			///ptclsys->appearParticle(_ptcldidSkillStart, getThisUnit()->getCenterPos(), 0);
		
			setBefaviorMaxFrame(FRAME_STEP);	// 最大の行動フレーム数設定
		}
		break;
	case SP_CHARGE_START:
		if(frmcntStep >= FRAME_CHARGE_PHASE)	// 溜め開始アニメーション完了フレーム
		{
			_phaseSkill = SP_CHARGING;	// 溜め中へ
			// アニメーション変更
			getThisUnit()->changeAnim(CharUnit::ANIMID_CHARGING_1, true);
		}
		break;
	case SP_CHARGING:
		if(frmcntStep >= FRAME_STEP)	// 溜め終了フレーム
		{
			_phaseSkill = SP_HIT_1;		// １打目ヒットへ
			resetBefaviorFrame();		// 行動フレームリセット
			// アニメーション変更
			getThisUnit()->changeAnim(CharUnit::ANIMID_SKILL_2, false);	// 技２アニメーションへ
			return true;	// 次のステップへ
		}
		break;
	}

	setBefaviorCurrentFrame(frmcntStep);	// 現在の行動フレーム数設定

	return false;	// 継続する
}

/*---------------------------------------------------------------------*//**
	スキルフレーム処理 - 実行中
**//*---------------------------------------------------------------------*/
bool Char1403_HighSaru_EnemySpirit::execSkillAct(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep)
{
	f32 angleHit = F32_PMAX;
	switch(_phaseSkill)
	{
	case SP_HIT_1:
		if(frmcntStep >= 7.0f)	// 攻撃フレーム
		{
			angleHit = TFW_DEG_TO_RAD(- 45.0f);	// 右半分
			_phaseSkill = SP_HIT_2;
		}
		break;
	case SP_HIT_2:
		if(frmcntStep >= 20.0f)	// 攻撃フレーム
		{
			angleHit = TFW_DEG_TO_RAD(+ 45.0f);	// 左半分
			_phaseSkill = SP_NULL;
		}
		break;
	}
	if(angleHit != F32_PMAX)
	{
		const CharLogicData* cldat = getCharLogicData(); ASSERT(cldat != 0L);
		// 範囲内の対象をリストアップ
		f32 lenEffctvSq = cldat->getSkillRange() * cldat->getSkillRange();
		List<Unit*> listUnits;
		CalcUtils::collectUnitFanRange(
			&listUnits,
			getThisUnit()->getPosition(),
			Gcalc::adjustAngle(getThisUnit()->getDirection()->y() + angleHit),
			Unit::UNITFLAG_CHAR,
			lenEffctvSq,
			TFW_COS45	);	// ±45°以内
		// リストアップしたユニットを攻撃
		for(ListIterator<Unit*> it = listUnits.iterator(); it.has(); it.next())
		{
			CharUnit* cunitTrg = (CharUnit*)it.object();
			CharStat* cstatTrg = cunitTrg->charStat(); ASSERT(cstatTrg != 0L);
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
		}

		return (_phaseSkill == SP_NULL);	// 状態終了なら次のステップへ
	}

	return false;	// 継続する
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
