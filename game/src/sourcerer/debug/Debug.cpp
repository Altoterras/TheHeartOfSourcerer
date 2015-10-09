/***********************************************************************//**
 *	Debug.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Debug.h"

// Friends
#include "../body/Game.h"

// External
#include "../../tfw/debug/TfwDebug.h"

// Library
#include <stdarg.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////
// グローバル関数

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	スクリーンデバッグ出力

	※通常は STRACE マクロを用いるが、namespace sourcerer 外などで
	　強制的に extern して使用するための用意
		 extern void debug_traceScreen(const char* str, ...);
**//*---------------------------------------------------------------------*/
void debug_traceScreen(const char* str, ...)
{
	va_list args;
	va_start(args, str);

	int			count;
	static char	szBuffer[1024];

	// 書式化の文字列作成
	#if defined(_WINDOWS)
		count = _vsnprintf(szBuffer, sizeof(szBuffer), str, args);
	#else
		count = vsnprintf(szBuffer, sizeof(szBuffer), str, args);
	#endif

	va_end(args);

	sourcerer::Game::getGame()->getScreenTrace()->trace(szBuffer);
}
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

namespace sourcerer
{

//==========================================================================
// Debug メソッド

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		pstr メッセージ文字列
**//*---------------------------------------------------------------------*/
void Debug::trace(const tfw::CStringBase* str)
{
	tfw::TfwDebug::trace(str);
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		pstr メッセージ文字列
**//*---------------------------------------------------------------------*/
void Debug::trace(const char* str)
{
	tfw::TfwDebug::trace(str);
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid 判定真偽値
**//*---------------------------------------------------------------------*/
void Debug::assert(bool isValid)
{
	tfw::TfwDebug::assert(isValid);
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid 判定真偽値
	@param		pmsg メッセージ文字列
**//*---------------------------------------------------------------------*/
void Debug::assert(bool isValid, const char* pmsg)
{
	tfw::TfwDebug::assert(isValid, pmsg);
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグモードか否か
	@retval		true デバッグモードである
	@retval		false デバッグモードでない
**//*---------------------------------------------------------------------*/
bool Debug::isDebugMode()
{
	return tfw::TfwDebug::isDebugMode();
}

}	// namespace sourcerer

////////////////////////////////////////////////////////////////////////////
