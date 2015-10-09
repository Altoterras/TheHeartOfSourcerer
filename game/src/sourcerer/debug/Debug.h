/***********************************************************************//**
 *	Debug.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_DEBUG_DEBUG_H_
#define _SRCR_DEBUG_DEBUG_H_

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

namespace sourcerer
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

#endif	// _SRCR_DEBUG_DEBUG_H_
