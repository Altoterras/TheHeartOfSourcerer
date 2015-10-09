/***********************************************************************//**
 *  TxtUtils.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/12/06.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TxtUtilsIos.h"

// Friends
#include "Txt.h"
#include "TxtChar.h"
#include "TxtUtils.h"
#include "implement/Utf8Txt.h"
#include "implement/SjisTxt.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TxtUtils メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	全角半角判定
**//*---------------------------------------------------------------------*/
bool TxtUtilsIos::isFullWidth(TxtDef::Encode enc, const TxtChar* tc)
{
	switch(enc)
	{
	case TxtDef::ENC_UTF8:
	case TxtDef::ENC_UNICODE:
		{
			unichar ucUnicode = (unichar)(tc->getCode() & 0xffff);
			NSString* str = [[NSString alloc] initWithCharacters:&ucUnicode length:1];
			const u8* szSjis = (const u8*)[str cStringUsingEncoding:NSShiftJISStringEncoding];
			bool isFullWidth = (szSjis[1] != '\0');
			#if !__has_feature(objc_arc)
				[str release];
			#endif
			return isFullWidth;
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	Unicode 文字を Shift-JIS 文字に変換する
**//*---------------------------------------------------------------------*/
u32 TxtUtilsIos::convUnicodeToSjisCode(u32 cUnicode)
{
	unichar ucUnicode = (unichar)(cUnicode & 0xffff);
	NSString* str = [[NSString alloc] initWithCharacters:&ucUnicode length:1];
	const u8* szSjis = (const u8*)[str cStringUsingEncoding:NSShiftJISStringEncoding];
	u32 codeSjis;
	if(szSjis[1] == '\0')
	{
		codeSjis = szSjis[0];
	}
	else
	{
		codeSjis = szSjis[0] << 8 | szSjis[1];
	}
	#if !__has_feature(objc_arc)
		[str release];
	#endif
	return codeSjis;
}

/*---------------------------------------------------------------------*//**
	UTF-8 テキストを Shift-JIS テキストに変換する
**//*---------------------------------------------------------------------*/
bool TxtUtilsIos::convUtf8ToSjisText(SjisTxt* dst, const Utf8Txt* src)
{
	NSString* strUtf8 = [NSString stringWithCString:src->getRawString()->getRaw() encoding:NSUTF8StringEncoding];
	const char* szSjis = (const char*)[strUtf8 cStringUsingEncoding:NSShiftJISStringEncoding];
	VcString strSjis(szSjis);
	dst->setSingleString(&strSjis);
	return true;	
}

/*---------------------------------------------------------------------*//**
	Shift-JIS テキストを UTF-8 テキストに変換する
**//*---------------------------------------------------------------------*/
bool TxtUtilsIos::convSjisToUtf8Text(Utf8Txt* dst, const SjisTxt* src)
{
	NSString* strSjis = [NSString stringWithCString:src->getRawString()->getRaw() encoding:NSShiftJISStringEncoding];
	const char* szUtf8 = (const char*)[strSjis cStringUsingEncoding:NSUTF8StringEncoding];
	VcString strUtf8(szUtf8);
	dst->setSingleString(&strUtf8);
	return true;	
}

/*---------------------------------------------------------------------*//**
	Unicode テキストを Shift-JIS テキストに変換する
**//*---------------------------------------------------------------------*/
bool TxtUtilsIos::convUnicodeToSjisText(SjisTxt* dst, const UnicodeTxt* src)
{
	Utf8Txt utf8txt;
	if(!TxtUtils::convEncode(&utf8txt, (const Txt*)src))	{	return false;	}
	return convUtf8ToSjisText(dst, &utf8txt);
}

/*---------------------------------------------------------------------*//**
	Shift-JIS テキストを Unicode テキストに変換する
**//*---------------------------------------------------------------------*/
bool TxtUtilsIos::convSjisToUnicodeText(UnicodeTxt* dst, const SjisTxt* src)
{
	Utf8Txt utf8txt;
	if(!convSjisToUtf8Text(&utf8txt, src))	{	return false;	}
	return TxtUtils::convEncode((Txt*)dst, src);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS