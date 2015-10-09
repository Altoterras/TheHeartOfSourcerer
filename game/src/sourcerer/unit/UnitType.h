/***********************************************************************//**
 *	UnitType.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/23.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_UNIT_TYPE_H_
#define _SRCR_UNIT_UNIT_TYPE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// 定数

SRCR_BEGIN_NS

// ユニット タイプ
enum UnitType
{
	UNITTYPE_NULL = 0,
	UNITTYPE_MY_CHAR,
	UNITTYPE_PARTNER_CHAR,
	UNITTYPE_HELPER_CHAR,
	UNITTYPE_ENEMY_CHAR,
	UNITTYPE_EXTRA_CHAR,
	UNITTYPE_MAPOBJ,
	UNITTYPE_EQUIPMENT_ITEM,
	UNITTYPE_PUT_ITEM,
	UNITTYPE_FIELD_ITEM,
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_UNIT_TYPE_H_
