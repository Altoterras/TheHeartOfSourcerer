/***********************************************************************//**
 *	MainViewController.mm
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2012/07/02.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インポートファイル

// Base

// Self
#import "MainViewController.h"

// Friends
#import "MainGlView.h"
#if ENABLE_MOVIE_PLAYBACK_VIEW
	#import "MoviePlaybackView.h"
#endif

// External

// Library

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// 定数

#if ENABLE_IAD
// AD バナーフラグ
static const unsigned int	ADBF_EXTERNAL_SHOWN		= 0x00000001;	// 外部からの表示要求
static const unsigned int	ADBF_EXTERNAL_HIDDEN	= 0x00000002;	// 外部からの非表示要求
static const unsigned int	ADBF_AD_RECV_LOADED		= 0x00000100;	// 広告受信成功
static const unsigned int	ADBF_AD_RECV_FAILED		= 0x00000200;	// 広告受信失敗
static const unsigned int	ADBF_CREATED			= 0x00010000;	// 広告バナー作成済み
static const unsigned int	ADBF_VISIBLED			= 0x00020000;	// 広告バナー表示済み
static const unsigned int	ADBF_REQ_CREATION_EXTR	= 0x01000000;	// 外部からの広告バナー作成要求
static const unsigned int	ADBF_REQ_CREATION_INTR	= 0x02000000;	// 内部からの広告バナー作成要求
#endif

//==========================================================================
// MainViewController 実装

@implementation MainViewController

@synthesize glView = _viewGl;
#if ENABLE_MOVIE_PLAYBACK_VIEW
@synthesize moviePlaybackView = _viewMp;
#endif
@synthesize gameCenterManipulator = _gmcmp;

//==========================================================================
// MainViewController メソッド

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
- (id)initWithCoder:(NSCoder*)aDecoder 
{
    self = [super initWithCoder:aDecoder];
    if(self)
	{
		_gmcmp = nil;
    }
    return self;
}

/*---------------------------------------------------------------------*//**
	解放処理
**//*---------------------------------------------------------------------*/
- (void)dealloc 
{
	#if !__has_feature(objc_arc)
		if(_gmcmp)	{	[_gmcmp release];	}
		[super dealloc];
	#endif
}

/*---------------------------------------------------------------------*//**
	画面回転方向を OS に知らせる（～iOS5）
**//*---------------------------------------------------------------------*/
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	TRACEERR("--- MainViewController::shouldAutorotateToInterfaceOrientation\n");

	return (interfaceOrientation == UIInterfaceOrientationLandscapeRight
			|| interfaceOrientation == UIInterfaceOrientationLandscapeLeft);
}

/*---------------------------------------------------------------------*//**
	画面回転方向を OS に知らせる（iOS6～）
**//*---------------------------------------------------------------------*/
- (BOOL)shouldAutorotate
{
	TRACEERR("--- MainViewController::shouldAutorotate\n");

	return YES;
}

/*---------------------------------------------------------------------*//**
	画面回転方向を OS に許可する（iOS6～）
**//*---------------------------------------------------------------------*/
- (NSUInteger)supportedInterfaceOrientations
{
	TRACEERR("--- MainViewController::supportedInterfaceOrientations\n");

	// ここでは全ての方向を許可するが、実際は Info.plist によって規定される
	#ifdef __IPHONE_6_0
		return UIInterfaceOrientationMaskAll;
	#else
		return 0x1e;	// 定義がないので手動実装 UIInterfaceOrientationMaskAll;
	#endif
}

/*---------------------------------------------------------------------*//**
	初期画面回転方向を OS に設定する（iOS6～）
**//*---------------------------------------------------------------------*/
- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation
{
	TRACEERR("--- MainViewController::preferredInterfaceOrientationForPresentation\n");

	return UIInterfaceOrientationLandscapeLeft;
}

/*---------------------------------------------------------------------*//**
	指定スコアボードビューコントローラーを表示する
**//*---------------------------------------------------------------------*/
- (void)showLeaderboardViewController:(NSString*)leaderboard 
{
    GKLeaderboardViewController* leaderboardViewController = [[GKLeaderboardViewController alloc] init];
    [leaderboardViewController setCategory:leaderboard];
    [leaderboardViewController setLeaderboardDelegate:self];
    [self presentViewController:leaderboardViewController animated:YES completion:nil];
	#if !__has_feature(objc_arc)
		[leaderboardViewController release];
	#endif
}

