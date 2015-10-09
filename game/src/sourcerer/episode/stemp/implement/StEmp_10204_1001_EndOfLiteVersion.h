/***********************************************************************//**
 *	StEmp_10204_1001_EndOfLiteVersion.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/10/29.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10204_1001_END_OF_LITE_VERSION_H_
#define _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10204_1001_END_OF_LITE_VERSION_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../StEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	Lite バージョン（体験版）の終了
 *
**//*---------------------------------------------------------------------*/
class StEmp_10204_1001_EndOfLiteVersion : public StEmp
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
	StEmp_10204_1001_EndOfLiteVersion();
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10204_1001_END_OF_LITE_VERSION_H_
