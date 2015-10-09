/***********************************************************************//**
 *  OsModule.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2012/12/11.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COMMON_OS_MODULE_H_
#define _TFW_COMMON_OS_MODULE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../Base.h"

// Friends
#include "OsMainModule.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	OS モジュール取得クラス
 *
**//*---------------------------------------------------------------------*/
class OsModule
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	static OsMainModule* getMainModule() { return s_modRef; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OsModule(OsMainModule* modStaticRef);

	//======================================================================
	// 変数
private:
	static OsMainModule* s_modRef;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_COMMON_OS_MODULE_H_