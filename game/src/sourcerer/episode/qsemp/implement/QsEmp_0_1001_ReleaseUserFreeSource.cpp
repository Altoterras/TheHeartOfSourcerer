/***********************************************************************//**
 *	QsEmp_0_1001_ReleaseUserFreeSource.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/20.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "QsEmp_0_1001_ReleaseUserFreeSource.h"

// Friends
#include "../EpisodeQsEmpType.h"
#include "../../../body/Conductor.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/Party.h"
#include "../../../source/SourceManager.h"

// External
#include "../../../../tfw/string/StringUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// QsEmp_0_1001_ReleaseUserFreeSource メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void QsEmp_0_1001_ReleaseUserFreeSource::begin(u32 param)
{
	QsEmp::begin(param);

	// パラメータを得る
	s32 numFreeMax = 0;
	s32 numRelease = 0;
	for(int i = 0; i < this->getParamNum(); i++)
	{
		const VcString* key = this->getParamKey(i);
		if(key != 0L)
		{
			if(key->equals("release_num"))
			{
				const VcString* valu = this->getParamValue(i);
				numRelease = StringUtils::toInteger(valu);
			}
			else if(key->equals("max_free_num"))
			{
				const VcString* valu = this->getParamValue(i);
				numFreeMax = StringUtils::toInteger(valu);
			}
		}
	}

	// ユーザーソース解放
	SourceManager* srcmng = Game::getGame()->getMyParty()->getSourceManager(); ASSERT(srcmng != 0L);
	s32 numReleased = srcmng->releaseReservedUserSource(numRelease, numFreeMax);

	// メッセージ表示
	if(numReleased > 0)
	{
		Conductor* conductor = Game::getGame()->getConductor(); ASSERT(conductor != 0L);
		VcString msg;
		ColorU8 colorMsg;
		Conductor::getSysRewardColor(&colorMsg);
		if(numReleased == 1)
		{
			msg.set(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_ADD_ONE_FREE_PAGE)->getRaw(), "\n"));
		}
		else
		{
			msg.set(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_ADD_PLURAL_FREE_PAGES)->getRaw(), numReleased, "\n"));
		}
		conductor->on(&msg, &colorMsg, 0);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
QsEmp_0_1001_ReleaseUserFreeSource::QsEmp_0_1001_ReleaseUserFreeSource()
	: QsEmp(EPQSEMPTYPE_0_1001)
{
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
