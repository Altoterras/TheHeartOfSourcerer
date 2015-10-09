/***********************************************************************//**
 *	BallMc.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "BallMc.h"

// Friends
#include "../MagicSys.h"
#include "../../common/CalcUtils.h"
#include "../../body/Game.h"
#include "../../map/Map.h"
#include "../../status/StatusDrawer.h"
#include "../../unit/Unit.h"

// External
#include "../../../etk/particle/Particle.h"
#include "../../../etk/particle/ParticleSys.h"
#include "../../../tfw/lib/Matrix.h"
#include "../../../tfw/lib/Vector.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// BallMc 定数

const f32 BallMc::RADIUS_MAX = 100.0f;
const f32 BallMc::RATIO_RADIUS_TO_SCALE = 0.033333f;	// １辺 60m なので 1 ÷ 30

//==========================================================================
// BallMc メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	位置等を反映する
**//*---------------------------------------------------------------------*/
void BallMc::reflectPosture()
{
	reflectColor();	// 色の更新
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BallMc::BallMc()
	: _radius(0.0f)
	, _radiusPtcl(1.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BallMc::~BallMc()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool BallMc::create(MagicSys* mgcsysOwnrRef, u16 mcid, const MagicClusterCreateParam* mccprm)
{
	if(!MagicCluster::create(mgcsysOwnrRef, mcid, mccprm))
	{
		return false;
	}

	// 半径算出
	calcRadius();

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void BallMc::destroy()
{
	MagicCluster::destroy();
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理
**//*---------------------------------------------------------------------*/
void BallMc::exec(const ExecCtx* ec)
{
	MagicCluster::exec(ec);
	if(!isValid())	{	return;	}
	
	// 様子の更新
	updateEleneFace(ec);

	// 半径算出
	calcRadius();
}

/*---------------------------------------------------------------------*//**
	エネルギー状態による様子の更新
**//*---------------------------------------------------------------------*/
void BallMc::updateEleneFace(const ExecCtx* ec)
{
	// 色の更新
	reflectColor();
	
	// パーティクル
	appearAtrbEffect(ec,
		EleneStat::FF_EFF_SMOKE | EleneStat::FF_EFF_SMOKE_L | EleneStat::FF_EFF_SMOKE_H,
		GameParticleDef::PTCLK_STONE_DUST);
	appearAtrbEffect(ec,
		EleneStat::FF_EFF_SAND | EleneStat::FF_EFF_SAND_L | EleneStat::FF_EFF_SAND_H,
		GameParticleDef::PTCLK_SAND_DUST);
	appearAtrbEffect(ec,
		EleneStat::FF_EFF_FOG | EleneStat::FF_EFF_FOG_L | EleneStat::FF_EFF_FOG_L
		| EleneStat::FF_EFF_MIST | EleneStat::FF_EFF_MIST_L | EleneStat::FF_EFF_MIST_L,
		GameParticleDef::PTCLK_SPRAY);
	appearAtrbEffect(ec,
		EleneStat::FF_EFF_SPRAY | EleneStat::FF_EFF_SPRAY_L | EleneStat::FF_EFF_SPRAY_H
		| EleneStat::FF_EFF_WGRAIN | EleneStat::FF_EFF_WGRAIN_L | EleneStat::FF_EFF_WGRAIN_H
		| EleneStat::FF_EFF_WATER,
		GameParticleDef::PTCLK_WATER);
	appearAtrbEffect(ec,
		EleneStat::FF_EFF_FIRE,
		GameParticleDef::PTCLK_FIRE);
	appearAtrbEffect(ec,
		EleneStat::FF_EFF_WIND,
		GameParticleDef::PTCLK_WIND);
	appearAtrbEffect(ec,
		EleneStat::FF_EFF_SHINE,
		GameParticleDef::PTCLK_LIGHT);
}

/*---------------------------------------------------------------------*//**
	色の更新
**//*---------------------------------------------------------------------*/
void BallMc::reflectColor()
{
	if((_tstat != 0L) && (_tstat->getEleneStat() != 0L))
	{
		// 色の更新
		ColorU8 col;
		StatusDrawer::calcEleneColorForParticle(&col, _tstat->getEleneStat()->getDfv());
		setColor(&col);
	}
}

/*---------------------------------------------------------------------*//**
	属性エフェクトを出す
**//*---------------------------------------------------------------------*/
void BallMc::appearAtrbEffect(const ExecCtx* ec, u32 eeff, u16 ptcldid)
{
	if(TFW_IS_FLAG(_tstat->getFuncFlags(), eeff))
	{
		int t = _tstat->getEleneStat()->getDfv()->getSoil() >> 12;
		if((::rand() & 0x3ff) < t)
		{
			ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
			Vector3F pos(*_pos);
			f32 radius = _radius * 0.75f;
			pos.x() += CalcUtils::randPlusMinusOneF() * radius;
			pos.y() += CalcUtils::randPlusMinusOneF() * radius;
			pos.z() += CalcUtils::randPlusMinusOneF() * radius;
			u16 ptclid = ptclsys->appearParticle(ptcldid, &pos, 0);

			// スケール設定
			Particle* ptcl = (ptclid != 0) ? ptclsys->getParticle(ptclid) : 0L;
			if(ptcl != 0L)
			{
				f32 scale = _radiusPtcl * RATIO_RADIUS_TO_SCALE * 0.5f;
				if(scale < 1.0f)
				{
					ptcl->setScale(scale);
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	半径を計算する
**//*---------------------------------------------------------------------*/
void BallMc::calcRadius()
{
	// 半径を計算
	f32 ra = (_tstat->getEleneStat()->getDfv()->sum() + _tstat->getEleneStat()->getNql()->sum()) / (f32)Elene::ELENE_VALID_FIGURE;
	ra *= 0.005f;	//ra *= 0.01f;	// ～Ver2.x までの式【2015/01/14 r-kishi】
	_radius = ra;
	if(_radius > RADIUS_MAX)
	{
		_radius = RADIUS_MAX;
	}

	calcParticleRadius();
}

/*---------------------------------------------------------------------*//**
	パーティクル半径を計算する

	※『ソウマボール半径.ods』参照
**//*---------------------------------------------------------------------*/
void BallMc::calcParticleRadius()
{
	f32 ra = _radius;

	// 半径をソウマ玉パーティクルのスケールに変換するレート値
	if(ra < 0.0f)
	{
		ra = 0.0f;
	}
	else if(ra > 50.0f)
	{
		ra = 50.0f;
	}
	if(ra < 25.0f)
	{
		ra = (ra * ra + 312.0f);
	}
	else
	{
		ra = ra - 50.0f;
		ra = 1562.0f - (ra * ra);
	}
	ra = (ra * 0.033f);	//_radiusPtcl = (ra * 0.1f) - 20.0f;	// ～Ver2.x までの式【2015/01/14 r-kishi】
	/*
	if(ra < 10.0f)
	{
		ra = 10.0f;
	}
	*/
	 _radiusPtcl = ra;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
