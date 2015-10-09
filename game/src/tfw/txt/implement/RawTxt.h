/***********************************************************************//**
 *  RawTxt.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2012/12/06.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TEXT_IMPLEMENT_RAW_TXT_H_
#define _TFW_STRING_TEXT_IMPLEMENT_RAW_TXT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Txt.h"
#include "../TxtChar.h"

#include "../../lib/TmpBuf.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;
class VwString;

/*---------------------------------------------------------------------*//**
 *	生文字保持テキスト
 *
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
class RawTxt : public Txt
{
	//======================================================================
	// 定数
public:
	static const u8		F_REF_STRING	= 0x01;	// 外部の文字列を参照する（内部で作成・削除しない）

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
	// 文字の数を得る
	virtual u32 getCharCount() const { return _countChar; }
	// 生文字数を得る
	virtual u32 getRawCount() const { return _strRaw->getLength(); };

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// 位置
	virtual void getHearPos(TxtPos* pos) const;
	virtual void getTailPos(TxtPos* pos) const;
	virtual TxtIterator iterator() const;
	virtual TxtIterator iteratorEnd() const;
	virtual bool nextPos(TxtPos* pos, TxtChar* tcBtwn) const;
	virtual bool prevPos(TxtPos* pos, TxtChar* tcBtwn) const;
	virtual bool isValidCharPos(const TxtPos* pos) const;

	// 操作
	virtual void setText(const Txt* txt);
#if 0
	virtual void setText(const TYPE_CLASS* strRaw);
#endif
	virtual void setString(TYPE_CLASS* strRaw, u8 flags);
	virtual bool getAt(TxtChar* tc, const TxtPos* pos) const;
	virtual bool setAt(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc);
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc);
	virtual bool insert(TxtPosArray* tpaSub, const TxtPos* pos, const Txt* txt);
	virtual bool remove(TxtPosArray* tpaSub, const TxtPos* pos, u32 countChar);
	virtual bool trimLeft(TxtPosArray* tpaSub, const TxtChar* tcTrim);
	virtual bool trimRight(TxtPosArray* tpaSub, const TxtChar* tcTrim);
	virtual bool subtext(Txt** txtMade, const TxtPos* posBegin, const TxtPos* posEnd) const;
	virtual bool subtext(Txt** txtMade, const TxtPos* posBegin) const;

	// 継承
	virtual bool nextPos(TxtPos* pos) const { return nextPos(pos, 0L); }
	virtual bool prevPos(TxtPos* pos) const { return prevPos(pos, 0L); }
	virtual bool setAt(const TxtPos* pos, const TxtChar* tc) { return setAt(0L, pos, tc); }
	virtual bool insert(const TxtPos* pos, const TxtChar* tc) { return insert(0L, pos, tc); }
	virtual bool insert(const TxtPos* pos, const Txt* txt) { return insert(0L, pos, txt); }
	virtual bool remove(TxtPosArray* tpaSub, const TxtPos* pos) { return remove(tpaSub, pos, 1); }
	virtual bool remove(const TxtPos* pos) { return remove(0L, pos, 1); }
	virtual bool remove(const TxtPos* pos, u32 numChar) { return remove(0L, pos, numChar); }
	virtual bool trimLeft(TxtPosArray* tpaSub) { return trimLeft(tpaSub, 0L); }
	virtual bool trimRight(TxtPosArray* tpaSub) { return trimRight(tpaSub, 0L); }
	virtual bool trim(TxtPosArray* tpaSub, const TxtChar* tcTrim) { return trim(tpaSub, tcTrim); }
	virtual bool trim(TxtPosArray* tpaSub) { return trim(tpaSub, 0L); }
	virtual bool trimLeft(const TxtChar* tcTrim) { return trimLeft(0L, tcTrim); }
	virtual bool trimLeft() { return trimLeft(0L, 0L); }
	virtual bool trimRight(const TxtChar* tcTrim) { return trimRight(0L, tcTrim); }
	virtual bool trimRight() { return trimRight(0L, 0L); }
	virtual bool trim(const TxtChar* tcTrim) { return trim(0L, tcTrim); }
	virtual bool trim() { return trim(0L, 0L); }

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
protected:
	RawTxt(s8 numCharRaw, s8 numCharRawMax);
	RawTxt(const RawTxt& src);
	virtual ~RawTxt();
public:
	virtual void copy(const Txt* src);

protected:
	bool curChar(CharPicker* cp, u32 indexRaw) const;
	bool prevChar(CharPicker* cp, u32 indexRaw) const;
	virtual bool curChar(CharPicker* cp, const TYPE_BASE* cstr, u32 lenStr, u32 indexRaw) const = 0;
	virtual bool prevChar(CharPicker* cp, const TYPE_BASE* cstr, u32 lenStr, u32 indexRaw) const = 0;
	u32 countChar() const;
	u32 countChar(const TYPE_BASE* cstr, u32 lenStr) const;
	virtual u32 toCode(const TYPE_BASE* arr, u16 numRaw) const = 0;
	virtual TYPE_BASE* toRaw(TYPE_BASE* arr, u32 code) const = 0;

	//======================================================================
	// 変数
protected:
	TYPE_CLASS* _strRaw;
	u32 _countChar;
	u8 _flags;

	const s8 _numCharRaw;
	const s8 _numCharRawMax;
};

//==========================================================================
// RawTxt メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	先頭位置を得る
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::getHearPos(TxtPos* pos) const
{
	pos->set(0, 0);
}

/*---------------------------------------------------------------------*//**
	終端位置を得る
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::getTailPos(TxtPos* pos) const
{
	pos->set((u32)_strRaw->getLength(), getCharCount());
}

/*---------------------------------------------------------------------*//**
	先頭イテレータを得る
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
TxtIterator RawTxt<TYPE_CLASS, TYPE_BASE>::iterator() const
{
	return TxtIterator(this, 0, 0);
}

/*---------------------------------------------------------------------*//**
	先頭イテレータを得る
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
TxtIterator RawTxt<TYPE_CLASS, TYPE_BASE>::iteratorEnd() const
{
	TxtIterator it(this, (u32)_strRaw->getLength(), getCharCount());
	prevPos(&it, 0L);
	return it;
}

/*---------------------------------------------------------------------*//**
	位置を進める
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::nextPos(TxtPos* pos, TxtChar* tcBtwn) const
{
	CharPicker cp;
	if(!curChar(&cp, pos->getRawIndex()))
	{
		return false;
	}
	pos->set(cp._indexRaw + cp._numRaw, pos->getCharIndex() + 1);
	if(tcBtwn != 0L)
	{
		tcBtwn->set(toCode(_strRaw->getRaw(cp._indexRaw), cp._numRaw), cp._numRaw * _numCharRaw);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	位置を戻す
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::prevPos(TxtPos* pos, TxtChar* tcBtwn) const
{
	CharPicker cp;
	if(!prevChar(&cp, pos->getRawIndex()))
	{
		return false;
	}
	pos->set(cp._indexRaw, pos->getCharIndex() - 1);
	if(tcBtwn != 0L)
	{
		tcBtwn->set(toCode(_strRaw->getRaw(cp._indexRaw), cp._numRaw), cp._numRaw * _numCharRaw);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	文字のある位置かを調べる
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::isValidCharPos(const TxtPos* pos) const
{
	s32 indexRaw = pos->getRawIndex();
	return (0 <= indexRaw) && (indexRaw < _strRaw->getLength());
}

/*---------------------------------------------------------------------*//**
	テキストを設定する
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::setText(const Txt* txt)
{
	copy(txt);
}

#if 0
/*---------------------------------------------------------------------*//**
	テキストを設定する
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::setText(const TYPE_CLASS* strRaw)
{
	_strRaw->set(strRaw);
	_countChar = countChar();
}
#endif

/*---------------------------------------------------------------------*//**
	生文字列を直接設定する
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::setString(TYPE_CLASS* strRaw, u8 flags)
{
	// フラグ設定
	u8 flagsOld = _flags;
	_flags = flags;

	if(TFW_IS_FLAG(_flags, F_REF_STRING))
	{
		// 現在の内部保持文字列は削除
		if(!TFW_IS_FLAG(flagsOld, F_REF_STRING))
		{
			delete _strRaw;
			_strRaw = 0L;
		}

		// 外部の文字列を参照する
		_strRaw = strRaw;
		_countChar = countChar();
	}
	else
	{
		// コピー
		_strRaw->set(strRaw);
		_countChar = countChar();
	}
}

/*---------------------------------------------------------------------*//**
	一文字取得する
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::getAt(TxtChar* tc, const TxtPos* pos) const
{
	CharPicker cp;
	if(!curChar(&cp, pos->getRawIndex()))
	{
		return false;
	}
	tc->set(toCode(_strRaw->getRaw(cp._indexRaw), cp._numRaw), cp._numRaw * _numCharRaw);
	return true;
}

/*---------------------------------------------------------------------*//**
	一文字設定する
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::setAt(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc)
{
	CharPicker cp;
	curChar(&cp, pos->getRawIndex());

	u16 bytesSrc = tc->getBytes();
	if(bytesSrc == (sizeof(TYPE_BASE) * cp._numRaw))
	{
		const u32 SIZE_SBUF = 16;
		u8 sbuf[SIZE_SBUF];
		TmpBuf tmpbuf(sizeof(TYPE_BASE) * (cp._numRaw + 1), sbuf, SIZE_SBUF);
		TYPE_BASE* cbuf = (TYPE_BASE*)tmpbuf.get();
		TYPE_BASE* cwk = toRaw(cbuf, tc->getCode());
		*cwk = '\0';
		ASSERT((cwk - cbuf) == cp._numRaw);

		for(u32 i = 0; i < cp._numRaw; i++)
		{
			if(!_strRaw->setAt(pos->getRawIndex() + i, cbuf[i]))
			{
				return false;
			}
		}
		movePosByChange(tpaSub, pos, cp._numRaw, 1, bytesSrc, 1);
		return true;
	}
	else
	{
		if((pos->getRawIndex() < (u32)_strRaw->getLength()) && !remove(tpaSub, pos, 1))
		{
			return false;
		}
		return insert(tpaSub, pos, tc);
	}
}

/*---------------------------------------------------------------------*//**
	一文字挿入する
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::insert(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc)
{
	const u32 SIZE_SBUF = 16;
	u8 sbuf[SIZE_SBUF];
	TmpBuf tmpbuf(sizeof(TYPE_BASE) * (_numCharRawMax + 1), sbuf, SIZE_SBUF);

	TYPE_BASE* cwk = toRaw((TYPE_BASE*)tmpbuf.get(), tc->getCode());
	*cwk = '\0';

	if(!_strRaw->insert(pos->getRawIndex(), (TYPE_BASE*)tmpbuf.get()))
	{
		return false;
	}
	_countChar++;

	movePosByInsert(tpaSub, pos, tc->getBytes(), 1);

	return true;
}

/*---------------------------------------------------------------------*//**
	テキストを挿入する
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::insert(TxtPosArray* tpaSub, const TxtPos* pos, const Txt* txt)
{
	const u32 SIZE_SBUF = 16;
	u8 sbuf[SIZE_SBUF];
	TmpBuf tmpbuf(sizeof(TYPE_BASE) * ((txt->getCharCount() * _numCharRawMax) + 1), sbuf, SIZE_SBUF);

	TYPE_BASE* cwk = (TYPE_BASE*)tmpbuf.get();
	s32 i = 0;
	s32 cntChar = 0;
	for(TxtIterator it = txt->iterator(); it.has(); it.next())
	{
		TxtChar tc = it.cur();
		cwk = toRaw(cwk, tc.getCode());
		cntChar++;
	}
	*cwk = '\0';

	if(!_strRaw->insert(pos->getRawIndex(), (TYPE_BASE*)tmpbuf.get()))
	{
		return false;
	}
	_countChar += cntChar;

	movePosByInsert(tpaSub, pos, (u32)(cwk - (TYPE_BASE*)tmpbuf.get()), cntChar);

	return true;
}

/*---------------------------------------------------------------------*//**
	文字を削除する
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::remove(TxtPosArray* tpaSub, const TxtPos* pos, u32 numChar)
{
	u32 cnt = numChar;
	u32 countRaw = 0;
	while(cnt > 0)
	{
		CharPicker cp;
		if(!curChar(&cp, pos->getRawIndex()))
		{
			return false;
		}
		if(!_strRaw->remove(cp._indexRaw, cp._numRaw))
		{
			return false;
		}
		ASSERT(_countChar > 0);
		_countChar--;
		cnt--;
		countRaw += cp._numRaw;
	}

	movePosByRemove(tpaSub, pos, countRaw, numChar);
	return true;
}

/*---------------------------------------------------------------------*//**
	左端空白削除
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::trimLeft(TxtPosArray* tpaSub, const TxtChar* tcTrim)
{
	TxtIterator itStart = iterator();
	TxtIterator itEnd(this);
	if(tcTrim != 0L)
	{
		TxtIterator it(itStart);
		while(it.has())
		{
			if(it.cur().getCode() != tcTrim->getCode())
			{
				break;
			}
			it.next();
		}
	}
	else
	{
		TxtIterator it(itStart);
		while(it.has())
		{
			u32 code = it.cur().getCode();
			if((code != ' ') && (code != '\t') && (code != '\r') && (code != '\n'))
			{
				break;
			}
			it.next();
		}
	}
	bool isTrim = false;
	if(!itEnd.isInvalid())
	{
		u32 count = itEnd.getRawIndex() - itStart.getRawIndex();
		if((count > 0) && _strRaw->remove(itStart.getRawIndex(), count))
		{
			isTrim = true;
			movePosByRemove(tpaSub, &itStart, itEnd.getRawIndex() - itStart.getRawIndex(), itEnd.getCharIndex() - itStart.getCharIndex());
		}
	}
	return isTrim;
}

/*---------------------------------------------------------------------*//**
	右端空白削除
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::trimRight(TxtPosArray* tpaSub, const TxtChar* tcTrim)
{
	TxtIterator itStart(this);
	TxtIterator itEnd = iteratorEnd();
	if(tcTrim != 0L)
	{
		TxtIterator it(itEnd);
		while(it.has())
		{
			if(it.cur().getCode() != tcTrim->getCode())
			{
				break;
			}
			itStart.copy(&it);
			it.prev();
		}
	}
	else
	{
		TxtIterator it(itEnd);
		while(it.has())
		{
			u32 code = it.cur().getCode();
			if((code != ' ') && (code != '\t') && (code != '\r') && (code != '\n'))
			{
				break;
			}
			itStart.copy(&it);
			it.prev();
		}
	}
	bool isTrim = false;
	if(!itStart.isInvalid())
	{
		u32 count = itEnd.getRawIndex() - itStart.getRawIndex();
		if((count > 0) && _strRaw->remove(itStart.getRawIndex(), count))
		{
			isTrim = true;
			movePosByRemove(tpaSub, &itStart, itEnd.getRawIndex() - itStart.getRawIndex(), itEnd.getCharIndex() - itStart.getCharIndex());
		}
	}
	return isTrim;
}

/*---------------------------------------------------------------------*//**
	部分テキストを作成する
	※ posEnd 終了位置は、この位置を含まない
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::subtext(Txt** txtMade, const TxtPos* posBegin, const TxtPos* posEnd) const
{
	Txt* txtNew = newSameType();
	TxtPos posSrc(*posBegin);
	TxtPos posDst;
	txtNew->getHearPos(&posDst);
	while(isValidCharPos(&posSrc) && posEnd->gt(&posSrc))
	{
		TxtChar tc;
		if(!this->getAt(&tc, &posSrc))
		{
			return false;
		}
		if(!txtNew->insert(&posDst, &tc))
		{
			return false;
		}
		if(!this->nextPos(&posSrc, 0L))
		{
			return false;
		}
		if(!txtNew->nextPos(&posDst))
		{
			return false;
		}
	}
	*txtMade = txtNew;
	return true;
}

/*---------------------------------------------------------------------*//**
	部分テキストを作成する
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::subtext(Txt** txtMade, const TxtPos* posBegin) const
{
	TxtPos tpos((u32)_strRaw->getLength(), getCharCount());
	return subtext(txtMade, posBegin, &tpos);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
RawTxt<TYPE_CLASS, TYPE_BASE>::RawTxt(s8 numCharRaw, s8 numCharRawMax)
	: _strRaw(new TYPE_CLASS())
	, _countChar(0)
	, _flags(0)
	, _numCharRaw(numCharRaw)
	, _numCharRawMax(numCharRawMax)
{
}

/*---------------------------------------------------------------------*//**
	コピー コンストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
RawTxt<TYPE_CLASS, TYPE_BASE>::RawTxt(const RawTxt& src)
	: _strRaw(new TYPE_CLASS())
	, _countChar(0)
	, _flags(0)
	, _numCharRaw(src._numCharRaw)
	, _numCharRawMax(src._numCharRawMax)
{
	copy(&src);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
RawTxt<TYPE_CLASS, TYPE_BASE>::~RawTxt()
{
	// 内部保持文字列を削除
	if(!TFW_IS_FLAG(_flags, F_REF_STRING))
	{
		delete _strRaw;
	}
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::copy(const Txt* src)
{
	if(this->getEncode() == src->getEncode())	// 同エンコードの場合のみ
	{
		if(TFW_IS_FLAG(((RawTxt*)src)->_flags, F_REF_STRING))
		{
			_strRaw = ((RawTxt*)src)->_strRaw;
			_countChar = ((RawTxt*)src)->_countChar;
		}
		else
		{
			_strRaw->copy(((RawTxt*)src)->_strRaw);
			_countChar = ((RawTxt*)src)->_countChar;
		}
		return;
	}

	Txt::copy(src);
}

/*---------------------------------------------------------------------*//**
	現在の位置の改行コードチェック
	※基本実装では、CR, CRLF, LF のどの組み合わせも認識する．
	　この実装を変更したい場合はこのメソッドをオーバーライドすること．
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::checkNewLineCodeCur(u32* code, u16* numr, u32 indexRaw) const
{
	if((u32)_strRaw->getLength() <= indexRaw)
	{
		if(code != 0L)	{	*code = 0;				}
		if(numr != 0L)	{	*numr = 0;				}
		return false;
	}
	u16 c = _strRaw->getAt(indexRaw);
	if(c == '\n')
	{
		if(code != 0L)	{	*code = NLCODE_LF;		}	// LF '\n'
		if(numr != 0L)	{	*numr = 1;				}
		return true;
	}
	if(c != '\r')
	{
		if(code != 0L)	{	*code = 0;				}
		if(numr != 0L)	{	*numr = 0;				}
		return false;
	}
	indexRaw++;
	if((u32)_strRaw->getLength() <= indexRaw)
	{
		if(code != 0L)	{	*code = NLCODE_CR;		}	// CR '\r'
		if(numr != 0L)	{	*numr = 1;				}
		return true;
	}
	c = _strRaw->getAt(indexRaw);
	if(c == '\n')
	{
		if(code != 0L)	{	*code = NLCODE_CRLF;	}	// CRLF "\r\n"
		if(numr != 0L)	{	*numr = 2;				}
		return true;
	}
	else
	{
		if(code != 0L)	{	*code = NLCODE_CR;		}	// CR '\r'
		if(numr != 0L)	{	*numr = 1;				}
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	直前の位置の改行コードチェック
	※基本実装では、CR, CRLF, LF のどの組み合わせも認識する．
	　この実装を変更したい場合はこのメソッドをオーバーライドすること．
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::checkNewLineCodePrev(u32* code, u16* numr, u32* indexRawOut, u32 indexRawIn) const
{
	if(indexRawIn <= 0)
	{
		if(code != 0L)			{	*code = 0;								}
		if(numr != 0L)			{	*numr = 0;								}
		if(indexRawOut != 0L)	{	*indexRawOut = TxtPos::INDEX_INVALID;	}
		return false;
	}
	u32 indexRaw2 = indexRawIn - 1;
	if((u32)_strRaw->getLength() <= indexRaw2)
	{
		if(code != 0L)			{	*code = 0;								}
		if(numr != 0L)			{	*numr = 0;								}
		if(indexRawOut != 0L)	{	*indexRawOut = TxtPos::INDEX_INVALID;	}
		return false;
	}
	u16 c2 = _strRaw->getAt(indexRaw2);
	u16 c1;
	u32 indexRaw1 = indexRaw2;
	if(indexRaw1 >= 1)
	{
		indexRaw1--;
		c1 = _strRaw->getAt(indexRaw1);
	}
	else
	{
		c1 = 0;
	}
	if((c1 == '\r') && (c2 == '\n'))
	{
		if(code != 0L)			{	*code = NLCODE_CRLF;					}	// CRLF "\r\n"
		if(numr != 0L)			{	*numr = 2;								}
		if(indexRawOut != 0L)	{	*indexRawOut = indexRaw1;				}
		return true;
	}
	if(c2 == '\r')
	{
		if(code != 0L)			{	*code = NLCODE_CR;						}	// CR '\r'
		if(numr != 0L)			{	*numr = 1;								}
		if(indexRawOut != 0L)	{	*indexRawOut = indexRaw2;				}
		return true;
	}
	if(c2 == '\n')
	{
		if(code != 0L)			{	*code = NLCODE_LF;						}	// CR '\n'
		if(numr != 0L)			{	*numr = 1;								}
		if(indexRawOut != 0L)	{	*indexRawOut = indexRaw2;				}
		return true;
	}
	else
	{
		if(code != 0L)			{	*code = 0;								}
		if(numr != 0L)			{	*numr = 0;								}
		if(indexRawOut != 0L)	{	*indexRawOut = TxtPos::INDEX_INVALID;	}
		return false;
	}
}

/*---------------------------------------------------------------------*//**
	位置配列の挿入操作による移動
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::movePosByInsert(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
{
	if(tpaSub == 0L)	{	return;	}
	for(int i = 0; i < tpaSub->getCount(); i++)
	{
		movePosByInsert(tpaSub->getPos(i), tposDo, numRaw, numChar);
	}
}

/*---------------------------------------------------------------------*//**
	位置配列の削除操作による移動
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::movePosByRemove(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
{
	if(tpaSub == 0L)	{	return;	}
	for(int i = 0; i < tpaSub->getCount(); i++)
	{
		movePosByRemove(tpaSub->getPos(i), tposDo, numRaw, numChar);
	}
}

/*---------------------------------------------------------------------*//**
	位置配列の変更操作による移動
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::movePosByChange(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRawOld, u32 numCharOld, u32 numRawNew, u32 numCharNew) const
{
	if(tpaSub == 0L)	{	return;	}
	for(int i = 0; i < tpaSub->getCount(); i++)
	{
		movePosByChange(tpaSub->getPos(i), tposDo, numRawOld, numCharOld, numRawNew, numCharNew);
	}
}

/*---------------------------------------------------------------------*//**
	位置の挿入操作による移動
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::movePosByInsert(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
{
	// 変化のない位置
	if(tposTrg->getRawIndex() <= tposDo->getRawIndex())
	{
		return;
	}
	// 移動
	tposTrg->set(tposTrg->getRawIndex() + numRaw, tposTrg->getCharIndex() + numChar);
}

/*---------------------------------------------------------------------*//**
	位置の削除操作による移動
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::movePosByRemove(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
{
	// 変化のない位置
	if(tposTrg->getRawIndex() <= tposDo->getRawIndex())
	{
		return;
	}
	// 削除範囲内
	if(tposTrg->getRawIndex() < (tposDo->getRawIndex() + numRaw))
	{
		tposTrg->copy(tposDo);
		return;
	}
	// 移動
	tposTrg->set(tposTrg->getRawIndex() - numRaw, tposTrg->getCharIndex() - numChar);
}

/*---------------------------------------------------------------------*//**
	位置の削除操作による移動
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
void RawTxt<TYPE_CLASS, TYPE_BASE>::movePosByChange(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRawOld, u32 numCharOld, u32 numRawNew, u32 numCharNew) const
{
	// 変化のない位置
	if(tposTrg->getRawIndex() <= tposDo->getRawIndex())
	{
		return;
	}
	// 移動
	tposTrg->set(tposTrg->getRawIndex() - numRawOld + numRawNew, tposTrg->getCharIndex() - numCharOld + numCharNew);
}

/*---------------------------------------------------------------------*//**
	現在の位置の文字をチェック
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::curChar(CharPicker* cp, u32 indexRaw) const
{
	return curChar(cp, _strRaw->getRaw(), (u32)_strRaw->getLength(), indexRaw);
}

/*---------------------------------------------------------------------*//**
	一つ前の位置の文字をチェック
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::prevChar(CharPicker* cp, u32 indexRaw) const
{
	return prevChar(cp, _strRaw->getRaw(), (u32)_strRaw->getLength(), indexRaw);
}

#if 0
/*---------------------------------------------------------------------*//**
	現在の位置の文字をチェック
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::curChar(CharPicker* cp, const TYPE_BASE* cstr, u32 lenStr, u32 indexRaw) const
{
	// 改行コード判定
	u32 code;
	if(checkNewLineCodeCur(&code, &cp->_numRaw, indexRaw))
	{
		cp->_indexRaw = indexRaw;
		return true;
	}
	// ASCII コードである
	if(indexRaw >= lenStr)
	{
		return false;
	}
	cp->_indexRaw = indexRaw;
	cp->_numRaw = 1;
	return true;
}

/*---------------------------------------------------------------------*//**
	一つ前の位置の文字をチェック
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
bool RawTxt<TYPE_CLASS, TYPE_BASE>::prevChar(CharPicker* cp, const TYPE_BASE* cstr, u32 lenStr, u32 indexRaw) const
{
	// 改行コード判定
	u32 code;
	if(checkNewLineCodePrev(&code, &cp->_numRaw, &cp->_indexRaw, indexRaw))
	{
		return true;
	}
	// ASCII コードである
	if((indexRaw <= 0) || (cp->_indexRaw < indexRaw))
	{
		return false;
	}
	cp->_indexRaw = indexRaw - 1;
	cp->_numRaw = 1;
	return true;
}
#endif

/*---------------------------------------------------------------------*//**
	内在する文字列の文字数をカウントする
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
u32 RawTxt<TYPE_CLASS, TYPE_BASE>::countChar() const
{
	return countChar(_strRaw->getRaw(), (u32)_strRaw->getLength());
}

/*---------------------------------------------------------------------*//**
	文字数をカウントする
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
u32 RawTxt<TYPE_CLASS, TYPE_BASE>::countChar(const TYPE_BASE* cstr, u32 lenStr) const
{
	s32 cnt = 0;
	u32 i = 0;
	while(i < lenStr)
	{
		CharPicker cp;
		if(!curChar(&cp, cstr, lenStr, i))
		{
			ASSERT(false);
			return cnt;
		}
		cnt++;
		i += cp._numRaw;
	}
	return cnt;
}

//==========================================================================
// RawTxt<TYPE_CLASS, TYPE_BASE>::CharPicker メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
template<class TYPE_CLASS, class TYPE_BASE>
RawTxt<TYPE_CLASS, TYPE_BASE>::CharPicker::CharPicker()
	: _indexRaw(0)
	, _numRaw(0)
{
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#endif	// _TFW_STRING_TEXT_IMPLEMENT_RAW_TXT_H_
