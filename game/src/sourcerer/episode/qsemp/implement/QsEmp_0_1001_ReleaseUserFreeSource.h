/***********************************************************************//**
 *	QsEmp_0_1001_ReleaseUserFreeSource.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/20.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_QSEMP_EPISODE_QS_EMP_0_1001_RELEASE_USER_FREE_SOURCE_H_
#define _SRCR_EPISODE_QSEMP_EPISODE_QS_EMP_0_1001_RELEASE_USER_FREE_SOURCE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../quest/qsemp/QsEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class SimpleSprite;

/*---------------------------------------------------------------------*//**
 *	クエスト終了演出
 *
**//*---------------------------------------------------------------------*/
class QsEmp_0_1001_ReleaseUserFreeSource : public QsEmp
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void begin(u32 param);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QsEmp_0_1001_ReleaseUserFreeSource();
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_QSEMP_EPISODE_QS_EMP_0_1001_RELEASE_USER_FREE_SOURCE_H_
