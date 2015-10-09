/***********************************************************************//**
 *	TxtPosArray.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2011/12/02.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TxtPosArray.h"

// Friends
#include "TxtPos.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TxtPosArray メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TxtPosArray::TxtPosArray() :
	_tposarr(0L),
	_num(0)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
		効率のため、外部のポインタをそのまま保持するので、
		外部でインスタンスを削除しないように
**//*---------------------------------------------------------------------*/
TxtPosArray::TxtPosArray(TxtPos** strarrEntr, int num) :
	_tposarr(strarrEntr),
	_num(num)
{
}

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ
**//*---------------------------------------------------------------------*/
TxtPosArray::TxtPosArray(const TxtPosArray& src) :
	_tposarr(0L),
	_num(0)
{
	copy(src);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
		少ない数の配列定義用
		存在しないインスタンスには NULL(0L) を指定する
**//*---------------------------------------------------------------------*/
TxtPosArray::TxtPosArray(TxtPos* tpos1) : _tposarr(0L), _num(0)	{	set(tpos1, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L);	}
TxtPosArray::TxtPosArray(TxtPos* tpos1, TxtPos* tpos2) : _tposarr(0L), _num(0)	{	set(tpos1, tpos2, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L);	}
TxtPosArray::TxtPosArray(TxtPos* tpos1, TxtPos* tpos2, TxtPos* tpos3) : _tposarr(0L), _num(0)	{	set(tpos1, tpos2, tpos3, 0L, 0L, 0L, 0L, 0L, 0L, 0L);	}
TxtPosArray::TxtPosArray(TxtPos* tpos1, TxtPos* tpos2, TxtPos* tpos3, TxtPos* tpos4) : _tposarr(0L), _num(0)	{	set(tpos1, tpos2, tpos3, tpos4, 0L, 0L, 0L, 0L, 0L, 0L);	}
TxtPosArray::TxtPosArray(TxtPos* tpos1, TxtPos* tpos2, TxtPos* tpos3, TxtPos* tpos4, TxtPos* tpos5) : _tposarr(0L), _num(0)	{	set(tpos1, tpos2, tpos3, tpos4, tpos5, 0L, 0L, 0L, 0L, 0L);	}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TxtPosArray::~TxtPosArray()
{
	clear();
}

/*---------------------------------------------------------------------*//**
	コピー演算子
**//*---------------------------------------------------------------------*/
TxtPosArray& TxtPosArray::operator=(const TxtPosArray& src)
{
	copy(src);
	return *this;
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void TxtPosArray::copy(const TxtPosArray& src)
{
	clear();
	_num = src._num;
	_tposarr = new TxtPos*[_num];
	for(int i = 0; i < _num; i++)
	{
		_tposarr[i] = src._tposarr[i];
	}
}

/*---------------------------------------------------------------------*//**
	クリア（内容を破棄）
**//*---------------------------------------------------------------------*/
void TxtPosArray::clear()
{
	delete[] _tposarr;
	_tposarr = 0L;
	_num = 0;
}

/*---------------------------------------------------------------------*//**
	少ない数の配列定義
**//*---------------------------------------------------------------------*/
void TxtPosArray::set(TxtPos* tpos1, TxtPos* tpos2, TxtPos* tpos3, TxtPos* tpos4, TxtPos* tpos5, TxtPos* tpos6, TxtPos* tpos7, TxtPos* tpos8, TxtPos* tpos9, TxtPos* tpos10)
{
	clear();

	TxtPos* ttmp[] = { tpos1, tpos2, tpos3, tpos4, tpos5, tpos6, tpos7, tpos8, tpos9, tpos10 };

	int cnt = 0;
	for(int i = 0; i < sizeof(ttmp) / sizeof(TxtPos*); i++)
	{
		if(ttmp[i] != 0L)	{	cnt++;	}
	}
	if(cnt > 0)
	{
		_num = cnt;
		_tposarr = new TxtPos*[_num];
		cnt = 0;
		for(int i = 0; i < sizeof(ttmp) / sizeof(TxtPos*); i++)
		{
			if(ttmp[i] != 0L)	{	_tposarr[cnt++] = ttmp[i];	}
		}
	}
}

/*---------------------------------------------------------------------*//**
	配列定義
		効率のため、外部のポインタをそのまま保持するので、
		外部でインスタンスを削除しないように
***//*---------------------------------------------------------------------*/
void TxtPosArray::set(TxtPos** strarr, int num)
{
	clear();

	_tposarr = strarr;
	_num = num;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
