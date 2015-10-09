/***********************************************************************//**
 *	DebugIos.h
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2009/04/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#if !defined(_DEBUG_IOS_H_)
#define _DEBUG_IOS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../tfw/debug/TfwDebug.h"
#include "../tfw/string/VcString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace etk
{
	class EtkBody;
}

/*---------------------------------------------------------------------*//**
 *	デバッグ クラス
 *
**//*---------------------------------------------------------------------*/
class DebugIos
{
	//======================================================================
	// クラス
public:
	class DbgIf : public tfw::TfwDebug::DbgIf
	{
	public:
		void trace(const tfw::CStringBase* str);
		void trace(const char* str);
		void traceScreen(const tfw::CStringBase* str);
		void traceScreen(const char* str);
		void assert(bool isValid);
		void assert(bool isValid, const tfw::CStringBase* msg);
		void assert(bool isValid, const char* msg);
		
	public:
		static etk::EtkBody* _bodyRef;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool init();
	static void setBody(etk::EtkBody* bodyRef) { _dbgif._bodyRef = bodyRef; }
	
	static void trace(const char* str);
	static void traceln(const char* str);

	static void assert(bool isValid);
	static void assert(bool isValid, const char* msg);

	//======================================================================
	// 変数
private:
	static DbgIf _dbgif;
};

////////////////////////////////////////////////////////////////////////////

#endif	// _DEBUG_IOS_H_

