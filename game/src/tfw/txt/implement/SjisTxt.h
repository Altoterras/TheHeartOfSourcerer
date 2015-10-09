/***********************************************************************//**
 *  SjisTxt.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/11/27.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TEXT_IMPLEMENT_SJIS_TXT_H_
#define _TFW_STRING_TEXT_IMPLEMENT_SJIS_TXT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "AsciiTxt.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	Shift-JIS テキスト
 *
**//*---------------------------------------------------------------------*/
class SjisTxt : public AsciiTxt
{
	//======================================================================
	// 定数
private:
	static const s8	NUM_BYTES_MAX	= 2;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字エンコードを得る
	virtual TxtDef::Encode getEncode() const { return TxtDef::ENC_SJIS; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
#if 0
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const Txt* txt);
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const VcString* strRaw);
#endif
	virtual bool trimLeft(TxtPosArray* tpaSub, const TxtChar* tcTrim);
	virtual bool trimRight(TxtPosArray* tpaSub, const TxtChar* tcTrim);

	virtual Txt* newSameType() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	SjisTxt();
#if 0
	explicit SjisTxt(const VcString* strRaw);
	SjisTxt(VcString* strRaw, u8 flags);
#endif
	SjisTxt(const SjisTxt& src);

protected:
	virtual bool curChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const;
	virtual bool prevChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const;
	virtual u32 toCode(const char* arr, u16 numRaw) const;
	virtual char* toRaw(char* arr, u32 code) const;

public:
	// Shift-JIS 判定
	static inline bool isByteSjisFst(u8 c)		{	return isByteGe(c, 0x81) && ( isByteLe(c, 0x9f) || isByteGe(c, 0xe0) );	}
	static inline bool isByteSjisSnd(u8 c)		{	return isByteGe(c, 0x40) && isByteLe(c, 0xfc);							}
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_TEXT_IMPLEMENT_SJIS_TXT_H_
