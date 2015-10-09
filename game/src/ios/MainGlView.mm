/***********************************************************************//**
 *	MainGlView.mm
 *	Enlight iOS Application
 *	
 *	Created by Ryoutarou Kishi on 2009/04/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インポートファイル

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "DebugIos.h"
#import "MainGlView.h"

#import "../tfw/file/File.h"
#import "../tfw/file/IdFile.h"
#import "../tfw/lib/Point.h"
#import "../tfw/string/VcString.h"
#import "../tfw/string/StringUtils.h"
#import "../tfw/ui/TouchPanel.h"
#import "../tfw/ui/MotionController.h"
#import "../sourcerer/body/Game.h"
#if defined(_PUZZLE_ICHTHYS)
	#import "../puzzle_ichthys/body/IchthysPuzzleBody.h"
#endif
#import "../atar/body/AtArBody.h"

////////////////////////////////////////////////////////////////////////////
// 定数

// 圧縮ファイル展開用リードバッファのサイズ
#define SIZE_UNCOMPRESS_READ_BUFFER_1		(5 * 1024 * 1024)	// 5MB
#define SIZE_UNCOMPRESS_READ_BUFFER_2		(12 * 1024 * 1024)	// 12MB

// サポートするタッチポイントの数
#define NUM_TOUCH_MAX						2

////////////////////////////////////////////////////////////////////////////
// グローバル変数

const char*			g_queryApp = nil;

////////////////////////////////////////////////////////////////////////////
// クラス

#if 0
//==========================================================================
// MainGlView プライベートメンバ

@interface MainGlView ()

@property (nonatomic, retain) EAGLContext* context;
@property (nonatomic, assign) NSTimer* animationTimer;

- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;

@end
#endif

//==========================================================================
// MainGlView 実装

@implementation MainGlView

@synthesize context = _context;
@synthesize animationTimer = _animationTimer;
@synthesize animationInterval = _animationInterval;
@synthesize mainViewController = _vcMain;
@synthesize txtfldSourceEdit = _txtfldSourceEdit;

//==========================================================================
// MainGlView メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 初期化・解放

/*---------------------------------------------------------------------*//**
	ビューレイヤーのインスタンスを作成するためのクラスを返す
		You must implement this method
**//*---------------------------------------------------------------------*/
+ (Class)layerClass
{
	return [CAEAGLLayer class];
}

/*---------------------------------------------------------------------*//**
	 システム初期化
**//*---------------------------------------------------------------------*/
- (void)initSystem
{
	TRACEERR("--- MainGlView::initSystem\n");

	_backingWidth		= 0;
	_backingHeight		= 0;
	_context			= nil;
	_vcMain				= nil;
	_viewRenderbuffer	= 0;
	_viewFramebuffer	= 0;
	_depthRenderbuffer	= 0;
	_animationTimer		= nil;
	_cntMoveIntvl		= 0;
	_scaleCoord			= 1.0f;
	_gcflags			= 0;
	_body				= nil;
	_osdep				= nil;
	_psns				= nil;
	_swFrame			= nil;
	
	// レイヤーの設定.
	CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;
	eaglLayer.opaque = YES;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, /*kEAGLColorFormatRGBA8*/kEAGLColorFormatRGB565, kEAGLDrawablePropertyColorFormat, nil];
			
	// コンテキストの作成.
	_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
	if (!_context || ![EAGLContext setCurrentContext:_context])
	{
		#if !__has_feature(objc_arc)
			[self release];
		#endif
		return;
	}
	
	// フレームレート設定.
	_animationInterval = RENDER_FPS;
	
	// ステータスバーを消す.
	[UIApplication sharedApplication].statusBarHidden = YES;
	// スリープ防止
	[UIApplication sharedApplication].idleTimerDisabled = YES;
	
	// その他の値の初期化.
	_cntMoveIntvl = 0;
	
	// マルチタッチの有効化.
	[self setMultipleTouchEnabled:YES];

