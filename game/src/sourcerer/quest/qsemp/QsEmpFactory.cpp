/***********************************************************************//**
 *	QsEmpFactory.cpp
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
#include "QsEmpFactory.h"

// Friends
#include "implement/QsEmp_Fixed_AchievePerform.h"
#include "implement/QsEmp_Fixed_ActualCancel.h"
#include "implement/QsEmp_Fixed_ActualFinish.h"
#include "implement/QsEmp_CallFinish.h"
#include "implement/QsEmp_ConfirmCancel.h"
#include "implement/QsEmp_ConfirmContract.h"
#include "implement/QsEmp_ConfirmHandOver.h"
#include "../../episode/qsemp/EpisodeQsEmpFactory.h"

// External
#include "../../../tfw/string/StringUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QsEmpFactory メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	イベントマター作成
**//*---------------------------------------------------------------------*/
EventMatter* QsEmpFactory::newEventMatter(const CStringBase* nameMatter)
{
	EventMatter* matter = 0L;

	VcString strWk(nameMatter);
	VcStringArray sa = StringUtils::split(&strWk, "_");
	if(sa.getCount() == 4)	// QsEmp_nnnn_nnnn_xxxx
	{
		s32 questid = StringUtils::toInteger(sa.get(1));
		u32 caseid = StringUtils::toInteger(sa.get(2));
		matter = EpisodeQsEmpFactory::newEventMatter(questid, caseid);
	}
	else if(sa.getCount() == 2)	// QsEmp_xxxx
	{
		matter = newQuestEventMatter(sa.get(1));
	}
	else if((sa.getCount() == 3) && (sa.get(1)->equals("Fixed")))	// QsEmp_Fixed_xxxx
	{
		matter = newQuestEventMatter(sa.get(2));
	}
	
	return matter;
}

/*---------------------------------------------------------------------*//**
	汎用イベントマター作成
**//*---------------------------------------------------------------------*/
EventMatter* QsEmpFactory::newQuestEventMatter(const VcString* name)
{
	if(name->equals("AchievePerform"))
	{
		return new QsEmp_Fixed_AchievePerform();
	}
	else if(name->equals("ActualCancel"))
	{
		return new QsEmp_Fixed_ActualCancel();
	}
	else if(name->equals("ActualFinish"))
	{
		return new QsEmp_Fixed_ActualFinish();
	}
	else if(name->equals("CallFinish"))
	{
		return new QsEmp_CallFinish();
	}
	else if(name->equals("ConfirmCancel"))
	{
		return new QsEmp_ConfirmCancel();
	}
	else if(name->equals("ConfirmContract"))
	{
		return new QsEmp_ConfirmContract();
	}
	else if(name->equals("ConfirmHandOver"))
	{
		return new QsEmp_ConfirmHandOver();
	}

	return 0L;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
