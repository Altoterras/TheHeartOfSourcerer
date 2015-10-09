/***********************************************************************//**
 *	SoumaScriptUtils.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2014/09/27.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_UTILS_H_
#define _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_UTILS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "SsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ES_BEGIN_NS

ES_END_NS

SRCR_BEGIN_NS

class Elene;

/*---------------------------------------------------------------------*//**
 *	ソウマスクリプトユーティリティ
 *
**//*---------------------------------------------------------------------*/
class SoumaScriptUtils
{
	//======================================================================
	// 定数


	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static s32 matchElementNameKeyword(const CStringBase* name, bool eqnoreCase);
	static s32 matchPointOrPointsKeyword(const CStringBase* str, bool eqnoreCase);
	static u32 convOperationFlagFromString(const VcString* str);
	static bool computeEleneBalanceFromIntent(Elene* eleneNql, Elene* eleneCql, u32 flagsFunc, s32 lfeneTotalSpec);
	static bool computeEleneBalanceAttributeAdded(Elene* eleneNql, Elene* eleneCql, u32 flagsAtrb);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOUMA_SCRIPT_SOUMA_SCRIPT_UTILS_H_
