/***********************************************************************//**
 *	GameCenterManipulator.mm
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2012/07/02
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インポートファイル

#import "GameCenterManipulator.h"

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// GameCenterManipulator 実装

@implementation GameCenterManipulator

@synthesize localPlayer = _localPlayer;
@synthesize gameCenterAuthenticationComplete = _gameCenterAuthenticationComplete;
@synthesize storedScores = _storedScores;
@synthesize storedScoresFilename = _storedScoresFilename;
@synthesize storedAchievements = _storedAchievements;
@synthesize storedAchievementsFilename = _storedAchievementsFilename;

//==========================================================================
// GameCenterManipulator メソッド

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
- (id)init
{
    self = [super init];
    if(self)
	{
        NSString* path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
		GKLocalPlayer* lp = [GKLocalPlayer localPlayer];
		if(lp)
		{
			_localPlayer = lp;
			NSString* playerId = lp.playerID;
			_storedScoresFilename = [[NSString alloc] initWithFormat:@"%@/%@.storedScores.plist", path, playerId];
			_storedAchievementsFilename = [[NSString alloc] initWithFormat:@"%@/%@.storedAchievements.plist", path, playerId];
		}
		else
		{
			_localPlayer = nil;
			_storedScoresFilename = nil;
			_storedAchievementsFilename = nil;
		}
        _writeLock = [[NSLock alloc] init];
		_storedScores = nil;
		_storedAchievements = nil;
		_gameCenterAuthenticationComplete = false;
    }
    return self;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
- (void)dealloc
{
	#if !__has_feature(objc_arc)
		[_writeLock release];
		if(_storedScores)				{	[_storedScores release];				}
		if(_storedAchievements)			{	[_storedAchievements release];			}
		if(_storedScoresFilename)		{	[_storedScoresFilename release];		}
		if(_storedAchievementsFilename)	{	[_storedAchievementsFilename release];	}
		[super dealloc];
	#endif
}

/*---------------------------------------------------------------------*//**
	GameCenter API が有効かを判定する
**//*---------------------------------------------------------------------*/
+ (bool)isGameCenterApiAvailable
{
	// iOS 4.1 以降のデバイスかを判定
    NSString *reqSysVer = @"4.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    BOOL osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);
	if(!osVersionSupported)
	{
		return false;
	}
    
	// GKLocalPlayer API 提供の判定
    Class gcClass = (NSClassFromString(@"GKLocalPlayer"));
    return gcClass != nil;
}

