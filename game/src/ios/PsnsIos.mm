/***********************************************************************//**
 *  PsnsIos.cpp
 *  Enlight iOS Application
 *
 *	Created by Ryoutarou Kishi on 2012/04/14.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#import "PsnsIos.h"

// Friends

// External
#import "../sourcerer/episode/save/Trophy.h"

// Library
#import <GameKit/GameKit.h>

////////////////////////////////////////////////////////////////////////////
// 定数

// V01 の達成情報テーブル
const static PsnsIos::AchievementInfo s_aiAchvInfoTableV01[] =
{
	PsnsIos::AchievementInfo(Trophy::TI_ITEM_ADEPT_1_SUCCEEDED_SYNTHESIS, "jp.co.altoterras.sourcerer01.achievement.item_q01"),
	PsnsIos::AchievementInfo(Trophy::TI_ITEM_ADEPT_2_HEALED_BY_ITEM, "jp.co.altoterras.sourcerer01.achievement.item_q02"),
	PsnsIos::AchievementInfo(Trophy::TI_ITEM_ADEPT_3_EXCHANGED_FOR_EXP, "jp.co.altoterras.sourcerer01.achievement.item_q03"),
	PsnsIos::AchievementInfo(Trophy::TI_SOUMA_ADEPT_1_CUSTOMIZED_SOURCE_OF_SOUMA, "jp.co.altoterras.sourcerer01.achievement.souma_q01"),
	PsnsIos::AchievementInfo(Trophy::TI_SOUMA_ADEPT_2_HEALED_BY_CUSTOM_SOUMA, "jp.co.altoterras.sourcerer01.achievement.souma_q02"),
	PsnsIos::AchievementInfo(Trophy::TI_SOUMA_ADEPT_3_DEFEATED_UNGETSU_BY_CUSTOM_SOUMA, "jp.co.altoterras.sourcerer01.achievement.souma_q03"),
	PsnsIos::AchievementInfo(Trophy::TI_SOUMA_ADEPT_4_DEFEATED_SARUNAHA_BY_CUSTOM_SOUMA, "jp.co.altoterras.sourcerer01.achievement.souma_q04"),
	PsnsIos::AchievementInfo(Trophy::TI_SOUMA_ADEPT_5_DEFEATED_KABUNAHA_BY_CUSTOM_SOUMA, "jp.co.altoterras.sourcerer01.achievement.souma_q05"),
	PsnsIos::AchievementInfo(Trophy::TI_SOUMA_ADEPT_6_DEFEATED_KAGEIWA_BY_CUSTOM_SOUMA, "jp.co.altoterras.sourcerer01.achievement.souma_q06"),
	PsnsIos::AchievementInfo(Trophy::TI_EXPLORATION_ADEPT_1_FOUND_HIDDEN_MAP, "jp.co.altoterras.sourcerer01.achievement.explorer_q01"),
	PsnsIos::AchievementInfo(-1, 0L),
};

#define LEADEREBOARD_ID_V01	"jp.co.altoterras.sourcerer01.leaderboard.score"

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PsnsIos メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	現在のプレイヤー名を得る
**//*---------------------------------------------------------------------*/
bool PsnsIos::getCurrentPlayerId(tfw::VcString* idstr) const
{
	if(!isValidGameCenter())
	{
		return false;
	}
	
	const char* szPlayerId = [_vcMainRef.gameCenterManipulator.localPlayer.playerID UTF8String];
	*idstr = szPlayerId;
	return true;
}

/*---------------------------------------------------------------------*//**
	スコアを更新する
**//*---------------------------------------------------------------------*/
bool PsnsIos::updateScore(u32 score)
{
	if(!isValidGameCenter())
	{
		return false;
	}

	NSString* leaderboardId = @LEADEREBOARD_ID_V01;
	#if __has_feature(objc_arc)
		GKScore* scoreSubmit = [[GKScore alloc] initWithCategory:leaderboardId];
	#else
		GKScore* scoreSubmit = [[[GKScore alloc] initWithCategory:leaderboardId] autorelease];
	#endif
	scoreSubmit.value = score;
	
	[_vcMainRef.gameCenterManipulator submitScore:scoreSubmit];
	return true;
}

/*---------------------------------------------------------------------*//**
	トロフィーを更新する
**//*---------------------------------------------------------------------*/
bool PsnsIos::updateTrophy(s32 itp, u8 rate)
{
	if(!isValidGameCenter())
	{
		return false;
	}
	
	const AchievementInfo* ai = findAchvInfo(itp);
	if(ai == 0L)
	{
		return false;
	}
	
	NSString* achievementId = [NSString stringWithCString:ai->_aid encoding:NSUTF8StringEncoding];
	#if __has_feature(objc_arc)
		GKAchievement* achievementSubmit = [[GKAchievement alloc] initWithIdentifier:achievementId];
	#else
		GKAchievement* achievementSubmit = [[[GKAchievement alloc] initWithIdentifier:achievementId] autorelease];
	#endif
	[achievementSubmit setPercentComplete: rate];
	
	[_vcMainRef.gameCenterManipulator submitAchievement:achievementSubmit];
	return true;
}

/*---------------------------------------------------------------------*//**
	全トロフィーをリセットする
**//*---------------------------------------------------------------------*/
bool PsnsIos::resetTrophies()
{
	if(!isValidGameCenter())
	{
		return false;
	}

	[_vcMainRef.gameCenterManipulator resetAchievements];
	
	return true;
}

/*---------------------------------------------------------------------*//**
	有効な PSNS ユーザーアカウントがログインされているか
**//*---------------------------------------------------------------------*/
bool PsnsIos::isValidUserAccount()
{
	return isValidGameCenter();
}

/*---------------------------------------------------------------------*//**
	PSNS ホーム画面を表示する
**//*---------------------------------------------------------------------*/
bool PsnsIos::showPsnsScreen(ScreenKind sk)
{
	if(!isValidGameCenter())
	{
		return false;
	}
	
	switch(sk)
	{
	case SK_SCORE:
		
		{
			NSString* leaderboardId = @LEADEREBOARD_ID_V01;
			[_vcMainRef showLeaderboardViewController:leaderboardId];
		}
		break;
	case SK_TROPHY:
		{
			[_vcMainRef showAchievementsViewController];
		}
		break;
	}
	
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	ゲームセンターの有効性判定
**//*---------------------------------------------------------------------*/
bool PsnsIos::isValidGameCenter() const
{
	if(_vcMainRef == 0L)
	{
		return false;
	}
	if(!_vcMainRef.gameCenterManipulator)
	{
		return false;
	}
	return [_vcMainRef.gameCenterManipulator isValidGameCenterPlayer];
}

/*---------------------------------------------------------------------*//**
	達成情報を探す
**//*---------------------------------------------------------------------*/
const PsnsIos::AchievementInfo* PsnsIos::findAchvInfo(s32 itp) const
{
	int i = 0;
	while(true)
	{
		if(s_aiAchvInfoTableV01[i]._itp == -1)
		{
			return 0L;
		}
		if(s_aiAchvInfoTableV01[i]._itp == itp)
		{
			return &s_aiAchvInfoTableV01[i];
		}
		i++;
	}
}

////////////////////////////////////////////////////////////////////////////
