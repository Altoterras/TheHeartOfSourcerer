/***********************************************************************//**
 *	AtArFixedStrTbl.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_BODY_AT_AR_FIXED_STR_TBL_H_
#define _ATAR_BODY_AT_AR_FIXED_STR_TBL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "AtArFixedData.h"

#include "../../tfw/lib/TypeUtils.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ATAR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	固定システム文字列テーブル
 *		※	FixedStringTable.ods の定義
 *
**//*---------------------------------------------------------------------*/
class AtArFixedStrTbl
{
	//======================================================================
	// 定数
public:
	enum
	{
		// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
		// 汎用

		SID_YES									= 101,
		SID_NO									= 102,
	};
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ
public:
	// 文字列を得る
	static inline const VcString* getString(u16 strid)
	{
		#if defined(_DEBUG)
			const StringTable* strtblSys = AtArFixedData::getFixedSystemStringTable();
			if(strtblSys == 0)
			{
				return TypeUtils::getEmptyString();
			}
			const VcString* str = strtblSys->getString(strid);
			if(str == 0)
			{
				return TypeUtils::getEmptyString();
			}
			return str;
		#else
			return AtArFixedData::getFixedSystemStringTable()->getString(strid);
		#endif
	}
};

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_BODY_AT_AR_FIXED_STR_TBL_H_