#if 0
	// ソース編集用テキストフィールドの作成
	_txtfldSourceEdit = [[UITextField alloc] initWithFrame:CGRectMake(10, 10, 200, 30)];
	_txtfldSourceEdit.borderStyle = UITextBorderStyleBezel;
	_txtfldSourceEdit.backgroundColor = [UIColor whiteColor];
	_txtfldSourceEdit.textColor = [UIColor blackColor];
	_txtfldSourceEdit.hidden = YES;
	[self addSubview:_txtfldSourceEdit];
	/*
	UITextField *tf = [[[UITextField alloc] initWithFrame:CGRectMake(10, 10, 200, 30)] autorelease];
	tf.borderStyle = UITextBorderStyleRoundedRect;
	tf.textColor = [UIColor blueColor];
	tf.placeholder = @"何かいれてね";
	tf.clearButtonMode = UITextFieldViewModeAlways;
	//[tf addTarget:self action:@selector(hoge:) forControlEvents:UIControlEventEditingDidEndOnExit];	// 編集終了後フォーカスが外れた時にhogeメソッドを呼び出す
	[self addSubview:tf];
	 */
#endif
}

/*---------------------------------------------------------------------*//**
	初期化（nib ファイルを使う場合）
**//*---------------------------------------------------------------------*/
- (id)initWithCoder:(NSCoder*)coder
{
	TRACEERR("--- MainGlView::initWithCoder\n");
	
	if((self = [super initWithCoder:coder]))
	{
		[self initSystem];
	}
	
	return self;
}

/*---------------------------------------------------------------------*//**
	初期化（nib ファイルを使わない場合）
**//*---------------------------------------------------------------------*/
- (id)initWithFrame:(CGRect)rect
{
	TRACEERR("--- MainGlView::initWithFrame\n");
	
	if((self = [super initWithFrame:rect]))
	{
		[self initSystem];
	}
	
	return self;
}

/*---------------------------------------------------------------------*//**
	解放
**//*---------------------------------------------------------------------*/
- (void)dealloc
{
	TRACEERR("--- MainGlView::dealloc\n");
	
	[self stopAnimation];
	
	[self destroyFramebuffer];
    
	if([EAGLContext currentContext] == _context)
	{
		[EAGLContext setCurrentContext:nil];
	}
    
	#if !__has_feature(objc_arc)
		if(_txtfldSourceEdit != nil)	{	[_txtfldSourceEdit release];	}
		[_vcMain release];
		[_context release];
		[super dealloc];
	#endif
}

