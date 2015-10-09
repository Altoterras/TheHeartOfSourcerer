/***********************************************************************//**
 *	FileAndroidAsset.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2012/12/14.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FileAndroidAsset.h"

// Friends
#include "../string/VcString.h"

// External

// Library

TFW_BEGIN_NS
	
////////////////////////////////////////////////////////////////////////////
// マクロ

#define DEVTRACE	//FTRACE

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// File メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ファイルを開く
**//*---------------------------------------------------------------------*/
bool FileAndroidAsset::open(const CStringBase* pathFile, Mode mode)
{
	DEVTRACE("== {FileAndroidAsset::open} 000");
	if(mode != MODE_READ)
	{
		return false;	// 読み込み以外はサポートしない
	}

	/*
	VcString path;
	convFullPath(&path, pathFile);
	TRACE("{FileAndroidAsset::open} file open. %s\n", path.getRaw());
	*/

	#if defined(_ANDROID)
		_asset = AAssetManager_open(_astmng, pathFile->getRaw(), AASSET_MODE_UNKNOWN);
	#endif
	DEVTRACE("== {FileAndroidAsset::open} 002: _asset=%x", _asset);
	if(_asset == 0L)
	{
		return false;
	}

	_mode = mode;
	DEVTRACE("== {FileAndroidAsset::open} 999: _asset=%x, _mode=%d", _asset, _mode);
	return true;
}

/*---------------------------------------------------------------------*//**
	ファイルを閉じる
**//*---------------------------------------------------------------------*/
void FileAndroidAsset::close()
{
	#if defined(_ANDROID)
		if(_asset != 0L)
		{
			AAsset_close(_asset);
			_asset = 0L;
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	読み込む
**//*---------------------------------------------------------------------*/
u32 FileAndroidAsset::read(void* buf, u32 size)
{
	#if defined(_ANDROID)
		if(_asset != 0L)
		{
			u32 sizeRead = AAsset_read(_asset, buf, size);
			return sizeRead;
		}
	#endif
	return 0;
}

/*---------------------------------------------------------------------*//**
	書き込む
**//*---------------------------------------------------------------------*/
u32 FileAndroidAsset::write(const void* buf, u32 size)
{
	ASSERTM(false, "{FileAndroidAsset::write} not implemented.");	// 未実装
	return 0;
}

/*---------------------------------------------------------------------*//**
	シーク
**//*---------------------------------------------------------------------*/
void FileAndroidAsset::seek(u32 offset, bool isTop)
{
	#if defined(_ANDROID)
		if(_asset != 0L)
		{
			AAsset_seek(_asset, offset, isTop ? SEEK_SET : SEEK_CUR);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	現在のシーク位置を得る
**//*---------------------------------------------------------------------*/
u32 FileAndroidAsset::getPosition() const
{
	#if defined(_ANDROID)
		return (u32)AAsset_seek(_asset, 0, SEEK_CUR);
	#endif
	return 0;
}

/*---------------------------------------------------------------------*//**
	フラッシュ
**//*---------------------------------------------------------------------*/
void FileAndroidAsset::flush()
{
	// サポートしないが、不正ではない
}

/*---------------------------------------------------------------------*//**
	ファイルサイズを得る
**//*---------------------------------------------------------------------*/
u32 FileAndroidAsset::getLength() const
{
	#if defined(_ANDROID)
		if(_asset != 0L)
		{
			return (u32)AAsset_getLength(_asset);
		}
	#endif
	return 0;
}

/*---------------------------------------------------------------------*//**
	ファイルが存在するかをチェックする
**//*---------------------------------------------------------------------*/
bool FileAndroidAsset::isExist(const CStringBase* pathFile) const
{
	#if defined(_ANDROID)
		AAsset* asset = AAssetManager_open(_astmng, pathFile->getRaw(), AASSET_MODE_UNKNOWN);
		if(asset != 0L)
		{
			AAsset_close(asset);
			return true;
		}
	#endif
	return false;
}

/*---------------------------------------------------------------------*//**
	ファイル ディスクリプタを得る
**//*---------------------------------------------------------------------*/
void* FileAndroidAsset::getDescriptor()
{
	#if defined(_ANDROID)
		if(_asset != 0L)
		{
			off_t start, length;
			int fd = AAsset_openFileDescriptor(_asset, &start, &length);
			AAsset_close(_asset);	// ここで閉じてしまうことに注意
			_asset = 0L;
			return reinterpret_cast<void*>(fd);
		}
	#endif
	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FileAndroidAsset::FileAndroidAsset(FILE_MNG_R* astmng)
	: File()
	, _astmng(astmng)
	, _asset(0L)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FileAndroidAsset::FileAndroidAsset(FILE_MNG_R* astmng, const CStringBase* pathFile, Mode mode)
	: File(pathFile, mode)
	, _astmng(astmng)
	, _asset(0L)
{
	open(pathFile, mode);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FileAndroidAsset::FileAndroidAsset(FILE_MNG_R* astmng, CcString pathFile, Mode mode)
	:  File(pathFile, mode)
	, _astmng(astmng)
	, _asset(0L)
{
	open(&pathFile, mode);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FileAndroidAsset::~FileAndroidAsset()
{
	if(_asset != 0L)
	{
		close();
	}
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
