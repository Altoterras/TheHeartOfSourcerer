/***********************************************************************//**
 *	SpiritType.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/23.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SPIRIT_SPIRIT_TYPE_H_
#define _SRCR_SPIRIT_SPIRIT_TYPE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// 定数

SRCR_BEGIN_NS

// スピリット タイプ
enum SpiritType
{
	SPRTTYPE_NULL = 0,
	SPRTTYPE_MY,
	SPRTTYPE_PARTNER,
	SPRTTYPE_HELPER,
	SPRTTYPE_ENEMY,
	SPRTTYPE_EXTRA,
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_SPIRIT_TYPE_H_