/*---------------------------------------------------------------------*//**
	フレームバッファの作成
**//*---------------------------------------------------------------------*/
- (BOOL)createFramebuffer
{
	TRACEERR("--- MainGlView::createFramebuffer\n");
	
	CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;
	u32 devtype = OsDepModIos::getDeviceType();
	float scaleCoord = 1.0f;
	unsigned int gcflags = 0;
	switch(devtype)
	{
		case OsDepModIos::DeviceType_iPhone3G:
		case OsDepModIos::DeviceType_iPhone3GS:
		case OsDepModIos::DeviceType_iPod1G:
		case OsDepModIos::DeviceType_iPod2G:
		case OsDepModIos::DeviceType_iPod3G:
			break;
			// iPad
		case OsDepModIos::DeviceType_iPad1G:
		case OsDepModIos::DeviceType_iPad2G:
			gcflags = etk::EtkBody::BCF_HD_RESOLUTION;
			break;
			// Retina iPad
		case OsDepModIos::DeviceType_iPad3G:
		case OsDepModIos::DeviceType_iPad4G:
		case OsDepModIos::DeviceType_iPad_Unknown:
		case OsDepModIos::DeviceType_iPhoneSimulator_iPad:
			gcflags = etk::EtkBody::BCF_RD_RESOLUTION;
			//gcflags = etk::EtkBody::BCF_HD_RESOLUTION | etk::EtkBody::BCF_RD_RESOLUTION;
			break;
			// Retina iPhone and iPod touch
		case OsDepModIos::DeviceType_iPhone4:
		case OsDepModIos::DeviceType_iPod4G:
		case OsDepModIos::DeviceType_iPod5G:	// Retina-Wide だが、このモデルはメモリが少ないので HD におさえておく
		default:
			scaleCoord = eaglLayer.contentsScale = [UIScreen mainScreen].scale;
			gcflags = etk::EtkBody::BCF_HD_RESOLUTION;
			break;
			// Retina-Wide iPhone and iPod touch
		case OsDepModIos::DeviceType_iPhone5:
		//case OsDepModIos::DeviceType_iPod5G:	// ↑に移動
		case OsDepModIos::DeviceType_iPhone_Unknown:
		case OsDepModIos::DeviceType_iPod_Unknown:
		case OsDepModIos::DeviceType_iPhoneSimulator_iPhone:
			scaleCoord = eaglLayer.contentsScale = [UIScreen mainScreen].scale;
			gcflags = etk::EtkBody::BCF_RD_RESOLUTION;
			break;
	}
	
#if 0
	// 空きメモリが少ない時には解像度を落とす処理を入れたいが、仮想メモリの仕様把握と測定に時間がかかるので今回はスキップ
	// 『メモリ効率の向上に 関するガイドライン』をしっかり読んで次回入れたい
	// iPod 5G で落ちる問題は上記 switch で HD 化することで暫定的に解決する
	
	// 空きメモリ量が少なければ解像度を１つ落とす
	u32 sizeTotalMem = 0;
	u32 sizeFreeMem = OsDepModIos::getFreeMemorySize(&sizeTotalMem);
	TRACELOGV("{createFramebuffer} sizeFreeMem=%fMB/%fMB\n", sizeFreeMem / (1024.0f * 1024.0f), sizeTotalMem / (1024.0f * 1024.0f));
	if(TFW_IS_FLAG(gcflags, etk::EtkBody::BCF_RD_RESOLUTION) && (sizeFreeMem < 350 * 1024 * 1024))	// 350 MB 未満
	{
		gcflags = etk::EtkBody::BCF_HD_RESOLUTION | etk::EtkBody::BCF_SAVE_RESOURCE_RUN;	// RD -> HD
		TRACELOG("{createFramebuffer} RD -> HD\n");
		
		u32 m1, f1, t1, r1;
		u8* p1;
		
		TRACELOG("\n");
		m1 = sizeFreeMem;
		p1 = (u8*)malloc(m1);
		if(p1 != 0L) { for(int i = 0; i < m1; i++) { p1[i] = i; } }
		TRACELOGV(" # p=%p, m=%f\n", p1, m1 / (1024.0f * 1024.0f));
		f1 = OsDepModIos::getFreeMemorySize(&t1, &r1);
		TRACELOGV(" -2- sizeFreeMem=%fMB/%fMB, resident=%fMB\n", f1 / (1024.0f * 1024.0f), t1 / (1024.0f * 1024.0f), r1 / (1024.0f * 1024.0f));
		free(p1);
		p1 = 0L;
		TRACELOGV(" -2.5- sizeFreeMem=%fMB/%fMB\n", f1 / (1024.0f * 1024.0f), t1 / (1024.0f * 1024.0f));
	
		TRACELOG("\n");
		m1 = 350 * 1024 * 1024;
		p1 = (u8*)malloc(m1);
		if(p1 != 0L) { for(int i = 0; i < m1; i++) { p1[i] = i; } }
		TRACELOGV(" # p=%p, m=%f\n", p1, m1 / (1024.0f * 1024.0f));
		f1 = OsDepModIos::getFreeMemorySize(&t1);
		TRACELOGV(" -3- sizeFreeMem=%fMB/%fMB\n", f1 / (1024.0f * 1024.0f), t1 / (1024.0f * 1024.0f));
		free(p1);
		p1 = 0L;
		TRACELOGV(" -3.5- sizeFreeMem=%fMB/%fMB\n", f1 / (1024.0f * 1024.0f), t1 / (1024.0f * 1024.0f));
	
		TRACELOG("\n");
		m1 = sizeTotalMem;
		p1 = (u8*)malloc(m1);
		if(p1 != 0L) { for(int i = 0; i < m1; i++) { p1[i] = i; } }
		TRACELOGV(" # p=%p, m=%f\n", p1, m1 / (1024.0f * 1024.0f));
		f1 = OsDepModIos::getFreeMemorySize(&t1);
		TRACELOGV(" -4- sizeFreeMem=%fMB/%fMB\n", f1 / (1024.0f * 1024.0f), t1 / (1024.0f * 1024.0f));
		free(p1);
		p1 = 0L;
		TRACELOGV(" -4.5- sizeFreeMem=%fMB/%fMB\n", f1 / (1024.0f * 1024.0f), t1 / (1024.0f * 1024.0f));
		
		TRACELOG("\n");
		m1 = 1 * 1024 * 1024 * 1024;	// 1GB
		p1 = (u8*)malloc(m1);
		if(p1 != 0L) { for(int i = 0; i < m1; i++) { p1[i] = i; } }
		TRACELOGV(" # p=%p, m=%f\n", p1, m1 / (1024.0f * 1024.0f));
		f1 = OsDepModIos::getFreeMemorySize(&t1);
		TRACELOGV(" -5- sizeFreeMem=%fMB/%fMB\n", f1 / (1024.0f * 1024.0f), t1 / (1024.0f * 1024.0f));
		free(p1);
		p1 = 0L;
		TRACELOGV(" -5.5- sizeFreeMem=%fMB/%fMB\n", f1 / (1024.0f * 1024.0f), t1 / (1024.0f * 1024.0f));
	
		TRACELOG("\n");
	}
	if(TFW_IS_FLAG(gcflags, etk::EtkBody::BCF_HD_RESOLUTION) && (sizeFreeMem < 150 * 1024 * 1024))	// 150 MB 未満
	{
		gcflags = etk::EtkBody::BCF_SAVE_RESOURCE_RUN;	// HD -> SD
		TRACELOG("{createFramebuffer} HD -> SD\n");
	}
#endif
	
	_scaleCoord = scaleCoord;
	_gcflags |= gcflags;
	
	glGenFramebuffersOES(1, &_viewFramebuffer);
	glGenRenderbuffersOES(1, &_viewRenderbuffer);
    
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, _viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _viewRenderbuffer);
	[_context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:eaglLayer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, _viewRenderbuffer);
    
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &_backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &_backingHeight);
    
	glGenRenderbuffersOES(1, &_depthRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _depthRenderbuffer);
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, _backingWidth, _backingHeight);
	///glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT24_OES, _backingWidth, _backingHeight);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, _depthRenderbuffer);
    
	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	
	return YES;
}

