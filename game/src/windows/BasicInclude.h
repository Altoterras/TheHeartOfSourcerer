/***********************************************************************//**
 *	BasicInclude.h
 *	Enlight Windows Application
 *
 *	Created by Ryoutarou Kishi on 2009/04/18.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#pragma once

////////////////////////////////////////////////////////////////////////////
// マクロ

// Windows ヘッダーから使用されていない部分を除外する
#define WIN32_LEAN_AND_MEAN

// デバッグメモリ設定
#define CRTDBG_MAP_ALLOC

// デバッグモード
#if defined(_DEBUG)
	#define DBGMODE		1
#endif

// デバッグ new
#if DBGMODE
	#include <cstdlib>
	#include <windows.h>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#endif

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Windows ヘッダー ファイル :
#include <windows.h>

// C ランタイム ヘッダー ファイル
#include <stdlib.h>
//#include <crtdbg.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

////////////////////////////////////////////////////////////////////////////
// 構造体

// フレーム情報
struct FrameInfo
{
	float	frame;
	float	dframe;
};
