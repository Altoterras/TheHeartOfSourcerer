/***********************************************************************//**
 *  TxtIterator.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/11/27.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_TEXT_TXT_ITERATOR_H_
#define _TFW_STRING_TEXT_TXT_ITERATOR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "TxtChar.h"
#include "TxtPos.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Txt;

/*---------------------------------------------------------------------*//**
 *	テキスト イテレータ
 *
**//*---------------------------------------------------------------------*/
class TxtIterator : public TxtPos
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool has() const;
	bool next();
	bool prev();
	TxtChar cur();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TxtIterator(const Txt* txtRef);
	TxtIterator(const Txt* txtRef, u32 indexRaw, u32 indexChar);
	TxtIterator(const Txt* txtRef, const TxtPos* pos);
	TxtIterator(const TxtIterator& src);
	void copy(const TxtIterator* src);

	//======================================================================
	// 変数
protected:
	const Txt* _txtRef;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_TEXT_TXT_POS_H_
