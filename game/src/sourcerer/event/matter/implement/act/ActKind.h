/***********************************************************************//**
 *	ActKind.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_ACT_KIND_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_ACT_KIND_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../../Base.h"

////////////////////////////////////////////////////////////////////////////
// 定数

SRCR_BEGIN_NS

enum ActKind
{
	ACT_NULL,
	ACT_ANIM_CHG,
	ACT_APPEAR_PARTICLE,
	ACT_CAM_CONF,
	ACT_CHANGE_MAP,
	ACT_CHANGE_STAT,
	ACT_CHANGE_STORY,
	ACT_CONFRONT,
	ACT_ENTER_CONFRONT,
	ACT_FLAG,
	ACT_GET_THING,
	ACT_IF,
	ACT_INTVL_SAVE,
	ACT_MCUT,
	ACT_MSG,
	ACT_MSG_NEXT_PAGE,
	ACT_OBTAIN_CTRL,
	ACT_PLAY_MUSIC,
	ACT_PLAY_SE,
	ACT_PROG,
	ACT_RTMV,
	ACT_SCORE_UP,
	ACT_SCR_EFF,
	ACT_SCR_MODE,
	ACT_SET_POS,
	ACT_SHADOW_CTRL,
	ACT_SHOW_OBJ,
	ACT_SPRITE,
	ACT_TEX_FACIAL,
	ACT_VAL_FADER,
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_ACT_KIND_H_
