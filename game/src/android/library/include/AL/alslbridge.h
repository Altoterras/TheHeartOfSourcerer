/***********************************************************************//**
 *	alslbridge.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2013/01/16.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _OPENAL_AL_SL_BRIDGE_H_
#define _OPENAL_AL_SL_BRIDGE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

////////////////////////////////////////////////////////////////////////////
// マクロ

// OpenSL エンジンは外部のインスタンスを使う
#define USE_EXTERNAL_ENGINE_INSTANCE	1

////////////////////////////////////////////////////////////////////////////
// グローバル関数

#ifdef __cplusplus
extern "C" {
#endif

#if USE_EXTERNAL_ENGINE_INSTANCE

extern void alslbridge_getOpenSlInterfaceToOpenAlBackend(SLObjectItf* objEngine, SLEngineItf* engineEngine, SLObjectItf* objOutputMix);
extern void alslbridge_setOpenSlInterfaceToOpenAlBackend(SLObjectItf objEngine, SLEngineItf engineEngine, SLObjectItf objOutputMix);

#endif	// USE_EXTERNAL_ENGINE_INSTANCE

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////

#endif	// _OPENAL_AL_SL_BRIDGE_H_
