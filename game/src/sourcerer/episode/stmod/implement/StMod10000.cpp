/***********************************************************************//**
 *	StMod10000.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/14.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StMod10000.h"

// Friends
#include "../../MapIdDef.h"
#include "../../StDepEvDef.h"
#include "../../StoryManager.h"
#include "../../char/CharIdDef.h"
#include "../../party/MyParty.h"
#include "../../save/SaveIdDef.h"
#include "../../save/Trophy.h"
#include "../../worldmap/WorldMap.h"
#include "../../worldmap/implement/WorldMapStateUpdaterVol01.h"
#include "../../../char/CharDef.h"
#include "../../../effect/AutoDecorationEffect.h"
#include "../../../event/EventSys.h"
#include "../../../body/Game.h"
#include "../../../save/SaveData.h"
#include "../../../save/SaveSys.h"
#include "../../../status/StatusChangeResult.h"
#include "../../../status/TransStat.h"
#include "../../../unit/char/CharUnit.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StMod10000 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
StMod10000::StMod10000()
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
StMod10000::~StMod10000()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool StMod10000::create(StoryManager* strymngRef)
{
	if(!StMod::create(strymngRef))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void StMod10000::destroy()
{
	StMod::destroy();
}

/*---------------------------------------------------------------------*//**
	パーティ編成を決定する
**//*---------------------------------------------------------------------*/
void StMod10000::onDecisionParty(s32 mapidNew, s32 mapidOld)
{
	bool isHelpPcCharEnable = true;

	// 村にヘルパーは入れない
	if(isHelpPcCharEnable)
	{
		bool isInVillage = MapIdDef::isBaseVillage(mapidNew);
		if(isInVillage)	{	isHelpPcCharEnable = false;	}
	}

	// ストーリーによる編成
	if(isHelpPcCharEnable)
	{
		if(	(StoryIdDef::STID_EP01_OP__START_ <= _strymngRef->getCurrentStoryId()) &&
			(_strymngRef->getCurrentStoryId() <= StoryIdDef::STID_EP01_CORPO__END_)	)
		{
			isHelpPcCharEnable = false;
		}
	}

	// セーブフラグ参照
	if(isHelpPcCharEnable)
	{
		SaveData* svd = Game::getGame()->getSaveSys()->getSaveData(); ASSERT(svd != 0L);
		if(!svd->getSaveF(SaveIdDef::SFID_ST10204_MEET_KOTSUME))	// コツメと合流していない
		{
			isHelpPcCharEnable = false;
		}
	}

	Game::getGame()->getMyParty()->setEnablePcChar(MyParty::PC_HLP, isHelpPcCharEnable);
}

/*---------------------------------------------------------------------*//**
	リアルタイムムービーの再生状態変更通知
**//*---------------------------------------------------------------------*/
void StMod10000::notifyChangedRtmvPlaying(bool isPlay, u16 evidRtmvMatter)
{
	// テクスチャ フェイシャル エフェクトを開始・終了
	AutoDecorationEffect* adeff = Game::getGame()->getAutoDecorationEffect(); ASSERT(adeff != 0L);
	if(isPlay)	// 再生開始
	{
		// 固定キャラの登録
		adeff->registerChar(CharIdDef::CHARID_PC1, true);
		adeff->registerChar(CharIdDef::CHARID_PC2, true);
		adeff->registerChar(CharIdDef::CHARID_PC3, true);
		// 開始
		adeff->start(evidRtmvMatter);
	}
	else		// 停止
	{
		// 終了
		adeff->end(evidRtmvMatter);
	}
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void StMod10000::notifyChangedMap(s32 mapidNew, s32 mapidOld)
{
	// トロフィー処理
	#if defined(_ENLIGHT_V01) && ENABLE_PSNS
		if(mapidNew == MapIdDef::MAPID_EP01_FOREST4_SECRET)
		{
			// 隠しマップ発見
			Game::getGame()->getTrophy()->achievementTrophy(Trophy::TI_EXPLORATION_ADEPT_1_FOUND_HIDDEN_MAP, 100);
		}
	#endif

	StMod::notifyChangedMap(mapidNew, mapidOld);
}

/*---------------------------------------------------------------------*//**
	ユニットのステータス影響通知
**//*---------------------------------------------------------------------*/
void StMod10000::notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat)
{
	// トロフィー処理
	#if defined(_ENLIGHT_V01) && ENABLE_PSNS
		const Unit* unitRecp = scres->getRecipientUnit();
		if((unitRecp != 0L) && (unitRecp->getUnitType() == UNITTYPE_ENEMY_CHAR) && TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_IN_DOWN))	// 敵を倒した
		{
			// 敵を倒したとき
			if(TFW_IS_FLAG(tstat->getSubFlags(), TransStat::SF_USER_CUSTOMIZED_SOUMA))	// ユーザー独自ソウマによって敵を倒した
			{
				switch(((CharUnit*)unitRecp)->getCharDef()->getCharDefId())
				{
				case CharIdDef::CHARID_ENEMY_UNGETSUIWA:	// 雲月岩
					Game::getGame()->getTrophy()->achievementTrophy(Trophy::TI_SOUMA_ADEPT_3_DEFEATED_UNGETSU_BY_CUSTOM_SOUMA, 100);
					break;
				case CharIdDef::CHARID_ENEMY_H_SARU:		// サルナハ
					Game::getGame()->getTrophy()->achievementTrophy(Trophy::TI_SOUMA_ADEPT_4_DEFEATED_SARUNAHA_BY_CUSTOM_SOUMA, 100);
					break;
				case CharIdDef::CHARID_ENEMY_H_KABUTOMUSHI:	// カブナハ
					Game::getGame()->getTrophy()->achievementTrophy(Trophy::TI_SOUMA_ADEPT_5_DEFEATED_KABUNAHA_BY_CUSTOM_SOUMA, 100);
					break;
				case CharIdDef::CHARID_ENEMY_KAGEIWA:		// 影岩
					Game::getGame()->getTrophy()->achievementTrophy(Trophy::TI_SOUMA_ADEPT_6_DEFEATED_KAGEIWA_BY_CUSTOM_SOUMA, 100);
					break;
				}
			}
		}
		else if(
			(	TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_ACT_E_RECV)			// エネルギー回復
				|| TFW_IS_FLAG(scres->getChangeFlags(), StatusChangeResult::CSFLAG_ACT_P_RECV)	)	// 物理回復
			&& TFW_IS_FLAG(unitRecp->getUnitFlags(), Unit::UNITFLAG_PC)	)	// 味方が
		{
			if((scres->getEneValuedDamage() < 0) || (scres->getPhysValuedDamage() < 0))	// 回復量が１以上
			{
				// 味方が回復した時
				if(tstat->getKind() == TransStat::KIND_ITEM)	// アイテムによって味方が回復
				{
					Game::getGame()->getTrophy()->achievementTrophy(Trophy::TI_ITEM_ADEPT_2_HEALED_BY_ITEM, 100);
				}
				else if(TFW_IS_FLAG(tstat->getSubFlags(), TransStat::SF_USER_CUSTOMIZED_SOUMA))	// ユーザー独自ソウマによって味方が回復
				{
					Game::getGame()->getTrophy()->achievementTrophy(Trophy::TI_SOUMA_ADEPT_2_HEALED_BY_CUSTOM_SOUMA, 100);
				}
			}
		}
	#endif

	StMod::notifyUnitInfluenceStat(unitRecv, scres, tstat);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
