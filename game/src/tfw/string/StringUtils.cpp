/***********************************************************************//**
 *  StringUtils.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/28.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StringUtils.h"

// Friends
#include "VcString.h"
#include "../lib/TypeUtils.h"

// External

// Library
#include <string.h>
#include <ctype.h>
#if SUPPORT_EXTLIB_DTOA
	#include <dtoa/dtoa.h>
#endif

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 静的関数

/*---------------------------------------------------------------------*//**
	文字列の分割
	@param	cSplitter		スプリッタ文字
	@return	作成した文字列配列
**//*---------------------------------------------------------------------*/
template<class TYPE, class CLASS, class ARRAY_CLASS>
static void splitTemplateByChar(ARRAY_CLASS* arr, const CLASS* str, const TYPE cSplitter)
{
	int lenSplitter = 1;

	// 要素数をカウント
	int cnt = 0;
	int idxStart = 0;
	int idxEnd = 0;
	while(true)
	{
		idxEnd = str->find(cSplitter, idxStart);
		if(idxEnd == -1)
		{
			if(idxStart <= (str->getLength() - 1))
			{
				cnt++;
			}
			break;
		}
		cnt++;
		idxStart = idxEnd + lenSplitter;
	}

	// 文字列配列を作成する
	CLASS** strarr = new CLASS*[cnt];
	cnt = 0;
	idxStart = 0;
	idxEnd = 0;
	while(true)
	{
		idxEnd = str->find(cSplitter, idxStart);
		if(idxEnd == -1)
		{
			if(idxStart <= (str->getLength() - 1))
			{
				strarr[cnt++] = new CLASS( str->substring(idxStart) );
			}
			break;
		}

		strarr[cnt++] = new CLASS( str->substring(idxStart, idxEnd) );
		idxStart = idxEnd + lenSplitter;
	}

	arr->set(strarr, cnt);
}

/*---------------------------------------------------------------------*//**
	文字列の分割
	@param	pszSplitter		スプリッタ文字列
	@param	idxBegin		開始インデックス
	@return	作成した文字列配列
**//*---------------------------------------------------------------------*/
template<class TYPE, class CLASS, class ARRAY_CLASS>
static void splitTemplateByStr(ARRAY_CLASS* arr, const CLASS* str, const TYPE* pszSplitter, int lenSplitter)
{
	// 要素数をカウント
	int cnt = 0;
	int idxStart = 0;
	int idxEnd = 0;
	while(true)
	{
		idxEnd = str->find(pszSplitter, idxStart);
		if(idxEnd == -1)
		{
			if(idxStart <= (str->getLength() - 1))
			{
				cnt++;
			}
			break;
		}
		cnt++;
		idxStart = idxEnd + lenSplitter;
	}

	// 文字列配列を作成する
	CLASS** strarr = new CLASS*[cnt];
	cnt = 0;
	idxStart = 0;
	idxEnd = 0;
	while(true)
	{
		idxEnd = str->find(pszSplitter, idxStart);
		if(idxEnd == -1)
		{
			if(idxStart <= (str->getLength() - 1))
			{
				strarr[cnt++] = new CLASS( str->substring(idxStart) );
			}
			break;
		}

		strarr[cnt++] = new CLASS( str->substring(idxStart, idxEnd) );
		idxStart = idxEnd + lenSplitter;
	}

	arr->set(strarr, cnt);
}

/*---------------------------------------------------------------------*//**
	パスを分離する
**//*---------------------------------------------------------------------*/
template<class TYPE, class CLASS, class CLASS_BASE>
static void dividePathTemplate(CLASS* strDir, CLASS* strFile, const TYPE* pcs, const CLASS_BASE* strPath)
{
	const TYPE* src = strPath->getRaw();
	pcs = VStringBase<TYPE, CLASS_BASE>::findLastStrByChar(src, '/');
	if(pcs == 0L)	{	pcs = VStringBase<TYPE, CLASS_BASE>::findLastStrByChar(src, '\\');	}

	if(pcs == 0L)
	{
		TYPE z = 0;
		if(strDir != 0L)	{	strDir->set(&z);		}
		if(strFile != 0L)	{	strFile->set(src);		}
	}
	else
	{
		if(strDir != 0L)	{	strDir->set(src, (int)(pcs - src + 1));	}	// 最後の \ を含める
		if(strFile != 0L)	{	strFile->set(pcs + 1);	}
	}
}

