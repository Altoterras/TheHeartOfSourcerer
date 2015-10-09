/***********************************************************************//**
 *	FileAndroid.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2013/01/03.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FILE_FILE_ANDROID_H_
#define _TFW_FILE_FILE_ANDROID_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "File.h"

// Library

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	Android 用ファイル クラス
 *
**//*---------------------------------------------------------------------*/
class FileAndroid : public File
{
	//======================================================================
	// 定数
public:
	static const u32	FAF_ASSET			= 0x01000000;
	static const u32	FAF_LARGE_ASSET		= 0x02000000;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 開いているかを得る
	virtual bool isOpened() const { return _file == 0L ? false : _file->isOpened(); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool open(const CStringBase* pathFile, Mode mode)	{	return _file->open(pathFile, mode);	}
	virtual void close()	{	_file->close();	}
	virtual u32 read(void* buf, u32 size)	{	return _file->read(buf, size);	}
	virtual u32 write(const void* buf, u32 size)	{	return _file->write(buf, size);	}
	virtual void seek(u32 offset, bool isTop)	{	_file->seek(offset, isTop);	}
	virtual u32 getPosition() const	{	return _file->getPosition();	}
	virtual void flush()	{	_file->flush();	}
	virtual u32 getLength() const	{	return _file->getLength();	}
	virtual bool isExist(const CStringBase* pathFile) const	{	return _file->isExist(pathFile);	}
	virtual void* getDescriptor()	{	return _file->getDescriptor();	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FileAndroid(u32 flags);
	FileAndroid(u32 flags, const CStringBase* pathFile, Mode mode);
	FileAndroid(u32 flags, CcString pathFile, Mode mode);
	virtual ~FileAndroid();

private:
	bool makeFileModule(u32 flags);

	//======================================================================
	// 変数
private:
	File* _file;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_FILE_FILE_ANDROID_H_