/*---------------------------------------------------------------------*//**
	フレームバッファの破棄
 **//*---------------------------------------------------------------------*/
- (void)destroyFramebuffer
{
	TRACEERR("--- MainGlView::destroyFramebuffer\n");
	
	if(_viewFramebuffer != 0)
	{
		glDeleteFramebuffersOES(1, &_viewFramebuffer);
		_viewFramebuffer = 0;
	}
	if(_viewRenderbuffer != 0)
	{
		glDeleteRenderbuffersOES(1, &_viewRenderbuffer);
		_viewRenderbuffer = 0;
	}    
	if(_depthRenderbuffer != 0)
	{
		glDeleteRenderbuffersOES(1, &_depthRenderbuffer);
		_depthRenderbuffer = 0;
	}
	
	_backingWidth = 0;
	_backingHeight = 0;
}

/*---------------------------------------------------------------------*//**
	サブビューの配置を調整する
**//*---------------------------------------------------------------------*/
- (void)layoutSubviews
{
	[EAGLContext setCurrentContext:_context];
	
	if(_viewFramebuffer == 0)	// 最初の実行である
	{
		// フレームバッファの作成
		[self createFramebuffer];
		
		// ゲームの初期化
		[self initGame:etk::EtkBody::SBCASE_OPEN];
	}
	
	[self drawView];
}

