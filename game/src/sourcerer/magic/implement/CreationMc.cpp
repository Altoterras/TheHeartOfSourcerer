/***********************************************************************//**
 *	CreationMc.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/17.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CreationMc.h"

// Friends
#include "../MagicSys.h"
#include "../../body/Game.h"

// External
#include "../../../etk/particle/Particle.h"
#include "../../../etk/particle/ParticleSys.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CreationMc メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool CreationMc::create(MagicSys* mgcsysOwnrRef, u16 mcid, const MagicClusterCreateParam* mccprm)
{
	if(!BallMc::create(mgcsysOwnrRef, mcid, mccprm))
	{
		return false;
	}

	// 消滅フラグを予め設定
	TFW_SET_FLAG(_dcflags, DCF_IS_CREATION, true);	// 創作中クラスターである

	return true;
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理
**//*---------------------------------------------------------------------*/
void CreationMc::exec(const ExecCtx* ec)
{
	BallMc::exec(ec);
	if(!isValid())	{	return;	}
	
	// 生存時間でアルファ値を変化させる
	f32 rateLife = 1.0f;
	if((_frameLife >= 0) && (_fcntElapsed > 0))
	{
		rateLife = 1.0f - _fcntElapsed / (f32)_frameLife;
	}

	// 生成率
	f32 rateCreation =
		((_tstat->getEleneStat()->getDfv()->getAethel() / (f32)_tstat->getEleneStat()->getNql()->getAethel()) * 0.2f) +
		((_tstat->getEleneStat()->getDfv()->getWind() / (f32)_tstat->getEleneStat()->getNql()->getWind()) * 0.2f) +
		((_tstat->getEleneStat()->getDfv()->getFire() / (f32)_tstat->getEleneStat()->getNql()->getFire()) * 0.2f) +
		((_tstat->getEleneStat()->getDfv()->getWater() / (f32)_tstat->getEleneStat()->getNql()->getWater()) * 0.2f) +
		((_tstat->getEleneStat()->getDfv()->getSoil() / (f32)_tstat->getEleneStat()->getNql()->getSoil()) * 0.2f);

	// パーティクルの位置設定
	f32 scale = _radiusPtcl * RATIO_RADIUS_TO_SCALE;
	if(_arrptclid[PTCLIDX_NQL] != 0)
	{
		Particle* ptcl = Game::getGame()->getParticleSys()->getParticle(_arrptclid[PTCLIDX_NQL]);
		ptcl->setScale(scale);								// スケール設定
		ptcl->position()->copy(_pos);						// 位置設定
		ptcl->setMulAlphaColor(rateLife);					// パーティクルの乗算アルファ設定
		ptcl->reflectPosture(true);							// 位置反映
	}
	if(_arrptclid[PTCLIDX_CQL] != 0)
	{
		Particle* ptcl = Game::getGame()->getParticleSys()->getParticle(_arrptclid[PTCLIDX_CQL]);
		ptcl->setScale(scale * rateCreation);				// スケール設定
		ptcl->position()->copy(_pos);						// 位置設定
		ptcl->setMulAlphaColor(rateLife * rateCreation);	// パーティクルの乗算アルファ設定
		ptcl->reflectPosture(true);							// 位置反映
	}
}

/*---------------------------------------------------------------------*//**
	パーティクルの出現
**//*---------------------------------------------------------------------*/
bool CreationMc::appearParticles(const MagicClusterCreateParam* mccprm)
{
	_arrptclid[PTCLIDX_NQL] = Game::getGame()->getParticleSys()->appearParticle(mccprm->_ptclkind, 0L, GameParticleDef::CF_NO_CALC_VEL | Particle::CF_EXT_COLOR);
	if(_arrptclid[PTCLIDX_NQL] == 0)	{	return false;	}

	_arrptclid[PTCLIDX_CQL] = Game::getGame()->getParticleSys()->appearParticle(mccprm->_ptclkind, 0L, GameParticleDef::CF_NO_CALC_VEL | Particle::CF_EXT_COLOR);
	if(_arrptclid[PTCLIDX_CQL] == 0)	{	return false;	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
