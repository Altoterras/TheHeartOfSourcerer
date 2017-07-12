/***********************************************************************//**
 *	MainViewController.h
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2012/07/02.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インポートファイル

#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>

#import <GoogleMobileAds/GoogleMobileAds.h>


#import "Base.h"
#import "GameCenterManipulator.h"

////////////////////////////////////////////////////////////////////////////
// クラス

#define ENABLE_MOVIE_PLAYBACK_VIEW	0
#define ENABLE_ADMOB 1

@class MainGlView;
#if ENABLE_MOVIE_PLAYBACK_VIEW
@class MoviePlaybackView;
#endif

/*---------------------------------------------------------------------*//**
 *	メイン ビューコントローラー
 *
**//*---------------------------------------------------------------------*/
@interface MainViewController : UIViewController <UINavigationControllerDelegate, GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate, GADBannerViewDelegate>
{
	MainGlView* _viewGl;
#if ENABLE_MOVIE_PLAYBACK_VIEW
	MoviePlaybackView* _viewMp;
#endif
#if ENABLE_ADMOB
    GADBannerView*  _adview;
	unsigned int _flagsAdBannerCtrl;
#endif
	GameCenterManipulator* _gmcmp;
}

@property (readwrite, retain) MainGlView* glView;
#if ENABLE_MOVIE_PLAYBACK_VIEW
@property (readwrite, retain) MoviePlaybackView* moviePlaybackView;
#endif
@property (readwrite, retain) GameCenterManipulator* gameCenterManipulator;

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;
- (BOOL)shouldAutorotate;
- (NSUInteger)supportedInterfaceOrientations;
- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation;
- (void)showLeaderboardViewController:(NSString*)leaderboard;
- (void)showAchievementsViewController;
#if ENABLE_ADMOB
- (void)requestAdBannerCreation;
#endif
- (void)showAdBanner:(BOOL)show;

@end

////////////////////////////////////////////////////////////////////////////
