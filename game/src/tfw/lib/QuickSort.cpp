/***********************************************************************//**
 *	QuickSort.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2012/03/07.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QuickSort.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QuickSort メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ソート実行
**//*---------------------------------------------------------------------*/
void QuickSort::run(void* arr, s32 unitsize, s32 length)
{
	sort(arr, unitsize, 0, length - 1);
}

/*---------------------------------------------------------------------*//**
	ソート処理
**//*---------------------------------------------------------------------*/
void QuickSort::sort(void* arr, s32 unitsize, s32 idxFirst, s32 idxLast)
{
	if(idxFirst == idxLast)
	{
		return;
	}
	s32 idxP = pivot(arr, unitsize, idxFirst, idxLast);
	if(idxP == -1)
	{
		return;
	}

	s32 idxA = idxFirst;
	s32 idxB = idxLast;

	while(idxA <= idxB)
	{
		while((idxA < idxLast) && (compare(arr, unitsize, idxA, idxP) < 0))
		{
			idxA++;
		}
		while((idxB > idxFirst) && (compare(arr, unitsize, idxB, idxP) >= 0))
		{
			idxB--;
		}
		if(idxA > idxB)
		{
			break;
		}
		swap(arr, unitsize, idxA, idxB);
		idxA++;
		idxB--;
	}

	sort(arr, unitsize, idxFirst, idxA - 1);
	sort(arr, unitsize, idxA, idxLast);
}

/*---------------------------------------------------------------------*//**
	軸要素の取得
**//*---------------------------------------------------------------------*/
s32 QuickSort::pivot(void* arr, s32 unitsize, s32 idxA, s32 idxB)
{
	s32 idxW = idxA + 1;
	while((idxW <= idxB) && (compare(arr, unitsize, idxA, idxW) == 0))
	{
		idxW++;
	}
	if(idxW > idxB)
	{
		return -1;	// ソート不要（全て同値）
	}
	if(compare(arr, unitsize, idxA, idxW) >= 0)
	{
		return idxA;
	}
	return idxW;
}

/*---------------------------------------------------------------------*//**
	比較
**//*---------------------------------------------------------------------*/
s32 QuickSort::compare(void* arr, s32 unitsize, s32 idxA, s32 idxB)
{
	ASSERT(_funcCompare != 0L);
	return _funcCompare((void*)((addr_size)arr + (unitsize * idxA)), (void*)((addr_size)arr + (unitsize * idxB)));
}

/*---------------------------------------------------------------------*//**
	置換
**//*---------------------------------------------------------------------*/
void QuickSort::swap(void* arr, s32 unitsize, s32 idxA, s32 idxB)
{
	ASSERT(_funcSwap != 0L);
	_funcSwap((void*)((addr_size)arr + (unitsize * idxA)), (void*)((addr_size)arr + (unitsize * idxB)));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QuickSort::QuickSort()
	: _funcCompare(0L)
	, _funcSwap(0L)
{
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QuickSort::QuickSort(CompareFunc funcCompare, SwapFunc funcSwap)
	: _funcCompare(funcCompare)
	, _funcSwap(funcSwap)
{
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
