/***********************************************************************//**
 *	EpisodeQsEmpType.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/06.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_QSEMP_EPISODE_QS_EMP_TYPE_H_
#define _SRCR_EPISODE_QSEMP_EPISODE_QS_EMP_TYPE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../event/matter/implement/ProgMatter.h"

////////////////////////////////////////////////////////////////////////////
// 定数

SRCR_BEGIN_NS

// クエスト用イベント マター プログラム タイプ
enum EpisodeQsEmpType
{
	STEMPTYPE_NULL	= ProgMatter::PROGTYPE_NULL + 4001,

	EPQSEMPTYPE_0_1001,		// QsEmp_0_1001_ReleaseUserFreeSource
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_QSEMP_EPISODE_QS_EMP_TYPE_H_
