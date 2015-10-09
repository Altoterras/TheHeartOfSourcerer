/***********************************************************************//**
 *	File.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// コンパイル設定

#if defined(_WINDOWS)
	#pragma warning(disable: 4996)
#endif

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "File.h"

// Friends
#include "../string/VcString.h"
#include "../txt/TxtUtils.h"
#include "../txt/implement/SjisTxt.h"
#include "../txt/implement/Utf8Txt.h"
#include "../txt/implement/UnicodeTxt.h"

// External

// Library
#include <stdio.h>
#include <sys/stat.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// File 静的メンバ変数

VcString* File::_pathBase = 0L;

//==========================================================================
// File メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ファイルを開く
**//*---------------------------------------------------------------------*/
bool File::open(const CStringBase* pathFile, Mode mode)
{
	const char* szMode = 0L;
	switch(mode)
	{
	case MODE_READ:			szMode = "rb";	break;
	case MODE_WRITE:		szMode = "wb";	break;
	case MODE_WRITE_ADD:	szMode = "ab";	break;
	}
	if(szMode == 0L)	{	return false;	}

	VcString path;
	convFullPath(&path, pathFile);
	TRACE(VcString::format("Open File: %s\n", path.getRaw()));
	_file = ::fopen(path, szMode);
	if(_file != 0L)
	{
		_mode = mode;
		return true;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	ファイルを閉じる
**//*---------------------------------------------------------------------*/
void File::close()
{
	if(_file == 0L)	{	return;	}
	::fclose(_file);
	_file = 0L;
}

/*---------------------------------------------------------------------*//**
	読み込む
**//*---------------------------------------------------------------------*/
u32 File::read(void* buf, u32 size)
{
	if(_file == 0L)	{	return 0;	}
	u32 sizeRead = (u32)::fread(buf, 1, size, _file);
	return sizeRead;
}

/*---------------------------------------------------------------------*//**
	文字列に読み込む
**//*---------------------------------------------------------------------*/
u32 File::readString(VcString* str, u32 size)
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

	delete[] buf;
	return sizeRead;
}

/*---------------------------------------------------------------------*//**
	テキストを１行読み込む

	@retval true 読み込み成功
	@retval false 読み込みが終了した
**//*---------------------------------------------------------------------*/
bool File::readTextLine(VcString* str)
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
u32 File::write(const void* buf, u32 size)
{
	if(_file == 0L)	{	return 0;	}
	return (u32)::fwrite(buf, 1, size, _file);
}

/*---------------------------------------------------------------------*//**
	文字列を書き込む
**//*---------------------------------------------------------------------*/
u32 File::writeString(const CStringBase* str)
{
	return write(str->getRaw(), str->getLength());
}

/*---------------------------------------------------------------------*//**
	シーク
**//*---------------------------------------------------------------------*/
void File::seek(u32 offset, bool isTop)
{
	if(_file == 0L)	{	return;	}
	::fseek(_file, offset, isTop ? SEEK_SET : SEEK_CUR);
}

/*---------------------------------------------------------------------*//**
	現在のシーク位置を得る
**//*---------------------------------------------------------------------*/
u32 File::getPosition() const
{
	if(_file == 0L)	{	return 0;	}
	return (u32)::ftell(_file);
}

/*---------------------------------------------------------------------*//**
	フラッシュ
**//*---------------------------------------------------------------------*/
void File::flush()
{
	if(_file == 0L)	{	return;	}
	::fflush(_file);
}

/*---------------------------------------------------------------------*//**
	ファイルサイズを得る
**//*---------------------------------------------------------------------*/
u32 File::getLength() const
{
	if(_file == 0L)	{	return 0;	}
	#if defined(_WINDOWS)
		int fd = ::_fileno(_file);
		struct _stat32 s;
		::_fstat32(fd, &s);
	#else
		int fd = fileno(_file);
		struct stat s;
		fstat(fd, &s);
	#endif
	return (u32)s.st_size;
}

/*---------------------------------------------------------------------*//**
	ファイルが存在するかをチェックする
**//*---------------------------------------------------------------------*/
bool File::isExist(const CStringBase* pathFile) const
{
	return File::isFileExist(pathFile);
}

/*---------------------------------------------------------------------*//**
	ファイル ディスクリプタを得る
**//*---------------------------------------------------------------------*/
void* File::getDescriptor()
{
	return reinterpret_cast<void*>(_file);
}

/*---------------------------------------------------------------------*//**
	ファイルサイズを調べる
**//*---------------------------------------------------------------------*/
u32 File::getFileLength(const CStringBase* pathFile)
{
	VcString path;
	convFullPath(&path, pathFile);
	struct stat s;
	int res = ::stat(path.getRaw(), &s);
	if(res == -1)	{	return 0;	}
	return (u32)s.st_size;
}

/*---------------------------------------------------------------------*//**
	ファイルの有無を調べる
**//*---------------------------------------------------------------------*/
bool File::isFileExist(const CStringBase* pathFile)
{
	VcString path;
	convFullPath(&path, pathFile);
	struct stat s;
	int res = ::stat(path.getRaw(), &s);
	#if defined(_IOS) || defined(_ANDROID)
		return (res != -1) && ((s.st_mode & S_IFDIR) == 0);
	#elif defined(_WINDOWS)
		return (res != -1) && ((s.st_mode & _S_IFDIR) == 0);
	#endif
}

/*---------------------------------------------------------------------*//**
	ディレクトリの有無を調べる
**//*---------------------------------------------------------------------*/
bool File::isDirExist(const CStringBase* pathDir)
{
	VcString path;
	convFullPath(&path, pathDir);

	// stat は最後が \ や / を受け付けない
	if((path.getLength() > 0) && ( (path.getAt(path.getLength() - 1) == '\\') || (path.getAt(path.getLength() - 1) == '/') ))
	{
		path.setAt(path.getLength() - 1, '\0');
	}

	struct stat s;
	int res = ::stat(path.getRaw(), &s);
	#if defined(_IOS) || defined(_ANDROID)
		return (res != -1) && ((s.st_mode & S_IFDIR) != 0);
	#elif defined(_WINDOWS)
		return (res != -1) && ((s.st_mode & _S_IFDIR) != 0);
	#endif
}

#if defined(_WINDOWS)
/*---------------------------------------------------------------------*//**
	ディレクトリの有無を調べる
**//*---------------------------------------------------------------------*/
bool File::isDirExist(const TCHAR* pathDir)
{
#if defined(UNICODE)
	VwString wstrDirPath(pathDir);
	UnicodeTxt wtxtDirPath;
	SjisTxt stxtDirPath;
	wtxtDirPath.setString(&wstrDirPath, UnicodeTxt::F_REF_STRING);
	TxtUtils::convEncode(&stxtDirPath, &wtxtDirPath);
	return isDirExist(stxtDirPath.getRawString());
#else
	return isDirExist(&CcString(pathDir));
#endif
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
File::File() : _file(0L), _mode(MODE_NULL)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
File::File(const CStringBase* pathFile, Mode mode) : _file(0L), _mode(MODE_NULL)
{
	open(pathFile, mode);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
File::File(CcString pathFile, Mode mode) : _file(0L), _mode(MODE_NULL)
{
	open(&pathFile, mode);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
File::~File()
{
	if(_file != 0L)
	{
		close();
	}
}

/*---------------------------------------------------------------------*//**
	初期化

	@param szBasePath 既定パス
	※設定後はファイルオープン時のパスの指定に
	　この既定パスからの相対パスを指定すること
**//*---------------------------------------------------------------------*/
void File::init(const CStringBase* pathBase)
{
	const u32 SIZE_SBUF = 64;
	u8 sbuf[SIZE_SBUF];
	TmpBuf tmpbuf;

	u32 len = (u32)pathBase->getLength();
	char* buf = 0L;
	if(((len > 0) && pathBase->getAt(len - 1) != '/') && (pathBase->getAt(len - 1) != '\\'))
	{
		buf = (char*)tmpbuf.make(sizeof(char) * (len + 2), sbuf, SIZE_SBUF);
		::strcpy(buf, pathBase->getRaw());
		buf[len + 0] = '/';
		buf[len + 1] = '\0';
	}
	else
	{
		buf = (char*)tmpbuf.make(sizeof(char) * (len + 1), sbuf, SIZE_SBUF);
		::strcpy(buf, pathBase->getRaw());
	}
	_pathBase = new VcString(buf);
	
	TRACE("BasePath: %s\n", _pathBase->getRaw());
}

/*---------------------------------------------------------------------*//**
	解放処理
**//*---------------------------------------------------------------------*/
void File::cleanup()
{
	if(_pathBase != 0L)
	{
		delete _pathBase;
		_pathBase = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フルパスに変換する
**//*---------------------------------------------------------------------*/
void File::convFullPath(VcString* pathFull, const CStringBase* path)
{
	#if defined(_WINDOWS)
		if( ((path->getLength() >= 1) && ((path->getAt(0) == '\\') || (path->getAt(0) == '/'))) || ((path->getLength() >=  2) && (path->getAt(1) == ':')) )
		{
			// 絶対パスである
			pathFull->set(path);
		}
		else
		{
			// 相対パスである
			*pathFull = (_pathBase != 0L) ? (VcString(_pathBase) + path->getRaw()) : VcString(path);
		}
	#elif defined(_IOS) || defined(_ANDROID)
		if((path->getLength() >= 1) && ((path->getAt(0) == '\\') || (path->getAt(0) == '/')))
		{
			// 絶対パスである
			pathFull->set(path);
		}
		else
		{
			// 相対パスである
			*pathFull = (_pathBase != 0L) ? (VcString(_pathBase) + path->getRaw()) : VcString(path);
		}
	#endif
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
