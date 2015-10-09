/***********************************************************************//**
 *	FileAndroidLargeAsset.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2012/12/18.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "FileAndroidLargeAsset.h"

// Friends
#include "FileAndroidAsset.h"
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
bool FileAndroidLargeAsset::open(const CStringBase* pathFile, Mode mode)
{
	_isOpened = false;

	if(mode != MODE_READ)
	{
		return false;	// 読み込み以外はサポートしない
	}

	_offset = 0;
	convFullPath(_pathFileOrg, pathFile);

	if(openAssetCached() < 0)
	{
		return false;
	}

	_mode = mode;
	_isOpened = true;
	_offset = 0;
	return true;
}

/*---------------------------------------------------------------------*//**
	ファイルを閉じる
**//*---------------------------------------------------------------------*/
void FileAndroidLargeAsset::close()
{
	// 全てのキャッシュされたファイルを閉じる
	for(int i = 0; i < _numCache; i++)
	{
		if(_oaarr[i]._asset != 0L)
		{
			#if defined(_ANDROID)
				AAsset_close(_oaarr[i]._asset);
			#endif
			_oaarr[i]._asset = 0L;
		}
	}

	_isOpened = false;
}

/*---------------------------------------------------------------------*//**
	読み込む
**//*---------------------------------------------------------------------*/
u32 FileAndroidLargeAsset::read(void* buf, u32 size)
{
	DEVTRACE("== {FileAndroidLargeAsset::read} 000: buf=%x, size=%d\n", buf, size);
	u32 rdsizeTotal = 0;
	while(true)
	{
		// ファイルを開く
		s32 ccidx = openAssetCached();
		if(ccidx == -1)
		{
			DEVTRACE("== {FileAndroidLargeAsset::read} 001: failed.\n");
			return 0;
		}
		ASSERT((0 <= ccidx) && (ccidx < _numCache));
		ASSERT(_oaarr[ccidx]._asset != 0L);

		// 分割ファイル内でのオフセットを求める
		u32 ofsDivFileTop = divIndexToOffset(_oaarr[ccidx]._dividx);
		DEVTRACE("== {FileAndroidLargeAsset::read} 001: ofsDivFileTop=%d, asset=%x\n", ofsDivFileTop, _oaarr[ccidx]._asset);
		u32 ofsInDivFile = _offset - ofsDivFileTop;
		DEVTRACE("== {FileAndroidLargeAsset::read} 002: ofsInDivFile=%d, _offset=%d\n", ofsInDivFile, _offset);

		// 読み込み
		#if defined(_ANDROID)
			AAsset_seek(_oaarr[ccidx]._asset, ofsInDivFile, SEEK_SET);
			DEVTRACE("== {FileAndroidLargeAsset::read} 003: buf=%x, size=%d\n", buf, size);
			u32 rdsize = AAsset_read(_oaarr[ccidx]._asset, buf, size);
		#else
			u32 rdsize = size;
		#endif

		// オフセット更新
		_offset += rdsize;
		rdsizeTotal += rdsize;
		DEVTRACE("== {FileAndroidLargeAsset::read} 004: rdsize=%d, _offset=%d\n", rdsize, _offset);

		// 次へ
		if(rdsize < size)
		{
			buf = (u8*)buf + rdsize;
			size -= rdsize;
		}
		else
		{
			break;
		}
	}

	DEVTRACE("== {FileAndroidLargeAsset::read} 999: _offset=%d\n", _offset);
	return rdsizeTotal;
}

/*---------------------------------------------------------------------*//**
	書き込む
**//*---------------------------------------------------------------------*/
u32 FileAndroidLargeAsset::write(const void* buf, u32 size)
{
	ASSERTM(false, "{FileAndroidLargeAsset::write} not supported.");	// サポートしない
	return 0;
}

/*---------------------------------------------------------------------*//**
	シーク
**//*---------------------------------------------------------------------*/
void FileAndroidLargeAsset::seek(u32 offset, bool isTop)
{
	if(isTop)
	{
		_offset = offset;
	}
	else
	{
		_offset += offset;
	}
}

