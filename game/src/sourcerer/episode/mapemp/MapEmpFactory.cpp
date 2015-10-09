/***********************************************************************//**
 *	MapEmpFactory.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/31.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MapEmpFactory.h"

// Friends
#include "../StDepEvDef.h"
#include "implement/MapEmp_10221_1001_LostCheck.h"
#include "implement/MapEmp_10241_1001_OutAfterRestAtHome.h"
#include "implement/MapEmp_10242_1701_RestAtHome.h"
#include "implement/MapEmp_10243_1001_DoorControl.h"
#include "implement/MapEmp_10243_1002_HandoverItemToCollector.h"

// External
#include "../../../tfw/string/StringUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapEmpFactory メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	イベントマター作成
**//*---------------------------------------------------------------------*/
EventMatter* MapEmpFactory::newEventMatter(const CStringBase* nameMatter)
{
	EventMatter* matter = 0L;

	VcString strWk(nameMatter);
	VcStringArray sa = StringUtils::split(&strWk, "_");
	if(sa.getCount() == 4)
	{
		s32 mapid = StringUtils::toInteger(sa.get(1));
		u32 caseid = StringUtils::toInteger(sa.get(2));
		matter = newEventMatter(mapid, caseid);
	}
	
	return matter;
}

/*---------------------------------------------------------------------*//**
	イベントマター作成
**//*---------------------------------------------------------------------*/
EventMatter* MapEmpFactory::newEventMatter(s32 mapid, u32 caseid)
{
	switch(mapid)
	{
	case 10221:
		switch(caseid)
		{
		case 1001: return new MapEmp_10221_1001_LostCheck();
		}
	case 10241:
		switch(caseid)
		{
		case 1001: return new MapEmp_10241_1001_OutAfterRestAtHome();
		}
		break;
	case 10242:
		switch(caseid)
		{
		case StDepEvDef::ECID_EP01_MAP_REST_AT_HOME: return new MapEmp_10242_1701_RestAtHome();
		}
		break;
	case 10243:
		switch(caseid)
		{
		case 1001: return new MapEmp_10243_1001_DoorControl();
		case 1002: return new MapEmp_10243_1002_HandoverItemToCollector();
		}
		break;
	}

	return 0L;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
