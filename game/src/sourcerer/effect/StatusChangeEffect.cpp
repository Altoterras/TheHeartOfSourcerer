/***********************************************************************//**
 *	StatusChangeEffect.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/04/14.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StatusChangeEffect.h"

// Friends
#include "GaugeAnimEffect.h"
#include "../common/CalcUtils.h"
#include "../body/Game.h"
#include "../magic/MagicCluster.h"
#include "../sound/GameSoundDef.h"
#include "../status/StatusChangeResult.h"
#include "../unit/Unit.h"

// External
#include "../../etk/particle/Particle.h"
#include "../../etk/particle/ParticleSys.h"
#include "../../etk/sound/SoundMngr.h"
#include "../../tfw/lib/Vector.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StatusChangeEffect メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
void StatusChangeEffect::on(const StatusChangeResult* scres)
{
	const Unit* unitTrgRef = scres->getTargetUnit(); ASSERT(unitTrgRef != 0L);
	if(unitTrgRef == 0L)	{	return;	}

	bool isAppearEffect = false;

	// エフェクトの位置
	Vector3F posTrg(*unitTrgRef->getCenterPos());
	Vector3F diff;	// エフェクト位置ずらしのためのベクトル
	if(scres->getActorMagicCluster() != 0L)
	{
		diff = *scres->getActorMagicCluster()->getPosition() - posTrg;
	}
	else if(scres->getActorUnit() != 0L)
	{
		diff = *scres->getActorUnit()->getCenterPos() - posTrg;
	}
	diff.normalize();
	diff *= 50.0f;				// 50cm ずらす
	posTrg += diff;
	moveVectorRandom(&posTrg);	// ランダムにずらす

	// エネルギーダメージエフェクト
	if(scres->getEneValuedDamage() > 0)
	{
		appearParticle(GameParticleDef::PTCLK_HIT_ATTACK, &posTrg, 0);
		Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_PYS_DAMAGE_1, false, 0.5f, &posTrg, 0L);
		isAppearEffect = true;
	}
	// エネルギー回復エフェクト
	else if(scres->getEneValuedDamage() < 0)
	{
		appearParticle(GameParticleDef::PTCLK_CURE, &posTrg, 0);
		Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_ENE_CURE, false, 1.0f, &posTrg, 0L);
		isAppearEffect = true;
	}

	// 物理ダメージエフェクト
	if(scres->getPhysValuedDamage() > 0)
	{
		if(isAppearEffect)	{	moveVectorRandom(&posTrg);	}	// ランダムにずらす
		appearParticle(GameParticleDef::PTCLK_DAMAGE, &posTrg, 0);
		Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_PYS_DAMAGE_1, false, 1.0f, &posTrg, 0L);
		isAppearEffect = true;
	}
	// 物理回復エフェクト
	else if(scres->getPhysValuedDamage() < 0)
	{
		if(isAppearEffect)	{	moveVectorRandom(&posTrg);	}	// ランダムにずらす
		appearParticle(GameParticleDef::PTCLK_CURE, &posTrg, 0);
		Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_ENE_CURE, false, 1.0f, &posTrg, 0L);
		isAppearEffect = true;
	}

	// 持続結界を強化した
	if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_STREN_WP_VEIL))
	{
		if(isAppearEffect)	{	moveVectorRandom(&posTrg);	}	// ランダムにずらす
		appearParticle(GameParticleDef::PTCLK_CURE, &posTrg, 0);
		Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_ENE_CURE, false, 1.0f, &posTrg, 0L);
		isAppearEffect = true;
	}
	// 瞬間結界を強化した
	if(TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_STREN_BR_VEIL))
	{
		appearParticle(GameParticleDef::PTCLK_BARRIER, &posTrg, 0);
		Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_MAKE_MCLUSTER, false, 1.0f, &posTrg, 0L);
		isAppearEffect = true;
	}

	// それ以外の何かが当たった
	if(!isAppearEffect)
	{
		appearParticle(GameParticleDef::PTCLK_HIT_CLUSTER, &posTrg, 0);
	}
}

/*---------------------------------------------------------------------*//**
	パーティクル出現
**//*---------------------------------------------------------------------*/
Particle* StatusChangeEffect::appearParticle(u16 ptcldid, const Vector3F* pos, u32 flags)
{
	ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
	s16 ptclid = ptclsys->appearParticle(ptcldid, pos, flags);
	if(ptclid <= 0)
	{
		return 0L;
	}
	Particle* ptcl = ptclsys->getParticle(ptclid);
	if(ptcl == 0L)
	{
		return 0L;
	}
	return ptcl;
}

/*---------------------------------------------------------------------*//**
	位置をランダムにずらす
**//*---------------------------------------------------------------------*/
void StatusChangeEffect::moveVectorRandom(Vector3F* v)
{
	// X, Z 軸方向に -30cm ～ +30cm ずらす
	v->x() += CalcUtils::randPlusMinusOneF() * 30.0f;
	v->z() += CalcUtils::randPlusMinusOneF() * 30.0f;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
