/***********************************************************************//**
 *	EnlightAppDelegate.mm
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2009/04/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インポートファイル

// Base

// Self
#import "EnlightAppDelegate.h"

// Friends
#import "DebugIos.h"
#import "MainGlView.h"
#import "MainViewController.h"
#if ENABLE_MOVIE_PLAYBACK_VIEW
	#import "MoviePlaybackView.h"
#endif
#import "OsDepModIos.h"
#import "PsnsIos.h"

// External
#import "../etk/body/EtkBody.h"

// Library

////////////////////////////////////////////////////////////////////////////
// マクロ

// 非マルチタスクモード時のバックグランドに入るときにウインドウを再作成するかどうかのコンパイルスイッチ
#define RECREATEWINDOW_IF_NOMTBACKGROUND	(0)

////////////////////////////////////////////////////////////////////////////
// グローバル変数定義

extern const char* g_queryApp;

////////////////////////////////////////////////////////////////////////////
// グローバル関数定義

//==========================================================================
// For Debug

void alert(const char* msg)
{
	EnlightAppDelegate* delg = (EnlightAppDelegate*)[[UIApplication sharedApplication] delegate];
	MainGlView* viewGl = delg.glView;
	
	NSString* str = [NSString stringWithCString:msg encoding:NSASCIIStringEncoding];
	UIAlertView* alertv = [[UIAlertView alloc] initWithTitle:@"alert!" message:str delegate:viewGl cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alertv show];
	#if !__has_feature(objc_arc)
		[alertv release];
	#endif
	//	[str release];
}

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EnlightAppDelegate 変数定義

@implementation EnlightAppDelegate

#pragma mark -
#pragma mark UI Component

@synthesize window = _window;
@synthesize glView = _viewGl;
#if ENABLE_MOVIE_PLAYBACK_VIEW
@synthesize moviePlaybackView = _viewMp;
#endif
@synthesize mainViewController = _vcMain;

const int VIEW_GL = 1;
const int VIEW_MOVIE_PLAYBACK = 2;

#pragma mark -
#pragma mark Game Center Support

//==========================================================================
// EnlightAppDelegate メソッド

/*---------------------------------------------------------------------*//**
	GameCenter の開始
**//*---------------------------------------------------------------------*/
- (void)startGameCenter
{
	TRACEERR("--- EnlightAppDelete::beginGameCenter\n");
	
#if defined(_ENLIGHT_V01) && !defined(_ENLIGHT_V01_LITE)
	if (![GameCenterManipulator isGameCenterApiAvailable])
	{
		return;
	}
	
	GKLocalPlayer* localPlayer = [GKLocalPlayer localPlayer];
	if(localPlayer)
	{
		// ローカルプレイヤーの認証処理
		//[localPlayer authenticateWithCompletionHandler:^(NSError *error)
		localPlayer.authenticateHandler = ^(UIViewController *viewController, NSError *error)
			{
				if(localPlayer.isAuthenticated)
				{
					NSString* playerId = localPlayer.playerID;
					// ログインしているプレイヤーが変更されている
					if(!_strGameCenterCurPlayerId || ![_strGameCenterCurPlayerId isEqualToString:playerId])
					{
						// プレイヤーの変更処理
						if(!_vcMain.gameCenterManipulator || ![_strGameCenterCurPlayerId isEqualToString:playerId])
						{
							_vcMain.gameCenterManipulator = [[[GameCenterManipulator alloc] init] autorelease];                        
						}
					 
						// スコアの保存
						[_vcMain.gameCenterManipulator loadStoredScores];
						[_vcMain.gameCenterManipulator resubmitStoredScores];
						// 達成の保存
						[_vcMain.gameCenterManipulator loadStoredAchievements];
					 
						// 現在のユーザーの変更
						if(_strGameCenterCurPlayerId)
						{
							[_strGameCenterCurPlayerId release];
						}
						_strGameCenterCurPlayerId = [[NSString alloc] initWithString:playerId];
					}

					// GameCenter が有効である
					if(_vcMain.gameCenterManipulator)
					{
						_vcMain.gameCenterManipulator.gameCenterAuthenticationComplete = true;
					}
				}
				else
				{
					// ユーザーは GameCenter にログインしていない
					if(_vcMain.gameCenterManipulator)
					{
						_vcMain.gameCenterManipulator.gameCenterAuthenticationComplete = false;
					}
				}
			};
		
	}
#endif
}

