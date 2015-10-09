/***********************************************************************//**
 *	TfwDebug.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/04/18.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TfwDebug.h"

// Friends

// External

// Library
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// 定数

#define SIZE_TRACE_FORMAT_BUFFER	(1024)

//==========================================================================
// 静的変数

tfw::TfwDebug::DbgIf* tfw::TfwDebug::_dbgif = 0L;

static wchar_t s_wszTraceFormatBuffer[SIZE_TRACE_FORMAT_BUFFER];
static char* s_szTraceFormatBuffer = (char*)s_wszTraceFormatBuffer;

//==========================================================================
// メソッド

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	デバッグ出力
	@param	printf 書式の引数
**//*---------------------------------------------------------------------*/
void tfw::TfwDebug::traceFormat(const char* str, ...)
{
	if(_dbgif == 0L) {	return;	}

	va_list args;
	va_start(args, str);

	// 書式化の文字列作成
	#if defined(_WINDOWS)
		int count = _vsnprintf(s_szTraceFormatBuffer, sizeof(char) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#else
		int count = vsnprintf(s_szTraceFormatBuffer, sizeof(char) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#endif

	va_end(args);

	_dbgif->trace(s_szTraceFormatBuffer);
}

/*---------------------------------------------------------------------*//**
	デバッグ出力（終端で改行）
	@param	printf 書式の引数
**//*---------------------------------------------------------------------*/
void tfw::TfwDebug::traceFormatLn(const char* str, ...)
{
	if(_dbgif == 0L) {	return;	}

	va_list args;
	va_start(args, str);
	
	// 書式化の文字列作成
	#if defined(_WINDOWS)
		int count = _vsnprintf(s_szTraceFormatBuffer, sizeof(char) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#else
		int count = vsnprintf(s_szTraceFormatBuffer, sizeof(char) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#endif
	
	va_end(args);
	
	if((count + 1) >= SIZE_TRACE_FORMAT_BUFFER)	{	return;	}
	s_szTraceFormatBuffer[count + 0] = '\n';
	s_szTraceFormatBuffer[count + 1] = '\0';
	
	_dbgif->trace(s_szTraceFormatBuffer);
}

/*---------------------------------------------------------------------*//**
	デバッグ出力
	@param	printf 書式の引数
**//*---------------------------------------------------------------------*/
void tfw::TfwDebug::traceFormat(const wchar_t* str, ...)
{
	if(_dbgif == 0L) {	return;	}

	va_list args;
	va_start(args, str);

	// 書式化の文字列作成
	#if defined(_WINDOWS)
		int count = _vsnwprintf_s(s_wszTraceFormatBuffer, sizeof(wchar_t) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#else
		int count = vswprintf(s_wszTraceFormatBuffer, sizeof(wchar_t) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#endif

	va_end(args);
	
	_dbgif->trace(s_wszTraceFormatBuffer);
}

/*---------------------------------------------------------------------*//**
	デバッグ出力（終端で改行）
	@param	printf 書式の引数
**//*---------------------------------------------------------------------*/
void tfw::TfwDebug::traceFormatLn(const wchar_t* str, ...)
{
	if(_dbgif == 0L) {	return;	}

	va_list args;
	va_start(args, str);
	
	// 書式化の文字列作成
	#if defined(_WINDOWS)
		int count = _vsnwprintf_s(s_wszTraceFormatBuffer, sizeof(wchar_t) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#else
		int count = vswprintf(s_wszTraceFormatBuffer, sizeof(wchar_t) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#endif
	
	va_end(args);
	
	if((count + 1) >= SIZE_TRACE_FORMAT_BUFFER)	{	return;	}
	s_wszTraceFormatBuffer[count + 0] = '\n';
	s_wszTraceFormatBuffer[count + 1] = '\0';

	_dbgif->trace(s_wszTraceFormatBuffer);
}

/*---------------------------------------------------------------------*//**
	スクリーンデバッグ出力
	@param	printf 書式の引数
**//*---------------------------------------------------------------------*/
void tfw::TfwDebug::traceScreenFormat(const char* str, ...)
{
	if(_dbgif == 0L) {	return;	}

	va_list args;
	va_start(args, str);

	// 書式化の文字列作成
	#if defined(_WINDOWS)
		int count = _vsnprintf(s_szTraceFormatBuffer, sizeof(char) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#else
		int count = vsnprintf(s_szTraceFormatBuffer, sizeof(char) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#endif

	va_end(args);

	_dbgif->traceScreen(s_szTraceFormatBuffer);
}

/*---------------------------------------------------------------------*//**
	スクリーンデバッグ出力（終端で改行）
	@param	printf 書式の引数
**//*---------------------------------------------------------------------*/
void tfw::TfwDebug::traceScreenFormatLn(const char* str, ...)
{
	if(_dbgif == 0L) {	return;	}

	va_list args;
	va_start(args, str);
	
	// 書式化の文字列作成
	#if defined(_WINDOWS)
		int count = _vsnprintf(s_szTraceFormatBuffer, sizeof(char) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#else
		int count = vsnprintf(s_szTraceFormatBuffer, sizeof(char) * SIZE_TRACE_FORMAT_BUFFER, str, args);
	#endif
	
	va_end(args);
	
	if((count + 1) >= SIZE_TRACE_FORMAT_BUFFER)	{	return;	}
	s_szTraceFormatBuffer[count + 0] = '\n';
	s_szTraceFormatBuffer[count + 1] = '\0';
	
	_dbgif->traceScreen(s_szTraceFormatBuffer);
}
#endif

////////////////////////////////////////////////////////////////////////////