/*---------------------------------------------------------------------*//**
	現在のシーク位置を得る
**//*---------------------------------------------------------------------*/
u32 FileAndroidLargeAsset::getPosition() const
{
	return _offset;
}

/*---------------------------------------------------------------------*//**
	フラッシュ
**//*---------------------------------------------------------------------*/
void FileAndroidLargeAsset::flush()
{
	// サポートしないが、不正ではない
}

/*---------------------------------------------------------------------*//**
	ファイルサイズを得る
**//*---------------------------------------------------------------------*/
u32 FileAndroidLargeAsset::getLength() const
{
	ASSERTM(false, "{FileAndroidLargeAsset::getLength} not supported.");	// サポートしない
	return 0;
}

/*---------------------------------------------------------------------*//**
	ファイルが存在するかをチェックする
**//*---------------------------------------------------------------------*/
bool FileAndroidLargeAsset::isExist(const CStringBase* pathFile) const
{
	return FileAndroidAsset::isFileExist(pathFile);
}

/*---------------------------------------------------------------------*//**
	ファイル ディスクリプタを得る
**//*---------------------------------------------------------------------*/
void* FileAndroidLargeAsset::getDescriptor()
{
	ASSERTM(false, "{FileAndroidLargeAsset::getDescriptor} not supported.");	// サポートしない
	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FileAndroidLargeAsset::FileAndroidLargeAsset(FILE_MNG_R* astmng, s32 numCache)
	: File()
	, _astmng(astmng)
	, _pathFileOrg(new VcString())
	, _oaarr(0L)
	, _offset(0)
	, _numCache(numCache)
	, _isOpened(false)
{
	createCache();
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FileAndroidLargeAsset::FileAndroidLargeAsset(FILE_MNG_R* astmng)
	: File()
	, _astmng(astmng)
	, _pathFileOrg(new VcString())
	, _oaarr(0L)
	, _offset(0)
	, _numCache(NUM_OPENED_CACHE_DEFAULT)
	, _isOpened(false)
{
	createCache();
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FileAndroidLargeAsset::FileAndroidLargeAsset(FILE_MNG_R* astmng, const CStringBase* pathFile, Mode mode, s32 numCache)
	: File(pathFile, mode)
	, _astmng(astmng)
	, _pathFileOrg(new VcString())
	, _oaarr(0L)
	, _offset(0)
	, _numCache(numCache)
	, _isOpened(false)
{
	if(createCache())
	{
		open(pathFile, mode);
	}
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FileAndroidLargeAsset::FileAndroidLargeAsset(FILE_MNG_R* astmng, CcString pathFile, Mode mode, s32 numCache)
	:  File(pathFile, mode)
	, _astmng(astmng)
	, _pathFileOrg(new VcString())
	, _oaarr(0L)
	, _offset(0)
	, _numCache(numCache)
	, _isOpened(false)
{
	if(createCache())
	{
		open(&pathFile, mode);
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FileAndroidLargeAsset::~FileAndroidLargeAsset()
{
	if(_isOpened)
	{
		close();
	}

	delete _pathFileOrg;
}

/*---------------------------------------------------------------------*//**
	キャッシュの作成
**//*---------------------------------------------------------------------*/
bool FileAndroidLargeAsset::createCache()
{
	_oaarr = new OpenedAsset[_numCache];
	return _oaarr != 0L;
}

/*---------------------------------------------------------------------*//**
	アセットファイルを開く
**//*---------------------------------------------------------------------*/
s32 FileAndroidLargeAsset::openAssetCached()
{
	DEVTRACE("== {FileAndroidLargeAsset::openAssetCached} 000\n");

	// キャッシュされた開かれたファイルの中から探す
	s32 ccidx = findOpenedAssetOnCache(_offset);
	DEVTRACE("== {FileAndroidLargeAsset::openAssetCached} 001: ccidx=%d\n", ccidx);
	if(ccidx != -1)
	{
		return ccidx;	// 既に開かれている
	}

	// 開かれていない場合はまずはキャッシュの空きを見つける
	ccidx = findEmptyAssetOnCache(true);
	DEVTRACE("== {FileAndroidLargeAsset::openAssetCached} 002: ccidx=%d\n", ccidx);
	ASSERT((0 <= ccidx) && (ccidx < _numCache));

	// キャッシュのインデックスを保存
	_oaarr[ccidx]._dividx = offsetToDivIndex(_offset);

	// パスを作成
	VcString path = VcString::format(".%03d", (_oaarr[ccidx]._dividx + 1));
	path = *_pathFileOrg + path;
	TRACE("{FileAndroidLargeAsset::openAssetCached} file open. %s\n", path.getRaw());

	// ファイルを開く
	#if defined(_ANDROID)
		_oaarr[ccidx]._asset = AAssetManager_open(_astmng, path.getRaw(), AASSET_MODE_UNKNOWN);
	#endif
	DEVTRACE("== {FileAndroidLargeAsset::openAssetCached} 999: asset=%x, dividx=%d\n", _oaarr[ccidx]._asset, _oaarr[ccidx]._dividx);
	return (_oaarr[ccidx]._asset != 0L) ? ccidx : -1;
}

/*---------------------------------------------------------------------*//**
	指定オフセット以下のアセットファイルを閉じる
**//*---------------------------------------------------------------------*/
s32 FileAndroidLargeAsset::closeAssetCached(u32 offsetUnder)
{
	// 指定オフセット以下のアセットファイルを閉じる
	s32 dividxUnder = offsetToDivIndex(offsetUnder);
	for(s32 i = 0; i < _numCache; i++)
	{
		if((_oaarr[i]._asset != 0L) && (_oaarr[i]._dividx <= dividxUnder))
		{
			DEVTRACE("== {FileAndroidLargeAsset::closeAssetCached} 001: i=%d, _dividx=%d, dividxUnder=%d\n", i, _oaarr[i]._dividx, dividxUnder);
			#if defined(_ANDROID)
				AAsset_close(_oaarr[i]._asset);
			#endif
			_oaarr[i]._asset = 0L;
			return i;
		}
	}
	// 指定オフセット以下で閉じれなかった場合は適当なアセットを閉じる
	for(s32 i = _numCache -1 ; i >= 0; i--)
	{
		if(_oaarr[i]._asset != 0L)
		{
			DEVTRACE("== {FileAndroidLargeAsset::closeAssetCached} 002: i=%d\n", i);
			#if defined(_ANDROID)
				AAsset_close(_oaarr[i]._asset);
			#endif
			_oaarr[i]._asset = 0L;
			return i;
		}
	}
	DEVTRACE("== {FileAndroidLargeAsset::closeAssetCached} 003: _numCache=%d\n", _numCache);
	return _numCache - 1;
}

/*---------------------------------------------------------------------*//**
	空のキャッシュを得る
**//*---------------------------------------------------------------------*/
s32 FileAndroidLargeAsset::findEmptyAssetOnCache(bool forceClose)
{
	for(s32 i = _numCache -1 ; i >= 0; i--)
	{
		if(_oaarr[i]._asset == 0L)
		{
			DEVTRACE("== {FileAndroidLargeAsset::findEmptyAssetOnCache} 001: i=%d\n", i);
			return i;
		}
	}
	// 強制クローズ
	if(!forceClose)
	{
		return -1;
	}
	return closeAssetCached(_offset);
}

/*---------------------------------------------------------------------*//**
	キャッシュされたアセットファイルを検索する
**//*---------------------------------------------------------------------*/
s32 FileAndroidLargeAsset::findOpenedAssetOnCache(u32 offset)
{
	s32 dividx = offsetToDivIndex(offset);
	for(int i = 0; i < _numCache; i++)
	{
		if((_oaarr[i]._asset != 0L) && (_oaarr[i]._dividx == dividx))
		{
			return i;
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
