/***********************************************************************//**
 *	FileBase.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/07/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FILE_FILE_BASE_H_
#define _TFW_FILE_FILE_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

class CStringBase;
class VcString;

/*---------------------------------------------------------------------*//**
	ファイル クラス

**//*---------------------------------------------------------------------*/
class FileBase
{
	//======================================================================
	// 定数

public:
	enum Mode
	{
		MODE_NULL = 0,
		MODE_READ = 1,
		MODE_WRITE,
		MODE_WRITE_ADD,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 開いているかを得る
	virtual bool isOpened() const = 0;
	// 読み込みモードかを得る
	virtual bool isReadMode() const = 0;
	// 書き込みモードかを得る
	virtual bool isWriteMode() const = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void close() = 0;
	virtual u32 read(void* buf, u32 size) = 0;
	virtual u32 readString(VcString* str, u32 size) = 0;
	virtual bool readTextLine(VcString* str) = 0;
	virtual u32 write(const void* buf, u32 size) = 0;
	virtual u32 writeString(const CStringBase* str) = 0;
	virtual void seek(u32 offset, bool isTop) = 0;
	virtual u32 getPosition() const = 0;
	virtual void flush() = 0;
	virtual u32 getLength() const = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	virtual ~FileBase() {}
};

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_FILE_FILE_BASE_H_
