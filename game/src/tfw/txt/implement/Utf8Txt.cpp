/***********************************************************************//**
 *  Utf8Txt.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/12/06.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Utf8Txt.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Utf8Txt メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

#if 0
/*---------------------------------------------------------------------*//**
	一文字設定する
**//*---------------------------------------------------------------------*/
bool Utf8Txt::setAt(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc)
{
	u16 bytesSrc = tc->getBytes();

	CharPicker cp;
	AsciiTxt::curChar(&cp, pos->getRawIndex());

	if(bytesSrc == cp._numRaw)
	{
		u8 cbuf[NUM_BYTES_MAX + 1];
		u16 bytes = 0;
		fromCode(cbuf, &bytes, tc->getCode());
		cbuf[bytes] = '\0';

		for(u32 i = 0; i < bytesSrc; i++)
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
bool Utf8Txt::insert(TxtPosArray* tpaSub, const TxtPos* pos, const TxtChar* tc)
{
	u8 cbuf[NUM_BYTES_MAX + 1];
	u16 bytes = 0;
	fromCode(cbuf, &bytes, tc->getCode());
	cbuf[bytes] = '\0';

	if(!_strRaw->insert(pos->getRawIndex(), (char*)cbuf))
	{
		return false;
	}
	_countChar++;

	movePosByInsert(tpaSub, pos, tc->getBytes(), 1);
	return true;
}

/*---------------------------------------------------------------------*//**
	テキストを挿入する
	※ 異種エンコードのテキストはサポート外
**//*---------------------------------------------------------------------*/
bool Utf8Txt::insert(TxtPosArray* tpaSub, const TxtPos* pos, const Txt* txt)
{
	ASSERT(txt->getEncode() == TxtDef::ENC_UTF8);
	if(txt->getEncode() != TxtDef::ENC_UTF8)	{	return false;	}
	return AsciiTxt::insert(tpaSub, pos, txt);
}

/*---------------------------------------------------------------------*//**
	文字列を挿入する
**//*---------------------------------------------------------------------*/
bool Utf8Txt::insert(TxtPosArray* tpaSub, const TxtPos* pos, const VcString* strRaw)
{
	Utf8Txt txt(strRaw);
	return insert(tpaSub, pos, &txt);
}
#endif

/*---------------------------------------------------------------------*//**
	左端空白削除
**//*---------------------------------------------------------------------*/
bool Utf8Txt::trimLeft(TxtPosArray* tpaSub, const TxtChar* tcTrim)
{
	if(tcTrim == 0L)
	{
		bool isTrim = AsciiTxt::trimLeft(tpaSub, 0L);
		// 全角スペースも対象
		TxtChar tcFwSp(0x300, 2);
		return AsciiTxt::trimLeft(tpaSub, tcTrim) || isTrim;
	}
	return AsciiTxt::trimLeft(tpaSub, tcTrim);
}

/*---------------------------------------------------------------------*//**
	右端空白削除
**//*---------------------------------------------------------------------*/
bool Utf8Txt::trimRight(TxtPosArray* tpaSub, const TxtChar* tcTrim)
{
	if(tcTrim == 0L)
	{
		bool isTrim = AsciiTxt::trimRight(tpaSub, 0L);
		// 全角スペースも対象
		TxtChar tcFwSp(0x300, 2);
		return AsciiTxt::trimRight(tpaSub, tcTrim) || isTrim;
	}
	return AsciiTxt::trimRight(tpaSub, tcTrim);
}

/*---------------------------------------------------------------------*//**
	自身と同型のテキストを作成する
**//*---------------------------------------------------------------------*/
Txt* Utf8Txt::newSameType() const
{
	return new Utf8Txt();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Utf8Txt::Utf8Txt()
	: AsciiTxt(NUM_BYTES_MAX)
{
}

#if 0
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Utf8Txt::Utf8Txt(const VcString* strRaw)
	: AsciiTxt(strRaw, NUM_BYTES_MAX)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Utf8Txt::Utf8Txt(VcString* strRaw, u8 flags)
	: AsciiTxt(strRaw, flags, NUM_BYTES_MAX)
{
}
#endif

/*---------------------------------------------------------------------*//**
	コピー コンストラクタ
**//*---------------------------------------------------------------------*/
Utf8Txt::Utf8Txt(const Utf8Txt& src)
{
	copy(&src);
}

/*---------------------------------------------------------------------*//**
	現在の位置の文字をチェック
**//*---------------------------------------------------------------------*/
bool Utf8Txt::curChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const
{
	char c;
	cp->_indexRaw = indexRaw;
	if(indexRaw >= lenStr)
	{
		return false;
	}
	c = cstr[cp->_indexRaw];
	// BOM 判定
	if(isByteUtf8Bom(c))
	{
		cp->_indexRaw += 2;	// BOM は無視する
		if(cp->_indexRaw >= lenStr)
		{
			return false;	// 不正なコード
		}
		c = cstr[cp->_indexRaw];
	}
	cp->_numRaw = getUtf8Bytes(c);
	if((cp->_indexRaw + cp->_numRaw) > lenStr)
	{
		return false;	// 不正なコード
	}
	// マルチバイト文字判定
	if(cp->_numRaw >= 2)
	{
		return true;
	}
	// 改行コード判定
	u32 code;
	if(checkNewLineCodeCur(&code, &cp->_numRaw, cp->_indexRaw))
	{
		return true;
	}
	// ASCII コードである
	cp->_numRaw = 1;
	return true;
}

/*---------------------------------------------------------------------*//**
	一つ前の位置の文字をチェック
**//*---------------------------------------------------------------------*/
bool Utf8Txt::prevChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const
{
	char c;
	u32 indexRawA = indexRaw - 1;
	if(indexRawA >= lenStr)
	{
		cp->_indexRaw = indexRawA;
		return false;
	}
	c = cstr[indexRawA];
	// BOM 判定
	if(isByteUtf8Bom(c))
	{
		if(indexRawA >= 2)
		{
			return false;	// 不正なコード
		}
		indexRawA -= 2;	// BOM は無視する
		c = cstr[indexRawA];
	}
	// マルチバイト文字判定
	if((1 <= indexRawA) && isByteUtf8Aft(c))
	{
		// バイト数を得る
		cp->_numRaw = 1;
		while(true)
		{
			if(indexRawA <= 0)
			{
				return false;	// 不正なコード
			}
			indexRawA--;
			c = cstr[indexRawA];
			cp->_numRaw++;
			if(!isByteUtf8Aft(c))
			{
				if(!isByteUtf8Fst(c))
				{
					return false;	// 不正なコード
				}
				break;
			}
		}
		cp->_indexRaw = indexRawA;
		return true;
	}
	// 改行コード判定
	u32 code;
	if(checkNewLineCodePrev(&code, &cp->_numRaw, &cp->_indexRaw, indexRaw))
	{
		return true;
	}
	// ASCII コードである
	cp->_numRaw = 1;
	cp->_indexRaw = indexRawA;
	return true;
}

/*---------------------------------------------------------------------*//**
	文字コードを算出する
**//*---------------------------------------------------------------------*/
u32 Utf8Txt::toCode(const char* arr, u16 numRaw) const
{
	const u8* c = (const u8*)arr;
	if((numRaw >= 2) && !isByteUtf8Fst(c[0]))	// 複数バイトだが、UTF8 コードでない場合（改行コードなど）
	{
		// コード化
		u32 code = c[0];
		for(u32 i = 1; i < numRaw; i++)
		{
			code = code << 8 | c[1];
		}
		// 特殊なコードの変換
		switch(code)
		{
		case 0x0d0a:	code = Txt::NLCODE_CRLF;	break;
		}
		return code;
	}
	// UTF8 コードを Unicode 化
	switch(numRaw)
	{
	case 1:	return   c[0];
	case 2:	return ((c[0] & 0x1f) << 6) |  (c[1] & 0x3f);
	case 3:	return ((c[0] & 0xf) << 12) | ((c[1] & 0x3f) << 6)  |  (c[2] & 0x3f);
	case 4:	return ((c[0] & 0x7) << 18) | ((c[1] & 0x3f) << 12) | ((c[2] & 0x3f) << 6)  |  (c[3] & 0x3f);
	case 5:	return ((c[0] & 0x3) << 24) | ((c[1] & 0x3f) << 18) | ((c[2] & 0x3f) << 12) | ((c[3] & 0x3f) << 6)  |  (c[4] & 0x3f);
	case 6:	return ((c[0] & 0x1) << 30) | ((c[1] & 0x3f) << 24) | ((c[2] & 0x3f) << 18) | ((c[3] & 0x3f) << 12) | ((c[4] & 0x3f) << 6) | (c[5] & 0x3f);
	}
	return 0;
}

#if 0
/*---------------------------------------------------------------------*//**
	文字コードから文字列を算出する
**//*---------------------------------------------------------------------*/
void Utf8Txt::fromCode(u8 cbuf[NUM_BYTES_MAX], u16* bytes, u32 code)
{
	// 特殊なコードの変換
	switch(code)
	{
	case Txt::NLCODE_CRLF:
		cbuf[0] = '\r';
		cbuf[1] = '\n';
		*bytes = 2;
		return;
	}
	// Unicode を UTF8 コード化
	if(code > 0x7fffffff)
	{
		// BOM
		return;
	}
	if(code >= 0x04000000)
	{
		cbuf[0] = 0xfc | ((code >> 30) & 0x1);
		cbuf[1] = 0x80 | ((code >> 24) & 0x3f);
		cbuf[2] = 0x80 | ((code >> 18) & 0x3f);
		cbuf[3] = 0x80 | ((code >> 12) & 0x3f);
		cbuf[4] = 0x80 | ((code >>  6) & 0x3f);
		cbuf[5] = 0x80 | ((code      ) & 0x3f);
		*bytes = 6;
		return;
	}
	if(code >= 0x00200000)
	{
		cbuf[0] = 0xf8 | ((code >> 24) & 0x3);
		cbuf[1] = 0x80 | ((code >> 18) & 0x3f);
		cbuf[2] = 0x80 | ((code >> 12) & 0x3f);
		cbuf[3] = 0x80 | ((code >>  6) & 0x3f);
		cbuf[4] = 0x80 | ((code      ) & 0x3f);
		*bytes = 5;
		return;
	}
	if(code >= 0x00010000)
	{
		cbuf[0] = 0xf0 | ((code >> 18) & 0x7);
		cbuf[1] = 0x80 | ((code >> 12) & 0x3f);
		cbuf[2] = 0x80 | ((code >>  6) & 0x3f);
		cbuf[3] = 0x80 | ((code      ) & 0x3f);
		*bytes = 4;
		return;
	}
	if(code >= 0x00000800)
	{
		cbuf[0] = 0xe0 | ((code >> 12) & 0xf);
		cbuf[1] = 0x80 | ((code >>  6) & 0x3f);
		cbuf[2] = 0x80 | ((code      ) & 0x3f);
		*bytes = 3;
		return;
	}
	if(code >= 0x00000080)
	{
		cbuf[0] = 0xc0 | ((code >>  6) & 0x1f);
		cbuf[1] = 0x80 | ((code      ) & 0x3f);
		*bytes = 2;
		return;
	}
	cbuf[0] = code & 0x7f;
	*bytes = 1;
}
#endif

/*---------------------------------------------------------------------*//**
	文字コードから文字列を算出する
**//*---------------------------------------------------------------------*/
char* Utf8Txt::toRaw(char* arr, u32 code) const
{
	// 特殊なコードの変換
	switch(code)
	{
	case Txt::NLCODE_CRLF:
		((u8*)arr)[0] = '\r';
		((u8*)arr)[1] = '\n';
		return arr + 2;
	}
	// Unicode を UTF8 コード化
	if(code > 0x7fffffff)
	{
		// BOM
		return arr;
	}
	if(code >= 0x04000000)
	{
		((u8*)arr)[0] = 0xfc | ((code >> 30) & 0x1);
		((u8*)arr)[1] = 0x80 | ((code >> 24) & 0x3f);
		((u8*)arr)[2] = 0x80 | ((code >> 18) & 0x3f);
		((u8*)arr)[3] = 0x80 | ((code >> 12) & 0x3f);
		((u8*)arr)[4] = 0x80 | ((code >>  6) & 0x3f);
		((u8*)arr)[5] = 0x80 | ((code      ) & 0x3f);
		return arr + 6;
	}
	if(code >= 0x00200000)
	{
		((u8*)arr)[0] = 0xf8 | ((code >> 24) & 0x3);
		((u8*)arr)[1] = 0x80 | ((code >> 18) & 0x3f);
		((u8*)arr)[2] = 0x80 | ((code >> 12) & 0x3f);
		((u8*)arr)[3] = 0x80 | ((code >>  6) & 0x3f);
		((u8*)arr)[4] = 0x80 | ((code      ) & 0x3f);
		return arr + 5;
	}
	if(code >= 0x00010000)
	{
		((u8*)arr)[0] = 0xf0 | ((code >> 18) & 0x7);
		((u8*)arr)[1] = 0x80 | ((code >> 12) & 0x3f);
		((u8*)arr)[2] = 0x80 | ((code >>  6) & 0x3f);
		((u8*)arr)[3] = 0x80 | ((code      ) & 0x3f);
		return arr + 4;
	}
	if(code >= 0x00000800)
	{
		((u8*)arr)[0] = 0xe0 | ((code >> 12) & 0xf);
		((u8*)arr)[1] = 0x80 | ((code >>  6) & 0x3f);
		((u8*)arr)[2] = 0x80 | ((code      ) & 0x3f);
		return arr + 3;
	}
	if(code >= 0x00000080)
	{
		((u8*)arr)[0] = 0xc0 | ((code >>  6) & 0x1f);
		((u8*)arr)[1] = 0x80 | ((code      ) & 0x3f);
		return arr + 2;
	}
	((u8*)arr)[0] = code & 0x7f;
	return arr + 1;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS