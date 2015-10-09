/***********************************************************************//**
 *  Utf8Txt.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/12/06.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TEXT_IMPLEMENT_UTF8_TXT_H_
#define _TFW_STRING_TEXT_IMPLEMENT_UTF8_TXT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "AsciiTxt.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	UTF-8 テキスト
 *
**//*---------------------------------------------------------------------*/
class Utf8Txt : public AsciiTxt
{
	//======================================================================
	// 定数
private:
	static const s8	NUM_BYTES_MAX	= 6;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字エンコードを得る
	virtual TxtDef::Encode getEncode() const { return TxtDef::ENC_UTF8; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
#if 0
	virtual bool setAt(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc);
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc);
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const Txt* txt);
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const VcString* strRaw);
#endif
	virtual bool trimLeft(TxtPosArray* tpaSub, const TxtChar* tcTrim);
	virtual bool trimRight(TxtPosArray* tpaSub, const TxtChar* tcTrim);

	virtual Txt* newSameType() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Utf8Txt();
#if 0
	explicit Utf8Txt(const VcString* strRaw);
	Utf8Txt(VcString* strRaw, u8 flags);
#endif
	Utf8Txt(const Utf8Txt& src);

	// utf8 判定
	static inline bool isByteUtf8Fst(u8 c)		{	return isByteGe(c, 0xc0);	}
	static inline bool isByteUtf8Aft(u8 c)		{	return isByteGe(c, 0x80) && isByteLt(c, 0xc0);	}
	static inline bool isByteUtf8Bom(u8 c)		{	return isByteGe(c, 0xfe);	}
	static inline u16 getUtf8Bytes(u8 cFst)		{	return isByteGe(cFst, 0xfc) ? 6 : isByteGe(cFst, 0xf8) ? 5 : isByteGe(cFst, 0xf0) ? 4 : isByteGe(cFst, 0xe0) ? 3 : isByteGe(cFst, 0xf0) ? 2 : 1;	}

protected:
	virtual bool curChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const;
	virtual bool prevChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const;
	virtual u32 toCode(const char* arr, u16 numRaw) const;
#if 0
	virtual void fromCode(u8 cbuf[NUM_BYTES_MAX], u16* bytes, u32 code);
#endif
	virtual char* toRaw(char* arr, u32 code) const;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_TEXT_IMPLEMENT_UTF8_TXT_H_
