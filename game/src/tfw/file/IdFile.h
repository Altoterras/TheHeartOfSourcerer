/***********************************************************************//**
 *	IdFile.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/07/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FILE_ID_FILE_H_
#define _TFW_FILE_ID_FILE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "File.h"

////////////////////////////////////////////////////////////////////////////
// 型定義

#if !defined(TBF_PACK_DEV_MODE_LIST)
	typedef const char TBF_PACK_DEV_MODE_FILE_LIST;
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

struct TbfIncludeIdFile;
class CStringBase;
class VcString;
class PackedIdFile;

/*---------------------------------------------------------------------*//**
 *	ID ファイル クラス
 *		一つに結合されて ID 管理されているファイルの読み込み用クラス
 *		書き込みには対応していない
 *
**//*---------------------------------------------------------------------*/
class IdFile : public FileBase
{
	//======================================================================
	// 定数
public:
	static const u32	PIFF_STORED_ZIP			= 0x00000001;	// 無圧縮 ZIP を読み込む
	static const u32	PIFF_STORED_OBB			= 0x00000002;	// Android OBB を読み込む

	// プラットフォーム
	enum Platform
	{
		PF_COMMON = 0,
		PF_IOS = 1,
		PF_WINDOWS = 2,
		PF_ANDROID = 3,
		PF_FNDETECT = 99,
		PF_COMMENT = 777,
	};

	//======================================================================
	// クラス
private:
	class ReadBuffer
	{
	public:
		ReadBuffer() : _buf(0L), _size(0), _used(false) {}
		~ReadBuffer() { delete[] _buf; }
		bool prepare(u32 size) { return (_buf = new u8[_size = size]) != 0L; }

	public:
		u8* _buf;
		u32 _size;
		bool _used;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 開いているかを得る
	virtual bool isOpened() const;
	// 読み込みモードかを得る
	virtual bool isReadMode() const { return isOpened() & true; }
	// 書き込みモードかを得る
	virtual bool isWriteMode() const { return false; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static void init(const CStringBase* name, const CStringBase* postfix, s32 sizeUncompressBufferFirst, s32 sizeUncompressBufferSecond, u32 flags);
	static void enableDevMode(TBF_PACK_DEV_MODE_FILE_LIST** filedef, const CStringBase* dir);
	static void cleanup();
	static const PackedIdFile* getPackedIdFile() { return _pidfile; }

	IdFile();
	IdFile(u32 fileid);
	virtual ~IdFile();

	virtual bool open(u32 fileid);
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

	static u32 getIdFileLength(u32 fileid);
	static bool isIdFileExist(u32 fileid);

	//======================================================================
	// 変数
private:
	const TbfIncludeIdFile* _incfCur;
	u32 _posUncompress;		// 圧縮ファイルのシークポイント
	u32 _sizeUncompress;	// 圧縮ファイルの展開後のサイズ

	// 静的ファイル情報・ワーク
	static PackedIdFile* _pidfile;
	static ReadBuffer* _rbUncompress1;		// 圧縮ファイル展開用バッファ(1)
	static ReadBuffer* _rbUncompress2;		// 圧縮ファイル展開用バッファ(2)
	static VcString* _postfixPlatform;		// プラットフォーム区別のためのファイル名の後に付加する定型文字列

	// 開発モード
	static const char** _filedefDevMode;
	static VcString* _dirDevMode;
	File* _fileDevMode;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_FILE_ID_FILE_H_