/*---------------------------------------------------------------------*//**
	ファイル名を名前と拡張子に分離する
**//*---------------------------------------------------------------------*/
template<class TYPE, class CLASS, class CLASS_BASE>
static void divideFileNameTemplate(CLASS* strName, CLASS* strExt, const TYPE* pcs, const CLASS_BASE* strFileName)
{
	const TYPE* src = strFileName->getRaw();
	pcs = VStringBase<TYPE, CLASS_BASE>::findLastStrByChar(src, '.');

	if(pcs == 0L)
	{
		TYPE z = 0;
		if(strName != 0L)	{	strName->set(&z);		}
		if(strExt != 0L)	{	strExt->set(src);		}
	}
	else
	{
		if(strName != 0L)	{	strName->set(src, (int)(pcs - src));	}	// 最後の . を含めない
		if(strExt != 0L)	{	strExt->set(pcs);		}					// . を含める
	}
}

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StringUtils メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	空文字列を得る
	@return	空文字列
**//*---------------------------------------------------------------------*/
const VcString* StringUtils::getEmptyString()
{
	return TypeUtils::getEmptyString();
}

/*---------------------------------------------------------------------*//**
	文字列の分割
	@param	strSplitter		スプリッタ文字列
	@return	作成した文字列配列
**//*---------------------------------------------------------------------*/
VcStringArray StringUtils::split(const VcString* str, const CStringBase* strSplitter)
{
	return split(str, strSplitter->getRaw());
}

/*---------------------------------------------------------------------*//**
	文字列の分割
	@param	cSplitter		スプリッタ文字
	@return	作成した文字列配列
**//*---------------------------------------------------------------------*/
VcStringArray StringUtils::split(const VcString* str, const char cSplitter)
{
	VcStringArray strarr;
	splitTemplateByChar(&strarr, str, cSplitter);
	return strarr;
}

/*---------------------------------------------------------------------*//**
	文字列の分割
	@param	pszSplitter		スプリッタ文字列
	@param	idxBegin		開始インデックス
	@return	作成した文字列配列
**//*---------------------------------------------------------------------*/
VcStringArray StringUtils::split(const VcString* str, const char* pszSplitter)
{
	int lenSplitter = (int)::strlen(pszSplitter);
	VcStringArray strarr;
	splitTemplateByStr(&strarr, str, pszSplitter, lenSplitter);
	return strarr;
}

/*---------------------------------------------------------------------*//**
	文字列の分割
	@param	strSplitter		スプリッタ文字列
	@return	作成した文字列配列
**//*---------------------------------------------------------------------*/
VwStringArray StringUtils::split(const VwString* str, const WStringBase* strSplitter)
{
	return split(str, strSplitter->getRaw());
}

/*---------------------------------------------------------------------*//**
	文字列の分割
	@param	cSplitter		スプリッタ文字
	@return	作成した文字列配列
**//*---------------------------------------------------------------------*/
VwStringArray StringUtils::split(const VwString* str, const u16 cSplitter)
{
	VwStringArray strarr;
	splitTemplateByChar(&strarr, str, cSplitter);
	return strarr;
}

/*---------------------------------------------------------------------*//**
	文字列の分割
	@param	pszSplitter		スプリッタ文字列
	@param	idxBegin		開始インデックス
	@return	作成した文字列配列
**//*---------------------------------------------------------------------*/
VwStringArray StringUtils::split(const VwString* str, const u16* pszSplitter)
{
	int lenSplitter = 0;
	while(pszSplitter[lenSplitter] != '\0'){ lenSplitter++; }
	VwStringArray strarr;
	splitTemplateByStr(&strarr, str, pszSplitter, lenSplitter);
	return strarr;
}

