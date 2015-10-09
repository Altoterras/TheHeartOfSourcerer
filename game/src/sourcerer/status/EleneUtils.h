/***********************************************************************//**
 *	EleneUtils.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2014/09/08.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_ELENE_UTILS_H_
#define _SRCR_STATUS_ELENE_UTILS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Elene.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	元素エネルギー量ユーティリティ
 *
**//*---------------------------------------------------------------------*/
class EleneUtils
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
#if defined(_DEBUG)
	static void _debug_toQualityStringFromQualityFlags(VcString* str, u32 flags);
	static void _debug_toOpStringFromFuncFlags(VcString* str, u32 flags);
	static void _debug_toAtrbStringFromFuncFlags(VcString* str, u32 flags);
	static void _debug_toFormStringFromFuncFlags(VcString* str, u32 flags);
#endif

	//======================================================================
	// 変数
private:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_ELENE_UTILS_H_