/*---------------------------------------------------------------------*//**
	ゲームの初期化
**//*---------------------------------------------------------------------*/
- (BOOL)initGame:(int)sbcase
{
	TRACEERR("### MainGlView::initGame\n");
	
	// iOS OS 依存部の作成
	_osdep = new OsDepModIos();
	// iOS PSNS の作成
	_psns = new PsnsIos();
	_psns->setMainViewController(_vcMain);
	
	// デバッグモジュール初期化
	::DebugIos::init();
	
	// ファイルモジュールの初期化
	{
		VcString pathRes;
		CcString prod("");
		CcString postfix("-i");
		if(_osdep->getResourceDirPath(&pathRes, prod))
		{
			// リソースディレクトリを取得し、基底ディレクトリとして設定する
			chdir(pathRes);																								// カレントディレクトリも変更しておく
			tfw::File::init(&pathRes);																					// ファイルの基底ディレクトリを設定する
			VcString fname;
			#if defined(_ENLIGHT_V01) && defined(_ENLIGHT_V01_LITE)
				fname.set("enlight1010il.dat");
			#elif defined(_ENLIGHT_V01) || defined(_ENLIGHT_PRPZL01)
				fname.set("enlight1010i.dat");
			#elif defined(_ENLIGHT_V02)
				fname.set("enlight1020i.dat");
			#elif defined(_PUZZLE_ICHTHYS)
				fname.set("puzzle1010i.dat");
			#elif defined(_ATAR)
				fname.set("atar1010i.dat");
			#endif
			tfw::IdFile::init(&fname, &postfix, SIZE_UNCOMPRESS_READ_BUFFER_1, SIZE_UNCOMPRESS_READ_BUFFER_2, 0);	// ID ファイルシステムの初期化
		}
		else
		{
			tfw::File::init(0L);
		}
	}
	
	// フレーム計測のためのストップウォッチを作成
	StopWatchIos* sw = new StopWatchIos();
	
	// 言語設定
	etk::LangId langid;
	// 選択可能な言語設定の配列を取得
	NSArray* languages = [NSLocale preferredLanguages];
	// 取得した配列から先頭の文字列を取得
	NSString* languageCur = [languages objectAtIndex:0];
	// 取得した文字列を比較
	if([languageCur compare:@"ja"] == NSOrderedSame)	{	langid = etk::LANGID_JP;	}	// 日本語の場合の処理
	else												{	langid = etk::LANGID_EN;	}	// 他の言語の場合の処理
	
	// 起動 URL クエリ解析
	if(g_queryApp != nil)
	{
		tfw::VcString query(g_queryApp);
		VcStringArray sa = StringUtils::split(&query, "&");
		for(int i = 0; i < sa.getCount(); i++)
		{
			// セーブデータ引き継ぎ起動
			if(sa.get(i)->equals("PasteSaves"))
			{
				_gcflags |= etk::EtkBody::BCF_BOOT_PASTE_SAVES;
			}
		}
	}
	
	// 表示フラグ
	u32 dispflags = [self getDisplayFlag:_vcMain.interfaceOrientation];
	TRACEERRV("### MainGlView::initGame: w=%d, h=%d, or=%d, df=%x\n", _backingWidth, _backingHeight, _vcMain.interfaceOrientation, dispflags);
	
    // ゲームの論理画面サイズ
    const f32 WIDTH_DEFAULT = 480.0f;
    const f32 HEIGHT_DEFAULT = 320.0f;
    f32 widthLogical = (f32)_backingWidth;
    f32 heightLogical = (f32)_backingHeight;
    {
        f32 qw = widthLogical / WIDTH_DEFAULT;
        f32 qh = heightLogical / HEIGHT_DEFAULT;
        if((qw > 1.0f) && (qh > 1.0f))
        {
            f32 q = qw > qh ? qh : qw;
            widthLogical /= q;
            heightLogical /= q;
        }
    }
    TRACELN("Screen: L{%f, %f}, V{%d, %d}, S{%f, %f}", widthLogical, heightLogical, _backingWidth, _backingHeight, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);

    // ゲームアプリケーション初期化
	etk::EtkBody* body = 0L;
	#if defined(_ENLIGHT_V01)
		body = new sourcerer::Game();
	#elif defined(_ENLIGHT_V01_LITE)
		body = new sourcerer::Game();
	#elif defined(_ENLIGHT_V02)
		body = new sourcerer::Game();
	#elif defined(_EDEV)
		body = new sourcerer::Game();
	#elif defined(_ENLIGHT_PRPZL01)
		body = new sourcerer::Game();
	#elif defined(_ATAR)
		body = new atar::AtArBody();
		//_gcflags |= etk::EtkBody::BCF_NO_ROT_DISPLAY;
	#endif
	if((body == 0L) || ! body->init(
        widthLogical,
        heightLogical,
		_backingWidth,
		_backingHeight,
		_scaleCoord,
		dispflags,
		RENDER_SPF,
		LOGIC_SPF,
		langid,
		(etk::EtkBody::StartBodyCase)sbcase,
		_gcflags,
		_osdep,
		_psns	))
	{
		return false;
	}
 	body->setStopWatch(sw);
	#if defined(_DEBUG)
		DebugIos::setBody(body);
	#endif

	// ポインタをつけてシステム開始
	_swFrame = sw;
	_body = body;
	
	// タイマー開始
	_animationInterval = RENDER_FPS;
	[self startAnimation];
	 
	return true;
}

