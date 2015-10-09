/***********************************************************************//**
 *  SjisTxt.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/11/27.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SjisTxt.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SjisTxt メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

#if 0
/*---------------------------------------------------------------------*//**
	テキストを挿入する
	※ 異種エンコードのテキストはサポート外
**//*---------------------------------------------------------------------*/
bool SjisTxt::insert(TxtPosArray* tpaSub, const TxtPos* pos, const Txt* txt)
{
	ASSERT(txt->getEncode() == TxtDef::ENC_SJIS);
	if(txt->getEncode() != TxtDef::ENC_SJIS)	{	return false;	}
	return AsciiTxt::insert(tpaSub, pos, txt);
}

/*---------------------------------------------------------------------*//**
	文字列を挿入する
**//*---------------------------------------------------------------------*/
bool SjisTxt::insert(TxtPosArray* tpaSub, const TxtPos* pos, const VcString* strRaw)
{
	SjisTxt txt(strRaw);
	return insert(tpaSub, pos, &txt);
}
#endif

/*---------------------------------------------------------------------*//**
	左端空白削除
**//*---------------------------------------------------------------------*/
bool SjisTxt::trimLeft(TxtPosArray* tpaSub, const TxtChar* tcTrim)
{
	if(tcTrim == 0L)
	{
		bool isTrim = AsciiTxt::trimLeft(tpaSub, 0L);
		// 全角スペースも対象
		TxtChar tcFwSp(0x8140, 2);
		return AsciiTxt::trimLeft(tpaSub, tcTrim) || isTrim;
	}
	return AsciiTxt::trimLeft(tpaSub, tcTrim);
}

/*---------------------------------------------------------------------*//**
	右端空白削除
**//*---------------------------------------------------------------------*/
bool SjisTxt::trimRight(TxtPosArray* tpaSub, const TxtChar* tcTrim)
{
	if(tcTrim == 0L)
	{
		bool isTrim = AsciiTxt::trimRight(tpaSub, 0L);
		// 全角スペースも対象
		TxtChar tcFwSp(0x8140, 2);
		return AsciiTxt::trimRight(tpaSub, tcTrim) || isTrim;
	}
	return AsciiTxt::trimRight(tpaSub, tcTrim);
}

/*---------------------------------------------------------------------*//**
	自身と同型のテキストを作成する
**//*---------------------------------------------------------------------*/
Txt* SjisTxt::newSameType() const
{
	return new SjisTxt();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SjisTxt::SjisTxt()
	: AsciiTxt(NUM_BYTES_MAX)
{
}

#if 0
/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SjisTxt::SjisTxt(const VcString* strRaw)
	: AsciiTxt(strRaw, NUM_BYTES_MAX)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SjisTxt::SjisTxt(VcString* strRaw, u8 flags)
	: AsciiTxt(strRaw, flags, NUM_BYTES_MAX)
{
}
#endif

/*---------------------------------------------------------------------*//**
	コピー コンストラクタ
**//*---------------------------------------------------------------------*/
SjisTxt::SjisTxt(const SjisTxt& src)
{
	copy(&src);
}

/*---------------------------------------------------------------------*//**
	現在の位置の文字をチェック
**//*---------------------------------------------------------------------*/
bool SjisTxt::curChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const
{
	char c;
	cp->_indexRaw = indexRaw;
	if(indexRaw >= lenStr)
	{
		return false;
	}
	c = cstr[cp->_indexRaw];
	// マルチバイト文字判定
	if(isByteSjisFst(c))
	{
		u32 indexRaw2 = cp->_indexRaw + 1;
		if(indexRaw2 < lenStr)
		{
			c = cstr[indexRaw2];
			if(isByteSjisSnd(c))
			{
				cp->_numRaw = 2;
				return true;
			}
		}
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
bool SjisTxt::prevChar(CharPicker* cp, const char* cstr, u32 lenStr, u32 indexRaw) const
{
	char c;
	u32 indexRaw2 = indexRaw - 1;
	if(indexRaw2 >= lenStr)
	{
		cp->_indexRaw = indexRaw2;
		return false;
	}
	c = cstr[indexRaw2];
	// マルチバイト文字判定
	if((1 <= indexRaw2) && isByteSjisSnd(c))
	{
		cp->_indexRaw = indexRaw2 - 1;
		c = cstr[cp->_indexRaw];
		if(isByteSjisFst(c))
		{
			cp->_numRaw = 2;
			return true;
		}
	}
	// 改行コード判定
	u32 code;
	if(checkNewLineCodePrev(&code, &cp->_numRaw, &cp->_indexRaw, indexRaw))
	{
		return true;
	}
	// ASCII コードである
	cp->_numRaw = 1;
	cp->_indexRaw = indexRaw2;
	return true;
}

/*---------------------------------------------------------------------*//**
	文字コードを算出する
**//*---------------------------------------------------------------------*/
u32 SjisTxt::toCode(const char* arr, u16 numRaw) const
{
	// コード化
	const u8* c = (const u8*)arr;
	u32 code;
	if(numRaw == 2)	{	code = (c[0] << 8) | c[1];	}
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
char* SjisTxt::toRaw(char* arr, u32 code) const
{
	if(code >= (u32)(0x1 << 8))
	{
		*(arr++) = (char)(code >> 8);
		*(arr++) = (char)(code & ((u32)(0x1 << 8) - 1));
	}
	else
	{
		*(arr++) = (char)code;
	}
	return arr;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS