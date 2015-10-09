/***********************************************************************//**
 *	GameCenterManipulator.h
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2012/07/02.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インポートファイル

#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

////////////////////////////////////////////////////////////////////////////
// クラス

/*---------------------------------------------------------------------*//**
 *	GameCenter 操作子
 *
**//*---------------------------------------------------------------------*/
@interface GameCenterManipulator : NSObject
{
	NSLock* _writeLock;

	GKLocalPlayer* _localPlayer;
	bool _gameCenterAuthenticationComplete;
	NSMutableArray* _storedScores;
	NSString* _storedScoresFilename;
	NSMutableDictionary* _storedAchievements;
	NSString* _storedAchievementsFilename;
}

@property (readonly, nonatomic) GKLocalPlayer* localPlayer;
@property (readwrite) bool gameCenterAuthenticationComplete;
@property (readonly, nonatomic) NSMutableArray* storedScores;
@property (readonly, nonatomic) NSString* storedScoresFilename;
@property (readonly, nonatomic) NSMutableDictionary* storedAchievements;
@property (readonly, nonatomic) NSString* storedAchievementsFilename;

+ (bool)isGameCenterApiAvailable;
- (bool)isValidGameCenterPlayer;
- (void)resubmitStoredScores;
- (void)loadStoredScores;
- (void)writeStoredScore;
- (void)storeScore:(GKScore*)score;
- (void)submitScore:(GKScore*)score;
- (void)writeStoredAchievements;
- (void)resubmitStoredAchievements;
- (void)loadStoredAchievements;
- (void)storeAchievement:(GKAchievement*)achievement;
- (void)submitAchievement:(GKAchievement*)achievement;
- (void)resetAchievements;

@end
