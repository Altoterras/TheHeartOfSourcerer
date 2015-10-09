/***********************************************************************//**
 *	StopWatch.h
 *	Enlight Windows Application
 *	
 *	Created by Ryoutarou Kishi on 2009/09/11.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#pragma once

#include "../tfw/lib/StopWatch.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace app
{

/*---------------------------------------------------------------------*//**
	ストップウォッチ

**//*---------------------------------------------------------------------*/
class StopWatch : public tfw::StopWatch
{
public:
	StopWatch();
	virtual void	start();
	virtual f32		getElapsedSeconds();

private:
	__int64			_start;
	f32				_period;
};

}	// namespace app
