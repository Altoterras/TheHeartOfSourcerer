/***********************************************************************//**
 *	FileAndroid.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2013/01/03.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FileAndroid.h"

// Friends
#include "FileAndroidAsset.h"
#include "FileAndroidLargeAsset.h"
#include "../common/OsModule.h"
#include "../common/AndroidMainModule.h"

// External

// Library
#if defined(_ANDROID)
#include <android/asset_manager_jni.h>
#endif

TFW_BEGIN_NS
	
////////////////////////////////////////////////////////////////////////////
// マクロ

#define DEVTRACE	//FTRACE

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// File メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FileAndroid::FileAndroid(u32 flags)
	: File()
	, _file(0L)
{
	makeFileModule(flags);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FileAndroid::FileAndroid(u32 flags, const CStringBase* pathFile, Mode mode)
	: File(pathFile, mode)
	, _file(0L)
{
	if(makeFileModule(flags))
	{
		open(pathFile, mode);
	}
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FileAndroid::FileAndroid(u32 flags, CcString pathFile, Mode mode)
	:  File(pathFile, mode)
	, _file(0L)
{
	if(makeFileModule(flags))
	{
		open(&pathFile, mode);
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FileAndroid::~FileAndroid()
{
	if(_file != 0L)
	{
		delete _file;
	}
}

/*---------------------------------------------------------------------*//**
	ファイルモジュールの作成
**//*---------------------------------------------------------------------*/
bool FileAndroid::makeFileModule(u32 flags)
{
	if(TFW_IS_FLAG(flags, FAF_ASSET) || TFW_IS_FLAG(flags, FAF_LARGE_ASSET))
	{
		// アセットマネージャを得る
		FILE_MNG_R* astmng = 0L;
		#if defined(_ANDROID)
		{
			JNIEnv* jni = ((AndroidMainModule*)OsModule::getMainModule())->_jnienv;
			jobject jobjActivity = ((AndroidMainModule*)OsModule::getMainModule())->_clazzActivity;

			// 各クラス取得
			jclass jclsContext = jni->FindClass("android/content/Context");
			if(jclsContext == NULL)								{	TRACE("{FileAndroid::makeFileModule} ### 101 ###\n");	return false;	}

			// AssetManager astmng = activity.getAssets();
			jmethodID jmethodGetAssets = jni->GetMethodID(jclsContext, "getAssets", "()Landroid/content/res/AssetManager;");
			if(jmethodGetAssets == NULL)						{	TRACE("{FileAndroid::makeFileModule} ### 201 ###\n");	return false;	}
			jobject jobjAssetManager = jni->CallObjectMethod(jobjActivity, jmethodGetAssets);
			if(jobjAssetManager == NULL)						{	TRACE("{FileAndroid::makeFileModule} ### 202 ###\n");	return false;	}

			astmng = AAssetManager_fromJava(jni, jobjAssetManager);
			if(astmng == NULL)									{	TRACE("{FileAndroid::makeFileModule} ### 301 ###\n");	return false;	}
		}
		#endif

		// ファイルモジュールの作成
		if(TFW_IS_FLAG(flags, FAF_LARGE_ASSET))
		{
			_file = new FileAndroidLargeAsset(astmng);
		}
		else
		{
			_file = new FileAndroidAsset(astmng);
		}
	}
	else
	{
		// ファイルモジュールの作成
		_file = new File();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
