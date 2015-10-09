/***********************************************************************//**
 *	GameSoundDef.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "GameSoundDef.h"

// Friends
#include "../file/GameFileIdDef.h"

// External

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GameSoundDef 静的変数

#define BGMDEF(bgmid_, loop_start_smpl_, loop_end_smpl_) \
	SoundMngr::BgmDef(bgmid_, FILEID_FROM_BGM(bgmid_, true, false), loop_start_smpl_, loop_end_smpl_)

const SoundMngr::BgmDef GameSoundDef::_bgms[GameSoundDef::NUM_BGM] =
{
	BGMDEF(BGM_101_TITLE, 0, S32_MAX),
	BGMDEF(BGM_102_OPENING, 0, S32_MAX),
	BGMDEF(BGM_103_VILLAGE, 0, S32_MAX),
	BGMDEF(BGM_105_CORPO, 0, S32_MAX),
	BGMDEF(BGM_106_FIELD, 0, S32_MAX),
	BGMDEF(BGM_107_FOREST_4, 0, S32_MAX),
	BGMDEF(BGM_108_CONFRONT, 642490, S32_MAX),
	BGMDEF(BGM_109_MIND_CLOUD, 0, S32_MAX),
	BGMDEF(BGM_110_MIND_MEMORY, 0, S32_MAX),
	BGMDEF(BGM_111_BATTLE_2, 0, S32_MAX),
	BGMDEF(BGM_112_FOREST_5, 0, S32_MAX),
	BGMDEF(BGM_113_NOBI_RUINS, 0, S32_MAX),
	BGMDEF(BGM_114_BATTLE_3, 0, S32_MAX),
	BGMDEF(BGM_115_MYSTIC_BEING, 0, S32_MAX),
	BGMDEF(BGM_116_ACENSION, 0, S32_MAX),
	BGMDEF(BGM_117_CORPOS_SOUMA, 0, S32_MAX),
	BGMDEF(BGM_118_EP01_LAST_BOSS, 0, S32_MAX),
	BGMDEF(BGM_119_EP01_ENDING, 0, S32_MAX),
	BGMDEF(BGM_120, 0, S32_MAX),
	BGMDEF(BGM_121_JOBUTSU, 0, S32_MAX),
};

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
