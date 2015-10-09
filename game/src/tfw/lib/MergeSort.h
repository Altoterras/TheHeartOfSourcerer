/***********************************************************************//**
 *	MergeSort.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2011/09/02.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_MERGE_SORT_H_
#define _TFW_LIB_MERGE_SORT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	マージ ソート
 *
**//*---------------------------------------------------------------------*/
class MergeSort
{
public:
	typedef int (* CompareFunc)(const void* a, const void* b, void* param);

public:
	static bool sort(void* src, void* tmp, u32 sizeElement, u32 numElement, CompareFunc func, u32 numSortUnit, void* param);

private:
	static bool merge(void* dst, const void* src, u32 n1, u32 n2, u32 sizeElm, CompareFunc func, void* param);
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_LIB_MERGE_SORT_H_