/*---------------------------------------------------------------------*//**
	空白削除
	@param		pstr			文字列
	@param		cTrim			トリムする指定文字
								'\0' を指定した場合は空白
								空白は ' ', '\t', '\r', '\n'
**//*---------------------------------------------------------------------*/
void StringUtils::trim(VcString* str, char cTrim)
{
	int indexBegin = 0;
	int indexEnd = 0;
	if(cTrim != '\0')
	{
		for(int i = 0; i < str->getLength(); i++)
		{
			char c = str->getAt(i);
			if(c != cTrim)
			{
				indexBegin = i;
				break;
			}
		}
		for(int i = str->getLength() - 1; i >= 0; i--)
		{
			char c = str->getAt(i);
			if(c != cTrim)
			{
				indexEnd = i + 1;
				break;
			}
		}
	}
	else
	{
		for(int i = 0; i < str->getLength(); i++)
		{
			char c = str->getAt(i);
			if((c != ' ') && (c != '\t') && (c != '\r') && (c != '\n'))
			{
				indexBegin = i;
				break;
			}
		}
		for(int i = str->getLength() - 1; i >= 0; i--)
		{
			char c = str->getAt(i);
			if((c != ' ') && (c != '\t') && (c != '\r') && (c != '\n'))
			{
				indexEnd = i + 1;
				break;
			}
		}
	}
	*str = str->substring(indexBegin, indexEnd);
}

/*---------------------------------------------------------------------*//**
	左空白削除
	@param		pstr			文字列
	@param		cTrim			トリムする指定文字
								'\0' を指定した場合は空白
								空白は ' ', '\t', '\r', '\n'
**//*---------------------------------------------------------------------*/
void StringUtils::trimLeft(VcString* str, char cTrim)
{
	int indexBegin = 0;
	if(cTrim != '\0')
	{
		for(int i = 0; i < str->getLength(); i++)
		{
			char c = str->getAt(i);
			if(c != cTrim)
			{
				indexBegin = i;
				break;
			}
		}
	}
	else
	{
		for(int i = 0; i < str->getLength(); i++)
		{
			char c = str->getAt(i);
			if((c != ' ') && (c != '\t') && (c != '\r') && (c != '\n'))
			{
				indexBegin = i;
				break;
			}
		}
	}
	*str = str->substring(indexBegin);
}

/*---------------------------------------------------------------------*//**
	右空白削除
	@param		pstr			文字列
	@param		cTrim			トリムする指定文字
								'\0' を指定した場合は空白
								空白は ' ', '\t', '\r', '\n'
**//*---------------------------------------------------------------------*/
void StringUtils::trimRight(VcString* str, char cTrim)
{
	int indexMax = str->getLength() - 1;
	int indexEnd = indexMax;
	if(cTrim != '\0')
	{
		for(int i = indexMax; i >= 0; i--)
		{
			char c = str->getAt(i);
			if(c != cTrim)
			{
				indexEnd = i + 1;
				break;
			}
		}
	}
	else
	{
		for(int i = indexMax; i >= 0; i--)
		{
			char c = str->getAt(i);
			if((c != ' ') && (c != '\t') && (c != '\r') && (c != '\n'))
			{
				indexEnd = i + 1;
				break;
			}
		}
	}
	*str = str->substring(0, indexEnd);
}

/*---------------------------------------------------------------------*//**
	括弧をスキップする
**//*---------------------------------------------------------------------*/
int	StringUtils::skipBracket(const CStringBase* str, char cStart, char cEnd, int index)
{
	const char* s = str->getRaw();
	int lenStr = str->getLength();
	int indent = 0;
	for(int i = index; i < lenStr; i++)
	{
		char c = s[i];
		if(c == cStart)
		{
			indent++;
		}
		else if(c == cEnd)
		{
			indent--;
			if(indent == 0)
			{
				return i + 1;
			}
		}
	}
	return lenStr;
}

/*---------------------------------------------------------------------*//**
	スペースをスキップする
**//*---------------------------------------------------------------------*/
int	StringUtils::skipSpace(const CStringBase* str, int index)
{
	const char* s = str->getRaw();
	int lenStr = str->getLength();
	for(int i = index; i < lenStr; i++)
	{
		if(s[i] != ' ')
		{
			return i + 1;
		}
	}
	return lenStr;
}

