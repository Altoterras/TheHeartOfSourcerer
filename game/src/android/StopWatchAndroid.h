/***********************************************************************//**
 *	StopWatchAndroid.h
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2009/09/11.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#if !defined(_STOP_WATCH_ANDROID_H_)
#define _STOP_WATCH_ANDROID_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../tfw/lib/StopWatch.h"
#include "CompilerSwitch.h"

// Library
#include <sys/time.h>

////////////////////////////////////////////////////////////////////////////
// 型

typedef struct timeval TIMEVALUE;

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	ストップウォッチ
 *
**//*---------------------------------------------------------------------*/
class StopWatchAndroid : public tfw::StopWatch
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	StopWatchAndroid();
	virtual void start();
	virtual f32 getElapsedSeconds();

	TIMEVALUE getTimeNow();
	TIMEVALUE subtractTime(TIMEVALUE a, TIMEVALUE b);
	
	//======================================================================
	// 変数
private:
	TIMEVALUE _timePrev;
};

////////////////////////////////////////////////////////////////////////////

#endif	// _STOP_WATCH_ANDROID_H_

