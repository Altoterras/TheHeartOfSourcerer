/***********************************************************************//**
 *  UnicodeTxt.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2012/12/06.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#if 1
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "UnicodeTxt.h"

// Friends
#include "Utf8Txt.h"
#include "../../string/VcString.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

#define CLASS_BASE	RawU16TxtBase

//==========================================================================
// UnicodeTxt メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	生の文字列を得る
**//*---------------------------------------------------------------------*/
void UnicodeTxt::takeRawString(VcString* str) const
{
	// 強制的に utf8 で 1 バイトキャラ文字列に変換する
	Utf8Txt txtUtf8;
	txtUtf8.copy(this);
	*str = *txtUtf8.getRawString();
}

/*---------------------------------------------------------------------*//**
	生のワイド文字列を得る
**//*---------------------------------------------------------------------*/
void UnicodeTxt::takeRawWideString(VwString* str) const
{
	str->set(_strRaw);
}

/*---------------------------------------------------------------------*//**
	エンコードせずに、テキストを設定する
**//*---------------------------------------------------------------------*/
void UnicodeTxt::setSingleString(const VcString* strRaw)
{
	_strRaw->setEasily(strRaw);
	_countChar = countChar();
}

/*---------------------------------------------------------------------*//**
	エンコードせずに、テキストを設定する
**//*---------------------------------------------------------------------*/
void UnicodeTxt::setWideString(const VwString* strRaw)
{
	_strRaw->set(strRaw);
	_countChar = countChar();
}

/*---------------------------------------------------------------------*//**
	自身と同型のテキストを作成する
**//*---------------------------------------------------------------------*/
Txt* UnicodeTxt::newSameType() const
{
	return new UnicodeTxt();
}
/*---------------------------------------------------------------------*//**
	現在の位置の文字をチェック
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::curChar(CharPicker* cp, const u16* cstr, u32 lenStr, u32 indexRaw) const
{
	// 改行コード判定
	u32 code;
	if(checkNewLineCodeCur(&code, &cp->_numRaw, indexRaw))
	{
		cp->_indexRaw = indexRaw;
		return true;
	}
	// 16bit Unicode コードはインデックスで判定できる
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
bool UnicodeTxt::prevChar(CharPicker* cp, const u16* cstr, u32 lenStr, u32 indexRaw) const
{
	// 改行コード判定
	u32 code;
	if(checkNewLineCodePrev(&code, &cp->_numRaw, &cp->_indexRaw, indexRaw))
	{
		return true;
	}
	// 16bit Unicode コードはインデックスで判定できる
	if((indexRaw <= 0) || (cp->_indexRaw < indexRaw))
	{
		return false;
	}
	cp->_indexRaw = indexRaw - 1;
	cp->_numRaw = 1;
	return true;
}

#if 0
/*---------------------------------------------------------------------*//**
	内在する文字列の文字数をカウントする
**//*---------------------------------------------------------------------*/
u32 UnicodeTxt::countChar() const
{
	return countChar(_strRaw->getRaw(), (u32)_strRaw->getLength());
}

/*---------------------------------------------------------------------*//**
	文字数をカウントする
**//*---------------------------------------------------------------------*/
u32 UnicodeTxt::countChar(const u16* cstr, u32 lenStr) const
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
#endif

/*---------------------------------------------------------------------*//**
	文字コードを算出する
**//*---------------------------------------------------------------------*/
u32 UnicodeTxt::toCode(const u16* arr, u16 numRaw) const
{
	// コード化
	const u16* c = (const u16*)arr;
	u32 code;
	if(numRaw == 2)	{	code = (c[0] << 16) | c[1];	}
	else			{	code = c[0];				}
	// 特殊なコードの変換
	switch(code)
	{
	case 0x0d0a:	code = Txt::NLCODE_CRLF;	break;
	}
	return code;
}

