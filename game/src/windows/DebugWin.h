/***********************************************************************//**
 *	DebugWin.cpp
 *	Enlight Windows Application
 *
 *	Created by Ryoutarou Kishi on 2009/04/18.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#pragma once

#include "../tfw/debug/TfwDebug.h"
#include "../tfw/string/VcString.h"
#include "../tfw/string/VwString.h"

////////////////////////////////////////////////////////////////////////////
// クラス定義

namespace etk
{
	class EtkBody;
}

namespace app
{

/*---------------------------------------------------------------------*//**
 *	デバッグクラス
 *
**//*---------------------------------------------------------------------*/
class DebugWin
{
public:
	//======================================================================
	// クラス
	class DbgIf : public tfw::TfwDebug::DbgIf
	{
	public:
		void trace(const tfw::CStringBase* str);
		void trace(const char* str);
		void trace(const tfw::WStringBase* str);
		void trace(const wchar_t* str);
		void traceScreen(const tfw::CStringBase* str);
		void traceScreen(const char* str);
		void assert(bool isValid);
		void assert(bool isValid, const tfw::CStringBase* msg);
		void assert(bool isValid, const char* msg);
		void assert(bool isValid, const tfw::WStringBase* msg);
		void assert(bool isValid, const wchar_t* msg);

	public:
		static etk::EtkBody* s_bodyRef;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool init();
	static void setBody(etk::EtkBody* bodyRef) { s_dbgif.s_bodyRef = bodyRef; }

	//======================================================================
	// 変数
private:
	static DbgIf s_dbgif;
};

}	// namespace app