/*---------------------------------------------------------------------*//**
	パスを分離する
**//*---------------------------------------------------------------------*/
void StringUtils::dividePath(VcString* strDir, VcString* strFile, const CStringBase* strPath)
{
	dividePathTemplate(strDir, strFile, (char*)0L, strPath);
}

/*---------------------------------------------------------------------*//**
	ファイル名を名前と拡張子に分離する
**//*---------------------------------------------------------------------*/
void StringUtils::divideFileName(VcString* strName, VcString* strExt, const CStringBase* strFileName)
{
	divideFileNameTemplate(strName, strExt, (char*)0L, strFileName);
}

/*---------------------------------------------------------------------*//**
	パスを分離する
**//*---------------------------------------------------------------------*/
void StringUtils::dividePath(VwString* strDir, VwString* strFile, const WStringBase* strPath)
{
	dividePathTemplate(strDir, strFile, (u16*)0L, strPath);
}

/*---------------------------------------------------------------------*//**
	ファイル名を名前と拡張子に分離する
**//*---------------------------------------------------------------------*/
void StringUtils::divideFileName(VwString* strName, VwString* strExt, const WStringBase* strFileName)
{
	divideFileNameTemplate(strName, strExt, (u16*)0L, strFileName);
}

/*---------------------------------------------------------------------*//**
	整数（int）化
**//*---------------------------------------------------------------------*/
int StringUtils::toInteger(s32* indexParseEndOut, const CStringBase* str, s32 radix)
{
	int n = 0;
	int m = 1;
	int i = 0;
	if(radix <= 10)
	{
		while(i < str->getLength())
		{
			char c = str->getAt(i);
			int nw = 0;
			if(('0' <= c) && (c <= '9'))		{	nw = c - '0';		}
			else if((i == 0) && (c == '-'))		{	m = -1;				}
			else								{	break;				}
			n = n * radix + nw;
			i++;
		}
	}
	else
	{
		while(i < str->getLength())
		{
			char c = str->getAt(i);
			int nw = 0;
			if(('0' <= c) && (c <= '9'))		{	nw = c - '0';		}
			else if(('a' <= c) && (c <= 'z'))	{	nw = 10 + c - 'a';	}
			else if(('A' <= c) && (c <= 'Z'))	{	nw = 10 + c - 'A';	}
			else if((i == 0) && (c == '-'))		{	m = -1;				}
			else								{	break;				}
			n = n * radix + nw;
			i++;
		}
	}
	n = n * m;
	if(indexParseEndOut != 0L)	{	*indexParseEndOut = (s32)i;	}
	return n;
}

/*---------------------------------------------------------------------*//**
	符号なし整数（unsigned int）化
**//*---------------------------------------------------------------------*/
unsigned int StringUtils::toUnsignedInteger(s32* indexParseEndOut, const CStringBase* str, s32 radix)
{
	unsigned int n = 0;
	int i = 0;
	if(radix <= 10)
	{
		while(i < str->getLength())
		{
			char c = str->getAt(i);
			int nw = 0;
			if(('0' <= c) && (c <= '9'))		{	nw = c - '0';		}
			else								{	break;				}
			n = n * radix + nw;
			i++;
		}
	}
	else
	{
		while(i < str->getLength())
		{
			char c = str->getAt(i);
			int nw = 0;
			if(('0' <= c) && (c <= '9'))		{	nw = c - '0';		}
			else if(('a' <= c) && (c <= 'z'))	{	nw = 10 + c - 'a';	}
			else if(('A' <= c) && (c <= 'Z'))	{	nw = 10 + c - 'A';	}
			else								{	break;				}
			n = n * radix + nw;
			i++;
		}
	}
	if(indexParseEndOut != 0L)	{	*indexParseEndOut = (s32)i;	}
	return n;
}

