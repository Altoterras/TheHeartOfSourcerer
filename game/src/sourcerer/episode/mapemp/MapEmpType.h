/***********************************************************************//**
 *	MapEmpType.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_QUEST_QSEMP_QS_EMP_TYPE_H_
#define _SRCR_QUEST_QSEMP_QS_EMP_TYPE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../event/matter/implement/ProgMatter.h"

////////////////////////////////////////////////////////////////////////////
// 定数

SRCR_BEGIN_NS

// マップ用イベント マター プログラム タイプ
enum MapEmpType
{
	STEMPTYPE_NULL	= ProgMatter::PROGTYPE_NULL + 1001,

	MAPEMPTYPE_10221_1001,
	MAPEMPTYPE_10241_1702,
	MAPEMPTYPE_10242_1701,
	MAPEMPTYPE_10243_1001,
	MAPEMPTYPE_10243_1002,
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_QUEST_QSEMP_QS_EMP_TYPE_H_
