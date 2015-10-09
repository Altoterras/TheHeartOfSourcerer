/***********************************************************************//**
 *	EleneEffect.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/25.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EleneEffect.h"

// Friends
#include "../common/CalcUtils.h"
#include "../body/Game.h"
#include "../particle/GameParticleDef.h"
#include "../status/EleneStat.h"
#include "../status/TransStat.h"

// External
#include "../../etk/particle/Particle.h"
#include "../../etk/particle/ParticleSys.h"
#include "../../etk/sound/SoundMngr.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Vector.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EleneEffect メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
void EleneEffect::on(const TransStat* tstat, const Vector3F* pos)
{
	appearEffect(tstat->getEleneStat(), tstat->getFuncFlags(), pos);
}

/*---------------------------------------------------------------------*//**
	エフェクトを出す
**//*---------------------------------------------------------------------*/
void EleneEffect::appearEffect(const EleneStat* eestat, u32 eeffFuncFlags, const Vector3F* pos)
{
	u32 eff = eeffFuncFlags & EleneStat::FF_MASK_EFF;
	u32 op = eeffFuncFlags & EleneStat::FF_MASK_OP;

	// 出すパーティクルの定義 ID を得る
	u16 ptcldefid = 0;
	switch(eff)
	{
	case EleneStat::FF_EFF_SMOKE:		// 煙
	case EleneStat::FF_EFF_SMOKE_L:		// 光煙
	case EleneStat::FF_EFF_SMOKE_H:		// 温煙
		ptcldefid = GameParticleDef::PTCLK_STONE_DUST;
		break;
	case EleneStat::FF_EFF_SAND:		// 砂
	case EleneStat::FF_EFF_SAND_L:		// 光砂
	case EleneStat::FF_EFF_SAND_H:		// 温砂
		ptcldefid = GameParticleDef::PTCLK_SAND_DUST;
		break;
	case EleneStat::FF_EFF_MIST:		// 靄
	case EleneStat::FF_EFF_MIST_L:		// 光靄
	case EleneStat::FF_EFF_FOG:			// 霧
	case EleneStat::FF_EFF_FOG_L:		// 光霧
		ptcldefid = GameParticleDef::PTCLK_SPRAY;
		break;
	case EleneStat::FF_EFF_WGRAIN:		// 水粒
	case EleneStat::FF_EFF_WGRAIN_L:	// 水粒（光）
	case EleneStat::FF_EFF_WGRAIN_H:	// 水粒（温）
	case EleneStat::FF_EFF_SPRAY:		// 飛沫
	case EleneStat::FF_EFF_SPRAY_L:		// 飛沫（光）
	case EleneStat::FF_EFF_SPRAY_H:		// 飛沫（温）
		ptcldefid = GameParticleDef::PTCLK_WATER;
		break;
	case EleneStat::FF_EFF_SHINE:		// 光
		ptcldefid = GameParticleDef::PTCLK_LIGHT;
		break;
	case EleneStat::FF_EFF_WIND:		// 風
		break;
	}
	if(ptcldefid == 0)	{	return;	}

	// 色指定
	const ColorU8* col = 0L;
	ColorU8 colWk;
	switch(eff)
	{
	case EleneStat::FF_EFF_SMOKE_L:		// 光煙
	case EleneStat::FF_EFF_SAND_L:		// 光砂
	case EleneStat::FF_EFF_MIST_L:		// 光靄
	case EleneStat::FF_EFF_FOG_L:		// 光霧
	case EleneStat::FF_EFF_WGRAIN_L:	// 水粒（光）
	case EleneStat::FF_EFF_SPRAY_L:		// 飛沫（光）
		col = ColorU8::setout(&colWk, 255, 255, 127, 255);
		break;
	case EleneStat::FF_EFF_SMOKE_H:		// 温煙
	case EleneStat::FF_EFF_SAND_H:		// 温砂
	case EleneStat::FF_EFF_MIST_H:		// 温靄
	case EleneStat::FF_EFF_FOG_H:		// 温霧
	case EleneStat::FF_EFF_WGRAIN_H:	// 水粒（温）
	case EleneStat::FF_EFF_SPRAY_H:		// 飛沫（温）
		col = ColorU8::setout(&colWk, 255, 127, 0, 255);
		break;
	}

	// 量を算出
	s32 amount = eestat->getCql()->sum();
	amount /= 10000 * 5;
	if(amount <= 0)			{	amount = 1;		}
	else if(amount > 16)	{	amount = 16;	}
	if(op == EleneStat::FF_OP_NOP)
	{
		amount *= 4;
	}
	
	// パーティクルを出す
	ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
	ptclsys->appearParticle(ptcldefid, pos, col, 0);
	// ２つ目乱数で
	for(int i = 0; i < amount - 1; i++)
	{
		Vector3F posRand(
			pos->x() + CalcUtils::randPlusMinusOneF(),
			pos->y() + CalcUtils::randF() * 2.0f,
			pos->z() + CalcUtils::randPlusMinusOneF()	);
		u16 ptclid = ptclsys->appearParticle(ptcldefid, pos, col, 0);
		if(ptclid != 0)
		{
			Particle* ptcl = ptclsys->getParticle(ptclid);
			if(ptcl != 0L)
			{
				s32 frameDelay = ::rand() & 0x3f;	// 0 ～ 63 フレームでばらける
				ptcl->setDelayFrame(frameDelay);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
