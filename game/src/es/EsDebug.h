/***********************************************************************//**
 *	Es.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2017/07/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_DEBUG_H_
#define _ES_ES_DEBUG_H_

////////////////////////////////////////////////////////////////////////////
// マクロ

#if defined(assert)
	#undef assert
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{
	class CStringBase;
}	// namespace tfw

namespace es
{

/*---------------------------------------------------------------------*//**
 *	デバッグ クラス
 *
**//*---------------------------------------------------------------------*/
class Debug
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static void trace(const tfw::CStringBase* str);
	static void trace(const char* str);

	static void assert(bool isValid);
	static void assert(bool isValid, const char* pmsg);

	static bool isDebugMode();
};

}	// namespace sourcerer

////////////////////////////////////////////////////////////////////////////
// グローバル関数

#if defined(_IOS)
extern void alert(const char* msg);
#endif

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_DEBUG_H_
