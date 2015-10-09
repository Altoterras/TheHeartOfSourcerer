/***********************************************************************//**
 *	EventMatterType.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_EVENT_MATTER_TYPE_H_
#define _SRCR_EVENT_MATTER_EVENT_MATTER_TYPE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

////////////////////////////////////////////////////////////////////////////
// 定数

SRCR_BEGIN_NS

// イベント マター タイプ
enum EventMatterType
{
	EVTYPE_NULL	= 0,

	EMTYPE_ACT = 1,
	EMTYPE_INTVLSAVE,
	EMTYPE_MAPCONF,
	EMTYPE_MOVIECUT,
	EMTYPE_PROG,
	EMTYPE_QUESTCONF,
	EMTYPE_REALTIMEMOVIE,
	EMTYPE_SCREFF,
	EMTYPE_SCRMODE,
	EMTYPE_SE,
	EMTYPE_SHDOWCTRL,
	EMTYPE_SHOWOBJ,
	EMTYPE_SPRITE,
	EMTYPE_STORYCONF,
	EMTYPE_TALK,
	EMTYPE_TEXFACIAL,
	EMTYPE_VALFADER,

	EMTYPE_CUSTOM	= 10000,
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_EVENT_MATTER_TYPE_H_
