/***********************************************************************//**
 *	EsBcExInfo.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2010/02/25.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_BC_EX_INFO_H_
#define _ES_ES_BC_EX_INFO_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsProgramCounter;

/*---------------------------------------------------------------------*//**
 *	バイトコードに埋め込む拡張情報
 *
**//*---------------------------------------------------------------------*/
class EsBcExInfo
{
	//======================================================================
	// 定数
public:
	static const s32	LEN_BC			= 8;			// バイトコード上の長さ

	static const u32	MAX_CODE_SIZE	= 0x00ffffff;	// 最大コード長

private:
	static const u8		VERIFY_KEY		= 0xfc;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline u32	getBeginIndex() const	{	return _idxBegin;	}
	inline u32	getEndIndex() const		{	return _idxEnd;		}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	EsBcExInfo();
	EsBcExInfo(u32 idxBegin, u32 idxEnd);
	EsBcExInfo(EsProgramCounter* pc);
	EsBcExInfo(const EsBcExInfo& src);
	void clear();
	void copy(const EsBcExInfo* src);

	void toBytecode(u8 bc[8]);
	void fromBytecode(EsProgramCounter* pc);
	void fromBytecode(const u8 bc[8]);

private:
	void verifyRange();

	//======================================================================
	// 変数
private:
	u32		_idxBegin;	// 開始インデックス；～0x00ffffff（15MB未満）
	u32		_idxEnd;	// 終了インデックス；～0x00ffffff（15MB未満）
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_BC_EX_INFO_H_
