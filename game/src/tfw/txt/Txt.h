/***********************************************************************//**
 *  Txt.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/11/27.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TEXT_TXT_H_
#define _TFW_STRING_TEXT_TXT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "TxtDef.h"
#include "TxtIterator.h"
#include "TxtPos.h"
#include "TxtPosArray.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class TxtChar;
class VwString;

/*---------------------------------------------------------------------*//**
 *	テキスト基本クラス
 *		このクラスは ASCII テキストのみを扱える
 *
**//*---------------------------------------------------------------------*/
class Txt
{
	//======================================================================
	// 定数
public:
	// 改行コード
	static const u32	NLCODE_CR	= 0x0d;
	static const u32	NLCODE_LF	= 0x0a;
	static const u32	NLCODE_CRLF	= 0x10000d0a;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字エンコードを得る
	virtual TxtDef::Encode getEncode() const { return TxtDef::ENC_NULL; }
	// 文字の数を得る
	virtual u32 getCharCount() const = 0;
	// 生文字数を得る
	virtual u32 getRawCount() const = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void takeRawString(VcString* str) const = 0;
	virtual void takeRawWideString(VwString* str) const = 0;

	// 位置
	virtual void getHearPos(TxtPos* pos) const = 0;
	virtual void getTailPos(TxtPos* pos) const = 0;
	virtual TxtIterator iterator() const = 0;
	virtual TxtIterator iteratorEnd() const = 0;
	virtual bool nextPos(TxtPos* pos, TxtChar* tcBtwn) const = 0;
	virtual bool nextPos(TxtPos* pos) const { return nextPos(pos, 0L); }
	virtual bool prevPos(TxtPos* pos, TxtChar* tcBtwn) const = 0;
	virtual bool prevPos(TxtPos* pos) const { return prevPos(pos, 0L); }
	virtual bool isValidCharPos(const TxtPos* pos) const = 0;

	// 操作
	virtual void setText(const Txt* txt) = 0;
	virtual void setSingleString(const VcString* strRaw) = 0;
	virtual void setWideString(const VwString* strRaw) = 0;
	virtual bool getAt(TxtChar* tc, const TxtPos* pos) const = 0;
	virtual bool setAt(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc) = 0;
	virtual bool setAt(const TxtPos* pos, const TxtChar* tc) { return setAt(0L, pos, tc); }
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc) = 0;
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const Txt* txt) = 0;
	virtual bool insert(const TxtPos* pos, const TxtChar* tc) { return insert(0L, pos, tc); }
	virtual bool insert(const TxtPos* pos, const Txt* txt) { return insert(0L, pos, txt); }
	virtual bool remove(TxtPosArray* tpaSub, const TxtPos* pos) { return remove(tpaSub, pos, 1); }
	virtual bool remove(TxtPosArray* tpaSub, const TxtPos* pos, u32 numChar) = 0;
	virtual bool remove(const TxtPos* pos) { return remove(0L, pos, 1); }
	virtual bool remove(const TxtPos* pos, u32 numChar) { return remove(0L, pos, numChar); }
	virtual void removeAll();
	virtual bool trimLeft(TxtPosArray* tpaSub, const TxtChar* tcTrim) = 0;
	virtual bool trimLeft(TxtPosArray* tpaSub) { return trimLeft(tpaSub, 0L); }
	virtual bool trimRight(TxtPosArray* tpaSub, const TxtChar* tcTrim) = 0;
	virtual bool trimRight(TxtPosArray* tpaSub) { return trimRight(tpaSub, 0L); }
	virtual bool trim(TxtPosArray* tpaSub, const TxtChar* tcTrim);
	virtual bool trim(TxtPosArray* tpaSub) { return trim(tpaSub, 0L); }
	virtual bool trimLeft(const TxtChar* tcTrim) { return trimLeft(0L, tcTrim); }
	virtual bool trimLeft() { return trimLeft(0L, 0L); }
	virtual bool trimRight(const TxtChar* tcTrim) { return trimRight(0L, tcTrim); }
	virtual bool trimRight() { return trimRight(0L, 0L); }
	virtual bool trim(const TxtChar* tcTrim) { return trim(0L, tcTrim); }
	virtual bool trim() { return trim(0L, 0L); }
	virtual bool subtext(Txt** txtMade, const TxtPos* posBegin, const TxtPos* posEnd) const = 0;
	virtual bool subtext(Txt** txtMade, const TxtPos* posBegin) const = 0;

	virtual Txt* newSameType() const = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	virtual ~Txt();
	virtual void copy(const Txt* src);
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_TEXT_TXT_H_
