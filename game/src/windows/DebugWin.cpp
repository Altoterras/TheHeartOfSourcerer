/***********************************************************************//**
 *	DebugWin.cpp
 *	Enlight Windows Application
 *
 *	Created by Ryoutarou Kishi on 2009/04/18.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

//--------------------------------------------------------------------------
// Basic

#include "BasicInclude.h"

//--------------------------------------------------------------------------
// Self

#include "DebugWin.h"

//--------------------------------------------------------------------------
// External

#include "../etk/body/EtkBody.h"
//#include "../tfw/string/CcString.h"
#include "../tfw/debug/ScreenTrace.h"
#if defined(UNICODE)
	#include "../tfw/string/VwString.h"
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

namespace app
{

//==========================================================================
// DebugWin 静的変数

etk::EtkBody* DebugWin::DbgIf::s_bodyRef = 0L;
DebugWin::DbgIf DebugWin::s_dbgif;

//==========================================================================
// DebugWin メソッド

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugWin::DbgIf::trace(const tfw::CStringBase* str)
{
	#if defined(_DEBUG)
		#if defined(UNICODE)
			tfw::VwString wk;
			wk.setEasily(str->getRaw());
			::OutputDebugString(wk.getRaw());
		#else
			::OutputDebugString(str->getRaw());
		#endif
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugWin::DbgIf::trace(const char* str)
{
	#if defined(_DEBUG)
		#if defined(UNICODE)
			tfw::VwString wk;
			wk.setEasily(str);
			::OutputDebugString(wk.getRaw());
		#else
			::OutputDebugString(str);
		#endif
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugWin::DbgIf::trace(const tfw::WStringBase* str)
{
	#if defined(_DEBUG)
		#if defined(UNICODE)
			::OutputDebugString(str->getRaw());
		#else
			tfw::VcString wk;
			wk.setEasily(str);
			::OutputDebugString(wk.getRaw());
		#endif
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグコンソール出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugWin::DbgIf::trace(const wchar_t* str)
{
	#if defined(_DEBUG)
		#if defined(UNICODE)
			::OutputDebugString(str);
		#else
			tfw::VcString wk;
			wk.setEasily(str);
			::OutputDebugString(wk.getRaw());
		#endif
	#endif
}


/*---------------------------------------------------------------------*//**
	@brief		デバッグスクリーン出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugWin::DbgIf::traceScreen(const tfw::CStringBase* str)
{
#if defined(_DEBUG)
	ScreenTrace* st;
	if ((s_bodyRef != 0L) && ((st = s_bodyRef->getScreenTrace()) != 0L))
	{
		st->trace(str);
	}
#endif
}

/*---------------------------------------------------------------------*//**
	@brief		デバッグスクリーン出力
	@param		str				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugWin::DbgIf::traceScreen(const char* str)
{
#if defined(_DEBUG)
	ScreenTrace* st;
	if ((s_bodyRef != 0L) && ((st = s_bodyRef->getScreenTrace()) != 0L))
	{
		st->trace(str);
	}
#endif
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
**//*---------------------------------------------------------------------*/
void DebugWin::DbgIf::assert(bool isValid)
{
	assert(isValid, "*** Assertion Failed ! ***");
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugWin::DbgIf::assert(bool isValid, const tfw::CStringBase* msg)
{
	assert(isValid, msg->getRaw());
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugWin::DbgIf::assert(bool isValid, const char* msg)
{
	if(isValid)	{	return;	}

	#if defined(_DEBUG)
		#if defined(UNICODE)
			tfw::VwString wk;
			wk.setEasily(msg);
			::OutputDebugString(_T("*** Assertion Failed ! *** "));
			::OutputDebugString(wk);
			::MessageBox(NULL, wk, _T("Assertion Failed !"), MB_OK | MB_ICONEXCLAMATION);
		#else
			::OutputDebugString(_T("*** Assertion Failed ! *** "));
			::OutputDebugString(msg);
			::MessageBox(NULL, msg, _T("Assertion Failed !"), MB_OK | MB_ICONEXCLAMATION);
		#endif
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugWin::DbgIf::assert(bool isValid, const tfw::WStringBase* msg)
{
	assert(isValid, (const wchar_t*)msg->getRaw());
}

/*---------------------------------------------------------------------*//**
	@brief		アサーション
	@param		isValid			判定真偽値
	@param		msg				メッセージ文字列
**//*---------------------------------------------------------------------*/
void DebugWin::DbgIf::assert(bool isValid, const wchar_t* msg)
{
	if(isValid)	{	return;	}

	#if defined(_DEBUG)
		#if defined(UNICODE)
			::OutputDebugString(_T("*** Assertion Failed ! *** "));
			::OutputDebugString(msg);
			::MessageBox(NULL, msg, _T("Assertion Failed !"), MB_OK | MB_ICONEXCLAMATION);
		#else
			tfw::VcString wk;
			wk.setEasily(msg);
			::OutputDebugString(_T("*** Assertion Failed ! *** "));
			::OutputDebugString(wk);
			::MessageBox(NULL, wk, _T("Assertion Failed !"), MB_OK | MB_ICONEXCLAMATION);
		#endif
	#endif
}

/*---------------------------------------------------------------------*//**
	@brief		初期化
	@param		-				-
	@return		成否
**//*---------------------------------------------------------------------*/
bool DebugWin::init()
{
	return tfw::TfwDebug::init(&s_dbgif);
}

////////////////////////////////////////////////////////////////////////////

}	// namespace app