/*---------------------------------------------------------------------*//**
	達成ビューコントローラーを表示する
**//*---------------------------------------------------------------------*/
- (void)showAchievementsViewController
{
	if([self presentedViewController])
	{
		[self dismissViewControllerAnimated:NO completion:nil];
	}
	GKAchievementViewController* achievementViewController = [[GKAchievementViewController alloc] init];
	[achievementViewController setAchievementDelegate:self];
	[self presentViewController:achievementViewController animated:YES completion:nil];
	#if !__has_feature(objc_arc)
		[achievementViewController release];
	#endif
}

#if ENABLE_IAD
/*---------------------------------------------------------------------*//**
	外部からの広告バナーを作成要求
**//*---------------------------------------------------------------------*/
- (void)requestAdBannerCreation
{
	_flagsAdBannerCtrl |= ADBF_REQ_CREATION_EXTR;
	[self respondAdBannerCreation];
}
#endif

#if ENABLE_IAD
/*---------------------------------------------------------------------*//**
	広告バナーを作成要求を処理する
**//*---------------------------------------------------------------------*/
- (void)respondAdBannerCreation
{
	// 内外からの作成要求が揃わない限り（作成できる状況にならない限り）作成しない
	if(!(_flagsAdBannerCtrl & ADBF_REQ_CREATION_INTR)) { return; }
	if(!(_flagsAdBannerCtrl & ADBF_REQ_CREATION_EXTR)) { return; }
	
	// iAD 作成
	_adview = [[ADBannerView alloc] initWithFrame:CGRectZero];
	
	// ランドスケープ表示
	CGRect rectThis = self.view.frame;
	CGRect rectAd = _adview.frame;
	rectAd.size.width = rectThis.size.width > rectThis.size.height ? rectThis.size.width : rectThis.size.height;
	_adview.frame = rectAd;
	[_adview setAutoresizingMask:UIViewAutoresizingFlexibleWidth];
	//_adview.requiredContentSizeIdentifiers = [NSSet setWithObject:ADBannerContentSizeIdentifierLandscape];
	//_adview.currentContentSizeIdentifier = ADBannerContentSizeIdentifierLandscape;
	
	// iAD をビューに設定
	_adview.alpha = 0.0f;		// 非表示にしておく
	[self moveAdBanner:false];
	[self.view addSubview:_adview];
	_adview.delegate = self;
	
	//TRACELOGV("!!! %f, %f, %f, %f, %f, %f", self.view.frame.size.width, self.view.frame.size.height, _adview.frame.size.width, _adview.frame.size.height, rect.size.width, rect.size.height);

	_flagsAdBannerCtrl &= ~(ADBF_REQ_CREATION_INTR | ADBF_REQ_CREATION_EXTR);
	_flagsAdBannerCtrl |= ADBF_CREATED;
}
#endif

#if ENABLE_IAD
/*---------------------------------------------------------------------*//**
	広告バナーを移動する（画面内=表示／画面外=非表示 制御）
**//*---------------------------------------------------------------------*/
- (void)moveAdBanner:(BOOL)show
{
	CGRect rectScrn = self.view.frame;	//[[UIScreen mainScreen] bounds];
	f32 y = ((self.interfaceOrientation == UIDeviceOrientationLandscapeRight) || (self.interfaceOrientation == UIDeviceOrientationLandscapeLeft)) ? (rectScrn.size.width > rectScrn.size.height ? rectScrn.size.height : rectScrn.size.width) : (rectScrn.size.width > rectScrn.size.height ? rectScrn.size.width : rectScrn.size.height);
	if(show) { y -= _adview.frame.size.height; }

	CGRect rectBnnr = CGRectMake(0, y, _adview.frame.size.width, _adview.frame.size.height);
	[UIView animateWithDuration:0.25 animations:^{
		_adview.frame = rectBnnr;
		[_adview setAlpha:show ? 1.0f : 0.0f];
	}];

	_flagsAdBannerCtrl = show ? _flagsAdBannerCtrl | ADBF_VISIBLED : _flagsAdBannerCtrl & ~ADBF_VISIBLED;
}
#endif

/*---------------------------------------------------------------------*//**
	外部からの広告バナーの表示制御
**//*---------------------------------------------------------------------*/
- (void)showAdBanner:(BOOL)show
{
#if ENABLE_IAD
	TRACEERRV("--- MainViewController::showAdBanner show=%d\n", show);
	_flagsAdBannerCtrl &= ~(ADBF_EXTERNAL_SHOWN | ADBF_EXTERNAL_HIDDEN);
	_flagsAdBannerCtrl |= show ? ADBF_EXTERNAL_SHOWN : ADBF_EXTERNAL_HIDDEN;
	[self ctrlAdBanner];
#endif
}

