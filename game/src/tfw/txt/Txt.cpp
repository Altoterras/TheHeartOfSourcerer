/***********************************************************************//**
 *  Txt.cpp
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
#include "Txt.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	両端空白削除

	@return トリミングを行ったかどうか
**//*---------------------------------------------------------------------*/
bool Txt::trim(TxtPosArray* tpaSub, const TxtChar* tcTrim)
{
	bool isTrimLeft = trimLeft(tpaSub, tcTrim);
	return trimRight(tpaSub, tcTrim) || isTrimLeft;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Txt::~Txt()
{
}

/*---------------------------------------------------------------------*//**
	全て削除
**//*---------------------------------------------------------------------*/
void Txt::removeAll()
{
	TxtPos pos;
	getHearPos(&pos);
	remove(&pos, getCharCount());
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void Txt::copy(const Txt* src)
{
	removeAll();
	TxtPos pos;
	getHearPos(&pos);
	insert(&pos, src);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS