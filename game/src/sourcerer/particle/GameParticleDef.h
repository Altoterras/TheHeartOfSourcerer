/***********************************************************************//**
 *	GameParticleDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_PARTICLE_GAME_PARTICLE_DEF_H_
#define _SRCR_PARTICLE_GAME_PARTICLE_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../etk/particle/ParticleDef.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	パーティクル定義
 *
**//*---------------------------------------------------------------------*/
class GameParticleDef : public ParticleDef
{
	//======================================================================
	// 定数
public:
	// パーティクル種別（モデル ID とは別だが、たいてい合わせてある）
	enum ParticleKind
	{
		PTCLK_MAGIC_MAKING_1		= 3,
		PTCLK_MAGIC_SERVE			= 4,

		PTCLK_FIRE					= 301,
		PTCLK_WATER					= 302,
		PTCLK_SOIL					= 303,
		PTCLK_WIND					= 304,
		PTCLK_ICE					= 305,
		PTCLK_STEAM					= 306,
		PTCLK_LIGHT					= 307,
		PTCLK_SAND_DUST				= 311,
		PTCLK_STONE_DUST			= 312,
		PTCLK_SPRAY					= 313,
		PTCLK_ITEM_SYNTHESIZE		= 351,

		PTCLK_HIT_ATTACK			= 501,
		PTCLK_DAMAGE				= 502,
		PTCLK_SHOCK_WAVE			= 503,
		PTCLK_CURE					= 504,
		PTCLK_CHARGE				= 505,
		PTCLK_DARK_SMOG				= 506,
		PTCLK_ABSORP				= 507,
		PTCLK_WEP_ENCHANT			= 508,
		PTCLK_HIT_CLUSTER			= 509,
		PTCLK_BARRIER				= 510,
		PTCLK_DETECT				= 511,
		PTCLK_VANISH				= 512,
		PTCLK_SOUMA_LOCKON			= 513,

		PTCLK_LVUP					= 515,

		PTCLK_STAT_CONFU			= 520,
		PTCLK_STAT_DULL				= 521,
		PTCLK_STAT_SLEEP			= 522,

		PTCLK_SKILL_SHOCK_WAVE		= 531,

		PTCLK_FILD_ITEM_BLINK		= 601,

		PTCLK_PROG_FAN_RANGE		= 20001,
	};

	static const s32	FRAME_PTCLK_CHARGE		= 39;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_PARTICLE_GAME_PARTICLE_DEF_H_