#if ENABLE_IAD
/*---------------------------------------------------------------------*//**
	広告バナーを制御する
**//*---------------------------------------------------------------------*/
- (void)ctrlAdBanner
{
	BOOL show = (_flagsAdBannerCtrl & ADBF_EXTERNAL_SHOWN) && (_flagsAdBannerCtrl & ADBF_AD_RECV_LOADED);
	if(show && (_flagsAdBannerCtrl & ADBF_VISIBLED))		{	return;	}
	if(!show && (!(_flagsAdBannerCtrl & ADBF_VISIBLED)))	{	return;	}
	//TRACEERRV("{ctrlAdBanner} show=%d\n", show);
	[self moveAdBanner:show];
}
#endif

/*---------------------------------------------------------------------*//**
	スコアボードビューコントローラーの終了通知
**//*---------------------------------------------------------------------*/
- (void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController*)viewController  
{
	[self dismissViewControllerAnimated:YES completion:nil];
}

/*---------------------------------------------------------------------*//**
	達成ビューコントローラーの終了通知
**//*---------------------------------------------------------------------*/
- (void)achievementViewControllerDidFinish:(GKAchievementViewController*)viewController 
{
    if([self presentedViewController])
	{
        [self dismissViewControllerAnimated:YES completion:nil];
    }
}

/*---------------------------------------------------------------------*//**
	初回ロード通知
**//*---------------------------------------------------------------------*/
- (void)viewDidLoad
{
	[super viewDidLoad];
	
#if ENABLE_IAD
	// 内部からの広告バナーを作成要求
	_flagsAdBannerCtrl |= ADBF_REQ_CREATION_INTR;
	[self respondAdBannerCreation];
#endif
}

/*---------------------------------------------------------------------*//**
	画面回転前通知
**//*---------------------------------------------------------------------*/
- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
	TRACEERRV("--- MainViewController::willRotateToInterfaceOrientation:%d <- %d\n", toInterfaceOrientation, self.interfaceOrientation);

	if(_viewGl != nil)
	{
		[_viewGl onRotateFromInterfaceOrientation:toInterfaceOrientation];
	}
}

/*---------------------------------------------------------------------*//**
	画面回転後通知
**//*---------------------------------------------------------------------*/
- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
	TRACEERRV("--- MainViewController::didRotateFromInterfaceOrientation:%d -> %d\n", fromInterfaceOrientation, self.interfaceOrientation);

#if 0	// UIDeviceOrientationPortrait をサポートする場合はこれをしないとダメか？？【2015/01/21 r-kishi】
	CGRect rect = [[UIScreen mainScreen] bounds];	//CGRect rect = [[UIScreen mainScreen] applicationFrame];
	TRACELOGV("!2! %f, %f, %f, %f", self.view.frame.size.width, self.view.frame.size.height, rect.size.width, rect.size.height);
	self.view.frame = rect;
#endif
}

#if ENABLE_IAD
/*---------------------------------------------------------------------*//**
	iAd 受信成功通知
**//*---------------------------------------------------------------------*/
-(void)bannerViewDidLoadAd:(ADBannerView*)banner
{
	TRACEERR("--- MainViewController::bannerViewDidLoadAd\n");
	_flagsAdBannerCtrl &= ~(ADBF_AD_RECV_LOADED | ADBF_AD_RECV_FAILED);
	_flagsAdBannerCtrl |= ADBF_AD_RECV_LOADED;
	[self ctrlAdBanner];
}
#endif

#if ENABLE_IAD
/*---------------------------------------------------------------------*//**
	iAd 受信失敗通知
**//*---------------------------------------------------------------------*/
-(void)bannerView:(ADBannerView*)banner didFailToReceiveAdWithError:(NSError *)error
{
	TRACEERR("--- MainViewController::didFailToReceiveAdWithError\n");
	_flagsAdBannerCtrl &= ~(ADBF_AD_RECV_LOADED | ADBF_AD_RECV_FAILED);
	_flagsAdBannerCtrl |= ADBF_AD_RECV_FAILED;
	[self ctrlAdBanner];
}
#endif

/*---------------------------------------------------------------------*//**
	メモリ切迫警告の受信通知
**//*---------------------------------------------------------------------*/
- (void)didReceiveMemoryWarning 
{
    [super didReceiveMemoryWarning];	// 親クラスに通知伝達
	
#if defined(_DEBUG)
	u32 sizeFreeMem, sizeTotalMem;
	if(OsDepModIos::getMemoryInfo(&sizeFreeMem, &sizeTotalMem, 0L))
	{
		TRACELOGV("{didReceiveMemoryWarning} free_memory_size = %fMB / %fMB\n", sizeFreeMem / (1024.0f * 1024.0f), sizeTotalMem / (1024.0f * 1024.0f));
	}
#endif
	
	// 使用されていないキャッシュデータやイメージをここで解放する
}

@end