/*---------------------------------------------------------------------*//**
	GameCenter の停止
**//*---------------------------------------------------------------------*/
- (void)stopGameCenter
{
	TRACEERR("--- EnlightAppDelete::endGameCenter\n");

	if(_vcMain.gameCenterManipulator)
	{
		_vcMain.gameCenterManipulator.gameCenterAuthenticationComplete = false;
	}
}

#pragma mark -
#pragma mark Application lifecycle

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
- (id)init
{
	TRACEERR("--- EnlightAppDelete::init\n");
	
	self = [super init];
	if(self)
	{
		_window = nil;
		_viewGl = nil;
		_vcMain = nil;
		_strGameCenterCurPlayerId = nil;
	}
	return self;
}

/*---------------------------------------------------------------------*//**
	解放処理
**//*---------------------------------------------------------------------*/
- (void)dealloc
{
	TRACEERR("--- EnlightAppDelete::dealloc\n");
	
	// ウインドウの破棄
	[self destroyWindow];

	// ゲームセンターの現在のプレイヤー文字列を破棄
	#if !__has_feature(objc_arc)
		if(_strGameCenterCurPlayerId)	{	[_strGameCenterCurPlayerId release];	}
	#endif
	_strGameCenterCurPlayerId = nil;

	#if !__has_feature(objc_arc)
		[super dealloc];
	#endif
}

/*---------------------------------------------------------------------*//**
	ウインドウの作成
**//*---------------------------------------------------------------------*/
- (bool)createWindow
{
	CGRect rect = [[UIScreen mainScreen] bounds];	//CGRect rect = [[UIScreen mainScreen] applicationFrame];

	// ビューコントローラーおよびウインドウの作成
	_vcMain = [[MainViewController alloc] init];
	_window = [[UIWindow alloc] initWithFrame:rect];

	// ビューの作成
	_viewGl = [[MainGlView alloc] initWithFrame:rect];
	_viewGl.mainViewController = _vcMain;
#if ENABLE_MOVIE_PLAYBACK_VIEW
	_viewMp = [[MoviePlaybackView alloc] initWithFrame:rect];
#endif

	// ウインドウ, ビューコントローラー, ビューの構成
	_vcMain.glView = _viewGl;
#if ENABLE_MOVIE_PLAYBACK_VIEW
	_vcMain.moviePlaybackView = _viewMp;
#endif
	if([_window respondsToSelector:@selector(rootViewController)])	// iOS6〜
	{
		_window.rootViewController = _vcMain;
	}
	else	// 〜iOS5
	{
		[_window addSubview:_viewGl];
	}
	[_window makeKeyAndVisible];

	[self changeActiveView:_viewGl];
	
#if ENABLE_IAD
	[_vcMain requestAdBannerCreation];
#endif
	
	return true;
}

/*---------------------------------------------------------------------*//**
	ウインドウの破棄
**//*---------------------------------------------------------------------*/
- (void)destroyWindow
{
	#if !__has_feature(objc_arc)
		[_window release];
		[_vcMain release];
		#if ENABLE_MOVIE_PLAYBACK_VIEW
			[_viewMp release];
		#endif
		[_viewGl release];
	#endif
	_window = nil;
	_vcMain = nil;
	#if ENABLE_MOVIE_PLAYBACK_VIEW
		_viewMp = nil;
	#endif
	_viewGl = nil;
}

/*---------------------------------------------------------------------*//**
	アプリケーション起動 URL ハンドル
**//*---------------------------------------------------------------------*/
- (BOOL)application:(UIApplication*)application handleOpenURL:(NSURL*)url
{
	TRACEERR("--- EnlightAppDelete::application\n");
	
	// クエリをバッファに保存する
	const int numBuf = 256;
	static char bufQuery[numBuf] = "";
	const char* szQuery = [[url query] cStringUsingEncoding:NSASCIIStringEncoding];
	for(int i = 0; (i < numBuf) && (i < ::strlen(szQuery)); i++)
	{
		bufQuery[i] = szQuery[i];
	}
	g_queryApp = bufQuery;

	return TRUE;
}