/*---------------------------------------------------------------------*//**
	ゲームの後処理
**//*---------------------------------------------------------------------*/
- (void)cleanupGame:(int)tbcase
{
	TRACEERR("### MainGlView::cleanupGame\n");
	
	// タイマー停止
	[self stopAnimation];

	// ポインタを外してシステム終了
	etk::EtkBody* body = _body;
	StopWatchIos* sw = _swFrame;
	_body = nil;
	_swFrame = nil;
	
	// ゲームアプリケーション解放
	if(body != nil)
	{
		body->cleanup((etk::EtkBody::TerminateBodyCase)tbcase);
		delete body;
	}
	
	// ファイルモジュールの後処理
	tfw::IdFile::cleanup();
	tfw::File::cleanup();
	
	// ストップウォッチの削除
	if(sw != nil)
	{
		delete sw;
	}
	
	// iOS PSNS の削除
	if(_psns != nil)
	{
		delete _psns;
		_psns = nil;
	}
	
	// iOS OS 依存部の削除
	if(_osdep != nil)
	{
		delete _osdep;
		_osdep = nil;
	}
}

/*---------------------------------------------------------------------*//**
    バックグラウンドに入る
**//*---------------------------------------------------------------------*/
- (void)enterBackground
{
	TRACEERR("### MainGlView::enterBackground\n");

	// タイマー停止
	if (_animationTimer)
	{
		[self stopAnimation];
	}

	// ゲームもバックグラウンドに入れる
	if(_body)
	{
		_body->enterBackground();
	}
}

/*---------------------------------------------------------------------*//**
	バックグラウンドを抜ける
**//*---------------------------------------------------------------------*/
- (void)leaveBackground
{
	TRACEERR("### MainGlView::leaveBackground\n");

	// タイマー再開
	[self startAnimation];

	// ゲームもバックグラウンドから抜ける
	if(_body)
	{
		_body->leaveBackground();
	}
}

/*---------------------------------------------------------------------*//**
    マルチタスクがメモリ常駐モードかを確認する
**//*---------------------------------------------------------------------*/
- (bool)checkResidentMemoryMultiTaskingMode
{
	if(!_body)	{	return false;	}
    return _body->isMultiTasking();
}

/*---------------------------------------------------------------------*//**
	ディスプレイフラグを取得する
**//*---------------------------------------------------------------------*/
- (u32)getDisplayFlag:(UIInterfaceOrientation)orientation
{
	u32 dispflags = 0;
	
    /*
	// ワイド画面フラグ設定
	if(TFW_IS_FLAG(_gcflags, etk::EtkBody::BCF_WIDE_DISPLAY))
	{
		dispflags |= TFW_DISPF_WIDE;
	}
     */
	
	// 回転フラグ設定（ここはアプリによって異なるので、将来的には変数化した方がいいかもしれない）
	if(!TFW_IS_FLAG(_gcflags, etk::EtkBody::BCF_NO_ROT_DISPLAY))
	{
		switch(orientation)
		{
		case UIInterfaceOrientationPortrait:
		case UIInterfaceOrientationPortraitUpsideDown:
			dispflags |= TFW_DISPF_RANG_270;
			break;
		default:
			dispflags |= TFW_DISPF_RANG_0;
			break;
		}
	}
	
	// デバイス回転フラグを設定
	dispflags |= [self getDeviceRotationFlag:orientation];
	
	return dispflags;
}

/*---------------------------------------------------------------------*//**
	デバイス回転フラグを取得する
**//*---------------------------------------------------------------------*/
- (u32)getDeviceRotationFlag:(UIInterfaceOrientation)orientation
{
	u32 dispflags = 0;
	switch(orientation)
	{
	case UIInterfaceOrientationPortrait:
		dispflags |= TFW_DISPF_DRANG_0;
		break;
	case UIInterfaceOrientationPortraitUpsideDown:
		dispflags |= TFW_DISPF_DRANG_180;
		break;
	case UIInterfaceOrientationLandscapeLeft:
		dispflags |= TFW_DISPF_DRANG_90;
		break;
	case UIInterfaceOrientationLandscapeRight:
		dispflags |= TFW_DISPF_DRANG_270;
		break;
	}

	//TRACEERRV("### MainGlView::getDeviceRotationFlag: orientation=%d, dispflags=%x\n", orientation, dispflags);

	return dispflags;
}

