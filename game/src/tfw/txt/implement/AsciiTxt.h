/***********************************************************************//**
 *  AsciiTxt.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/11/27.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TEXT_IMPLEMENT_ASCII_TXT_H_
#define _TFW_STRING_TEXT_IMPLEMENT_ASCII_TXT_H_

#if 1
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "RawTxt.h"
//#include "RawS8TxtBase.h"

#include "../../string/VcString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

#define CLASS_BASE	RawTxt<VcString, char>
#define CLASS_SUPER	RawTxt
//#define CLASS_BASE	RawS8TxtBase
//#define CLASS_SUPER	RawS8TxtBase

/*---------------------------------------------------------------------*//**
 *	ASCII テキスト
 *
**//*---------------------------------------------------------------------*/
class AsciiTxt : public CLASS_BASE
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字エンコードを得る
	virtual TxtDef::Encode getEncode() const { return TxtDef::ENC_ASCII; }
	// 生の文字列へのポインタを得る
	virtual const VcString* getRawString() const { return _strRaw; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void takeRawString(VcString* str) const;
	virtual void takeRawWideString(VwString* str) const;
	virtual void setSingleString(const VcString* strRaw);
	virtual void setWideString(const VwString* strRaw);
	virtual Txt* newSameType() const;

protected:
	// 汎用比較関数
	static inline bool isByteGe(u8 c, u8 n)	{	return c >= n;	}
	static inline bool isByteGt(u8 c, u8 n)	{	return c > n;	}
	static inline bool isByteLe(u8 c, u8 n)	{	return c <= n;	}
	static inline bool isByteLt(u8 c, u8 n)	{	return c < n;	}
	static inline bool isByteEq(u8 c, u8 n)	{	return c == n;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	AsciiTxt();
#if 0
	explicit AsciiTxt(const VcString* strRaw);
	AsciiTxt(VcString* strRaw, u8 flags);
#endif
protected:
	explicit AsciiTxt(s32 numCharRawMax);
#if 0
	AsciiTxt(const VcString* strRaw, s8 numCharRawMax);
	AsciiTxt(VcString* strRaw, u8 flags, s8 numCharRawMax);
#endif
public:
	AsciiTxt(const AsciiTxt& src);

protected:
	virtual bool curChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const;
	virtual bool prevChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const;
#if 0
	virtual u32 countChar() const;
	virtual u32 countChar(const char* cstr, u32 lenStr) const;
#endif
	virtual u32 toCode(const char* arr, u16 numRaw) const;
	virtual char* toRaw(char* arr, u32 code) const;
};

#undef CLASS_BASE

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#else
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Txt.h"
#include "../TxtChar.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ASCII テキスト
 *
**//*---------------------------------------------------------------------*/
class AsciiTxt : public Txt
{
	//======================================================================
	// 定数
private:
	static const u32	NUM_BYTES_MAX	= 2;

	//======================================================================
	// クラス
protected:
	class CharPicker
	{
	public:
		u32 _indexRaw;
		u16 _numRaw;
	public:
		CharPicker();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字エンコードを得る
	virtual TxtDef::Encode getEncode() const { return TxtDef::ENC_ASCII; }
	// 文字の数を得る
	virtual u32 getCharCount() const { return _countChar; }
	// 生の文字列へのポインタを得る
	virtual const VcString* getRawString() const { return _strRaw; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void takeRawString(VcString* str) const;
	virtual void takeRawWideString(VwString* str) const;

	virtual void getHearPos(TxtPos* pos) const;
	virtual void getTailPos(TxtPos* pos) const;
	virtual TxtIterator iterator() const;
	virtual TxtIterator iteratorEnd() const;
	virtual bool nextPos(TxtPos* pos, TxtChar* tcBtwn) const;
	virtual bool prevPos(TxtPos* pos, TxtChar* tcBtwn) const;
	virtual bool isValidCharPos(const TxtPos* pos) const;

	void setText(const Txt* txt);
	void setText(const VcString* strRaw);
	virtual bool getAt(TxtChar* tc, const TxtPos* pos) const;
	virtual bool setAt(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc);
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc);
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const Txt* txt);
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const VcString* strRaw);
	virtual bool remove(TxtPosArray* tpaSub, const TxtPos* pos, u32 countChar);
	virtual bool trimLeft(TxtPosArray* tpaSub, const TxtChar* tcTrim);
	virtual bool trimRight(TxtPosArray* tpaSub, const TxtChar* tcTrim);
	virtual bool subtext(Txt** txtMade, const TxtPos* posBegin, const TxtPos* posEnd) const;
	virtual bool subtext(Txt** txtMade, const TxtPos* posBegin) const;

	virtual Txt* newSameType() const;

protected:
	virtual bool checkNewLineCodeCur(u32* code, u16* numr, u32 indexRaw) const;
	virtual bool checkNewLineCodePrev(u32* code, u16* numr, u32* indexRawOut, u32 indexRawIn) const;
	void movePosByInsert(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRaw, u32 numChar) const;
	void movePosByRemove(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRaw, u32 numChar) const;
	void movePosByChange(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRawOld, u32 numCharOld, u32 numRawNew, u32 numCharNew) const;
	virtual void movePosByInsert(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRaw, u32 numChar) const;
	virtual void movePosByRemove(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRaw, u32 numChar) const;
	virtual void movePosByChange(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRawOld, u32 numCharOld, u32 numRawNew, u32 numCharNew) const;

	// 汎用比較関数
	static inline bool isByteGe(u8 c, u8 n)	{	return c >= n;	}
	static inline bool isByteGt(u8 c, u8 n)	{	return c > n;	}
	static inline bool isByteLe(u8 c, u8 n)	{	return c <= n;	}
	static inline bool isByteLt(u8 c, u8 n)	{	return c < n;	}
	static inline bool isByteEq(u8 c, u8 n)	{	return c == n;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	AsciiTxt();
	explicit AsciiTxt(const VcString* strRaw);
	AsciiTxt(const AsciiTxt& src);
	virtual ~AsciiTxt();
	virtual void copy(const Txt* src);

protected:
	virtual bool curChar(CharPicker* cp, u32 indexRaw) const;
	virtual bool prevChar(CharPicker* cp, u32 indexRaw) const;
	virtual bool curChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const;
	virtual bool prevChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const;
	virtual u32 countChar() const;
	virtual u32 countChar(const char* cstr, u32 lenStr) const;
	virtual u32 toCode(u32 indexRaw, u16 numRaw) const;

	//======================================================================
	// 変数
protected:
	VcString* _strRaw;
	u32 _countChar;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
#endif

#endif	// _TFW_STRING_TEXT_IMPLEMENT_ASCII_TXT_H_
