/***********************************************************************//**
 *	DebugIos.mm
 *	Enlight iPhone Application
 *	
 *	Created by Ryoutarou Kishi on 2009/04/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Self
#include "DebugIos.h"

// External
#include "../etk/body/EtkBody.h"
#include "../tfw/debug/ScreenTrace.h"
#include <Foundation/NSObjCRuntime.h>
#include <Foundation/NSString.h>

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// DebugIos::DbgIf 静的変数

etk::EtkBody* DebugIos::DbgIf::_bodyRef = 0L;

//==========================================================================
// DebugIos::DbgIf メソッド

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugIos::DbgIf::trace(const tfw::CStringBase* str)
{
	#if defined(_DEBUG)
		const char* psz = str->getRaw();
		/*
		NSString* msg = [[NSString alloc] initWithUTF8String:psz];
		NSLog(msg);
		[msg release];
		 */
		::fputs(psz, stderr);
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugIos::DbgIf::trace(const char* str)
{
	#if defined(_DEBUG)
		/*
		NSString* msg = [[NSString alloc] initWithUTF8String:pstr];
		NSLog(msg);
		[msg release];
		 */
		::fputs(str, stderr);
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグスクリーン出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugIos::DbgIf::traceScreen(const tfw::CStringBase* str)
{
	#if defined(_DEBUG)
		ScreenTrace* st;
		if((_bodyRef != 0L) && ((st = _bodyRef->getScreenTrace()) != 0L))
		{
			st->trace(str);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグスクリーン出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugIos::DbgIf::traceScreen(const char* str)
{
	#if defined(_DEBUG)
		ScreenTrace* st;
		if((_bodyRef != 0L) && ((st = _bodyRef->getScreenTrace()) != 0L))
		{
			st->trace(str);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
**//*---------------------------------------------------------------------*/
void DebugIos::DbgIf::assert(bool isValid)
{
	assert(isValid, "");
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugIos::DbgIf::assert(bool isValid, const tfw::CStringBase* msg)
{
	assert(isValid, msg->getRaw());
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugIos::DbgIf::assert(bool isValid, const char* msg)
{
	if(isValid)	{	return;	}

	#if defined(_DEBUG)
		trace("*** Assertion Failed ! ***\n");
		trace(msg);
	#endif
}

//==========================================================================
// DebugIos 静的変数

DebugIos::DbgIf DebugIos::_dbgif;

//==========================================================================
// DebugIos メソッド

/*---------------------------------------------------------------------*//**
	@brief		初期化
	@param		-				-
	@return		成否
**//*---------------------------------------------------------------------*/
bool DebugIos::init()
{
	return tfw::TfwDebug::init(&_dbgif);
}
	
/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugIos::trace(const char* str)
{
	_dbgif.trace(str);
}

/*---------------------------------------------------------------------*//**
 @brief		デバッグコンソール行出力
 @param		str				メッセージ文字列
 **//*---------------------------------------------------------------------*/
void DebugIos::traceln(const char* str)
{
	_dbgif.trace(str);
	_dbgif.trace("\n");
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
**//*---------------------------------------------------------------------*/
void DebugIos::assert(bool isValid)
{
	_dbgif.assert(isValid);
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugIos::assert(bool isValid, const char* msg)
{
	_dbgif.assert(isValid, msg);
}
