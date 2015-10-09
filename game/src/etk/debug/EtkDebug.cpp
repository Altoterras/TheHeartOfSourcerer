/***********************************************************************//**
 *	EtkDebug.cpp
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2013/06/28.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EtkDebug.h"

// Friends
#include "../body/EtkBody.h"

// External
#include "../../tfw/debug/ScreenTrace.h"

////////////////////////////////////////////////////////////////////////////
// グローバル関数

ETK_BEGIN_NS

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	スクリーンデバッグ出力
**//*---------------------------------------------------------------------*/
void etkdebug_traceScreen(const char* msg)
{
	/*
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
	*/

	etk::EtkBody::getBody()->getScreenTrace()->trace(msg);
}
#endif

ETK_END_NS