/***********************************************************************//**
 *	File.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/06/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FILE_FILE_H_
#define _TFW_FILE_FILE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "FileBase.h"

// Friends
#include "../string/CcString.h"

////////////////////////////////////////////////////////////////////////////
// 外部構造体

#if defined(_WINDOWS)
	struct _iobuf;
	typedef struct _iobuf FILE;
#else
	typedef	struct __sFILE FILE;
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;

/*---------------------------------------------------------------------*//**
 *	ファイル クラス
 *
**//*---------------------------------------------------------------------*/
class File : public FileBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 開いているかを得る
	virtual bool isOpened() const { return _file != 0L; }
	// 読み込みモードかを得る
	virtual bool isReadMode() const { return (_mode == MODE_READ) || (_mode == MODE_WRITE_ADD); }
	// 書き込みモードかを得る
	virtual bool isWriteMode() const { return (_mode == MODE_WRITE) || (_mode == MODE_WRITE_ADD); }
	// ベースパスを得る
	static const VcString* getBasePath() { return _pathBase; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool open(const CStringBase* pathFile, Mode mode);
	inline bool open(CcString pathFile, Mode mode) { return open(&pathFile, mode); }
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
	virtual bool isExist(const CStringBase* pathFile) const;
	inline bool isExist(CcString pathFile) const { return isExist(&pathFile); }
	virtual void* getDescriptor();

	static u32 getFileLength(const CStringBase* pathFile);
	inline static u32 getFileLength(CcString pathFile) { return getFileLength(&pathFile); }
	static bool isFileExist(const CStringBase* pathFile);
	inline static bool isFileExist(CcString pathFile) { return isFileExist(&pathFile); }
	static bool isDirExist(const CStringBase* pathDir);
	inline static bool isDirExist(CcString pathDir) { return isDirExist(&pathDir); }
#if defined(_WINDOWS)
	static bool isDirExist(const TCHAR* pathDir);
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	static void init(const CStringBase* pathBase);
	inline static void init(CcString pathBase) { return init(&pathBase); }
	static void cleanup();

	File();
	File(const CStringBase* pathFile, Mode mode);
	File(CcString pathFile, Mode mode);
	virtual ~File();

protected:
	static void convFullPath(VcString* pathFull, const CStringBase* path);

	//======================================================================
	// 変数
protected:
	FILE* _file;
	Mode _mode;
	static VcString* _pathBase;	// UTF-8 か Shift-JIS のパス文字列
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_FILE_FILE_H_
