/***********************************************************************//**
 *	TfwTime.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2013/06/11.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TfwTime.h"

// Friends

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TfwTime メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TfwTime::TfwTime()
	: _msec(0)
	, _sec(0)
	, _min(0)
	, _hour(0)
	, _mday(0)
	, _mon(0)
	, _year(0)
{
}

/*---------------------------------------------------------------------*//**
	64bit 数値から時間
**//*---------------------------------------------------------------------*/
void TfwTime::convU64ToTime(TfwTime* time, u64 t)
{
	time->_msec = t & 0x3ff;	// milli-seconds after the minute [0-999]
	t >>= 10;
	time->_sec = t & 0x3f;		// seconds after the minute [0-59]
	t >>= 6;
	time->_min = t & 0x3f;		// minutes after the hour [0-59]
	t >>= 6;
	time->_hour = t & 0x1f;		// hours since midnight [0-23]
	t >>= 5;
	time->_mday = t & 0x1f;		// day of the month [1-31]
	t >>= 5;
	time->_mon = t & 0xf;		// months since January [0-11]
	t >>= 4;
	time->_year = t;			// years since 1900
}

/*---------------------------------------------------------------------*//**
	時間から 64bit
**//*---------------------------------------------------------------------*/
void TfwTime::convTimeToU64(u64* t, const TfwTime* time)
{
	*t = time->_year;
	*t <<= 4;
	*t |= (time->_mon & 0xf);
	*t <<= 5;
	*t |= (time->_mday & 0x1f);
	*t <<= 5;
	*t |= (time->_hour & 0x1f);
	*t <<= 6;
	*t |= (time->_min & 0x3f);
	*t <<= 6;
	*t |= (time->_sec & 0x3f);
	*t <<= 10;
	*t |= (time->_msec & 0x3ff);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
