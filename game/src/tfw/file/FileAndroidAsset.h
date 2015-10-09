/***********************************************************************//**
 *	FileAndroidAsset.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2012/12/14.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FILE_FILE_ANDROID_ASSET_H_
#define _TFW_FILE_FILE_ANDROID_ASSET_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "File.h"

// Library
#if defined(_ANDROID)
#include <android/asset_manager.h>
#endif

////////////////////////////////////////////////////////////////////////////
// 外部構造体

#if defined(_ANDROID)
	#define FILE_R AAsset
	#define FILE_MNG_R AAssetManager
#else
	#define FILE_R void
	#define FILE_MNG_R void
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	Android 用ファイル クラス
 *
**//*---------------------------------------------------------------------*/
class FileAndroidAsset : public File
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 開いているかを得る
	virtual bool isOpened() const { return _asset != 0L; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool open(const CStringBase* pathFile, Mode mode);
	virtual void close();
	virtual u32 read(void* buf, u32 size);
	virtual u32 write(const void* buf, u32 size);
	virtual void seek(u32 offset, bool isTop);
	virtual u32 getPosition() const;
	virtual void flush();
	virtual u32 getLength() const;
	virtual bool isExist(const CStringBase* pathFile) const;
	virtual void* getDescriptor();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FileAndroidAsset(FILE_MNG_R* astmng);
	FileAndroidAsset(FILE_MNG_R* astmng, const CStringBase* pathFile, Mode mode);
	FileAndroidAsset(FILE_MNG_R* astmng, CcString pathFile, Mode mode);
	virtual ~FileAndroidAsset();

	//======================================================================
	// 変数
private:
	FILE_MNG_R* _astmng;
	FILE_R* _asset;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_FILE_FILE_ANDROID_ASSET_H_