/*---------------------------------------------------------------------*//**
	GameCenter API が有効かを判定する
**//*---------------------------------------------------------------------*/
- (bool)isValidGameCenterPlayer
{
	if(!_gameCenterAuthenticationComplete)
	{
		return false;
	}
	if(!_localPlayer)
	{
		return false;
	}
	if(!_localPlayer.authenticated)
	{
		return false;
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	保存されたリーダーボードの再送を試みる
**//*---------------------------------------------------------------------*/
- (void)resubmitStoredScores
{
    if(_storedScores)
	{
		// Keeping an index prevents new entries to be added when the network is down 
		int index = (int)[_storedScores count] - 1;
		while( index >= 0 )
		{
			GKScore* score = [_storedScores objectAtIndex:index];
			[self submitScore:score];
			[_storedScores removeObjectAtIndex:index];
			index--;
		}
		[self writeStoredScore];
    }
}

/*---------------------------------------------------------------------*//**
	保存されたリーダーボードをファイルから読み込み、未送信分を送信する
**//*---------------------------------------------------------------------*/
- (void)loadStoredScores
{
	NSArray* unarchivedObj = [NSKeyedUnarchiver unarchiveObjectWithFile:_storedScoresFilename];

	if(unarchivedObj)
	{
		_storedScores = [[NSMutableArray alloc] initWithArray:unarchivedObj];
		[self resubmitStoredScores];
	}
	else
	{
		_storedScores = [[NSMutableArray alloc] init];
	}
}

/*---------------------------------------------------------------------*//**
	保存されたリーダーボードをファイルに書き込む
**//*---------------------------------------------------------------------*/
- (void)writeStoredScore
{
	[_writeLock lock];
	NSData* archivedScore = [NSKeyedArchiver archivedDataWithRootObject:_storedScores];
	NSError* error;
	[archivedScore writeToFile:_storedScoresFilename options:NSDataWritingFileProtectionNone error:&error];
	if(error)
	{
		// 失敗
	}
	[_writeLock unlock];
}

/*---------------------------------------------------------------------*//**
	リーダーボードを保存する（送信はあとで）
**//*---------------------------------------------------------------------*/
- (void)storeScore:(GKScore*)score 
{
	[_storedScores addObject:score];
	[self writeStoredScore];
}

/*---------------------------------------------------------------------*//**
	リーダーボードをサーバーへ送信する．失敗した場合はファイルに保存する
**//*---------------------------------------------------------------------*/
- (void)submitScore:(GKScore*)score 
{
	GKLocalPlayer* lp = [GKLocalPlayer localPlayer];
	if(lp && lp.authenticated)
	{
		if(!score.value)
		{
			// リーダーボード値が無効
			return;
		}

		// リーダーボード送信
		[score reportScoreWithCompletionHandler:^(NSError *error)
			{
				if(!error || (![error code] && ![error domain]))	// 成功
				{
					// リーダーボードは正しく送信されたので、他の保存されたリーダーボードも再送信を試みる
					[self resubmitStoredScores];
				}
				else	// エラー
				{
					// 次回認証時には送信できるようにリーダーボードをファイルに保存する
					[self storeScore:score];
				}
			}
		];
	} 
}

/*---------------------------------------------------------------------*//**
	保存された達成の再送を試みる
**//*---------------------------------------------------------------------*/
- (void)resubmitStoredAchievements
{
	if(_storedAchievements)
	{
		for(NSString* key in _storedAchievements)
		{
			GKAchievement* achievement = [_storedAchievements objectForKey:key];
			[_storedAchievements removeObjectForKey:key];
			[self submitAchievement:achievement];
		} 
		[self writeStoredAchievements];
	}
}
 
/*---------------------------------------------------------------------*//**
	保存された達成をファイルから読み込み、未送信分を送信する
**//*---------------------------------------------------------------------*/
- (void)loadStoredAchievements
{
	if(!_storedAchievements)
	{
		NSDictionary* unarchivedObj = [NSKeyedUnarchiver unarchiveObjectWithFile:_storedAchievementsFilename];

		if(unarchivedObj)
		{
			_storedAchievements = [[NSMutableDictionary alloc] initWithDictionary:unarchivedObj];
			[self resubmitStoredAchievements];
		}
		else
		{
			_storedAchievements = [[NSMutableDictionary alloc] init];
		}
	}    
}

/*---------------------------------------------------------------------*//**
	保存された達成をファイルに書き込む
**//*---------------------------------------------------------------------*/
- (void)writeStoredAchievements
{
	[_writeLock lock];
	NSData* archivedAchievements = [NSKeyedArchiver archivedDataWithRootObject:_storedAchievements];
	NSError* error;
	[archivedAchievements writeToFile:_storedAchievementsFilename options:NSDataWritingFileProtectionNone error:&error];
	if(error)
	{
		// 失敗
	}
	[_writeLock unlock];
}

/*---------------------------------------------------------------------*//**
	達成を保存する（送信はあとで）
**//*---------------------------------------------------------------------*/
- (void)storeAchievement:(GKAchievement *)achievement 
{
	GKAchievement * currentStorage = [_storedAchievements objectForKey:achievement.identifier];
	if (!currentStorage || (currentStorage && currentStorage.percentComplete < achievement.percentComplete))
	{
		[_storedAchievements setObject:achievement forKey:achievement.identifier];
		[self writeStoredAchievements];
	}
}

/*---------------------------------------------------------------------*//**
	達成をサーバーへ送信する．失敗した場合はファイルに保存する
**//*---------------------------------------------------------------------*/
- (void)submitAchievement:(GKAchievement *)achievement 
{
	if(achievement)
	{
		// 達成送信
		[achievement reportAchievementWithCompletionHandler: ^(NSError* error)
			{
				if(!error)	// 成功
				{
					// 保存された達成からエントリーを削除する
					if([_storedAchievements objectForKey:achievement.identifier])
					{
						[_storedAchievements removeObjectForKey:achievement.identifier];
					} 
					// 達成は正しく送信されたので、他の保存された達成も再送信を試みる
					[self resubmitStoredAchievements];
				}
				else		// エラー
				{
					// 次回認証時には送信できるように達成をファイルに保存する
					[self storeAchievement:achievement];
				}
			}
		];
	}
}

/*---------------------------------------------------------------------*//**
	プレイヤーの全ての達成をリセットする
**//*---------------------------------------------------------------------*/
- (void)resetAchievements
{
	[GKAchievement resetAchievementsWithCompletionHandler: ^(NSError *error) 
		{
			if(!error)	// 成功
			{
				#if !__has_feature(objc_arc)
					[_storedAchievements release];
				#endif
				_storedAchievements = [[NSMutableDictionary alloc] init];
				// 以前の保存された達成を上書きする
				[self writeStoredAchievements];              
			}
			else		// エラー
			{
				// 達成のクリアに失敗
			}
		}
	];
}

@end
