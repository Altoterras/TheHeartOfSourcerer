/***********************************************************************//**
 *  UnicodeTxt.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2012/12/06.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TEXT_IMPLEMENT_UNICODE_TXT_H_
#define _TFW_STRING_TEXT_IMPLEMENT_UNICODE_TXT_H_

#if 1
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "RawTxt.h"
//#include "RawU16TxtBase.h"

#include "../../string/VwString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS
	
#define CLASS_BASE	RawTxt<VwString, u16>
#define CLASS_SUPER	RawTxt
//#define CLASS_BASE	RawU16TxtBase
//#define CLASS_SUPER	RawU16TxtBase

/*---------------------------------------------------------------------*//**
 *	Unicode テキスト
 *	※	固定 16 ビットの Unicode．UTF-16 ではない
 *
**//*---------------------------------------------------------------------*/
class UnicodeTxt : public CLASS_BASE
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字エンコードを得る
	virtual TxtDef::Encode getEncode() const { return TxtDef::ENC_UNICODE; }
	// 生のワイド文字列へのポインタを得る
	virtual const VwString* getRawWideString() const { return _strRaw; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void takeRawString(VcString* str) const;
	virtual void takeRawWideString(VwString* str) const;
	virtual void setSingleString(const VcString* strRaw);
	virtual void setWideString(const VwString* strRaw);
	virtual Txt* newSameType() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	UnicodeTxt();
#if 0
	explicit UnicodeTxt(const VwString* strRaw);
	UnicodeTxt(VwString* strRaw, u8 flags);
#endif
	UnicodeTxt(const UnicodeTxt& src);

protected:
	virtual bool curChar(CharPicker* cp, const u16* cstr, u32 lenStr, u32 indexRaw) const;
	virtual bool prevChar(CharPicker* cp, const u16* cstr, u32 lenStr, u32 indexRaw) const;
#if 0
	virtual u32 countChar() const;
	virtual u32 countChar(const u16* cstr, u32 lenStr) const;
#endif
	virtual u32 toCode(const u16* arr, u16 numRaw) const;
	virtual u16* toRaw(u16* arr, u32 code) const;
};

#undef CLASS_BASE

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#else
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Txt.h"
#include "../TxtChar.h"

#include "../../string/VwString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	Unicode テキスト
 *	※	固定 16 ビットの Unicode．UTF-16 ではない
 *
**//*---------------------------------------------------------------------*/
class UnicodeTxt : public Txt
{
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
	virtual TxtDef::Encode getEncode() const { return TxtDef::ENC_UNICODE; }
	// 文字の数を得る
	virtual u32 getCharCount() const { return _countChar; }
	// 生の文字列へのポインタを得る
	virtual const VwString* getRawWideString() const { return _strRaw; }

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
	void setText(const VwString* strRaw);
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	UnicodeTxt();
	explicit UnicodeTxt(const VcString* strRaw);
	UnicodeTxt(const UnicodeTxt& src);
	virtual ~UnicodeTxt();
	virtual void copy(const Txt* src);

protected:
	virtual bool curChar(CharPicker* cp, u32 indexRaw) const;
	virtual bool prevChar(CharPicker* cp, u32 indexRaw) const;
	virtual bool curChar(CharPicker* cp, const u16* cstr, u32 lenStr, u32 indexRaw) const;
	virtual bool prevChar(CharPicker* cp, const u16* cstr, u32 lenStr, u32 indexRaw) const;
	virtual u32 countChar() const;
	virtual u32 countChar(const u16* cstr, u32 lenStr) const;
	virtual u32 toCode(u32 indexRaw, u16 numRaw) const;

	//======================================================================
	// 変数
protected:
	VwString* _strRaw;
	u32 _countChar;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
#endif

#endif	// _TFW_STRING_TEXT_IMPLEMENT_UNICODE_TXT_H_
