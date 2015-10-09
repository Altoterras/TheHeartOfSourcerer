/***********************************************************************//**
 *	StopWatchIos.h
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2009/09/11.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#if !defined(_STOP_WATCH_IOS_H_)
#define _STOP_WATCH_IOS_H_

////////////////////////////////////////////////////////////////////////////
// マクロ

#define USE_UNIX_TIME	1

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../tfw/lib/StopWatch.h"

#if USE_UNIX_TIME
	#include <sys/time.h>
#endif

////////////////////////////////////////////////////////////////////////////
// 型

#if USE_UNIX_TIME
	typedef struct timeval TIMEVALUE;
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	ストップウォッチ
 *
**//*---------------------------------------------------------------------*/
class StopWatchIos : public tfw::StopWatch
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	StopWatchIos();
	virtual void start();
	virtual f32 getElapsedSeconds();

	#if USE_UNIX_TIME
		TIMEVALUE getTimeNow();
		TIMEVALUE subtractTime(TIMEVALUE a, TIMEVALUE b);
	#endif
	
	//======================================================================
	// 変数
private:
	#if USE_UNIX_TIME
		TIMEVALUE _timePrev;
	#else
		NSDate* _start;
	#endif
};

////////////////////////////////////////////////////////////////////////////

#endif	// _STOP_WATCH_IOS_H_

