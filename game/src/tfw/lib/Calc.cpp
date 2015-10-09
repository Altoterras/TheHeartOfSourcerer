/***********************************************************************//**
 *	Calc.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2015/03/04.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Calc.h"

// Friends
#include "Vector.h"

// External

// Library
#include <math.h>

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Calc メソッド

/*---------------------------------------------------------------------*//**
 *	簡易 sin
 *		★★★ 計測する限り、math.h の ::sinf の方が速かった ★★★
 *	
**//*---------------------------------------------------------------------*/
f32 sinSeries(f32 radian, int i, int maxi)
{
	if(i > maxi)
	{
		return 1.0f;
	}
	return 1.0f - (radian * radian / (2.0f * i + 2.0f) / (2.0f * i + 3.0f) * sinSeries(radian, i + 1, maxi));
}
f32 Calc::sin(f32 radian)
{
	return radian * sinSeries(radian, 0, 8);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
