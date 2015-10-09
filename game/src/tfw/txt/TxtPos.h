/***********************************************************************//**
 *  TxtPos.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/11/27.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TEXT_TXT_POS_H_
#define _TFW_STRING_TEXT_TXT_POS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Txt;

/*---------------------------------------------------------------------*//**
 *	テキスト文字位置基底クラス
 *		イテレータと同等．
 *		イテレータよりも簡易的なものだが，
 *		文字列の取得や挿入などの操作にも使用する．
 *
**//*---------------------------------------------------------------------*/
class TxtPos
{
	//======================================================================
	// 定数
public:
	static const u32 INDEX_INVALID	=	0xffffffff;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 生バイト列のインデックスを得る
	inline u32 getRawIndex() const { return _indexRaw; }
	// 文字列におけるインデックス（文字コードに依存）を得る
	inline u32 getCharIndex() const { return _indexChar; }
	// 無効な位置かを得る
	inline bool isInvalid() const { return _indexRaw == INDEX_INVALID;	}

	/*
	// 生バイト列のインデックスを設定する
	inline void setRawIndex(u32 indexRaw) { _indexRaw = indexRaw; }
	// 文字列におけるインデックス（文字コードに依存）を設定する
	inline void setCharIndex(u32 indexChar) { _indexChar = indexChar; }
	*/

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool gt(const TxtPos* x) const;
	bool lt(const TxtPos* x) const;
	bool ge(const TxtPos* x) const;
	bool le(const TxtPos* x) const;
	bool eq(const TxtPos* x) const;

	void set(u32 indexRaw, u32 indexChar);
	void invalidate();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TxtPos();
	TxtPos(u32 indexRaw, u32 indexChar);
	TxtPos(const TxtPos& src);
	void copy(const TxtPos* src);

	//======================================================================
	// 変数
protected:
	u32 _indexRaw;
	u32 _indexChar;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_TEXT_TXT_POS_H_
