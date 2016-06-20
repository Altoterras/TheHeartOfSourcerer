/***********************************************************************//**
 *	Base.h
 *	Enlight BasicModel Application
 *
 *	Created by Ryoutarou Kishi on 2012/11/13.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _BASICMODEL_BASE_H_
#define _BASICMODEL_BASE_H_

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

#define BASICMODEL_BEGIN_NS namespace basicmodel {
#define BASICMODEL_END_NS }

#if defined(_DEBUG)
extern void debug_traceScreen(const char* str, ...);
#define STRACE	debug_traceScreen
#else
#define STRACE	(void)
#endif

////////////////////////////////////////////////////////////////////////////
// 定数

BASICMODEL_BEGIN_NS

// スクリーンサイズ
static const s32 SCR_W					= 480;
static const s32 SCR_H					= 320;

BASICMODEL_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _BASICMODEL_BASE_H_