/***********************************************************************//**
 *  StringUtils.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/28.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_STRING_UTILS_H_
#define _TFW_STRING_STRING_UTILS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "CcString.h"
#include "VcStringArray.h"
#include "VwString.h"
#include "VwStringArray.h"
#include "../collection/InsArray.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;

/*---------------------------------------------------------------------*//**
 *	文字列ユーティリティ
 *	
**//*---------------------------------------------------------------------*/
class StringUtils
{
	//======================================================================
	// メソッド
public:
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス

	// 空文字列を得る
	static const VcString* getEmptyString();

	// 文字列の分割
	static VcStringArray split(const VcString* str, const CStringBase* strSplitter);
	inline static VcStringArray split(const VcString* str, CcString strSplitter) { return split(str, &strSplitter); }
	static VcStringArray split(const VcString* str, const char cSplitter);
	static VcStringArray split(const VcString* str, const char* pszSplitter);
	static VwStringArray split(const VwString* str, const WStringBase* strSplitter);
	static VwStringArray split(const VwString* str, const u16 cSplitter);
	static VwStringArray split(const VwString* str, const u16* pszSplitter);

	// 空白削除
	static void trim(VcString* str, char cTrim);
	inline static void trim(VcString* str) { trim(str, '\0'); }
	static void trimLeft(VcString* str, char cTrim);
	inline static void trimLeft(VcString* str) { trim(str, '\0'); }
	static void trimRight(VcString* str, char cTrim);
	inline static void trimRight(VcString* str) { trim(str, '\0'); }

	// 括弧やスペースに対する処理
	static int skipBracket(const CStringBase* str, char cStart, char cEnd, int index);
	static int skipSpace(const CStringBase* str, int index);

	// パスの分割
	static void dividePath(VcString* strDir, VcString* strFile, const CStringBase* strPath);
	static void divideFileName(VcString* strName, VcString* strExt, const CStringBase* strFileName);
	static void dividePath(VwString* strDir, VwString* strFile, const WStringBase* strPath);
	static void divideFileName(VwString* strName, VwString* strExt, const WStringBase* strFileName);

	// 文字列の数値化
	static int toInteger(s32* indexParseEndOut, const CStringBase* str, s32 radix);
	inline static int toInteger(s32* indexParseEndOut, CcString str, s32 radix) { return toInteger(indexParseEndOut, &str, radix); }
	inline static int toInteger(const CStringBase* str, s32 radix) { return toInteger(0L, str, radix); }
	inline static int toInteger(CcString str, s32 radix) { return toInteger(0L, &str, radix); }
	inline static int toInteger(s32* indexParseEndOut, const CStringBase* str) { return toInteger(indexParseEndOut, str, 10); }
	inline static int toInteger(s32* indexParseEndOut, CcString str) { return toInteger(indexParseEndOut, &str, 10); }
	inline static int toInteger(const CStringBase* str) { return toInteger(0L, str, 10); }
	inline static int toInteger(CcString str) { return toInteger(0L, &str, 10); }
	static unsigned int toUnsignedInteger(s32* indexParseEndOut, const CStringBase* str, s32 radix);
	inline static unsigned int toUnsignedInteger(s32* indexParseEndOut, CcString str, s32 radix) { return toUnsignedInteger(indexParseEndOut, &str, radix); }
	inline static unsigned int toUnsignedInteger(const CStringBase* str, s32 radix) { return toUnsignedInteger(0L, str, radix); }
	inline static unsigned int toUnsignedInteger(CcString str, s32 radix) { return toUnsignedInteger(0L, &str, radix); }
	inline static unsigned int toUnsignedInteger(s32* indexParseEndOut, const CStringBase* str) { return toUnsignedInteger(indexParseEndOut, str, 10); }
	inline static unsigned int toUnsignedInteger(s32* indexParseEndOut, CcString str) { return toUnsignedInteger(indexParseEndOut, &str, 10); }
	inline static unsigned int toUnsignedInteger(const CStringBase* str) { return toUnsignedInteger(0L, str, 10); }
	inline static unsigned int toUnsignedInteger(CcString str) { return toUnsignedInteger(0L, &str, 10); }
	static bool toBoolean(s32* indexParseEndOut, const CStringBase* str);
	inline static bool toBoolean(s32* indexParseEndOut, CcString str) { return toBoolean(indexParseEndOut, str); }
	inline static bool toBoolean(const CStringBase* str) { return toBoolean(0L, str); }
	inline static bool toBoolean(CcString str) { return toBoolean(0L, &str); }
	static f64 toDouble(s32* indexParseEndOut, const CStringBase* strNumber, const CStringBase* strInfinity);
	inline static f64 toDouble(s32* indexParseEndOut, CcString strNumber, CcString strInfinity) { return toDouble(indexParseEndOut, &strNumber, &strInfinity); }
	inline static f64 toDouble(const CStringBase* strNumber, const CStringBase* strInfinity) { return toDouble(0L, strNumber, strInfinity); }
	inline static f64 toDouble(CcString strNumber, CcString strInfinity) { return toDouble(0L, &strNumber, &strInfinity); }
	static f64 toDouble(s32* indexParseEndOut, const CStringBase* strNumber, u32 radix, const CStringBase* strInfinity);
	inline static f64 toDouble(s32* indexParseEndOut, CcString strNumber, u32 radix, CcString strInfinity) { return toDouble(indexParseEndOut, &strNumber, radix, &strInfinity); }
	inline static f64 toDouble(const CStringBase* strNumber, u32 radix, const CStringBase* strInfinity) { return toDouble(0L, strNumber, radix, strInfinity); }
	inline static f64 toDouble(CcString strNumber, u32 radix, CcString strInfinity) { return toDouble(0L, &strNumber, radix, &strInfinity); }
	
	// 数値の文字列化
	static VcString toString(f64 dval, s32 radix);
	static VcString toString(f64 dval, s32 mode, s32 precision);

	// 英数の半角・全角変換
	static void conv1ByteTo2Byte(VcString* dst, const CStringBase* src);
	inline static void conv1ByteTo2Byte(VcString* dst, CcString src) { conv1ByteTo2Byte(dst, &src); }

	// 行処理
	static int getLineLength(const VcString* str, int index, int tabcnt);

	// 特殊な比較
	static bool equalsIgnoreCaseNum(const CStringBase* str1, const CStringBase* str2, s32 num);
	inline static bool equalsIgnoreCaseNum(CcString str1, CcString str2, s32 num) { return equalsIgnoreCaseNum(&str1, &str2, num); }
	static s32 compare(const CStringBase* str1, const CStringBase* str2, s32 num);
	inline static s32 compare(CcString str1, CcString str2, s32 num) { return compare(&str1, &str2, num); }
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_STRING_UTILS_H_
