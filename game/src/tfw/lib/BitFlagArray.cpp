/***********************************************************************//**
 *	BitFlagArray.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2011/08/05.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "BitFlagArray.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define GET_FLAG(arr_, id_)			(((arr_)[(id_) >> 3] & (0x1 << ((id_) & 0x7))) != 0)
#define SET_FLAG(arr_, id_, b_)		((arr_)[(id_) >> 3] = ((arr_)[(id_) >> 3] & ~(0x1 << ((id_) & 0x7))) | (((b_) << ((id_) & 0x7)) /*& ~(0x1 << ((id_) & 0x7))*/))

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// 静的メンバ変数

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	フラグが使用済みかを判定
**//*---------------------------------------------------------------------*/
bool BitFlagArray::getFlag(u16 flag) const
{
	if(_numFlag <= flag)
	{
		ASSERT(false);
		return false;
	}
	return GET_FLAG(_arrFlag, flag);
}

/*---------------------------------------------------------------------*//**
	フラグが使用済みかを設定
**//*---------------------------------------------------------------------*/
bool BitFlagArray::setFlag(u16 flag, bool on)
{
	if(_numFlag <= flag)
	{
		ASSERT(false);
		return false;
	}
	SET_FLAG(_arrFlag, flag, on);
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
BitFlagArray::BitFlagArray()
	: _numFlag(0)
	, _arrFlag(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
BitFlagArray::~BitFlagArray()
{
	ASSERT(_arrFlag == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool BitFlagArray::create(u32 numFlag)
{
	u32 numArrNew = numFlag / 8;
	u8* arrFlagNew = new u8[numArrNew];
	if(arrFlagNew == 0L)	{	return false;	}
	TFW_ZERO_MEMORY(arrFlagNew, sizeof(u8) * numArrNew);

	_arrFlag = arrFlagNew;
	_numFlag = numFlag;

	return true;
}

/*---------------------------------------------------------------------*//**
	再作成
**//*---------------------------------------------------------------------*/
bool BitFlagArray::recreate(u32 numFlagNew)
{
	u8* arrFlagOld = _arrFlag;
	u32 numArrOld = _numFlag / 8;

	u32 numArrNew = numFlagNew / 8;
	u8* arrFlagNew = new u8[numArrNew];
	if(arrFlagNew == 0L)	{	return false;	}
	if(numArrNew >= numArrOld)
	{
		TFW_COPY_MEMORY(arrFlagNew, arrFlagOld, sizeof(u8) * numArrOld);
		if(numArrNew > numArrOld)
		{
			TFW_ZERO_MEMORY(&arrFlagNew[numArrOld], sizeof(u8) * (numArrNew - numArrOld));
		}
	}
	else
	{
		TFW_COPY_MEMORY(arrFlagNew, arrFlagOld, sizeof(u8) * numArrNew);
	}
	delete[] arrFlagOld;

	_arrFlag = arrFlagNew;
	_numFlag = numFlagNew;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void BitFlagArray::destroy()
{
	delete[] _arrFlag;
	_arrFlag = 0L;
	_numFlag = 0;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS

