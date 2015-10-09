/***********************************************************************//**
 *	QsEmpType.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_QSEMP_QS_EMP_TYPE_H_
#define _SRCR_EPISODE_QSEMP_QS_EMP_TYPE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../event/matter/implement/ProgMatter.h"

////////////////////////////////////////////////////////////////////////////
// 定数

SRCR_BEGIN_NS

// クエスト用イベント マター プログラム タイプ
enum QsEmpType
{
	STEMPTYPE_NULL	= ProgMatter::PROGTYPE_NULL + 3001,

	QSEMPTYPE_ACHIEVE_PERFORM,
	QSEMPTYPE_ACTUAL_CANCEL_QUEST,
	QSEMPTYPE_ACTUAL_FINISH_QUEST,
	QSEMPTYPE_CALL_FINISH_QUEST,
	QSEMPTYPE_CONFIRM_CANCEL_QUEST,
	QSEMPTYPE_CONFIRM_CONTRACT_QUEST,
	QSEMPTYPE_CONFIRM_HANDOVER_THING,
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_QSEMP_QS_EMP_TYPE_H_
