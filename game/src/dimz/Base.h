/***********************************************************************//**
 *	Base.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/05/22.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _DIMZ_BASE_H_
#define _DIMZ_BASE_H_

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

#define DIMZ_BEGIN_NS namespace dimz {
#define DIMZ_END_NS }

#if defined(_DEBUG)
extern void debug_traceScreen(const char* str, ...);
#define STRACE	debug_traceScreen
#else
#define STRACE	//
#endif

////////////////////////////////////////////////////////////////////////////
// 定数

DIMZ_BEGIN_NS

// スクリーンサイズ
static const s32 SCR_W					= 480;
static const s32 SCR_H					= 320;

DIMZ_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _DIMZ_BASE_H_