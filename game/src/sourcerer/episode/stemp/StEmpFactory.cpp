/***********************************************************************//**
 *	StEmpFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/29.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StEmpFactory.h"

// Friends
#include "../StDepEvDef.h"
#include "implement/StEmp_10101_1001_MagicTutorial01.h"
#include "implement/StEmp_10101_1002_MagicTutorial02.h"
#include "implement/StEmp_10102_1001_BattleTutorial01.h"
#include "implement/StEmp_10201_1001_GetShield.h"
#include "implement/StEmp_10202_1001_AsaOnlyEvent.h"
#include "implement/StEmp_10202_1002_UngetsuFirstBattle.h"
#include "implement/StEmp_10203_1001_UngetsuSecondBattle.h"
#include "implement/StEmp_10204_1001_EndOfLiteVersion.h"
#include "implement/StEmp_10204_1002_ClearVol01Lite.h"
#include "implement/StEmp_10401_1001_ClearVol01.h"

// External
#include "../../../tfw/string/StringUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StEmpFactory メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	イベントマター作成
**//*---------------------------------------------------------------------*/
EventMatter* StEmpFactory::newEventMatter(const CStringBase* nameMatter)
{
	EventMatter* matter = 0L;

	VcString strWk(nameMatter);
	VcStringArray sa = StringUtils::split(&strWk, "_");
	if(sa.getCount() == 4)
	{
		s32 storyid = StringUtils::toInteger(sa.get(1));
		u32 caseid = StringUtils::toInteger(sa.get(2));
		matter = newEventMatter(storyid, caseid);
	}
	
	return matter;
}

/*---------------------------------------------------------------------*//**
	イベントマター作成
**//*---------------------------------------------------------------------*/
EventMatter* StEmpFactory::newEventMatter(s32 storyid, u32 caseid)
{
	switch(storyid)
	{
	case 10101:
		switch(caseid)
		{
		case 1001:		return new StEmp_10101_1001_MagicTutorial01();
		case 1002:		return new StEmp_10101_1002_MagicTutorial02();
		}
	case 10102:
		switch(caseid)
		{
		case 1001:		return new StEmp_10102_1001_BattleTutorial01();
		}
	case 10201:
		switch(caseid)
		{
		case 1001:		return new StEmp_10201_1001_GetShield();
		}
		break;
	case 10202:
		switch(caseid)
		{
		case 1001:		return new StEmp_10202_1001_AsaOnlyEvent();
		case 1002:		return new StEmp_10202_1002_UngetsuFirstBattle();
		}
		break;
	case 10203:
		switch(caseid)
		{
		case 1001:		return new StEmp_10203_1001_UngetsuSecondBattle();
		}
	case 10204:
		switch(caseid)
		{
		case 1001:		return new StEmp_10204_1001_EndOfLiteVersion();
		case 1002:		return new StEmp_10204_1002_ClearVol01Lite();
		}
	case 10401:
		switch(caseid)
		{
		case 1001:		return new StEmp_10401_1001_ClearVol01();
		}
	}

	return 0L;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
