/***********************************************************************//**
 *	FileAndroidLargeAsset.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2012/12/18.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FILE_FILE_ANDROID_LARGE_ASSET_H_
#define _TFW_FILE_FILE_ANDROID_LARGE_ASSET_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "File.h"

// Library
#if defined(_ANDROID)
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
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

class VcString;

/*---------------------------------------------------------------------*//**
 *	Android 用大ファイル クラス
 *
**//*---------------------------------------------------------------------*/
class FileAndroidLargeAsset : public File
{
	//======================================================================
	// 定数
public:
	// 分割されたファイルのサイズ（ビット演算をするので、２の累乗であること）
//	const static u32	SIZE_DIVFILE				= 1024 * 1024;			// 1MB
//	const static u32	BITSHIFT_DIVFILE			= 20;					// 1MB は 2 の 20 乗
	const static u32	SIZE_DIVFILE				= 32 * 1024 * 1024;		// 32MB
	const static u32	BITSHIFT_DIVFILE			= 25;					// 32MB は 2 の 25 乗

	// オープンキャッシュ数
	const static s32	NUM_OPENED_CACHE_DEFAULT	= 1;

	//======================================================================
	// クラス
private:
	class OpenedAsset
	{
	public:
		FILE_R*	_asset;
		s32 _dividx;

		OpenedAsset() : _asset(0L), _dividx(0) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 開いているかを得る
	virtual bool isOpened() const { return _isOpened; }

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
	FileAndroidLargeAsset(FILE_MNG_R* astmng, s32 numCache);
	FileAndroidLargeAsset(FILE_MNG_R* astmng);
	FileAndroidLargeAsset(FILE_MNG_R* astmng, const CStringBase* pathFile, Mode mode, s32 numCache);
	FileAndroidLargeAsset(FILE_MNG_R* astmng, CcString pathFile, Mode mode, s32 numCache);
	virtual ~FileAndroidLargeAsset();

private:
	bool createCache();
	s32 openAssetCached();
	s32 closeAssetCached(u32 offsetUnder);
	s32 findEmptyAssetOnCache(bool forceClose);
	s32 findOpenedAssetOnCache(u32 offset);
	inline s32 offsetToDivIndex(u32 offset) const { return (s32)(offset >> BITSHIFT_DIVFILE); }
	inline u32 divIndexToOffset(s32 dividx) const { return ((u32)dividx << BITSHIFT_DIVFILE); }

	//======================================================================
	// 変数
private:
	FILE_MNG_R* _astmng;
	VcString* _pathFileOrg;
	OpenedAsset* _oaarr;
	u32 _offset;
	s32 _numCache;
	bool _isOpened;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_FILE_FILE_ANDROID_LARGE_ASSET_H_
