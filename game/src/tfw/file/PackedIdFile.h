/***********************************************************************//**
 *	PackedIdFile.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/07/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_FILE_PACKED_ID_FILE_H_
#define _TFW_FILE_PACKED_ID_FILE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../string/CcString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

struct TbfIncludeIdFile;
template<class TYPE> class List;
class CStringBase;
class VcString;
class File;

/*---------------------------------------------------------------------*//**
 *	ID ファイル クラス
 *		一つに結合されて ID 管理されているファイルの読み込み用クラス
 *		書き込みには対応していない
 *
**//*---------------------------------------------------------------------*/
class PackedIdFile
{
	//======================================================================
	// 定数
public:
	static const u32	PIFF_STORED_ZIP			= 0x00000001;	// 無圧縮 ZIP を読み込む

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// リストを得る
	inline const List<TbfIncludeIdFile*>* getList() const { return _listIncf; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	PackedIdFile();
	~PackedIdFile();
	bool create(const CStringBase* name, u32 flags);
	inline bool create(CcString name, u32 flags) { return create(&name, flags); }
	void destroy();

	const TbfIncludeIdFile*	findFileInfo(u32 fileid) const;
	u32 read(const TbfIncludeIdFile* incf, void* buf, u32 size);
	void seek(const TbfIncludeIdFile* incf, u32 offset, bool isTop);
	u32 getPosition(const TbfIncludeIdFile* incf) const;

	//======================================================================
	// 変数
private:
	File* _file;
	List<TbfIncludeIdFile*>* _listIncf;
	u32 _offsetHead;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_FILE_PACKED_ID_FILE_H_
