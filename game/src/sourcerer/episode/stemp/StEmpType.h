/***********************************************************************//**
 *	StEmpType.h
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

// ストーリー用イベント マター プログラム タイプ
enum StEmpType
{
	STEMPTYPE_NULL	= ProgMatter::PROGTYPE_NULL + 2001,

	STEMPTYPE_10101_1001,
	STEMPTYPE_10102_1001,
	STEMPTYPE_10201_1001,
	STEMPTYPE_10202_1001,
	STEMPTYPE_10202_1002,
	STEMPTYPE_10203_1001,
	STEMPTYPE_10204_1001,
	STEMPTYPE_10204_1002,
	STEMPTYPE_10401_1001,
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_QUEST_QSEMP_QS_EMP_TYPE_H_
