/***********************************************************************//**
 *	GameSoundDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BODY_SOUND_SOUND_DEF_H_
#define _SRCR_BODY_SOUND_SOUND_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../etk/sound/SoundMngr.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	サウンド定義
 *
**//*---------------------------------------------------------------------*/
class GameSoundDef
{
	//======================================================================
	// 定数
public:
	// BGM
	static const u16	BGM_NULL				= 0;
	static const u16	BGM_101_TITLE			= 101;
	static const u16	BGM_102_OPENING			= 102;
	static const u16	BGM_103_VILLAGE			= 103;
	static const u16	BGM_105_CORPO			= 105;
	static const u16	BGM_106_FIELD			= 106;
	static const u16	BGM_107_FOREST_4		= 107;
	static const u16	BGM_108_CONFRONT		= 108;
	static const u16	BGM_109_MIND_CLOUD		= 109;
	static const u16	BGM_110_MIND_MEMORY		= 110;
	static const u16	BGM_111_BATTLE_2		= 111;
	static const u16	BGM_112_FOREST_5		= 112;
	static const u16	BGM_113_NOBI_RUINS		= 113;
	static const u16	BGM_114_BATTLE_3		= 114;
	static const u16	BGM_115_MYSTIC_BEING	= 115;
	static const u16	BGM_116_ACENSION		= 116;
	static const u16	BGM_117_CORPOS_SOUMA	= 117;
	static const u16	BGM_118_EP01_LAST_BOSS	= 118;
	static const u16	BGM_119_EP01_ENDING		= 119;
	static const u16	BGM_120					= 120;
	static const u16	BGM_121_JOBUTSU			= 121;
	static const s32	NUM_BGM					= 20;

	// SE
	static const u16	SE_NULL				= 0;
	static const u16	SE_OK				= 10101;
	static const u16	SE_CANCEL			= 10102;
	static const u16	SE_CSR_MOVE			= 10103;
	static const u16	SE_QUEST_ADV		= 10104;
	static const u16	SE_QUEST_CANCEL		= 10105;
	static const u16	SE_CMENU_OPEN		= 10108;
	static const u16	SE_CMENU_CLOSE		= 10109;
	static const u16	SE_GAME_START		= 10110;
	static const u16	SE_WALK_SOIL		= 10201;
	static const u16	SE_RUN_SOIL			= 10202;
	static const u16	SE_FLOATRING_1		= 10211;
	static const u16	SE_BIRDSONG_1		= 10221;
	static const u16	SE_BIRDSONG_2		= 10222;
	static const u16	SE_BIRDSONG_3		= 10223;
	static const u16	SE_BUGSONG_1		= 10225;
	static const u16	SE_BUGSONG_2		= 10226;
	static const u16	SE_ROLLING_STONE	= 10231;
	static const u16	SE_FLAME			= 10235;
	static const u16	SE_WATER_MURMUR		= 10241;
	static const u16	SE_WATER_DROP		= 10243;
	static const u16	SE_WIND_CAVE		= 10255;
	static const u16	SE_SEARCHING_PAPER	= 10272;
	static const u16	SE_STREAM_NOISE		= 11111;
	static const u16	SE_STREAM_WIND		= 11112;
	static const u16	SE_SERVE_MCLUSTER	= 11203;
	static const u16	SE_EXPLOSION_1		= 12101;
	static const u16	SE_EXPLOSION_2		= 12102;
	static const u16	SE_GROUND_RUMBLING	= 12104;
	static const u16	SE_PYS_DAMAGE_1		= 12109;
	static const u16	SE_PYS_DAMAGE_2		= 12110;
	static const u16	SE_PYS_DAMAGE_3		= 12111;
	static const u16	SE_WOOD_GUARD_2		= 12113;
	static const u16	SE_IRON_GUARD_2		= 12115;
	static const u16	SE_ENE_FLING_ATTACK	= 12153;
	static const u16	SE_ENE_SHAKE_STICK	= 12154;
	static const u16	SE_MAKE_MCLUSTER	= 12157;
	static const u16	SE_SHINE_1			= 13101;
	static const u16	SE_SHINE_2			= 13102;
	static const u16	SE_ENE_CURE			= 13103;
	static const u16	SE_LIGHTING_2		= 13104;
	static const u16	SE_HOLY_CHANTING	= 13113;
	static const u16	SE_BREAK_STONE		= 17106;
	static const u16	SE_CRYSTAL_BALL		= 17113;

	//======================================================================
	// 変数
public:
	static const SoundMngr::BgmDef _bgms[NUM_BGM];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOUND_SOUND_DEF_H_
