/***********************************************************************//**
 *	DebugAndroid.h
 *	Enlight Android Application
 *
 *	Created by Ryoutarou Kishi on 2012/12/10.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#pragma once

#include "../tfw/debug/TfwDebug.h"
#include "../tfw/string/CStringBase.h"
#include "CompilerSwitch.h"

////////////////////////////////////////////////////////////////////////////
// クラス定義

namespace app
{

/*---------------------------------------------------------------------*//**
 *	デバッグクラス
 *
**//*---------------------------------------------------------------------*/
class DebugAndroid
{
	//======================================================================
	// クラス
public:
	class DbgIf : public tfw::TfwDebug::DbgIf
	{
	public:
		void trace(const tfw::CStringBase* str);
		void trace(const char* str);
		void assert(bool isValid);
		void assert(bool isValid, const tfw::CStringBase* msg);
		void assert(bool isValid, const char* msg);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool init();
	
	static void trace(const tfw::CStringBase* str);
	static void trace(const tfw::CStringBase& str);
	static void trace(const char* str);

	static void assert(bool isValid);
	static void assert(bool isValid, const tfw::CStringBase* msg);
	static void assert(bool isValid, const tfw::CStringBase& msg);
	static void assert(bool isValid, const char* msg);

	//======================================================================
	// 変数
private:
	static DbgIf s_dbgif;
};

}	// namespace app
