/***********************************************************************//**
 *	MemoryFile.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/07/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FILE_MEMORY_FILE_H_
#define _TFW_FILE_MEMORY_FILE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "FileBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class CStringBase;
class VcString;

/*---------------------------------------------------------------------*//**
 *	メモリ ファイル クラス
 *		あたかもファイルのようなインターフェイスを持つ
 *		バッファメモリ操作クラス
 *		オンメモリーのデータをファイル用に読むためのもの
 *
**//*---------------------------------------------------------------------*/
class MemoryFile : public FileBase
{
	//======================================================================
	// アクセサ

public:
	// 開いているかを得る
	virtual bool isOpened() const { return (_bufRead != 0L) || (_bufWrite != 0L); }
	// 読み込みモードかを得る
	virtual bool isReadMode() const { return (_bufRead != 0L); }
	// 書き込みモードかを得る
	virtual bool isWriteMode() const { return (_bufWrite != 0L); }

	//======================================================================
	// メソッド

public:
	MemoryFile();
	MemoryFile(const char* buf, u32 size);
	MemoryFile(char* buf, u32 size);
	virtual ~MemoryFile();

	virtual bool open(const char* buf, u32 size);
	virtual bool open(char* buf, u32 size);
	virtual void close();
	virtual u32 read(void* buf, u32 size);
	virtual u32 readString(VcString* str, u32 size);
	virtual bool readTextLine(VcString* str);
	virtual u32 write(const void* buf, u32 size);
	virtual u32 writeString(const CStringBase* str);
	virtual void seek(u32 offset, bool isTop);
	virtual u32 getPosition() const;
	virtual void flush();
	virtual u32 getLength() const;

	//======================================================================
	// メンバ変数

private:
	const char* _bufRead;
	char* _bufWrite;
	u32 _sizeBuf;
	u32 _idxPos;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_FILE_MEMORY_FILE_H_
