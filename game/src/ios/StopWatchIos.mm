/***********************************************************************//**
 *	StopWatchIos.mm
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2009/09/11.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "StopWatchIos.h"

#include <sys/time.h>

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StopWatchIos メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StopWatchIos::StopWatchIos()
{
	#if USE_UNIX_TIME
		_timePrev = getTimeNow();
	#else
		_start = 0L;
	#endif
}

#if USE_UNIX_TIME
/*---------------------------------------------------------------------*//**
	現在の時間を得る
**//*---------------------------------------------------------------------*/
TIMEVALUE StopWatchIos::getTimeNow()
{
	TIMEVALUE tv;
	gettimeofday(&tv, 0);
	return tv;
}

/*---------------------------------------------------------------------*//**
	時間の差を得る
**//*---------------------------------------------------------------------*/
TIMEVALUE StopWatchIos::subtractTime(TIMEVALUE a, TIMEVALUE b)
{
	TIMEVALUE tv;
	tv.tv_usec = a.tv_usec - b.tv_usec;
	tv.tv_sec = a.tv_sec - b.tv_sec + (tv.tv_usec / 1000000);
	tv.tv_usec %= 1000000;
	return tv;
}
#endif	// #if USE_UNIX_TIME

/*---------------------------------------------------------------------*//**
	計測開始
**//*---------------------------------------------------------------------*/
void StopWatchIos::start()
{
	#if USE_UNIX_TIME
		_timePrev = getTimeNow();
	#else
		_start = [[NSDate alloc] init];
	#endif
}

/*---------------------------------------------------------------------*//**
	経過時間を得る
**//*---------------------------------------------------------------------*/
f32 StopWatchIos::getElapsedSeconds()
{
	#if USE_UNIX_TIME
		TIMEVALUE timeDiff = subtractTime(getTimeNow(), _timePrev);
		return timeDiff.tv_sec + (timeDiff.tv_usec * 0.000001f);
	#else
		if(_start == 0L)	{	return 0.0f;	}
		return - [_start timeIntervalSinceNow]; 
	#endif
}
