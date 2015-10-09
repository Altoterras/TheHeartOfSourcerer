/***********************************************************************//**
 *	Base.h
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ATAR_BASE_H_
#define _ATAR_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../tfw/Base.h"
#include "../tfw/debug/ScreenTrace.h"
#include "../etk/body/Env.h"
//#include "debug/Debug.h"

////////////////////////////////////////////////////////////////////////////
// ネームスペース

namespace tfw {}
using namespace tfw;
namespace etk {}
using namespace etk;

////////////////////////////////////////////////////////////////////////////
// マクロ

#define ATAR_BEGIN_NS namespace atar {
#define ATAR_END_NS }

// バージョンオプション
#define OPTVER_NO_FIRSTLOAD			(0)		// タイトルでの起動時ロードをしない
#define OPTVER_DEMO					(0)
#define OPTVER_TV_DEMO				(0)
#define OPTVER_DEVELOPER			(0)
#define OPTVER_TESTER				(0)

////////////////////////////////////////////////////////////////////////////
// 定数

ATAR_BEGIN_NS

// ダウンロード URL
#define URL_BASE_DEFAULT_01		"http://atar01.altoterras.co.jp/"
#define URL_BASE_DEFAULT_02		"http://atar02.altoterras.co.jp/"
#define URL_BASE_DEFAULT_03		"http://atar03.altoterras.co.jp/"
#define URL_BASE_DEFAULT_04		"http://atar04.altoterras.co.jp/"
#define URL_BASE_DEFAULT_05		"http://atar05.altoterras.co.jp/"
#define URL_BASE_DEFAULT_06		"http://atar01.sigrest.co.jp/"
#define URL_BASE_DEFAULT_07		"http://atar02.sigrest.co.jp/"
#define URL_BASE_DEFAULT_08		"http://atar03.sigrest.co.jp/"
#define URL_BASE_DEFAULT_09		"http://atar04.sigrest.co.jp/"
#define URL_BASE_DEFAULT_10		"http://atar05.sigrest.co.jp/"
#if OPTVER_DEMO
	#define URL_SUB_DIR				"demo/"
#elif OPTVER_TV_DEMO
	#define URL_SUB_DIR				"tv/"
#elif OPTVER_DEVELOPER
	#define URL_SUB_DIR				"dev/"
#elif OPTVER_TESTER
	#define URL_SUB_DIR				"test/"
#else
	#define URL_SUB_DIR				""
#endif

// サポートページ URL
#define URL_GETMARKER			"http://atar01.altoterras.co.jp/pancrase/getmarker.html"
#define URL_GOSUPPORT			"http://atar01.altoterras.co.jp/pancrase/gosupport.html"

ATAR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ATAR_BASE_H_