/*---------------------------------------------------------------------*//**
	画面回転前通知
**//*---------------------------------------------------------------------*/
- (void)onRotateFromInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	if(_body)
	{
		u32 dispflags = [self getDisplayFlag:interfaceOrientation];
		
		TRACEERRV("### MainGlView::onRotateFromInterfaceOrientation: w=%d, h=%d, or=%d, df=%x\n", _backingWidth, _backingHeight, interfaceOrientation, dispflags);
		_body->getTfw()->resizeView(_backingWidth, _backingHeight, dispflags);
	}
}

/*---------------------------------------------------------------------*//**
	フォーカスを得られるかどうか
**//*---------------------------------------------------------------------*/
- (BOOL)canBecomeFirstResponder
{
	#if defined(_SUPPORT_MOTION_EVENT)
		return YES;	// モーションイベントをサポートする場合は必要
	#else
		return NO;
	#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アニメーション設定

/*---------------------------------------------------------------------*//**
	タイマーの開始
**//*---------------------------------------------------------------------*/
- (void)startAnimation
{
	TRACEERR("*** MainGlView::startAnimation\n");
	
	_animationTimer = [NSTimer scheduledTimerWithTimeInterval:_animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}

/*---------------------------------------------------------------------*//**
	タイマーの停止
**//*---------------------------------------------------------------------*/
- (void)stopAnimation
{
	TRACEERR("*** MainGlView::stopAnimation\n");
	
	if (_animationTimer)
	{
		[_animationTimer invalidate];
	}
	_animationTimer = nil;
}

/*---------------------------------------------------------------------*//**
	タイマー間隔の設定
**//*---------------------------------------------------------------------*/
- (void)setAnimationInterval:(NSTimeInterval)interval
{
	TRACEERR("*** MainGlView::setAnimationInterval\n");
	
	_animationInterval = interval;
	if (_animationTimer)
	{
		[self stopAnimation];
		[self startAnimation];
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 描画

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
- (void)drawView
{
	if(_viewFramebuffer == 0)	{	return;	}
	
	float frameElapse = 1.0f;
	if(_swFrame != nil)
	{
		float timeElapse = _swFrame->getElapsedSeconds();
		if(timeElapse != 0.0f)	{	frameElapse = timeElapse * LOGIC_SPF;	}
		_swFrame->start();		// この時点をフレームの開始時点とする
	}
	
	[EAGLContext setCurrentContext:_context];
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, _viewFramebuffer);
	
	if(_body != nil)
	{
		_body->renderFrame();
		_body->updateFrame(frameElapse);
	}
    
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _viewRenderbuffer);
	[_context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// タッチメソッド

#define TOUCHTRACE		//::DebugIos::trace
#define TOUCHTRACELN	//::DebugIos::traceln

/*---------------------------------------------------------------------*//**
	タッチ開始
**//*---------------------------------------------------------------------*/
- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	TOUCHTRACE("{touchesBegan} ");
	if(_body)
	{
		tfw::TouchPanel* ui = _body->getTouchPanel();
		tfw::TouchPanel::Touch arrt[NUM_TOUCH_MAX];
		int i = 0;
		for(UITouch* touch in touches)
		{
			if(i >= NUM_TOUCH_MAX)	{	break;	}
			CGPoint pt = [touch locationInView:self];
			arrt[i]._pt->set(pt.x, pt.y);
			TOUCHTRACE(tfw::VcString::format("[%d]<%f,%f>", i + 1, pt.x, pt.y));
			if(i == 0)	{	ui->moveCursor(pt.x, pt.y);	}
			i++;
		}
		ui->beganTouches(arrt, i);
	}
	TOUCHTRACELN("");
}

/*---------------------------------------------------------------------*//**
	タッチ移動
**//*---------------------------------------------------------------------*/
- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	// 間引き
	if(_cntMoveIntvl++ > 0)
	{
		if(_cntMoveIntvl >= 3)	{	_cntMoveIntvl = 0;	}
		return;
	}
	
	TOUCHTRACE("{touchesMoved} ");
	if(_body)
	{
		tfw::TouchPanel* ui = _body->getTouchPanel();
		tfw::TouchPanel::Touch arrt[NUM_TOUCH_MAX];
		int i = 0;
		for(UITouch* touch in touches)
		{
			if(i >= NUM_TOUCH_MAX)	{	break;	}
			CGPoint pt = [touch locationInView:self];
			arrt[i]._pt->set(pt.x, pt.y);
			TOUCHTRACE(tfw::VcString::format("[%d]<%f,%f>", i + 1, pt.x, pt.y));
			if(i == 0)	{	ui->moveCursor(pt.x, pt.y);	}
			i++;
		}
		ui->movedTouches(arrt, i);
	}
	TOUCHTRACELN("");
}

/*---------------------------------------------------------------------*//**
	タッチ終了
**//*---------------------------------------------------------------------*/
- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	TOUCHTRACE("{touchesEnded} ");
	if(_body)
	{
		tfw::TouchPanel* ui = _body->getTouchPanel();
		tfw::TouchPanel::Touch arrt[NUM_TOUCH_MAX];
		int i = 0;
		for(UITouch* touch in touches)
		{
			if(i >= NUM_TOUCH_MAX)	{	break;	}
			CGPoint pt = [touch locationInView:self];
			arrt[i++]._pt->set(pt.x, pt.y);
			TOUCHTRACE(tfw::VcString::format("[%d]{%f,%f}", i, pt.x, pt.y));
		}
		ui->endedTouches(arrt, i);
	}
	TOUCHTRACELN("");
}

/*---------------------------------------------------------------------*//**
	タッチキャンセル
**//*---------------------------------------------------------------------*/
- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
	TOUCHTRACE("{touchesCancelled} ");
	if(_body)
	{
		tfw::TouchPanel* ui = _body->getTouchPanel();
		tfw::TouchPanel::Touch arrt[NUM_TOUCH_MAX];
		int i = 0;
		for(UITouch* touch in touches)
		{
			if(i >= NUM_TOUCH_MAX)	{	break;	}
			CGPoint pt = [touch locationInView:self];
			arrt[i++]._pt->set(pt.x, pt.y);
			TOUCHTRACE(tfw::VcString::format("[%d]<%f,%f>", i, pt.x, pt.y));
		}
		ui->cancelledTouches(arrt, i);
	}
	TOUCHTRACELN("");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// モーションメソッド

#if defined(_SUPPORT_MOTION_EVENT)

#define MOTIONTRACE		::DebugIos::trace
#define MOTIONTRACELN	::DebugIos::traceln

/*---------------------------------------------------------------------*//**
	モーション開始
**//*---------------------------------------------------------------------*/
- (void)motionBegan:(UIEventSubtype)motion withEvent:(UIEvent *)event
{
	MOTIONTRACE("{motionBegan} ");
	if(_body)
	{
		tfw::MotionController* ui = _body->getMotionController();
		tfw::MotionController::Motion m;
		ui->beganMotion(&m);
	}
	MOTIONTRACELN("");
}

/*---------------------------------------------------------------------*//**
	モーション終了
**//*---------------------------------------------------------------------*/
- (void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event
{
	MOTIONTRACE("{motionEnded} ");
	if(_body)
	{
		tfw::MotionController* ui = _body->getMotionController();
		tfw::MotionController::Motion m;
		ui->endedMotion(&m);
	}
	MOTIONTRACELN("");
}

/*---------------------------------------------------------------------*//**
	モーションキャンセル
**//*---------------------------------------------------------------------*/
- (void)motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent *)event
{
	MOTIONTRACE("{motionCancelled} ");
	if(_body)
	{
		tfw::MotionController* ui = _body->getMotionController();
		tfw::MotionController::Motion m;
		ui->cancelledMotion(&m);
	}
	MOTIONTRACELN("");
}

#endif	// #if SUPPORT_SHAKE_EVENT

@end

////////////////////////////////////////////////////////////////////////////
