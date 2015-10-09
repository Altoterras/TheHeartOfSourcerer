/***********************************************************************//**
 *	EnlightAppDelegate.h
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2009/04/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インポートファイル

#import <UIKit/UIKit.h>

////////////////////////////////////////////////////////////////////////////
// クラス

#define ENABLE_MOVIE_PLAYBACK_VIEW	0

@class MainGlView;
@class MainViewController;
#if ENABLE_MOVIE_PLAYBACK_VIEW
@class MoviePlaybackView;
#endif

/*---------------------------------------------------------------------*//**
 *	アプリケーション デリゲート クラス
 *
**//*---------------------------------------------------------------------*/
@interface EnlightAppDelegate : NSObject <UIApplicationDelegate>
{
	UIWindow* _window;
	MainGlView* _viewGl;
	MainViewController* _vcMain;
#if ENABLE_MOVIE_PLAYBACK_VIEW
	MoviePlaybackView* _viewMp;
#endif
	NSString* _strGameCenterCurPlayerId;
	
	const int VIEW_GL;
	const int VIEW_MOVIE_PLAYBACK;
}

@property(strong, nonatomic) UIWindow* window;
@property(nonatomic, retain) IBOutlet MainGlView* glView;
#if ENABLE_MOVIE_PLAYBACK_VIEW
@property(nonatomic, retain) IBOutlet MoviePlaybackView* moviePlaybackView;
#endif
@property(nonatomic, retain) IBOutlet MainViewController* mainViewController;

- (void)changeActiveView:(UIView*)viewNewActive;
- (UIButton*)createButtonOnView:(UIView*)view target:(id)target action:(SEL)action uitag:(int)uitag show:(bool)show frame:(CGRect)rect title:(NSString*)title titleFontSize:(int)fontsizeTitle normalTitleColor:(UIColor*)colTitleN highlightedTitleColor:(UIColor*)colTitleH normalBgImageName:(NSString*)imgnmBgN highlightedImageName:(NSString*)imgnmBgH;
- (void)destroyAllButtonsOnView:(UIView*)view;

@end

////////////////////////////////////////////////////////////////////////////