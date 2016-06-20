/***********************************************************************//**
 *	DebugAndroid.cpp
 *	Enlight Android Application
 *
 *	Created by Ryoutarou Kishi on 2012/12/10.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Basic
#include "BasicInclude.h"

// Self
#include "DebugAndroid.h"

// External
#include "../tfw/string/CcString.h"
#include "../tfw/string/VcString.h"
#if defined(UNICODE)
	#include "../tfw/string/VwString.h"
#endif

// Library
#include <android/log.h>

////////////////////////////////////////////////////////////////////////////
// マクロ

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "sourcerer01", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "sourcerer01", __VA_ARGS__))

////////////////////////////////////////////////////////////////////////////
// クラス

namespace app
{

//==========================================================================
// DebugAndroid 静的変数

DebugAndroid::DbgIf DebugAndroid::s_dbgif;

//==========================================================================
// DebugAndroid メソッド

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugAndroid::DbgIf::trace(const tfw::CStringBase* str)
{
	#if defined(_DEBUG)
		#if defined(UNICODE)
			tfw::WcvString wk;
			wk.setEasily(str->getRaw());
			LOGI(wk.getRaw());
		#else
			LOGI(str->getRaw());
		#endif
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugAndroid::DbgIf::trace(const char* str)
{
	#if defined(_DEBUG)
		#if defined(UNICODE)
			tfw::WcvString wk;
			wk.setEasily(str);
			LOGI(wk.getRaw());
		#else
			LOGI(str);
		#endif
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
**//*---------------------------------------------------------------------*/
void DebugAndroid::DbgIf::assert(bool isValid)
{
	assert(isValid, "*** Assertion Failed ! ***");
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugAndroid::DbgIf::assert(bool isValid, const tfw::CStringBase* msg)
{
	assert(isValid, msg->getRaw());
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugAndroid::DbgIf::assert(bool isValid, const char* msg)
{
	if(isValid)	{	return;	}

	#if defined(_DEBUG)
		#if defined(UNICODE)
			tfw::WcvString wk;
			wk.setEasily(msg);
			wk.insert(0, "*** Assertion Failed ! *** ");
			wk.add("\n");
			LOGW(wk.getRaw());
		#else
			tfw::VcString wk("*** Assertion Failed ! *** ");
			wk.add(msg);
			wk.add("\n");
			LOGW(wk.getRaw());
		#endif
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		初期化
	@param		-				-
	@return		成否
**//*---------------------------------------------------------------------*/
bool DebugAndroid::init()
{
	return tfw::TfwDebug::init(&s_dbgif);
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugAndroid::trace(const tfw::CStringBase* str)
{
	s_dbgif.trace(str);
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugAndroid::trace(const tfw::CStringBase& str)
{
	s_dbgif.trace(&str);
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugAndroid::trace(const char* str)
{
	s_dbgif.trace(str);
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
**//*---------------------------------------------------------------------*/
void DebugAndroid::assert(bool isValid)
{
	s_dbgif.assert(isValid);
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugAndroid::assert(bool isValid, const tfw::CStringBase* msg)
{
	s_dbgif.assert(isValid, msg);
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugAndroid::assert(bool isValid, const tfw::CStringBase& msg)
{
	s_dbgif.assert(isValid, &msg);
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugAndroid::assert(bool isValid, const char* msg)
{
	s_dbgif.assert(isValid, msg);
}

////////////////////////////////////////////////////////////////////////////

}	// namespace app
