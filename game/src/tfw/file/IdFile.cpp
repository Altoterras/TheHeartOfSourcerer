/***********************************************************************//**
 *	IdFile.cpp
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
#include "IdFile.h"

// Friends
#include "PackedIdFile.h"
#include "../lib/ZCompress.h"
#include "../string/VcString.h"
#include "../string/StringUtils.h"
#include "../tbf/TbfDefine.h"

// External

// Library
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// IdFile 静的メンバ変数

PackedIdFile* IdFile::_pidfile = 0L;
IdFile::ReadBuffer* IdFile::_rbUncompress1 = 0L;
IdFile::ReadBuffer* IdFile::_rbUncompress2 = 0L;
VcString* IdFile::_postfixPlatform = 0L;
const char** IdFile::_filedefDevMode = 0L;
VcString* IdFile::_dirDevMode = 0L;

//==========================================================================
// IdFile メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	開いているかどうか
**//*---------------------------------------------------------------------*/
bool IdFile::isOpened() const
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 通常モード
	if(_filedefDevMode == 0L)
	{
		return _incfCur != 0L;
	}
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 開発モード
	else
	{
		if(_fileDevMode == 0L)	{	return false;	}
		return _fileDevMode->isOpened();
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
IdFile::IdFile() : _incfCur(0L), _posUncompress(0), _sizeUncompress(0), _fileDevMode(0L)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
IdFile::IdFile(u32 fileid) : _incfCur(0L), _posUncompress(0), _sizeUncompress(0), _fileDevMode(0L)
{
	open(fileid);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
IdFile::~IdFile()
{
	close();
}

/*---------------------------------------------------------------------*//**
	初期化

	@param sizeUncompressBufferFirst 圧縮ファイル読込用バッファ１のサイズ
	@param sizeUncompressBufferSecond 圧縮ファイル読込用バッファ２のサイズ
**//*---------------------------------------------------------------------*/
void IdFile::init(const CStringBase* fname, const CStringBase* postfix, s32 sizeUncompressBufferFirst, s32 sizeUncompressBufferSecond, u32 flags)
{
	// ファイルパック情報を得る
	_pidfile = new PackedIdFile();
	if(!_pidfile->create(fname, flags))
	{
		VcString msg;
		ASSERTM(false, VcString::format(&msg, "{IdFile::init} faild. - fname=%s", fname->getRaw()));
		delete _pidfile;
		_pidfile = 0L;
	}

	// ファイル名の後に付加する定型文字列を作成する
	_postfixPlatform = new VcString(postfix);

	// 圧縮展開用バッファ作成
	_rbUncompress1 = new ReadBuffer();
	if(!_rbUncompress1->prepare(sizeUncompressBufferFirst))
	{
		VcString msg;
		ASSERTM(false, VcString::format(&msg, "{ReadBuffer::prepare} preparing buffer1 failed."));
		delete _rbUncompress1;
		_rbUncompress1 = 0L;
	}
	_rbUncompress2 = new ReadBuffer();
	if(!_rbUncompress2->prepare(sizeUncompressBufferSecond))
	{
		VcString msg;
		ASSERTM(false, VcString::format(&msg, "{ReadBuffer::prepare} preparing buffer2 failed."));
		delete _rbUncompress2;
		_rbUncompress2 = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デバッグ用のファイル読みを有効化する

	@param filedef TbfPack ツールで出力される開発用
**//*---------------------------------------------------------------------*/
void IdFile::enableDevMode(TBF_PACK_DEV_MODE_FILE_LIST** filedef, const CStringBase* dir)
{
	_filedefDevMode = filedef;
	_dirDevMode = new VcString(dir);
}

/*---------------------------------------------------------------------*//**
	解放処理
**//*---------------------------------------------------------------------*/
void IdFile::cleanup()
{
	// 圧縮展開用バッファの削除
	if(_rbUncompress1 != 0L)
	{
		delete _rbUncompress1;
		_rbUncompress1 = 0L;
	}
	if(_rbUncompress2 != 0L)
	{
		delete _rbUncompress2;
		_rbUncompress2 = 0L;
	}

	// ファイル名の後に付加する定型文字列を削除する
	if(_postfixPlatform != 0L)
	{
		delete _postfixPlatform;
		_postfixPlatform = 0L;
	}

	// ファイルパック情報を削除
	if(_pidfile != 0L)
	{
		_pidfile->destroy();
		delete _pidfile;
		_pidfile = 0L;
	}
	if(_dirDevMode != 0L)
	{
		delete _dirDevMode;
		_dirDevMode = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	ファイルを開く
**//*---------------------------------------------------------------------*/
bool IdFile::open(u32 fileid)
{
	// 圧縮ファイル変数の初期化
	_posUncompress = 0;
	_sizeUncompress = U32_MAX;		// 無効なファイルサイズ

	bool isCompress = false;
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 通常モード
	if(_filedefDevMode == 0L)
	{
		if(_pidfile == 0L)
		{
			ASSERTM(false, "{IdFile.open} no packed id file.");
			return false;
		}
		ASSERT(_incfCur == 0L);

		// ファイル情報を得る
		const TbfIncludeIdFile*	incf = _pidfile->findFileInfo(fileid);
		if(incf == 0L)
		{
			TRACE(VcString::format("{IdFile.open} file (fileid=%d) not found.", fileid));
			return false;
		}

		///TRACE(VcString::format("*** {IdFile.open} file: fileid=%d, offset=%d, size=%d\n", incf->_fileid, incf->_offset, incf->_size));
		// そのファイルの先頭にシーク
		_pidfile->seek(incf, 0, true);

		// 圧縮ファイルかどうか
		isCompress = (incf->_isCompress == 1);

		// 成功
		_incfCur = incf;
	}
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 開発モード
	else
	{
		// ファイルパス等を探す
		VcString catdir;
		VcString filename;
		Platform pf;
		int i = 0;
		while(_filedefDevMode[i][0] != '*')
		{
			VcString fd(_filedefDevMode[i]);
			VcStringArray saFd = StringUtils::split(&fd, '|');
			if(saFd.getCount() != 5)
			{
				// 不正な定義
				i++;
				continue;
			}
			u32 fileidWk = (u32)StringUtils::toInteger(saFd.get(0));
			if(fileid == fileidWk)
			{
				catdir = *saFd.get(1);
				filename = *saFd.get(2);
				isCompress = false;	//saFd.get(3)->equals("1");
				pf = (Platform)StringUtils::toInteger(saFd.get(4));
				break;
			}
			i++;
		}
		if(filename.getLength() <= 0)	{	return false;	}	// 見つからなかった

		// プラットフォームによるファイル名変換
		if((pf == PF_FNDETECT) && (_postfixPlatform != 0L))
		{
			VcString name, ext;
			StringUtils::divideFileName(&name, &ext, &filename);
			filename = name + *_postfixPlatform + ext;
		}

		// ファイルを開く
		VcString filepath(*_dirDevMode + catdir + '/' + filename);
		File* fileDevMode = new File(&filepath, File::MODE_READ);
		if(!fileDevMode->isOpened())
		{
			delete fileDevMode;
			return false;
		}
		_fileDevMode = fileDevMode;
	}

	// 圧縮ファイルは予め展開しておく
	if(isCompress)
	{
		ASSERTM(!_rbUncompress1->_used, "{IdFile::open} uncmp-buffer 1 is already used.");
		_rbUncompress1->_used = true;
		// 圧縮ファイルを読む
		u32 sizeOrgFile = getLength();
		if(sizeOrgFile > _rbUncompress1->_size)
		{
			// エラー
			VcString msg;
			ASSERTM(sizeOrgFile <= _rbUncompress1->_size, VcString::format(&msg, "{IdFile::open} uncmp-buffer is too short. - file_size=%d, uncmp_buf_size=%d", sizeOrgFile, _rbUncompress1->_size)->getRaw());
			close();
			return false;
		}
		if(sizeOrgFile > read(_rbUncompress1->_buf, sizeOrgFile))
		{
			// エラー
			ASSERTM(false, "{IdFile::open} reading compressed-file is failed.");
			close();
			return false;
		}
		// 圧縮を展開する
		ASSERTM(!_rbUncompress2->_used, "{IdFile::open} uncmp-buffer 2 is already used.");
		_rbUncompress2->_used = true;
		u32 sizeUncompressFile = _rbUncompress2->_size;
		if(!ZCompress::uncompress(_rbUncompress2->_buf, &sizeUncompressFile, _rbUncompress1->_buf, sizeOrgFile))
		{
			// 展開失敗
			ASSERTM(false, "{IdFile::open} uncompress failed.");
			close();
			return false;
		}
		_rbUncompress1->_used = false;
		// 展開成功
		_posUncompress = 0;						// 展開ファイルのファイル位置
		_sizeUncompress = sizeUncompressFile;	// 展開ファイルのサイズ
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ファイルを閉じる
**//*---------------------------------------------------------------------*/
void IdFile::close()
{
	if(_sizeUncompress != U32_MAX)	// 圧縮ファイルの場合
	{
		ASSERT(!_rbUncompress1->_used);
		_rbUncompress2->_used = false;
		_posUncompress = 0;
		_sizeUncompress = U32_MAX;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 通常モード
	if(_filedefDevMode == 0L)
	{
		if(_incfCur == 0L)		{	return;	}
		_incfCur = 0L;
	}
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 開発モード
	else
	{
		if(_fileDevMode == 0L)	{	return;	}
		_fileDevMode->close();
		delete _fileDevMode;
		_fileDevMode = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	読み込む
**//*---------------------------------------------------------------------*/
u32 IdFile::read(void* buf, u32 size)
{
	// 圧縮ファイルの読み込み
	if(_sizeUncompress != U32_MAX)
	{
		u32 sizeValid = TFW_MIN(size, _sizeUncompress - _posUncompress);
		if(sizeValid > 0)	{	::memcpy(buf, _rbUncompress2->_buf + _posUncompress, sizeValid);	}
		_posUncompress += sizeValid;
		return sizeValid;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 通常モード
	if(_filedefDevMode == 0L)
	{
		if(_pidfile == 0L)	{	ASSERT(false);	return 0;	}
		if(_incfCur == 0L)	{	ASSERT(false);	return 0;	}
		return _pidfile->read(_incfCur, buf, size);
	}
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 開発モード
	else
	{
		if(_fileDevMode == 0L)	{	return 0;	}
		return _fileDevMode->read(buf, size);
	}
}

/*---------------------------------------------------------------------*//**
	文字列に読み込む
**//*---------------------------------------------------------------------*/
u32 IdFile::readString(VcString* str, u32 size)
{
	char* buf = new char[size];
	if(buf == 0)	{	return 0;	}

	u32 sizeRead = read(buf, size);

	if(sizeRead > 0)
	{
		str->set(buf, sizeRead);
	}
	else
	{
		str->set("");
	}

	return sizeRead;
}

/*---------------------------------------------------------------------*//**
	テキストを１行読み込む

	@retval true 読み込み成功
	@retval false 読み込みが終了した
**//*---------------------------------------------------------------------*/
bool IdFile::readTextLine(VcString* str)
{
	char c;
	VcString strWk;

	while(read(&c, 1) == 1)
	{
		if(c == '\r')
		{
		}
		else if(c == '\n')
		{
			*str = strWk;
			return true;
		}
		else
		{
			strWk += c;
		}
	}
	
	if(strWk.getLength() > 0)
	{
		*str = strWk;
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	書き込む
**//*---------------------------------------------------------------------*/
u32 IdFile::write(const void* buf, u32 size)
{
	ASSERTM(false, "IdFile::write is unsupported.");
	return 0;
}

/*---------------------------------------------------------------------*//**
	文字列を書き込む
**//*---------------------------------------------------------------------*/
u32 IdFile::writeString(const CStringBase* str)
{
	return write(str->getRaw(), str->getLength());
}

/*---------------------------------------------------------------------*//**
	シーク
**//*---------------------------------------------------------------------*/
void IdFile::seek(u32 offset, bool isTop)
{
	// 圧縮ファイルのシーク
	if(_sizeUncompress != U32_MAX)
	{
		if(isTop)	{	_posUncompress = offset;	}
		else		{	_posUncompress += offset;	}
		return;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 通常モード
	if(_filedefDevMode == 0L)
	{
		if(_pidfile == 0L)	{	ASSERT(false);	return;		}
		if(_incfCur == 0L)	{	ASSERT(false);	return;		}
		return _pidfile->seek(_incfCur, offset, isTop);
	}
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 開発モード
	else
	{
		if(_fileDevMode == 0L)	{	return;	}
		_fileDevMode->seek(offset, isTop);
	}
}

/*---------------------------------------------------------------------*//**
	現在のシーク位置を得る
**//*---------------------------------------------------------------------*/
u32 IdFile::getPosition() const
{
	// 圧縮ファイルのシーク位置
	if(_sizeUncompress != U32_MAX)
	{
		return _posUncompress;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 通常モード
	if(_filedefDevMode == 0L)
	{
		if(_pidfile == 0L)	{	ASSERT(false);	return 0;	}
		if(_incfCur == 0L)	{	ASSERT(false);	return 0;	}
		return _pidfile->getPosition(_incfCur);
	}
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 開発モード
	else
	{
		if(_fileDevMode == 0L)	{	return 0;	}
		return _fileDevMode->getPosition();
	}
}

/*---------------------------------------------------------------------*//**
	フラッシュ
**//*---------------------------------------------------------------------*/
void IdFile::flush()
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 通常モード
	if(_filedefDevMode == 0L)
	{
		// 何もしない
	}
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 開発モード
	else
	{
		if(_fileDevMode == 0L)	{	return;	}
		_fileDevMode->flush();
	}
}

/*---------------------------------------------------------------------*//**
	ファイルサイズを得る
**//*---------------------------------------------------------------------*/
u32 IdFile::getLength() const
{
	// 圧縮ファイルのファイルサイズ
	if(_sizeUncompress != U32_MAX)
	{
		return _sizeUncompress;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 通常モード
	if(_filedefDevMode == 0L)
	{
		if(_pidfile == 0L)	{	ASSERT(false);	return 0;	}
		if(_incfCur == 0L)	{	ASSERT(false);	return 0;	}
		return _incfCur->_size;
	}
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 開発モード
	else
	{
		if(_fileDevMode == 0L)	{	return 0;	}
		return _fileDevMode->getLength();
	}
}

/*---------------------------------------------------------------------*//**
	ファイルサイズを調べる（低速）
**//*---------------------------------------------------------------------*/
u32 IdFile::getIdFileLength(u32 fileid)
{
	const TbfIncludeIdFile* incf = _pidfile->findFileInfo(fileid);
	if(incf == 0L)	{	return 0;	}
	return incf->_size;
}

/*---------------------------------------------------------------------*//**
	ファイルの有無を調べる（低速）
**//*---------------------------------------------------------------------*/
bool IdFile::isIdFileExist(u32 fileid)
{
	const TbfIncludeIdFile* incf = _pidfile->findFileInfo(fileid);
	return incf != 0L;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
