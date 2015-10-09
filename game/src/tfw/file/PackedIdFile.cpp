/***********************************************************************//**
 *	PackedIdFile.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/07/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PackedIdFile.h"

// Friends
#if defined(_ANDROID)
#include "FileAndroid.h"
#else
#include "File.h"
#endif
#include "../collection/List.h"
#include "../string/VcString.h"
#include "../tbf/TbfDefine.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PackedIdFile メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PackedIdFile::PackedIdFile()
	: _file(0L)
	, _listIncf(0L)
	, _offsetHead(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PackedIdFile::~PackedIdFile()
{
	ASSERT(_listIncf == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PackedIdFile::create(const CStringBase* fname, u32 flags)
{
	TbfHeader head;
	u32 readsize;

	// ファイルを開く
	#if defined(_ANDROID)
		_file = new FileAndroid(flags, fname, File::MODE_READ);
	#else
		_file = new File(fname, File::MODE_READ);
	#endif
	if(!_file->isOpened())	// 開けなかった
	{
		// 失敗
		VcString msg;
		ASSERTM(false, VcString::format(&msg, "{PackedIdFile::create} file-open faild. - fname=%s", fname->getRaw()));
		goto FAILED;
	}

	// 無圧縮 ZIP ヘッダをスキップする
	if(TFW_IS_FLAG(flags, PIFF_STORED_ZIP))
	{
		#if defined(_WINDOWS)
			#define PACKED
		#else
			#define PACKED	 __attribute__ ((packed))
		#endif
		struct ZipFileHeader
		{
			u8 _signature[4];		// シグネチャ 0x04034b50
			u16 _version;			// バージョン
			u16 _flags;				// 汎用フラグ
			u16 _method;			// 圧縮メソッド
			u16 _timeUpdate;		// ファイルの最終変更時間
			u16 _dateUpdate;		// ファイルの最終変更日付
			u32 _crc;				// CRC-32
			u32 _sizeCompressed;	// 圧縮サイズ
			u32 _sizeOriginal;		// 非圧縮サイズ
			u16 _lengthFileName;	// ファイル名の長さ
			u16 _lengthExtra;		// 拡張フィールドの長さ
		} PACKED;
		ZipFileHeader ziphead;
		readsize = _file->read(&ziphead, sizeof(ZipFileHeader));
		if(readsize != sizeof(ZipFileHeader))
		{
			// 失敗
			VcString msg;
			ASSERTM(false, VcString::format(&msg, "{PackedIdFile::create} read zip-header faild. - readsize=%d/%d", readsize, sizeof(ZipFileHeader)));
			goto FAILED;
		}
		///TRACE("{PackedIdFile::create} stored-zip file: filename-len=%d, extra-len=%d, sizeof(ZipFileHeader)=%d\n", ziphead._lengthFileName, ziphead._lengthExtra, sizeof(ZipFileHeader));
		_offsetHead = sizeof(ZipFileHeader);
		// ファイル名をスキップ
		_file->seek(ziphead._lengthFileName, false);
		_offsetHead += ziphead._lengthFileName;
		// 拡張フィールドをスキップ
		if(ziphead._lengthExtra > 0)
		{
			_file->seek(ziphead._lengthExtra, false);
			_offsetHead += ziphead._lengthExtra;
		}
	}
	else
	{
		_offsetHead = 0;
	}

	// TBF ヘッダを読む
	readsize = _file->read(&head, sizeof(TbfHeader));
	if(readsize != sizeof(TbfHeader))
	{
		// 失敗
		VcString msg;
		ASSERTM(false, VcString::format(&msg, "{PackedIdFile::create} read header faild. - readsize=%d/%d", readsize, sizeof(TbfHeader)));
		goto FAILED;
	}
	if(head._typeInclude != TbfHeader::INCTYPE_ID)	// ID ファイルではない
	{
		// 失敗
		VcString msg;
		ASSERTM(false, VcString::format(&msg, "{PackedIdFile::create} infaild header. - type=%d/%d", head._typeInclude, TbfHeader::INCTYPE_ID));
		goto FAILED;
	}

	// 内包ファイル情報をリストに保存する
	_listIncf = new List<TbfIncludeIdFile*>(true);
	for(u32 i = 0; i < head._numIncludeFile; i++)
	{
		TbfIncludeIdFile* incf = new TbfIncludeIdFile;
		_file->read(incf, sizeof(TbfIncludeIdFile));
		_listIncf->addTail(incf);
	}

	return true;
	
FAILED:
	// 失敗
	delete _file;
	_file = 0L;
	return false;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PackedIdFile::destroy()
{
	delete _listIncf;
	_listIncf = 0L;

	if((_file != 0L) && (_file->isOpened()))	{	_file->close();	}
	delete _file;
	_file = 0L;
}

/*---------------------------------------------------------------------*//**
	ファイル情報を検索する
**//*---------------------------------------------------------------------*/
const TbfIncludeIdFile* PackedIdFile::findFileInfo(u32 fileid) const
{
	for(ListIterator<TbfIncludeIdFile*> it = _listIncf->iterator(); it.has(); it.next())
	{
		const TbfIncludeIdFile* incf = it.object();
		if(incf->_fileid == fileid)
		{
			return incf;
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	読み込み
**//*---------------------------------------------------------------------*/
u32 PackedIdFile::read(const TbfIncludeIdFile* incf, void* buf, u32 size)
{
	if(_file == 0L)	{	return 0;	}
	u32 sizeRemain = (_offsetHead + incf->_offset + incf->_size) - _file->getPosition();
	u32 sizeReadTry = TFW_MIN(sizeRemain, size);
	u32 sizeReadRes = _file->read(buf, sizeReadTry);
	return sizeReadRes;
}

/*---------------------------------------------------------------------*//**
	シーク
**//*---------------------------------------------------------------------*/
void PackedIdFile::seek(const TbfIncludeIdFile* incf, u32 offset, bool isTop)
{
	if(_file == 0L)	{	return;	}
	// シーク
	if(isTop)
	{
		_file->seek(_offsetHead + incf->_offset + offset, true);
	}
	else
	{
		_file->seek(_offsetHead + _file->getPosition() + offset, true);
	}
	return;
}

/*---------------------------------------------------------------------*//**
	現在のシーク位置を得る
**//*---------------------------------------------------------------------*/
u32 PackedIdFile::getPosition(const TbfIncludeIdFile* incf) const
{
	if(_file == 0L)	{	return 0;	}
	return _file->getPosition() - incf->_offset - _offsetHead;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
