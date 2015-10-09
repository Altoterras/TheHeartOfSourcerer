/***********************************************************************//**
 *	TrophyBase.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/08/20.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TrophyBase.h"

// Friends

// External
#include "../../tfw/psns/PsnsBase.h"
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TrophyBase メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	トロフィーカウントを得る
**//*---------------------------------------------------------------------*/
u8 TrophyBase::getTrophyAchievementRate(s32 itp) const
{
	if((itp < 0) || (NUM_TROPHY_MAX <= itp))	{	ASSERT(false);	return 0;	}
	return _tprate[itp];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	トロフィーアップ

	@param itp トロフィーインデックス
	@param rate 達成レート 0:0% ～ 100(RATE_MAX):100%
**//*---------------------------------------------------------------------*/
void TrophyBase::achievementTrophy(s32 itp, u8 rate)
{
	if((itp < 0) || (NUM_TROPHY_MAX <= itp))	{	ASSERT(false);	return;	}

	if(rate > RATE_MAX)
	{
		rate = RATE_MAX;
	}
	if(_tprate[itp] != rate)
	{
		// 変更
		_tprate[itp] = rate;
		TFW_SET_FLAG(_tpflags[itp], F_CHANGED, true);	// 変更フラグ ON
		updatePsnsTrophy();								// デフォルト実装では自動更新する
	}
}

/*---------------------------------------------------------------------*//**
	PSNS のトロフィーを更新する
**//*---------------------------------------------------------------------*/
bool TrophyBase::updatePsnsTrophy()
{
	if(_psnsRef == 0L)	{	return false;	}

	for(int itp = 0; itp < NUM_TROPHY_MAX; itp++)
	{
		if(TFW_IS_FLAG(_tpflags[itp], F_CHANGED))
		{
			// PSNS トロフォー情報の更新
			if(!_psnsRef->updateTrophy(itp, _tprate[itp]))
			{
				return false;
			}

			TFW_SET_FLAG(_tpflags[itp], F_CHANGED, false);	// 変更フラグ OFF
		}
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TrophyBase::TrophyBase(PsnsBase* psnsRef)
	: _psnsRef(psnsRef)
{
	TFW_ZERO_MEMORY(_tprate, sizeof(u16) * NUM_TROPHY_MAX);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
