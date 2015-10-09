/***********************************************************************//**
 *	QuickSort.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2012/03/07.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_QUICK_SORT_H_
#define _TFW_LIB_QUICK_SORT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	クイック ソート
 *
**//*---------------------------------------------------------------------*/
class QuickSort
{
	//======================================================================
	// 型
public:
	typedef s32 (* CompareFunc)(const void* a, const void* b);
	typedef void (* SwapFunc)(void* a, void* b);

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void run(void* arr, s32 unitsize, s32 length);

private:
	void sort(void* arr, s32 unitsize, s32 idxFirst, s32 idxLast);
	s32 pivot(void* arr, s32 unitsize, s32 idxA, s32 idxB);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QuickSort();
	QuickSort(CompareFunc funcCompare, SwapFunc funcSwap);

protected:
	virtual s32 compare(void* arr, s32 unitsize, s32 idxA, s32 idxB);
	virtual void swap(void* arr, s32 unitsize, s32 idxA, s32 idxB);

	//======================================================================
	// 変数
private:
	CompareFunc _funcCompare;
	SwapFunc _funcSwap;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_LIB_QUICK_SORT_H_