/*---------------------------------------------------------------------*//**
	真偽数値（bool）化
**//*---------------------------------------------------------------------*/
#if defined(_IOS)
#include <ctype.h>
int __stricmp(const char* s1, const char* s2)
{
	int len1 = (int)::strlen(s1);
	int len2 = (int)::strlen(s2);
	char* s1L = new char[len1 + 1];
	char* s2L = new char[len2 + 1];
	for(int i = 0; i < len1; i++) { s1L[i] = tolower(s1[i]); }
	s1L[len1] = 0;
	for(int i = 0; i < len2; i++) { s2L[i] = tolower(s2[i]); }
	s2L[len2] = 0;
	int res = ::strcmp(s1L, s2L);
	delete[] s1L;
	delete[] s2L;
	return res;
}
#endif
#if defined(_WINDOWS)
	#define STRICMP ::_stricmp
#elif defined(_IOS)
	#define STRICMP __stricmp
#elif defined(_ANDROID)
	#define STRICMP strcasecmp
#endif
bool StringUtils::toBoolean(s32* indexParseEndOut, const CStringBase* str)
{
	const char* pc = str->getRaw();
	if((str->getLength() == 1) && (pc[0] == '1'))
	{
		if(indexParseEndOut != 0L)	{	*indexParseEndOut = 1;	}
		return true;
	}
	if((str->getLength() == 1) && (pc[0] == '0'))
	{
		if(indexParseEndOut != 0L)	{	*indexParseEndOut = 1;	}
		return false;
	}
	if(STRICMP(pc, "true") == 0)
	{
		if(indexParseEndOut != 0L)	{	*indexParseEndOut = 4;	}
		return true;
	}
	if(STRICMP(pc, "false") == 0)
	{
		if(indexParseEndOut != 0L)	{	*indexParseEndOut = 5;	}
		return false;
	}
	if(STRICMP(pc, "yes") == 0)
	{
		if(indexParseEndOut != 0L)	{	*indexParseEndOut = 3;	}
		return true;
	}
	if(STRICMP(pc, "no") == 0)
	{
		if(indexParseEndOut != 0L)	{	*indexParseEndOut = 2;	}
		return false;
	}
	if(indexParseEndOut != 0L)	{	*indexParseEndOut = 0;	}
	return false;
}

/*---------------------------------------------------------------------*//**
	10 進数文字列を倍制度浮動小数点（double）数値化
**//*---------------------------------------------------------------------*/
f64 StringUtils::toDouble(s32* indexParseEndOut, const CStringBase* strNumber, const CStringBase* strInfinity)
{
#if SUPPORT_EXTLIB_DTOA
	const s8* pc = (const s8*)strNumber->getRaw();
	const s8* pc0 = pc;

	// スペースをスキップ
	while(true)
	{
		if((*pc == '\t') || (*pc == '\n') || (*pc == '\v') || (*pc == '\f') || (*pc == '\r') || (*pc == ' '))
		{
		}
		else if(*pc == '\0')
		{
			break;
		}
		else
		{
			break;
		}
		pc++;
	}

	// 無限大判定
	if(strInfinity != 0L)
	{
		const s8* pcw = pc;
		bool isNeg = ((*pcw) == '-');
		if(((*pcw) == '+') || isNeg)	{	pcw++;	}
		if(::strncmp((const char*)pcw, strInfinity->getRaw(), strInfinity->getLength()) == 0)
		{
			return isNeg ? TypeUtils::getF64NegativeInfinity() : TypeUtils::getF64PositiveInfinity();
		}
	}

	// dtoa にお任せ
	char* se;
	double d = _strtod((const char*)pc, &se);

	// 過大値の補正
	if(d == HUGE_VAL)			{	d = TypeUtils::getF64PositiveInfinity();	}
	else if (d == -HUGE_VAL)	{	d = TypeUtils::getF64NegativeInfinity();	}

	// 終了時の解析インデックスを返す
	if(indexParseEndOut != 0L)
	{
		*indexParseEndOut = (s32)(se - (const char*)pc0);
	}

	return d;
#else
	ASSERT(false);
	return 0.0;
#endif
}

