/***********************************************************************//**
 *	Base.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/05/22.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _EDEV_BASE_H_
#define _EDEV_BASE_H_

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

#define EDEV_BEGIN_NS namespace edev {
#define EDEV_END_NS }

#if defined(_DEBUG)
extern void debug_traceScreen(const char* str, ...);
#define STRACE	debug_traceScreen
#else
#define STRACE	//
#endif

////////////////////////////////////////////////////////////////////////////
// 定数

EDEV_BEGIN_NS

EDEV_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _EDEV_BASE_H_