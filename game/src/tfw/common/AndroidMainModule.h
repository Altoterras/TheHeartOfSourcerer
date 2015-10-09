/***********************************************************************//**
 *  AndroidMainModule.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2013/01/02.
 *  Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_COMMON_ANDROID_MAIN_MODULE_H_
#define _TFW_COMMON_ANDROID_MAIN_MODULE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "OsMainModule.h"

// Library
#if defined(_ANDROID)
#include <jni.h>
#endif	// #if defined(_ANDROID)

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	Android メイン モジュール
 *
**//*---------------------------------------------------------------------*/
class AndroidMainModule : public OsMainModule
{
#if defined(_ANDROID)
public:
	JNIEnv* _jnienv;
	jobject _clazzActivity;

public:
	AndroidMainModule() : _jnienv(0L), _clazzActivity(0) {}
#endif	// #if defined(_ANDROID)
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_COMMON_ANDROID_MAIN_MODULE_H_