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
#include "TxtUtils.h"

// Friends
#include "Txt.h"
#include "TxtChar.h"
#if defined(_IOS)
#include "TxtUtilsIos.h"
#endif
#include "implement/SjisTxt.h"
#include "implement/UnicodeTxt.h"
#include "implement/Utf8Txt.h"
#if defined(_ANDROID)
#include "../common/AndroidMainModule.h"
#endif
#include "../common/OsModule.h"
#include "../lib/TmpBuf.h"

// External

// Library
#if defined(_WINDOWS)
#include <winnls.h>
#endif

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
bool TxtUtils::isFullWidth(TxtDef::Encode enc, const TxtChar* tc)
{
	switch(enc)
	{
	default:
		return false;
	case TxtDef::ENC_SJIS:
		return (tc->getBytes() >= 2) && (tc->getCode() != Txt::NLCODE_CRLF);
	case TxtDef::ENC_UTF8:
	case TxtDef::ENC_UNICODE:
		#if defined(_WINDOWS)
		{
			if(tc->getCode() == Txt::NLCODE_CRLF)
			{
				return false;
			}
			else
			{
				WCHAR wc = (WCHAR)(tc->getCode() & 0xffff);
				WORD type;
				::GetStringTypeExW(
					LOCALE_SYSTEM_DEFAULT,
					CT_CTYPE3,
					&wc,
					1,
					&type);
				return TFW_IS_FLAG(type, C3_FULLWIDTH) || TFW_IS_FLAG(type, C3_HIRAGANA);
			}
		}
		#elif defined(_IOS)
		{
			return TxtUtilsIos::isFullWidth(enc, tc);
		}
		#elif defined(_ANDROID)
		{
			u32 sjiscode = convUnicodeToSjisCode(tc->getCode());
			return (sjiscode > 0xff);
		}
		#endif
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	Unicode 文字を Shift-JIS 文字に変換する
**//*---------------------------------------------------------------------*/
u32 TxtUtils::convUnicodeToSjisCode(u32 cUnicode)
{
	#if defined(_WINDOWS)
	{
		WCHAR wc = (WCHAR)(cUnicode & 0xffff);
		u8 cbuf[2] = { 0, 0 };
		if(::WideCharToMultiByte(
			CP_ACP,					// Shift-JIS (日本語 OS 依存)
			WC_NO_BEST_FIT_CHARS,	// コードにないものは近い文字に
			&wc,
			1,
			(char*)cbuf,
			2,
			NULL,
			NULL) == 0)
		{
			return 0;	// エラー
		}
		return (cbuf[1] == 0) ? cbuf[0] : (cbuf[0] << 8 | cbuf[1]);
	}
	#elif defined(_IOS)
	{
		return TxtUtilsIos::convUnicodeToSjisCode(cUnicode);
	}
	#elif defined(_ANDROID)
	{
		JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;

		jclass jclsString = jni->FindClass("java/lang/String");
		if(jclsString == NULL)								{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 101 ###\n");	return false;	}
		jmethodID jmethodStringGetBytes = jni->GetMethodID(jclsString, "getBytes", "(Ljava/lang/String;)[B");
		if(jmethodStringGetBytes == NULL)					{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 201 ###\n");	return false;	}
		jstring jstrEncSjis = jni->NewStringUTF("Shift_JIS");
		if(jstrEncSjis == NULL)								{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 301 ###\n");	return false;	}

		// Unicode の Java String を作成
		jchar jcUnicode = (jchar)(cUnicode & 0xffff);
		jstring jstrUnicode = jni->NewString(&jcUnicode, 1);
		if(jstrUnicode == NULL)								{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 401 ###\n");	return false;	}

		// Shift-JIS にエンコードした Java バイト配列を取得
		jbyteArray jbarrSjis = (jbyteArray)jni->CallObjectMethod(jstrUnicode, jmethodStringGetBytes, jstrEncSjis);
		if(jbarrSjis == NULL)								{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 402 ###\n");	return false;	}

		// Java バイト配列の内容を取得
		jboolean jb;
		jsize lenSjis = jni->GetArrayLength(jbarrSjis);
		jbyte* jbSjis = jni->GetByteArrayElements(jbarrSjis, &jb);
		if(jbSjis == NULL)									{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 403 ###\n");	return false;	}
		VcString strSjis((const char*)jbSjis, (int)lenSjis);
		return (strSjis.getLength() <= 1) ? (u32)strSjis.getAt(0) : (u32)((strSjis.getAt(0) << 8) | strSjis.getAt(1));
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	Unicode テキストを Shift-JIS テキストに変換する
**//*---------------------------------------------------------------------*/
bool TxtUtils::convUnicodeToSjisText(SjisTxt* dst, const UnicodeTxt* src)
{
	#if defined(_WINDOWS)
	{
		const u32 SIZE_SBUF = 256;
		u8 sbuf[SIZE_SBUF];
		TmpBuf tmpbuf;
		// Unicode → Shit-JIS
		s32 lenSjis = ::WideCharToMultiByte(
			CP_ACP,							// Shift-JIS (日本語 OS 依存)
			WC_NO_BEST_FIT_CHARS,			// コードにないものは近い文字に
			(LPCWSTR)src->getRawWideString()->getRaw(),
			src->getRawWideString()->getLength(),
			0L,
			0,
			0L,
			0L);
		if(lenSjis == 0)
		{
			return false;
		}
		char* bufSjis = (char*)tmpbuf.make(sizeof(char) * lenSjis, sbuf, SIZE_SBUF);
		if(::WideCharToMultiByte(
			CP_ACP,							// Shift-JIS (日本語 OS 依存)
			WC_NO_BEST_FIT_CHARS,			// コードにないものは近い文字に
			(LPCWSTR)src->getRawWideString()->getRaw(),
			src->getRawWideString()->getLength(),
			bufSjis,
			lenSjis,
			0L,
			0L) < lenSjis)
		{
			return false;
		}
		// Shift-JIS をセット
		VcString strSjis(bufSjis, lenSjis);
		dst->setSingleString(&strSjis);
		return true;
	}
	#elif defined(_IOS)
	{
		return TxtUtilsIos::convUnicodeToSjisText(dst, src);
	}
	#elif defined(_ANDROID)
	{
		JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;

		jclass jclsString = jni->FindClass("java/lang/String");
		if(jclsString == NULL)								{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 101 ###\n");	return false;	}
		jmethodID jmethodStringGetBytes = jni->GetMethodID(jclsString, "getBytes", "(Ljava/lang/String;)[B");
		if(jmethodStringGetBytes == NULL)					{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 201 ###\n");	return false;	}
		jstring jstrEncSjis = jni->NewStringUTF("Shift_JIS");
		if(jstrEncSjis == NULL)								{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 301 ###\n");	return false;	}

		// Unicode の Java String を作成
		const VwString* wstrUnicode = src->getRawWideString();
		jstring jstrUnicode = jni->NewString((jchar*)wstrUnicode->getRaw(), wstrUnicode->getLength());
		if(jstrUnicode == NULL)								{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 401 ###\n");	return false;	}

		// Shift-JIS にエンコードした Java バイト配列を取得
		jbyteArray jbarrSjis = (jbyteArray)jni->CallObjectMethod(jstrUnicode, jmethodStringGetBytes, jstrEncSjis);
		if(jbarrSjis == NULL)								{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 402 ###\n");	return false;	}

		// Java バイト配列の内容を取得
		jboolean jb;
		jsize lenSjis = jni->GetArrayLength(jbarrSjis);
		jbyte* jbSjis = jni->GetByteArrayElements(jbarrSjis, &jb);
		if(jbSjis == NULL)									{	TRACE("{TxtUtils::convUnicodeToSjisText} ### 403 ###\n");	return false;	}
		VcString strSjis((const char*)jbSjis, (int)lenSjis);
		dst->setSingleString(&strSjis);
		jni->ReleaseByteArrayElements(jbarrSjis, jbSjis, 0);
		return true;
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	Shift-JIS テキストを Unicode テキストに変換する
**//*---------------------------------------------------------------------*/
bool TxtUtils::convSjisToUnicodeText(UnicodeTxt* dst, const SjisTxt* src)
{
	#if defined(_WINDOWS)
	{
		const u32 SIZE_SBUF = 256;
		u8 sbuf[SIZE_SBUF];
		TmpBuf tmpbuf;
		// Shift-JIS → Unicode
		s32 lenUnicode = ::MultiByteToWideChar(
			CP_ACP,							// Shift-JIS (日本語 OS 依存)
			0,
			src->getRawString()->getRaw(),
			-1,
			0L,
			0);
		if(lenUnicode == 0)
		{
			return false;
		}
		WCHAR* bufUnicode = (WCHAR*)tmpbuf.make(sizeof(WCHAR) * lenUnicode, sbuf, SIZE_SBUF);
		if(::MultiByteToWideChar(
			CP_ACP,							// Shift-JIS (日本語 OS 依存)
			0,
			src->getRawString()->getRaw(),
			-1,
			bufUnicode,
			lenUnicode) < lenUnicode)
		{
			return false;
		}
		// Unicode をセット
		VwString strUnicode((u16*)bufUnicode, lenUnicode);
		dst->setWideString(&strUnicode);
		return true;
	}
	#elif defined(_IOS)
	{
		return TxtUtilsIos::convSjisToUnicodeText(dst, src);
	}
	#elif defined(_ANDROID)
	{
		JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;

		jclass jclsString = jni->FindClass("java/lang/String");
		if(jclsString == NULL)								{	TRACE("{TxtUtils::convSjisToUnicodeText} ### 101 ###\n");	return false;	}
		jmethodID jmethodStringCtor = jni->GetMethodID(jclsString, "<init>", "([BLjava/lang/String;)V");
		if(jmethodStringCtor == NULL)						{	TRACE("{TxtUtils::convSjisToUnicodeText} ### 102 ###\n");	return false;	}
		jmethodID jmethodStringToCharArray = jni->GetMethodID(jclsString, "toCharArray", "()[C");
		if(jmethodStringToCharArray == NULL)				{	TRACE("{TxtUtils::convSjisToUnicodeText} ### 201 ###\n");	return false;	}
		jstring jstrEncSjis = jni->NewStringUTF("Shift_JIS");
		if(jstrEncSjis == NULL)								{	TRACE("{TxtUtils::convSjisToUnicodeText} ### 301 ###\n");	return false;	}

		// Shift-JIS の Java バイト配列を作成
		const VcString* strSjis = src->getRawString();
		jboolean jb;
		jbyteArray jbarrSjis = jni->NewByteArray(src->getCharCount());
		if(jbarrSjis == NULL)								{	TRACE("{TxtUtils::convSjisToUnicodeText} ### 401 ###\n");	return false;	}
		jbyte* jbSjis = jni->GetByteArrayElements(jbarrSjis, &jb);
		if(jbSjis == NULL)									{	TRACE("{TxtUtils::convSjisToUnicodeText} ### 402 ###\n");	return false;	}
		TFW_COPY_MEMORY(jbSjis, strSjis->getRaw(), sizeof(char) * src->getCharCount());
		jni->ReleaseByteArrayElements(jbarrSjis, jbSjis, 0);

		// Unicode の Java String を作成
		jstring jstrSrcStrOnUnicode = (jstring)jni->NewObject(jclsString, jmethodStringCtor, jbarrSjis, jstrEncSjis); 
		if(jstrSrcStrOnUnicode == NULL)						{	TRACE("{TxtUtils::convSjisToUnicodeText} ### 403 ###\n");	return false;	}

		// Java キャラクタ配列を取得
		jcharArray jcarrUnicode = (jcharArray)jni->CallObjectMethod(jstrSrcStrOnUnicode, jmethodStringToCharArray);
		if(jcarrUnicode == NULL)							{	TRACE("{TxtUtils::convSjisToUnicodeText} ### 404 ###\n");	return false;	}

		// Java キャラクタ配列の内容を取得
		jsize lenUnicode = jni->GetArrayLength(jcarrUnicode);
		jchar* jbUnicode = jni->GetCharArrayElements(jcarrUnicode, &jb);
		if(jbUnicode == NULL)								{	TRACE("{TxtUtils::convSjisToUnicodeText} ### 405 ###\n");	return false;	}
		VwString strUnicode((const u16*)jbUnicode, (int)lenUnicode);
		dst->setWideString(&strUnicode);
		jni->ReleaseCharArrayElements(jcarrUnicode, jbUnicode, 0);
		return true;
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	UTF-8 テキストを Shift-JIS テキストに変換する
**//*---------------------------------------------------------------------*/
bool TxtUtils::convUtf8ToSjisText(SjisTxt* dst, const Utf8Txt* src)
{
	#if defined(_WINDOWS)
	{
		const u32 SIZE_SBUF = 256;
		u8 sbuf1[SIZE_SBUF];
		u8 sbuf2[SIZE_SBUF];
		TmpBuf tmpbuf1, tmpbuf2;
		// UTF-8 → Unicode
		s32 lenUnicode = ::MultiByteToWideChar(
			CP_UTF8,						// UTF-8
			0,
			src->getRawString()->getRaw(),
			-1,
			0L,
			0);
		if(lenUnicode == 0)
		{
			return false;
		}
		WCHAR* bufUnicode = (WCHAR*)tmpbuf1.make(sizeof(WCHAR) * lenUnicode, sbuf1, SIZE_SBUF);
		if(::MultiByteToWideChar(
			CP_UTF8,						// UTF-8
			0,
			src->getRawString()->getRaw(),
			-1,
			bufUnicode,
			lenUnicode) < lenUnicode)
		{
			return false;
		}
		// Unicode → Shit-JIS
		s32 lenSjis = ::WideCharToMultiByte(
			CP_ACP,							// Shift-JIS (日本語 OS 依存)
			WC_NO_BEST_FIT_CHARS,			// コードにないものは近い文字に
			bufUnicode,
			lenUnicode,
			0L,
			0,
			0L,
			0L);
		if(lenSjis == 0)
		{
			return false;
		}
		char* bufSjis = (char*)tmpbuf2.make(sizeof(char) * lenSjis, sbuf2, SIZE_SBUF);
		if(::WideCharToMultiByte(
			CP_ACP,							// Shift-JIS (日本語 OS 依存)
			WC_NO_BEST_FIT_CHARS,			// コードにないものは近い文字に
			bufUnicode,
			lenUnicode,
			bufSjis,
			lenSjis,
			0L,
			0L) < lenSjis)
		{
			return false;
		}
		// Shift-JIS をセット
		VcString strSjis(bufSjis, lenSjis);
		dst->setSingleString(&strSjis);
		return true;
	}
	#elif defined(_IOS)
	{
		return TxtUtilsIos::convUtf8ToSjisText(dst, src);
	}
	#elif defined(_ANDROID)
	{
		JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;

		jclass jclsString = jni->FindClass("java/lang/String");
		if(jclsString == NULL)								{	TRACE("{TxtUtils::convUtf8ToSjisText} ### 101 ###\n");	return false;	}
		jmethodID jmethodStringGetBytes = jni->GetMethodID(jclsString, "getBytes", "(Ljava/lang/String;)[B");
		if(jmethodStringGetBytes == NULL)					{	TRACE("{TxtUtils::convUtf8ToSjisText} ### 201 ###\n");	return false;	}
		jstring jstrEncSjis = jni->NewStringUTF("Shift_JIS");
		if(jstrEncSjis == NULL)								{	TRACE("{TxtUtils::convUtf8ToSjisText} ### 301 ###\n");	return false;	}

		// UTF-8 の Java String を作成
		const VcString* strUtf8 = src->getRawString();
		jstring jstrUtf8 = jni->NewStringUTF(strUtf8->getRaw());
		if(jstrUtf8 == NULL)								{	TRACE("{TxtUtils::convUtf8ToSjisText} ### 401 ###\n");	return false;	}

		// Shift-JIS にエンコードした Java バイト配列を取得
		jbyteArray jbarrSjis = (jbyteArray)jni->CallObjectMethod(jstrUtf8, jmethodStringGetBytes, jstrEncSjis);
		if(jbarrSjis == NULL)								{	TRACE("{TxtUtils::convUtf8ToSjisText} ### 402 ###\n");	return false;	}

		// Java バイト配列の内容を取得
		jboolean jb;
		jsize lenSjis = jni->GetArrayLength(jbarrSjis);
		jbyte* jbSjis = jni->GetByteArrayElements(jbarrSjis, &jb);
		if(jbSjis == NULL)									{	TRACE("{TxtUtils::convUtf8ToSjisText} ### 403 ###\n");	return false;	}
		VcString strSjis((const char*)jbSjis, (int)lenSjis);
		dst->setSingleString(&strSjis);
		jni->ReleaseByteArrayElements(jbarrSjis, jbSjis, 0);
		return true;
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	Shift-JIS テキストを UTF-8 テキストに変換する
**//*---------------------------------------------------------------------*/
bool TxtUtils::convSjisToUtf8Text(Utf8Txt* dst, const SjisTxt* src)
{
	#if defined(_WINDOWS)
	{
		const u32 SIZE_SBUF = 256;
		u8 sbuf1[SIZE_SBUF];
		u8 sbuf2[SIZE_SBUF];
		TmpBuf tmpbuf1, tmpbuf2;
		// Shift-JIS → Unicode
		s32 lenUnicode = ::MultiByteToWideChar(
			CP_ACP,							// Shift-JIS (日本語 OS 依存)
			0,
			src->getRawString()->getRaw(),
			-1,
			0L,
			0);
		if(lenUnicode == 0)
		{
			return false;
		}
		WCHAR* bufUnicode = (WCHAR*)tmpbuf1.make(sizeof(WCHAR) * lenUnicode, sbuf1, SIZE_SBUF);
		if(::MultiByteToWideChar(
			CP_ACP,							// Shift-JIS (日本語 OS 依存)
			0,
			src->getRawString()->getRaw(),
			-1,
			bufUnicode,
			lenUnicode) < lenUnicode)
		{
			return false;
		}
		// Unicode → UTF-8
		s32 lenUtf8 = ::WideCharToMultiByte(
			CP_UTF8,						// UTF-8
			0,
			bufUnicode,
			lenUnicode,
			0L,
			0,
			0L,
			0L);
		if(lenUtf8 == 0)
		{
			return false;
		}
		char* bufUtf8 = (char*)tmpbuf2.make(sizeof(char) * lenUtf8, sbuf2, SIZE_SBUF);
		if(::WideCharToMultiByte(
			CP_UTF8,						// UTF-8
			0,
			bufUnicode,
			lenUnicode,
			bufUtf8,
			lenUtf8,
			0L,
			0L) < lenUtf8)
		{
			return false;
		}
		// UTF-8 をセット
		VcString strUtf8(bufUtf8, lenUtf8);
		dst->setSingleString(&strUtf8);
		return true;
	}
	#elif defined(_IOS)
	{
		return TxtUtilsIos::convSjisToUtf8Text(dst, src);
	}
	#elif defined(_ANDROID)
	{
		JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;

		jclass jclsString = jni->FindClass("java/lang/String");
		if(jclsString == NULL)								{	TRACE("{TxtUtils::convSjisToUtf8Text} ### 101 ###\n");	return false;	}
		jmethodID jmethodStringCtor = jni->GetMethodID(jclsString, "<init>", "([BLjava/lang/String;)V");
		if(jmethodStringCtor == NULL)						{	TRACE("{TxtUtils::convSjisToUtf8Text} ### 102 ###\n");	return false;	}
		jmethodID jmethodStringGetBytes = jni->GetMethodID(jclsString, "getBytes", "(Ljava/lang/String;)[B");
		if(jmethodStringGetBytes == NULL)					{	TRACE("{TxtUtils::convSjisToUtf8Text} ### 201 ###\n");	return false;	}
		jstring jstrEncSjis = jni->NewStringUTF("Shift_JIS");
		if(jstrEncSjis == NULL)								{	TRACE("{TxtUtils::convSjisToUtf8Text} ### 301 ###\n");	return false;	}
		jstring jstrEncUtf8 = jni->NewStringUTF("UTF-8");
		if(jstrEncUtf8 == NULL)								{	TRACE("{TxtUtils::convSjisToUtf8Text} ### 302 ###\n");	return false;	}

		// Shift-JIS の Java バイト配列を作成
		const VcString* strSjis = src->getRawString();
		jboolean jb;
		jbyteArray jbarrSjis = jni->NewByteArray(src->getCharCount());
		if(jbarrSjis == NULL)								{	TRACE("{TxtUtils::convSjisToUtf8Text} ### 401 ###\n");	return false;	}
		jbyte* jbSjis = jni->GetByteArrayElements(jbarrSjis, &jb);
		if(jbSjis == NULL)									{	TRACE("{TxtUtils::convSjisToUtf8Text} ### 402 ###\n");	return false;	}
		TFW_COPY_MEMORY(jbSjis, strSjis->getRaw(), sizeof(char) * src->getCharCount());
		jni->ReleaseByteArrayElements(jbarrSjis, jbSjis, 0);

		// Unicode の Java String を作成
		jstring jstrSrcStrOnUnicode = (jstring)jni->NewObject(jclsString, jmethodStringCtor, jbarrSjis, jstrEncSjis); 
		if(jstrSrcStrOnUnicode == NULL)						{	TRACE("{TxtUtils::convSjisToUtf8Text} ### 403 ###\n");	return false;	}

		// UTF-8 にエンコードした Java バイト配列を取得
		jbyteArray jbarrUtf8 = (jbyteArray)jni->CallObjectMethod(jstrSrcStrOnUnicode, jmethodStringGetBytes, jstrEncUtf8);
		if(jbarrUtf8 == NULL)								{	TRACE("{TxtUtils::convUtf8ToSjisText} ### 404 ###\n");	return false;	}

		// Java バイト配列の内容を取得
		jsize lenUtf8 = jni->GetArrayLength(jbarrUtf8);
		jbyte* jbUtf8 = jni->GetByteArrayElements(jbarrUtf8, &jb);
		if(jbUtf8 == NULL)									{	TRACE("{TxtUtils::convUtf8ToUtf8Text} ### 405 ###\n");	return false;	}
		VcString strUtf8((const char*)jbUtf8, (int)lenUtf8);
		dst->setSingleString(&strUtf8);
		jni->ReleaseByteArrayElements(jbarrUtf8, jbUtf8, 0);
		return true;
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	テキストエンコード変換
**//*---------------------------------------------------------------------*/
bool TxtUtils::convEncode(Txt* txtDst, const Txt* txtSrc)
{
	TxtDef::Encode encDst = txtDst->getEncode();
	TxtDef::Encode encSrc = txtSrc->getEncode();
	switch(encDst)
	{
	case TxtDef::ENC_SJIS:
		switch(encSrc)
		{
		case TxtDef::ENC_UTF8:
			return TxtUtils::convUtf8ToSjisText((SjisTxt*)txtDst, (const Utf8Txt*)txtSrc);
		case TxtDef::ENC_UNICODE:
			return TxtUtils::convUnicodeToSjisText((SjisTxt*)txtDst, (const UnicodeTxt*)txtSrc);
		}
	case TxtDef::ENC_UTF8:
		switch(encSrc)
		{
		case TxtDef::ENC_SJIS:
			return TxtUtils::convSjisToUtf8Text((Utf8Txt*)txtDst, (const SjisTxt*)txtSrc);
		}
	case TxtDef::ENC_UNICODE:
		switch(encSrc)
		{
		case TxtDef::ENC_SJIS:
			return TxtUtils::convSjisToUnicodeText((UnicodeTxt*)txtDst, (const SjisTxt*)txtSrc);
		}
	}

	txtDst->removeAll();
	TxtPos pos;
	txtDst->getHearPos(&pos);
	return txtDst->insert(&pos, txtSrc);
}

/*---------------------------------------------------------------------*//**
	テキストを短くする（半角文字としてカウントする）
**//*---------------------------------------------------------------------*/
bool TxtUtils::shortenByRawCharCount(Txt* txt, u32 countRawChar)
{
	if(txt->getRawCount() > countRawChar)
	{
		for(TxtIterator it = txt->iteratorEnd(); it.has(); it.prev())
		{
			if(it.getRawIndex() <= countRawChar)
			{
				txt->remove(&it, txt->getCharCount() - it.getCharIndex());
				return true;	// 短くした
			}
		}
	}
	return false;	// 短くしなかった
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS