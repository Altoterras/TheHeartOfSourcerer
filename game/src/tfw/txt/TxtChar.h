/***********************************************************************//**
 *  TxtChar.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/11/27.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TEXT_TXT_CHAR_H_
#define _TFW_STRING_TEXT_TXT_CHAR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../Base.h"

// Friends
#include "../string/VcString.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Txt;

/*---------------------------------------------------------------------*//**
 *	テキスト文字基底クラス
 *
**//*---------------------------------------------------------------------*/
class TxtChar
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 文字コード（エンコード形式依存）を得る
	inline u32 getCode() const { return _code; }
	// 文字コードの指定のバイトを得る
	inline u8 getCodeByte(u32 index) const { return (u8)((_code >> ((_bytes - index - 1) * 8)) & 0xff); }
	// バイト数を得る
	inline u16 getBytes() const { return _bytes; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	VcString makeString() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TxtChar();
	explicit TxtChar(char c);
	TxtChar(u32 code, u16 bytes);
	TxtChar(const TxtChar& src);
	void copy(const TxtChar* src);
	void set(u32 code, u16 bytes);

	//======================================================================
	// 変数
protected:
	u32 _code;
	u16 _bytes;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_TEXT_TXT_CHAR_H_