/*---------------------------------------------------------------------*//**
	進数を指定して文字列を倍制度浮動小数点（double）数値化
**//*---------------------------------------------------------------------*/
f64 StringUtils::toDouble(s32* indexParseEndOut, const CStringBase* strNumber, u32 radix, const CStringBase* strInfinity)
{
	const s8* pc = (const s8*)strNumber->getRaw();

	// スペースをスキップ
	while(true)
	{
		if((*pc == '\t') || (*pc == '\n') || (*pc == '\v') || (*pc == '\f') || (*pc == '\r') || (*pc == ' '))	{					}
		else if(*pc == '\0')																					{	return 0.0f;	}
		else																									{	break;			}
		pc++;
	}

	// 符号判定
	const s8* pcw = pc;
	bool isNeg = ((*pcw) == '-');
	if(((*pcw) == '+') || isNeg)	{	pcw++;	}

	// 進数自動判定
	if(radix == 0)	// 進数未指定
	{
		if(*pc == '0')
		{
			if(*(pc + 1) != '\0')
			{
				if((*(pc + 1) == 'x') || (*(pc + 1) == 'X'))	{	radix = 16;		}
				else											{	radix = 8;		}
			}
		}
		else
		{
			radix = 10;
		}
	}

	// 16 進数の場合の "0x" をスキップ
	if(radix == 16)
	{
		if(*(++pc) == '\0')	{	return 0.0f;	}
		if(*(++pc) == '\0')	{	return 0.0f;	}
	}

	// 10 進数の場合は 10 進専用の toDouble に任せる
	if(radix == 10)
	{
		return toDouble(indexParseEndOut, strNumber, strInfinity);
	}

	// 数値化
	double d = 0.0;
	const s8* ss = pc;
	do
	{
		u32 digit;
		char c = *pc;
		if('0' <= c && c <= '9')		{	digit = c - '0';		}
		else if('a' <= c && c <= 'z')	{	digit = c - 'a' + 10;	}
		else if('A' <= c && c <= 'Z')	{	digit = c - 'A' + 10;	}
		else							{	break;					}
		if(digit >= radix)				{	break;					}
		d = (d * radix) + digit;
	}
	while(*(++pc) != '\0');

	// 終了時の解析インデックスを返す
	if(indexParseEndOut != 0L)
	{
		*indexParseEndOut = (s32)(pc - ss);
	}

	return d;
}

/*---------------------------------------------------------------------*//**
	倍制度浮動小数点（double）数値の文字列化
**//*---------------------------------------------------------------------*/
VcString StringUtils::toString(f64 dval, s32 radix)
{
#if SUPPORT_EXTLIB_DTOA
	char* cstr = _dtoa_radixed(dval, radix);
	VcString str(cstr);
	::free(cstr);
	return str;
#else
	ASSERT(false);
	return VcString();
#endif
}

/*---------------------------------------------------------------------*//**
	倍制度浮動小数点（double）数値の文字列化
**//*---------------------------------------------------------------------*/
VcString StringUtils::toString(f64 dval, s32 mode, s32 precision)
{
#if SUPPORT_EXTLIB_DTOA
	typedef union { double d; unsigned long L[2]; } U;
	U u;
	u.d = dval;
	int decpt;
	int sign;
	char* rve;
	char* cstr = _dtoa(u.d, mode, precision, &decpt, &sign, &rve);
	VcString str(cstr);
	::free(cstr);
	return str;
#else
	ASSERT(false);
	return VcString();
#endif
}

/*---------------------------------------------------------------------*//**
	半角→全角変換（Shift-JIS）
**//*---------------------------------------------------------------------*/
void StringUtils::conv1ByteTo2Byte(VcString* dst, const CStringBase* src)
{
	for(int i = 0; i < src->getLength(); i++)
	{
		char c = src->getAt(i);

		u16 cd = 0;
		if(('0' <= c) && (c <= '9'))
		{
			cd = 0x824F + c - '0';		// "０" + x
		}
		else if(('a' <= c) && (c <= 'f'))
		{	
			cd = 0x8281 + c - 'a';		// "ａ" + x
		}
		else if(('A' <= c) && (c <= 'F'))
		{	
			cd = 0x8260 + c - 'F';		// "Ａ" + x
		}
		else
		{
			switch(c)
			{
			case '/':	cd = 0x815E;	break;
			}
		}

		dst->add((s8)((cd >> 8) & 0xff));
		dst->add((s8)((cd >> 0) & 0xff));
	}
}

