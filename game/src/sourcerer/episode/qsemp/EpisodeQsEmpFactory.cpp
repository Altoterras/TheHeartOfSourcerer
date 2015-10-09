/***********************************************************************//**
 *	EpisodeQsEmpFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/06.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EpisodeQsEmpFactory.h"

// Friends
#include "implement/QsEmp_0_1001_ReleaseUserFreeSource.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EpisodeQsEmpFactory メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	イベントマター作成
**//*---------------------------------------------------------------------*/
EventMatter* EpisodeQsEmpFactory::newEventMatter(s32 questid, u32 caseid)
{
	switch(questid)
	{
	case 0:
		switch(caseid)
		{
		case 1001:	return new QsEmp_0_1001_ReleaseUserFreeSource();
		}
	}

	return 0L;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
