/***********************************************************************//**
 *  AsciiTxt.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/11/27.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#if 1
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "AsciiTxt.h"

// Friends
#include "../../string/VwString.h"

// External

// Library

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

#define CLASS_BASE	RawS8TxtBase

//==========================================================================
// AsciiTxt メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	生の文字列を得る
**//*---------------------------------------------------------------------*/
void AsciiTxt::takeRawString(VcString* str) const
{
	*str = *_strRaw;
}

/*---------------------------------------------------------------------*//**
	生のワイド文字列を得る
**//*---------------------------------------------------------------------*/
void AsciiTxt::takeRawWideString(VwString* str) const
{
	str->setEasily(_strRaw);
}

/*---------------------------------------------------------------------*//**
	エンコードせずに、テキストを設定する
**//*---------------------------------------------------------------------*/
void AsciiTxt::setSingleString(const VcString* strRaw)
{
	_strRaw->set(strRaw);
	_countChar = countChar();
}

/*---------------------------------------------------------------------*//**
	エンコードせずに、テキストを設定する
**//*---------------------------------------------------------------------*/
void AsciiTxt::setWideString(const VwString* strRaw)
{
	_strRaw->setEasily(strRaw);
	_countChar = countChar();
}

/*---------------------------------------------------------------------*//**
	自身と同型のテキストを作成する
**//*---------------------------------------------------------------------*/
Txt* AsciiTxt::newSameType() const
{
	return new AsciiTxt();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::AsciiTxt()
	: CLASS_SUPER(1, 1)
{
}

#if 0
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::AsciiTxt(const VcString* strRaw)
	: CLASS_SUPER(strRaw, 1, 1)
{
	// setSingleString(strRaw);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::AsciiTxt(VcString* strRaw, u8 flags)
	: CLASS_SUPER(strRaw, flags, 1, 1)
{
	// setString(strRaw, flags);
}
#endif

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::AsciiTxt(s32 numCharRawMax)
	: CLASS_SUPER(1, numCharRawMax)
{
}

#if 0
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::AsciiTxt(const VcString* strRaw, s8 numCharRawMax)
	: CLASS_SUPER(strRaw, 1, numCharRawMax)
{
	// setSingleString(strRaw);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::AsciiTxt(VcString* strRaw, u8 flags, s8 numCharRawMax)
	: CLASS_SUPER(strRaw, flags, 1, numCharRawMax)
{
	// setString(strRaw, flags);
}
#endif

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::AsciiTxt(const AsciiTxt& src)
	: CLASS_SUPER(src)
{
	// copy(&src);
}

/*---------------------------------------------------------------------*//**
	現在の位置の文字をチェック
**//*---------------------------------------------------------------------*/
bool AsciiTxt::curChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const
{
	// 改行コード判定
	u32 code;
	if(checkNewLineCodeCur(&code, &cp->_numRaw, indexRaw))
	{
		cp->_indexRaw = indexRaw;
		return true;
	}
	// ASCII コードはインデックスで判定できる
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
bool AsciiTxt::prevChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const
{
	// 改行コード判定
	u32 code;
	if(checkNewLineCodePrev(&code, &cp->_numRaw, &cp->_indexRaw, indexRaw))
	{
		return true;
	}
	// ASCII コードはインデックスで判定できる
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
u32 AsciiTxt::countChar() const
{
	return countChar(_strRaw->getRaw(), (u32)_strRaw->getLength());
}

/*---------------------------------------------------------------------*//**
	文字数をカウントする
**//*---------------------------------------------------------------------*/
u32 AsciiTxt::countChar(const char* cstr, u32 lenStr) const
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
u32 AsciiTxt::toCode(const char* arr, u16 numRaw) const
{
	// コード化
	u32 code = *(const u8*)arr;
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
char* AsciiTxt::toRaw(char* arr, u32 code) const
{
	*(arr++) = (char)code;
	return arr;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

#else
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "AsciiTxt.h"

// Friends
#include "../../string/VwString.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// AsciiTxt メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	生の文字列を得る
**//*---------------------------------------------------------------------*/
void AsciiTxt::takeRawString(VcString* str) const
{
	*str = *_strRaw;
}

/*---------------------------------------------------------------------*//**
	生のワイド文字列を得る
**//*---------------------------------------------------------------------*/
void AsciiTxt::takeRawWideString(VwString* str) const
{
	str->set(_strRaw);
}

/*---------------------------------------------------------------------*//**
	先頭位置を得る
**//*---------------------------------------------------------------------*/
void AsciiTxt::getHearPos(TxtPos* pos) const
{
	pos->set(0, 0);
}

/*---------------------------------------------------------------------*//**
	終端位置を得る
**//*---------------------------------------------------------------------*/
void AsciiTxt::getTailPos(TxtPos* pos) const
{
	pos->set((u32)_strRaw->getLength(), getCharCount());
}

/*---------------------------------------------------------------------*//**
	先頭イテレータを得る
**//*---------------------------------------------------------------------*/
TxtIterator AsciiTxt::iterator() const
{
	return TxtIterator(this, 0, 0);
}

/*---------------------------------------------------------------------*//**
	先頭イテレータを得る
**//*---------------------------------------------------------------------*/
TxtIterator AsciiTxt::iteratorEnd() const
{
	TxtIterator it(this, (u32)_strRaw->getLength(), getCharCount());
	prevPos(&it, 0L);
	return it;
}

/*---------------------------------------------------------------------*//**
	位置を進める
**//*---------------------------------------------------------------------*/
bool AsciiTxt::nextPos(TxtPos* pos, TxtChar* tcBtwn) const
{
	CharPicker cp;
	if(!curChar(&cp, pos->getRawIndex()))
	{
		return false;
	}
	pos->set(cp._indexRaw + cp._numRaw, pos->getCharIndex() + 1);
	if(tcBtwn != 0L)
	{
		tcBtwn->set(toCode(cp._indexRaw, cp._numRaw), cp._numRaw);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	位置を戻す
**//*---------------------------------------------------------------------*/
bool AsciiTxt::prevPos(TxtPos* pos, TxtChar* tcBtwn) const
{
	CharPicker cp;
	if(!prevChar(&cp, pos->getRawIndex()))
	{
		return false;
	}
	pos->set(cp._indexRaw, pos->getCharIndex() - 1);
	if(tcBtwn != 0L)
	{
		tcBtwn->set(toCode(cp._indexRaw, cp._numRaw), cp._numRaw);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	文字のある位置かを調べる
**//*---------------------------------------------------------------------*/
bool AsciiTxt::isValidCharPos(const TxtPos* pos) const
{
	s32 indexRaw = pos->getRawIndex();
	return (0 <= indexRaw) && (indexRaw < _strRaw->getLength());
}

/*---------------------------------------------------------------------*//**
	テキストを設定する
	※ 異種エンコードのテキストはサポート外
**//*---------------------------------------------------------------------*/
void AsciiTxt::setText(const Txt* txt)
{
	copy(txt);
}

/*---------------------------------------------------------------------*//**
	テキストを設定する
**//*---------------------------------------------------------------------*/
void AsciiTxt::setText(const VcString* strRaw)
{
	_strRaw->copy(strRaw);
	_countChar = countChar();
}

/*---------------------------------------------------------------------*//**
	一文字取得する
**//*---------------------------------------------------------------------*/
bool AsciiTxt::getAt(TxtChar* tc, const TxtPos* pos) const
{
	CharPicker cp;
	if(!curChar(&cp, pos->getRawIndex()))
	{
		return false;
	}
	tc->set(toCode(cp._indexRaw, cp._numRaw), cp._numRaw);
	return true;
}

/*---------------------------------------------------------------------*//**
	一文字設定する
**//*---------------------------------------------------------------------*/
bool AsciiTxt::setAt(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc)
{
	u16 bytesSrc = tc->getBytes();

	CharPicker cp;
	curChar(&cp, pos->getRawIndex());

	if(bytesSrc == cp._numRaw)
	{
		for(u32 i = 0; i < bytesSrc; i++)
		{
			if(!_strRaw->setAt(pos->getRawIndex() + i, tc->getCodeByte(i)))
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
bool AsciiTxt::insert(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc)
{
	char cbuf[NUM_BYTES_MAX + 1];
	s32 i = 0;
	while(i < tc->getBytes())
	{
		cbuf[i++] = tc->getCodeByte(i);
	}
	ASSERT(i <= NUM_BYTES_MAX);
	cbuf[i] = '\0';

	if(!_strRaw->insert(pos->getRawIndex(), cbuf))
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
bool AsciiTxt::insert(TxtPosArray* tpaSub, const TxtPos* pos, const Txt* txt)
{
#if 1
	char* cbuf = new char[(txt->getCharCount() * NUM_BYTES_MAX) + 1];
	s32 i = 0;
	s32 cntRaw = 0;
	s32 cntChar = 0;
	for(TxtIterator it = txt->iterator(); it.has(); it.next())
	{
		TxtChar tc = it.cur();
		for(int j = 0; j < tc.getBytes(); j++)
		{
			cbuf[i++] = tc.getCodeByte(j);
			cntRaw++;
		}
		cntChar++;
	}
	cbuf[i] = '\0';

	if(!_strRaw->insert(pos->getRawIndex(), cbuf))
	{
		delete[] cbuf;
		return false;
	}
	_countChar++;

	movePosByInsert(tpaSub, pos, cntRaw, cntChar);

	delete[] cbuf;
	return true;
#else
	// ※ 異種エンコードのテキストはサポート外
	if(!this->_strRaw->insert(pos->getRawIndex(), ((AsciiTxt*)txt)->getRawString()))
	{
		return false;
	}
	this->_countChar += txt->getCharCount();

	movePosByInsert(tpaSub, pos, ((AsciiTxt*)txt)->getRawString()->getLength(), txt->getCharCount());
	return true;
#endif
}

/*---------------------------------------------------------------------*//**
	文字列を挿入する
**//*---------------------------------------------------------------------*/
bool AsciiTxt::insert(TxtPosArray* tpaSub, const TxtPos* pos, const VcString* strRaw)
{
	AsciiTxt txt(strRaw);
	return insert(tpaSub, pos, &txt);
}

/*---------------------------------------------------------------------*//**
	文字を削除する
**//*---------------------------------------------------------------------*/
bool AsciiTxt::remove(TxtPosArray* tpaSub, const TxtPos* pos, u32 numChar)
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
bool AsciiTxt::trimLeft(TxtPosArray* tpaSub, const TxtChar* tcTrim)
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
bool AsciiTxt::trimRight(TxtPosArray* tpaSub, const TxtChar* tcTrim)
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
bool AsciiTxt::subtext(Txt** txtMade, const TxtPos* posBegin, const TxtPos* posEnd) const
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
bool AsciiTxt::subtext(Txt** txtMade, const TxtPos* posBegin) const
{
	TxtPos tpos((u32)_strRaw->getLength(), getCharCount());
	return subtext(txtMade, posBegin, &tpos);
}

/*---------------------------------------------------------------------*//**
	自身と同型のテキストを作成する
**//*---------------------------------------------------------------------*/
Txt* AsciiTxt::newSameType() const
{
	return new AsciiTxt();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::AsciiTxt()
	: _strRaw(new VcString())
	, _countChar(0)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::AsciiTxt(const VcString* strRaw)
	: _strRaw(new VcString())
	, _countChar(0)
{
	setText(strRaw);
}

/*---------------------------------------------------------------------*//**
	コピー コンストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::AsciiTxt(const AsciiTxt& src)
	: _strRaw(new VcString())
	, _countChar(0)
{
	copy(&src);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::~AsciiTxt()
{
	delete _strRaw;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void AsciiTxt::copy(const Txt* src)
{
	if(this->getEncode() == src->getEncode())
	{
		// 同エンコードの場合は単純コピー
		_strRaw->copy(((AsciiTxt*)src)->_strRaw);
		_countChar = ((AsciiTxt*)src)->_countChar;
		return;
	}

	Txt::copy(src);
}

/*---------------------------------------------------------------------*//**
	現在の位置の改行コードチェック
	※基本実装では、CR, CRLF, LF のどの組み合わせも認識する．
	　この実装を変更したい場合はこのメソッドをオーバーライドすること．
**//*---------------------------------------------------------------------*/
bool AsciiTxt::checkNewLineCodeCur(u32* code, u16* numr, u32 indexRaw) const
{
	if((u32)_strRaw->getLength() <= indexRaw)
	{
		if(code != 0L)	{	*code = 0;				}
		if(numr != 0L)	{	*numr = 0;				}
		return false;
	}
	char c = _strRaw->getAt(indexRaw);
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
bool AsciiTxt::checkNewLineCodePrev(u32* code, u16* numr, u32* indexRawOut, u32 indexRawIn) const
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
	char c2 = _strRaw->getAt(indexRaw2);
	char c1;
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
void AsciiTxt::movePosByInsert(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
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
void AsciiTxt::movePosByRemove(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
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
void AsciiTxt::movePosByChange(TxtPosArray* tpaSub, const TxtPos* tposDo, u32 numRawOld, u32 numCharOld, u32 numRawNew, u32 numCharNew) const
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
void AsciiTxt::movePosByInsert(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
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
void AsciiTxt::movePosByRemove(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRaw, u32 numChar) const
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
void AsciiTxt::movePosByChange(TxtPos* tposTrg, const TxtPos* tposDo, u32 numRawOld, u32 numCharOld, u32 numRawNew, u32 numCharNew) const
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
bool AsciiTxt::curChar(CharPicker* cp, u32 indexRaw) const
{
	return curChar(cp, _strRaw->getRaw(), (u32)_strRaw->getLength(), indexRaw);
}

/*---------------------------------------------------------------------*//**
	一つ前の位置の文字をチェック
**//*---------------------------------------------------------------------*/
bool AsciiTxt::prevChar(CharPicker* cp, u32 indexRaw) const
{
	return prevChar(cp, _strRaw->getRaw(), (u32)_strRaw->getLength(), indexRaw);
}

/*---------------------------------------------------------------------*//**
	現在の位置の文字をチェック
**//*---------------------------------------------------------------------*/
bool AsciiTxt::curChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const
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
bool AsciiTxt::prevChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const
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
u32 AsciiTxt::countChar() const
{
	return countChar(_strRaw->getRaw(), (u32)_strRaw->getLength());
}

/*---------------------------------------------------------------------*//**
	文字数をカウントする
**//*---------------------------------------------------------------------*/
u32 AsciiTxt::countChar(const char* cstr, u32 lenStr) const
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
u32 AsciiTxt::toCode(u32 indexRaw, u16 numRaw) const
{
	// コード化
	const u8* c = (const u8*)_strRaw->getRaw(indexRaw);
	u32 code;
	if(numRaw == 2)	{	code = c[0] << 8 | c[1];	}
	else			{	code = c[0];				}
	// 特殊なコードの変換
	switch(code)
	{
	case 0x0d0a:	code = Txt::NLCODE_CRLF;	break;
	}
	return code;
}


//==========================================================================
// AsciiTxt::CharPicker メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
AsciiTxt::CharPicker::CharPicker()
	: _indexRaw(0)
	, _numRaw(0)
{
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
#endif