/*---------------------------------------------------------------------*//**
	行の長さを得る
	※改行コードは \n とみなす（\r は無視）
**//*---------------------------------------------------------------------*/
int StringUtils::getLineLength(const VcString* str, int index, int tabcnt)
{
	if((index < 0) || (str->getLength() <= index))	{	return -1;	}	// 不正なインデックス

	// 行頭のインデックスを求める
	int iwk = index - 1;
	while(true)
	{
		if(iwk < 0)					{	break;									}
		char cwk = str->getAt(iwk);
		if(cwk == '\n')				{	break;									}
		else if(cwk == '\r')		{											}
		iwk--;
	}
	iwk++;

	int len = 0;

	// 行末まで
	while(true)
	{
		char cwk = str->getAt(iwk);
		if(cwk == '\n')				{	break;									}
		else if(cwk == '\r')		{											}
		else if(cwk == '\t')		{	len += tabcnt - (len & (tabcnt - 1));	}
		else						{	len++;									}
		iwk++;
		if(iwk >= str->getLength())	{	break;									}
	}

	return len;
}

/*---------------------------------------------------------------------*//**
	大文字小文字を無視して指定数比較
**//*---------------------------------------------------------------------*/
bool StringUtils::equalsIgnoreCaseNum(const CStringBase* str1, const CStringBase* str2, s32 num)
{
	if(str1 == str2)	{	return 0;	}
	#if defined(_IOS) || defined(_ANDROID) || 1
		const char* s1 = str1->getRaw();
		const char* s2 = str2->getRaw();
		int len1 = 0;
		int len2 = 0;
		while((len1 < num) && (s1[len1] != '\0')) { len1++; }
		while((len2 < num) && (s2[len2] != '\0')) { len2++; }
		static char buf1[8];
		static char buf2[8];
		char* s1L = ((len1 + 1) <= (sizeof(buf1) / sizeof(char))) ? buf1 : new char[len1 + 1];
		char* s2L = ((len2 + 1) <= (sizeof(buf2) / sizeof(char))) ? buf2 : new char[len2 + 1];
		for(int i = 0; i < len1; i++) { s1L[i] = tolower(s1[i]); }
		s1L[len1] = 0;
		for(int i = 0; i < len2; i++) { s2L[i] = tolower(s2[i]); }
		s2L[len2] = 0;
		int res = ::strcmp(s1L, s2L);
		if(s1L != buf1) { delete[] s1L; }
		if(s2L != buf2) { delete[] s2L; }
		return res == 0;
	#elif defined(_WINDOWS)
		return ::_strnicmp(str1->getRaw(), str2->getRaw(), num) == 0;
	#endif
}

/*---------------------------------------------------------------------*//**
	文字列大小比較
**//*---------------------------------------------------------------------*/
s32 StringUtils::compare(const CStringBase* str1, const CStringBase* str2, s32 num)
{
	if(str1 == str2)	{	return 0;	}
	#if defined(_IOS) && 0
		const char* s1 = str1->getRaw();
		const char* s2 = str2->getRaw();
		int len1 = 0;
		int len2 = 0;
		while((len1 < num) && (s1[len1] != '\0')) { len1++; }
		while((len2 < num) && (s2[len2] != '\0')) { len2++; }
		static char buf1[8];
		static char buf2[8];
		char* s1L = ((len1 + 1) <= (sizeof(buf1) / sizeof(char))) ? buf1 : new char[len1 + 1];
		char* s2L = ((len2 + 1) <= (sizeof(buf2) / sizeof(char))) ? buf2 : new char[len2 + 1];
		for(int i = 0; i < len1; i++) { s1L[i] = s1[i]; }
		s1L[len1] = 0;
		for(int i = 0; i < len2; i++) { s2L[i] = s2[i]; }
		s2L[len2] = 0;
		int res = ::strcmp(s1L, s2L);
		if(s1L != buf1) { delete[] s1L; }
		if(s2L != buf2) { delete[] s2L; }
		return res;
	#elif defined(_IOS) || defined(_ANDROID)
		return ::strncmp(str1->getRaw(), str2->getRaw(), num);
	#elif defined(_WINDOWS)
		return ::_strnicmp(str1->getRaw(), str2->getRaw(), num);
	#endif
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS