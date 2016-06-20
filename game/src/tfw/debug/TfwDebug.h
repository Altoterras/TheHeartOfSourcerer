/***********************************************************************//**
 *	TfwDebug.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/04/18.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_DEBUG_TFW_DEBUG_H_
#define _TFW_DEBUG_TFW_DEBUG_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#if defined(_DEBUG) && defined(_WINDOWS)
	#include <cstdlib>
	#include <crtdbg.h>
#endif

#include <new>

////////////////////////////////////////////////////////////////////////////
// マクロ

#if defined(_DEBUG) && defined(_WINDOWS)
	#define malloc(a_) _malloc_dbg(a_, _NORMAL_BLOCK, __FILE__, __LINE__) 
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#if !defined(_WINDOWS)	// アップル
	#if defined(assert)
		#undef assert
	#endif
#endif

#if defined(_DEBUG)
	#if !defined(TRACE)
		#define TRACE			::tfw::TfwDebug::traceFormat
		#define TRACELN			::tfw::TfwDebug::traceFormatLn
	#endif
	#if !defined(STRACE)
		#define STRACE			::tfw::TfwDebug::traceScreenFormat
		#define STRACELN		::tfw::TfwDebug::traceScreenFormatLn
	#endif
	#if !defined(ASSERT)
		#define ASSERT(a_)		::tfw::TfwDebug::assert(a_)
	#endif
	#define ASSERTM(a_, b_)	::tfw::TfwDebug::assert(a_, b_)
#else
	#define TRACE			(void)
	#define STRACE			(void)
	#define TRACELN			(void)
	#define ASSERT			(void)
	#define ASSERTM			(void)
	/*
	#define TRACE			//
	#define STRACE			//
	#define TRACELN			//
	#define ASSERT			//
	#define ASSERTM			//
	*/
#endif
#if 0	// フルトレース
	#define FTRACE	TRACE
#else
	#define FTRACE			(void)
	/*
	#define FTRACE			//
	*/
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{

class CStringBase;
class WStringBase;

/*---------------------------------------------------------------------*//**
 *	デバッグ
 *	
**//*---------------------------------------------------------------------*/
class TfwDebug
{
public:
	class DbgIf
	{
	public:
		virtual void trace(const CStringBase* str)						{	}
		virtual void trace(const char* str)								{	}
		virtual void trace(const WStringBase* str)						{	}
		virtual void trace(const wchar_t* str)							{	}
		virtual void traceScreen(const CStringBase* str)				{	}
		virtual void traceScreen(const char* str)						{	}
		virtual void assert(bool isValid)								{	}
		virtual void assert(bool isValid, const CStringBase* msg)		{	}
		virtual void assert(bool isValid, const char* msg)				{	}
		virtual void assert(bool isValid, const WStringBase* msg)		{	}
		virtual void assert(bool isValid, const wchar_t* msg)			{	}
	};

public:
	static bool init(DbgIf* dbgif)								{	_dbgif = dbgif;	return true;							}
	
	static void trace(const CStringBase* str)					{	if(_dbgif != 0L) { _dbgif->trace(str); }				}
	static void trace(const char* str)							{	if(_dbgif != 0L) { _dbgif->trace(str); }				}
	static void traceFormat(const char* str, ...);
	static void traceFormatLn(const char* str, ...);
	static void trace(const WStringBase* str)					{	if(_dbgif != 0L) { _dbgif->trace(str); }				}
	static void trace(const wchar_t* str)						{	if(_dbgif != 0L) { _dbgif->trace(str); }				}
	static void traceFormat(const wchar_t* str, ...);
	static void traceFormatLn(const wchar_t* str, ...);

	static void traceScreen(const CStringBase* str)				{	if(_dbgif != 0L) { _dbgif->traceScreen(str); }			}
	static void traceScreen(const char* str)					{	if(_dbgif != 0L) { _dbgif->traceScreen(str); }			}
	static void traceScreenFormat(const char* str, ...);
	static void traceScreenFormatLn(const char* str, ...);

	static void assert(bool isValid)							{	if(_dbgif != 0L) { _dbgif->assert(isValid); }			}
	static void assert(bool isValid, const CStringBase* msg)	{	if(_dbgif != 0L) { _dbgif->assert(isValid, msg); }		}
	static void assert(bool isValid, const char* msg)			{	if(_dbgif != 0L) { _dbgif->assert(isValid, msg); }		}
	static void assert(bool isValid, const WStringBase* msg)	{	if(_dbgif != 0L) { _dbgif->assert(isValid, msg); }		}
	static void assert(bool isValid, const wchar_t* msg)		{	if(_dbgif != 0L) { _dbgif->assert(isValid, msg); }		}

#if defined(_DEBUG)
	static bool isDebugMode()									{	return true;											}
#else
	static bool isDebugMode()									{	return false;											}
#endif

private:
	static DbgIf* _dbgif;
};

}	// namespace tfw

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_DEBUG_TFW_DEBUG_H_