/*---------------------------------------------------------------------*//**
	アプリケーションが起動し終わった時の通知
**//*---------------------------------------------------------------------*/
- (void)applicationDidFinishLaunching:(UIApplication*)application
{
	TRACEERR("--- EnlightAppDelete::applicationDidFinishLaunching\n");

	// ウインドウ作成
	[self createWindow];
	
	// マルチタスクのバックグラウンド化通知を手動登録する
	#if 0
		if((&UIApplicationDidEnterBackgroundNotification) != 0L)
		{
			[[NSNotificationCenter defaultCenter] addObserver:self 
				selector:@selector(applicationDidEnterBackgroundCustom:)
				name:UIApplicationDidEnterBackgroundNotification
				object:[UIApplication sharedApplication]];
		}
	#endif
	
	// GameCenter の開始
	[self startGameCenter];
}

/*---------------------------------------------------------------------*//**
	アプリケーションが使用されなくなった（隠されたり終了したり）直後の通知
**//*---------------------------------------------------------------------*/
- (void)applicationWillResignActive:(UIApplication*)application
{
	[_viewGl setAnimationInterval: 1.0 / 5.0];
}

/*---------------------------------------------------------------------*//**
	アプリケーションがアクティブになった直後の通知
**//*---------------------------------------------------------------------*/
- (void)applicationDidBecomeActive:(UIApplication*)application
{
	[_viewGl setAnimationInterval: RENDER_FPS];
	
	///[UIViewController attemptRotationToDeviceOrientation];
}

/*---------------------------------------------------------------------*//**
	アプリケーションが終了する直前の通知
**//*---------------------------------------------------------------------*/
- (void)applicationWillTerminate:(UIApplication*)application
{
	TRACEERR("--- EnlightAppDelete::applicationWillTerminate\n");
	
	// ゲームの後始末
	[_viewGl cleanupGame:etk::EtkBody::TBCASE_CLOSE];
	
	// GameCenter の停止
	[self stopGameCenter];
}

/*---------------------------------------------------------------------*//**
	マルチタスクで、アプリケーションがバッググラウンドに入る時の通知（手動実装版）
**//*---------------------------------------------------------------------*/
- (void)applicationDidEnterBackground:(UIApplication*)application
{
	TRACEERR("--- EnlightAppDelete::applicationDidEnterBackground\n");

	if([_viewGl checkResidentMemoryMultiTaskingMode])
    {
        // ゲーム一時停止
        [_viewGl enterBackground];
    }
    else
    {
        // ゲームの後始末
        [_viewGl cleanupGame:etk::EtkBody::TBCASE_ENTERBG];
		
		// ウインドウを一度破棄する
		if(RECREATEWINDOW_IF_NOMTBACKGROUND)
		{
			// ウインドウの破棄
			// ウインドウを保持したままのゲーム再作成は、バックグラウンドに入った後の回転処理がうまくつながらない症状の対処
			[self destroyWindow];
		}
	}
	
	// GameCenter の停止
	[self stopGameCenter];
}

/*---------------------------------------------------------------------*//**
	マルチタスクで、アプリケーションが再開する時の通知（手動実装版）
**//*---------------------------------------------------------------------*/
- (void)applicationWillEnterForeground:(UIApplication*)application
{
	TRACEERR("\n--- EnlightAppDelete::applicationWillEnterForeground\n");

	if((_viewGl != nil) && [_viewGl checkResidentMemoryMultiTaskingMode])
    {
        // ゲーム再開
        [_viewGl leaveBackground];
    }
    else
    {
		if(RECREATEWINDOW_IF_NOMTBACKGROUND)
		{
			// ウインドウの再作成
			[self createWindow];	// ゲームの初期化はこの再作成時におけるビュー作成の layoutSubviews で行われる
		}
		else
		{
			TRACEERRV("--- EnlightAppDelete::applicationWillEnterForeground !2! %d\n", _vcMain.interfaceOrientation);

			// 回転を伝える
			if(_vcMain.view != nil)
			{
				_viewGl.transform = _vcMain.view.transform;
			}
			
			// ゲームの再作成
			[_viewGl initGame:etk::EtkBody::SBCASE_ENTERFG];
		}
    }
}

