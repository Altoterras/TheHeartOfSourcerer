/***********************************************************************//**
 *	TfwTime.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2013/06/11.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_TFW_TIME_H_
#define _TFW_LIB_TFW_TIME_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include <math.h>

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	時間
 *
**//*---------------------------------------------------------------------*/
class TfwTime
{
	//======================================================================
	// メソッド
public:
	static void convU64ToTime(TfwTime* time, u64 t);
	static void convTimeToU64(u64* t, const TfwTime* time);
	
	TfwTime();
	
	//======================================================================
	// 変数
public:
	s32 _msec;	// milli-seconds after the minute [0-999]
	s32	_sec;	// seconds after the minute [0-59]
	s32	_min;	// minutes after the hour [0-59]
	s32	_hour;	// hours since midnight [0-23]
	s32	_mday;	// day of the month [1-31]
	s32	_mon;	// months since January [0-11]
	s32	_year;	// years since 1900
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_LIB_TFW_TIME_H_

