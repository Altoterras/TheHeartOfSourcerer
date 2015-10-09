/***********************************************************************//**
 *	MergeSort.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2011/09/02.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MergeSort.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MergeSort メソッド

/*---------------------------------------------------------------------*//**
	ソート
**//*---------------------------------------------------------------------*/
bool MergeSort::sort(void* src, void* tmp, u32 sizeElement, u32 numElement, CompareFunc func, u32 numSortUnit, void* param)
{
	u32 iLo, iHi;
	void* p1;
	void* p2;
	void* p3;

	for(iLo = 0; iLo < numElement; iLo++)
	{
		iHi = iLo + numSortUnit;
		if(iHi > numElement)
		{
			iHi = numElement;
		}

		for(u32 i = iLo + 1; i < iHi; i++)
		{
			p1 = (u8*)src + (i * sizeElement);
			p2 = (u8*)p1 - sizeElement;

			for(u32 j = i; j > iLo; j--)
			{
				if(func(p2, p1, param) <= 0)
				{
					break;
				}

				TFW_COPY_MEMORY(tmp, p2, sizeElement);
				TFW_COPY_MEMORY(p2, p1, sizeElement);
				TFW_COPY_MEMORY(p1, tmp, sizeElement);
				p1 = p2;
				p2 = (u8*)p1 - sizeElement;
			}
		}
	}

	p1 = src;
	p2 = tmp;
	for(u32 iRun = numSortUnit; iRun < numElement; iRun *= 2)
	{
		for(iLo = 0; iLo < numElement; iLo += 2 * sizeElement)
		{
			iHi = iLo + iRun;
			u32 i = iLo * sizeElement;
			if(iHi >= numElement)
			{
				TFW_COPY_MEMORY((u8*)p2 + i, (u8*)p1 + i, (numElement - iLo) * sizeElement);
				break;
			}

			if(!merge((u8*)p2 + i, (u8*)p1 + i, iRun, ((iHi + iRun) > numElement) ? numElement - iHi : iRun, sizeElement, func, param))
			{
				return false;
			}
		}

		p3 = p1;
		p1 = p2;
		p2 = p3;
	}
	if(src != p1)
	{
		TFW_COPY_MEMORY(src, tmp, numElement * sizeElement);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	配列マージ
**//*---------------------------------------------------------------------*/
bool MergeSort::merge(void* dst, const void* src, u32 n1, u32 n2, u32 sizeElm, CompareFunc func, void* param)
{
	const void* p1;
	const void* p2;
	void* p3;

	u32 nTotal = n1 + n2;

	p2 = (u8*)src + n1 * sizeElm;
	p1 = (u8*)p2 - sizeElm;
	if(func(p1, p2, param) <= 0)
	{
		TFW_COPY_MEMORY(dst, src, nTotal * sizeElm);
		return true;
	}

	p1 = src;
	p3 = dst;
	bool isFrom1;
	while(nTotal > 0)
	{
		isFrom1 = (n2 == 0);
		if(!isFrom1 && (n1 > 0))
		{
			isFrom1 = (func(p1, p2, param) <= 0);
		}

		if(isFrom1)
		{
			TFW_COPY_MEMORY(p3, p1, sizeElm);
			n1--;
			p1 = (u8*)p1 + sizeElm;
		}
		else
		{
			TFW_COPY_MEMORY(p3, p2, sizeElm);
			n2--;
			p2 = (u8*)p2 + sizeElm;
		}

		p3 = (u8*)p3 + sizeElm;
		nTotal--;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