#if 0
/*---------------------------------------------------------------------*//**
	サポートしている画面回転向きの通知
**//*---------------------------------------------------------------------*/
- (NSUInteger)application:(UIApplication*)application supportedInterfaceOrientationsForWindow:(UIWindow*)window
{
	///return UIInterfaceOrientationMaskLandscape;
	return UIInterfaceOrientationMaskAll;
}
#endif

/*---------------------------------------------------------------------*//**
	アクティブビューの変更
**//*---------------------------------------------------------------------*/
- (void)changeActiveView:(UIView*)viewNewActive
{
	if(_vcMain == nil)
	{
		return;
	}
	if(_vcMain.view == viewNewActive)
	{
		return;
	}
	
	// 回転を伝える
	if(_vcMain.view != nil)
	{
		viewNewActive.transform = _vcMain.view.transform;
	}
	
	// 指定ビューを前面に
	_vcMain.view = viewNewActive;
	
	// フォーカスを移動
	[_vcMain.view becomeFirstResponder];
}

/*---------------------------------------------------------------------*//**
	ビューにボタンを追加する
**//*---------------------------------------------------------------------*/
- (UIButton*)createButtonOnView:(UIView*)view
	target:(id)target
	action:(SEL)action
	uitag:(int)uitag
	show:(bool)show
	frame:(CGRect)rect
	title:(NSString*)title
	titleFontSize:(int)fontsizeTitle
	normalTitleColor:(UIColor*)colTitleN
	highlightedTitleColor:(UIColor*)colTitleH
	normalBgImageName:(NSString*)imgnmBgN
	highlightedImageName:(NSString*)imgnmBgH
{
	UIButton* btn = [UIButton buttonWithType:UIButtonTypeCustom];
	if([btn respondsToSelector:@selector(setAttributedTitle)])	// iOS6 以上
	{
		NSMutableAttributedString* atitle = [[NSMutableAttributedString alloc] initWithString:title];
		[atitle addAttributes:[NSDictionary dictionaryWithObject:[UIFont boldSystemFontOfSize:fontsizeTitle] forKey:NSFontAttributeName] range:NSMakeRange(0, atitle.length)];
		[atitle addAttributes:[NSDictionary dictionaryWithObject:colTitleN forKey:NSForegroundColorAttributeName] range:NSMakeRange(0, atitle.length)];
		[btn setAttributedTitle:atitle forState:UIControlStateNormal];
	}
	else	// iOS6以前
	{
		[btn setTitle:title forState:UIControlStateNormal];
		[btn setTitleColor:colTitleN forState:UIControlStateNormal];
		[btn setTitleColor:colTitleH forState:UIControlStateHighlighted];
		[btn.titleLabel setFont:[UIFont fontWithName:@"HiraKakuProN-W6" size:fontsizeTitle]];
	}
	[btn setFrame: rect];
	[btn setTag: uitag];
	[btn addTarget:target action:action forControlEvents:UIControlEventTouchUpInside];
	[btn setBackgroundImage:[UIImage imageNamed:imgnmBgN] forState:UIControlStateNormal];
	[btn setBackgroundImage:[UIImage imageNamed:imgnmBgH] forState:UIControlStateHighlighted];
	btn.hidden = !show;
	[view addSubview: btn];
	
	return btn;
}

/*---------------------------------------------------------------------*//**
	ビューに追加したボタンを全て削除する
**//*---------------------------------------------------------------------*/
- (void)destroyAllButtonsOnView:(UIView*)view
{
	for(UIView* subview in [view subviews])
	{
		[subview removeFromSuperview];
	}
}

@end