/*---------------------------------------------------------------------*//**
	文字コードから生文字列を作成する
**//*---------------------------------------------------------------------*/
u16* UnicodeTxt::toRaw(u16* arr, u32 code) const
{
	if(code >= (u32)(0x1 << 16))
	{
		*(arr++) = (char)(code >> 16);
		*(arr++) = (char)(code & ((u32)(0x1 << 16) - 1));
	}
	else
	{
		*(arr++) = (char)code;
	}
	return arr;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
UnicodeTxt::UnicodeTxt()
	: CLASS_SUPER(2, 1)
{
}

#if 0
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
UnicodeTxt::UnicodeTxt(const VwString* strRaw)
	: CLASS_SUPER(strRaw, 2, 1)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
UnicodeTxt::UnicodeTxt(VwString* strRaw, u8 flags)
	: CLASS_SUPER(strRaw, flags, 2, 1)
{
}
#endif

/*---------------------------------------------------------------------*//**
	コピー コンストラクタ
**//*---------------------------------------------------------------------*/
UnicodeTxt::UnicodeTxt(const UnicodeTxt& src)
	: CLASS_SUPER(src)
{
	// copy(&src);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#else
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "UnicodeTxt.h"

// Friends
#include "Utf8Txt.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// UnicodeTxt メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	生の文字列を得る
**//*---------------------------------------------------------------------*/
void UnicodeTxt::takeRawString(VcString* str) const
{
	// 強制的に utf8 で 1 バイトキャラ文字列に変換する
	Utf8Txt txtUtf8;
	txtUtf8.copy(this);
	*str = *txtUtf8.getRawString();
}

/*---------------------------------------------------------------------*//**
	生のワイド文字列を得る
**//*---------------------------------------------------------------------*/
void UnicodeTxt::takeRawWideString(VwString* str) const
{
	str->set(_strRaw);
}

/*---------------------------------------------------------------------*//**
	先頭位置を得る
**//*---------------------------------------------------------------------*/
void UnicodeTxt::getHearPos(TxtPos* pos) const
{
	pos->set(0, 0);
}

/*---------------------------------------------------------------------*//**
	終端位置を得る
**//*---------------------------------------------------------------------*/
void UnicodeTxt::getTailPos(TxtPos* pos) const
{
	pos->set((u32)_strRaw->getLength(), getCharCount());
}

/*---------------------------------------------------------------------*//**
	先頭イテレータを得る
**//*---------------------------------------------------------------------*/
TxtIterator UnicodeTxt::iterator() const
{
	return TxtIterator(this, 0, 0);
}

/*---------------------------------------------------------------------*//**
	先頭イテレータを得る
**//*---------------------------------------------------------------------*/
TxtIterator UnicodeTxt::iteratorEnd() const
{
	TxtIterator it(this, (u32)_strRaw->getLength(), getCharCount());
	prevPos(&it, 0L);
	return it;
}

/*---------------------------------------------------------------------*//**
	位置を進める
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::nextPos(TxtPos* pos, TxtChar* tcBtwn) const
{
	CharPicker cp;
	if(!curChar(&cp, pos->getRawIndex()))
	{
		return false;
	}
	pos->set(cp._indexRaw + cp._numRaw, pos->getCharIndex() + 1);
	if(tcBtwn != 0L)
	{
		tcBtwn->set(toCode(cp._indexRaw, cp._numRaw), cp._numRaw * 2);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	位置を戻す
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::prevPos(TxtPos* pos, TxtChar* tcBtwn) const
{
	CharPicker cp;
	if(!prevChar(&cp, pos->getRawIndex()))
	{
		return false;
	}
	pos->set(cp._indexRaw, pos->getCharIndex() - 1);
	if(tcBtwn != 0L)
	{
		tcBtwn->set(toCode(cp._indexRaw, cp._numRaw), cp._numRaw * 2);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	文字のある位置かを調べる
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::isValidCharPos(const TxtPos* pos) const
{
	s32 indexRaw = pos->getRawIndex();
	return (0 <= indexRaw) && (indexRaw < _strRaw->getLength());
}

/*---------------------------------------------------------------------*//**
	テキストを設定する
	※ 異種エンコードのテキストはサポート外
**//*---------------------------------------------------------------------*/
void UnicodeTxt::setText(const Txt* txt)
{
	copy(txt);
}

/*---------------------------------------------------------------------*//**
	テキストを設定する
**//*---------------------------------------------------------------------*/
void UnicodeTxt::setText(const VcString* strRaw)
{
	_strRaw->copy(strRaw);
	_countChar = countChar();
}

/*---------------------------------------------------------------------*//**
	テキストを設定する
**//*---------------------------------------------------------------------*/
void UnicodeTxt::setText(const VwString* strRaw)
{
	_strRaw->copy(strRaw);
	_countChar = countChar();
}

/*---------------------------------------------------------------------*//**
	一文字取得する
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::getAt(TxtChar* tc, const TxtPos* pos) const
{
	CharPicker cp;
	if(!curChar(&cp, pos->getRawIndex()))
	{
		return false;
	}
	tc->set(toCode(cp._indexRaw, cp._numRaw), cp._numRaw * 2);
	return true;
}

/*---------------------------------------------------------------------*//**
	一文字設定する
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::setAt(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc)
{
	u16 bytesSrc = tc->getBytes();

	CharPicker cp;
	curChar(&cp, pos->getRawIndex());

	if(bytesSrc * 2 == cp._numRaw)
	{
		u32 idxDst = pos->getRawIndex();
		u32 cntSrc = 0;
		while(cntSrc < bytesSrc)
		{
			u16 c;
			u32 numC;
			if((cntSrc + 1) < bytesSrc)
			{
				c =  tc->getCodeByte(cntSrc + 0) << 16 | tc->getCodeByte(cntSrc + 1);
				numC = 2;
			}
			else
			{
				c =  tc->getCodeByte(cntSrc + 0);
				numC = 1;
			}
			if(!_strRaw->setAt(idxDst, c))
			{
				return false;
			}
			idxDst++;
			cntSrc += numC;
		}
		movePosByChange(tpaSub, pos, cp._numRaw, 1, cntSrc, 1);
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
bool UnicodeTxt::insert(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc)
{
	if(!_strRaw->insert(pos->getRawIndex(), (u16)tc->getCode()))
	{
		return false;
	}
	_countChar++;

	movePosByInsert(tpaSub, pos, 1, 1);
	return true;
}

/*---------------------------------------------------------------------*//**
	テキストを挿入する
	※ 異種エンコードのテキストはサポート外
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::insert(TxtPosArray* tpaSub, const TxtPos* pos, const Txt* txt)
{
	s32 i = pos->getRawIndex();
	s32 cnt = 0;
	for(TxtIterator it = txt->iterator(); it.has(); it.next())
	{
		TxtChar tc = it.cur();
		if(!_strRaw->insert(i++, (u16)tc.getCode()))
		{
			return false;
		}
		_countChar++;
		cnt++;
	}

	movePosByInsert(tpaSub, pos, cnt, cnt);

	return true;
}

/*---------------------------------------------------------------------*//**
	文字列を挿入する
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::insert(TxtPosArray* tpaSub, const TxtPos* pos, const VcString* strRaw)
{
	UnicodeTxt txt(strRaw);
	return insert(tpaSub, pos, &txt);
}

/*---------------------------------------------------------------------*//**
	文字を削除する
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::remove(TxtPosArray* tpaSub, const TxtPos* pos, u32 numChar)
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
	return false;
}

/*---------------------------------------------------------------------*//**
	左端空白削除
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::trimLeft(TxtPosArray* tpaSub, const TxtChar* tcTrim)
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
bool UnicodeTxt::trimRight(TxtPosArray* tpaSub, const TxtChar* tcTrim)
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
bool UnicodeTxt::subtext(Txt** txtMade, const TxtPos* posBegin, const TxtPos* posEnd) const
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
bool UnicodeTxt::subtext(Txt** txtMade, const TxtPos* posBegin) const
{
	TxtPos tpos((u32)_strRaw->getLength(), getCharCount());
	return subtext(txtMade, posBegin, &tpos);
}

/*---------------------------------------------------------------------*//**
	自身と同型のテキストを作成する
**//*---------------------------------------------------------------------*/
Txt* UnicodeTxt::newSameType() const
{
	return new UnicodeTxt();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
UnicodeTxt::UnicodeTxt()
	: _strRaw(new VwString())
	, _countChar(0)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
UnicodeTxt::UnicodeTxt(const VcString* strRaw)
	: _strRaw(new VwString())
	, _countChar(0)
{
	setText(strRaw);
}

/*---------------------------------------------------------------------*//**
	コピー コンストラクタ
**//*---------------------------------------------------------------------*/
UnicodeTxt::UnicodeTxt(const UnicodeTxt& src)
	: _strRaw(new VwString())
	, _countChar(0)
{
	copy(&src);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
UnicodeTxt::~UnicodeTxt()
{
	delete _strRaw;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void UnicodeTxt::copy(const Txt* src)
{
	_strRaw->copy(((UnicodeTxt*)src)->_strRaw);
	_countChar = ((UnicodeTxt*)src)->_countChar;
}

/*---------------------------------------------------------------------*//**
	現在の位置の改行コードチェック
	※基本実装では、CR, CRLF, LF のどの組み合わせも認識する．
	　この実装を変更したい場合はこのメソッドをオーバーライドすること．
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::checkNewLineCodeCur(u32* code, u16* numr, u32 indexRaw) const
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
bool UnicodeTxt::checkNewLineCodePrev(u32* code, u16* numr, u32* indexRawOut, u32 indexRawIn) const
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
void UnicodeTxt::movePosByInsert(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
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
void UnicodeTxt::movePosByRemove(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
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
void UnicodeTxt::movePosByChange(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRawOld, u32 numCharOld, u32 numRawNew, u32 numCharNew) const
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
void UnicodeTxt::movePosByInsert(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
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
void UnicodeTxt::movePosByRemove(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
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
void UnicodeTxt::movePosByChange(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRawOld, u32 numCharOld, u32 numRawNew, u32 numCharNew) const
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
bool UnicodeTxt::curChar(CharPicker* cp, u32 indexRaw) const
{
	return curChar(cp, _strRaw->getRaw(), (u32)_strRaw->getLength(), indexRaw);
}

/*---------------------------------------------------------------------*//**
	一つ前の位置の文字をチェック
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::prevChar(CharPicker* cp, u32 indexRaw) const
{
	return prevChar(cp, _strRaw->getRaw(), (u32)_strRaw->getLength(), indexRaw);
}

/*---------------------------------------------------------------------*//**
	現在の位置の文字をチェック
**//*---------------------------------------------------------------------*/
bool UnicodeTxt::curChar(CharPicker* cp, const u16* cstr, u32 lenStr, u32 indexRaw) const
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
bool UnicodeTxt::prevChar(CharPicker* cp, const u16* cstr, u32 lenStr, u32 indexRaw) const
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

/*---------------------------------------------------------------------*//**
	内在する文字列の文字数をカウントする
**//*---------------------------------------------------------------------*/
u32 UnicodeTxt::countChar() const
{
	return countChar(_strRaw->getRaw(), (u32)_strRaw->getLength());
}

/*---------------------------------------------------------------------*//**
	文字数をカウントする
**//*---------------------------------------------------------------------*/
u32 UnicodeTxt::countChar(const u16* cstr, u32 lenStr) const
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

/*---------------------------------------------------------------------*//**
	文字コードを算出する
**//*---------------------------------------------------------------------*/
u32 UnicodeTxt::toCode(u32 indexRaw, u16 numRaw) const
{
	// コード化
	const u16* c = (const u16*)_strRaw->getRaw(indexRaw);
	u32 code;
	if(numRaw == 2)	{	code = c[0] << 16 | c[1];	}
	else			{	code = c[0];				}
	// 特殊なコードの変換
	switch(code)
	{
	case 0x0d0a:	code = Txt::NLCODE_CRLF;	break;
	}
	return code;
}


//==========================================================================
// UnicodeTxt::CharPicker メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
UnicodeTxt::CharPicker::CharPicker()
	: _indexRaw(0)
	, _numRaw(0)
{
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
#endif