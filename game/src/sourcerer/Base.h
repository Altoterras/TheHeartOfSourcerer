/***********************************************************************//**
 *	Base.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BASE_H_
#define _SRCR_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../etk/Base.h"
#include "../etk/body/Env.h"
#include "../tfw/Base.h"
#include "../tfw/debug/ScreenTrace.h"
#include "../tfw/string/VcString.h"
#include "debug/Debug.h"

////////////////////////////////////////////////////////////////////////////
// ネームスペース

namespace tfw {}
using namespace tfw;
namespace etk {}
using namespace etk;

////////////////////////////////////////////////////////////////////////////
// マクロ

#if !defined(SRCR_BEGIN_NS)
	#define SRCR_BEGIN_NS namespace sourcerer {
	#define SRCR_END_NS }
#endif

//==========================================================================
// コンパイルスイッチ

#define ENABLE_QUEST			(1)			// クエスト有効
#if defined(_ENLIGHT_V01) && !defined(_ENLIGHT_V01_LITE)
#define ENABLE_PSNS				(1)			// PSNS 有効
#else
#define ENABLE_PSNS				(0)			// PSNS 無効
#endif
#define ENABLE_OSTXTFLD			(0)			// OS のテキストフィールド有効化
#define ENABLE_POPNUM			(1)			// ダメージ・回復時のポップ数値表示有効化

////////////////////////////////////////////////////////////////////////////
// 定数

SRCR_BEGIN_NS

// 標準フレームレート
static const s32 FRAMERATE_DEFAULT		= 30;
static const s32 FRAMERATE = FRAMERATE_DEFAULT;

// プロダクト定義 ID
enum ProductDefId
{
	PRODDEFID_V01 = 1,
	PRODDEFID_V01_LITE,
	PRODDEFID_V02,
	NUM_PRODDEFID
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_BASE_H_