/***********************************************************************//**
 *	TitleScene.cpp
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/06/07.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TitleScene.h"

// Friends
#include "../body/AtArBody.h"
#include "../body/MaterialManager.h"
#include "../body/MovieFileTextureIos.h"
#include "../body/LoadingScreen.h"
#include "../file/AtArFileIdDef.h"
#include "../gui/AtArFontSet.h"
#include "../gui/AtArGui.h"
#include "../gui/window/implement/HelpWindow.h"
#import "../../ios/EnlightAppDelegate.h"
#import "../../ios/MainGlView.h"
#include "../../ios/OsDepModIos.h"

// External
#include "../../etk/body/EtkExecCtx.h"
#include "../../etk/gui/EtkGui.h"
#include "../../tfw/debug/ScreenTrace.h"
#include "../../tfw/gcmn/EasyStringDrawer.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/gui/Panel.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/string/VcString.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////
// マクロ

#define SHOW_RETRY_ALERT_VIEW		(1)

////////////////////////////////////////////////////////////////////////////
// 定数

#define TW_TITLE_WIDE		(1136.0f / 2048.0f)
#define TH_TITLE_WIDE		(640.0f / 2048.0f)
#define TW_TITLE_DEFAULT	(960.0f / 1024.0f)
#define TH_TITLE_DEFAULT	(640.0f / 1024.0f)

#define X_UI_BASE			(((OsDepModIos*)EtkBody::getBody()->getOsDepMod())->getEnlightAppDelegate().glView.bounds.size.width / 2)
#define Y_UI_BASE			(((OsDepModIos*)EtkBody::getBody()->getOsDepMod())->getEnlightAppDelegate().glView.bounds.size.height / 2)
#define RECT_START			X_UI_BASE - 128, Y_UI_BASE - 152, 256,  64
#define RECT_HELP			X_UI_BASE - 128, Y_UI_BASE -  72, 256,  64
#define RECT_GETMARKER		X_UI_BASE - 128, Y_UI_BASE +   8, 256,  64
#define RECT_GOSUPPORT		X_UI_BASE - 128, Y_UI_BASE +  88, 256,  64

#define W_GUI_TEX			(1024.0f)
#define H_GUI_TEX			(1024.0f)
#define UV_START_N			600.0f,   0.0f, 200.0f, 200.0f
#define UV_START_H			600.0f, 200.0f, 200.0f, 200.0f
#define UV_HELP_N			600.0f,   0.0f, 200.0f, 200.0f
#define UV_HELP_H			600.0f, 200.0f, 200.0f, 200.0f
#define UV_GETMARKER_N		600.0f,   0.0f, 200.0f, 200.0f
#define UV_GETMARKER_H		600.0f, 200.0f, 200.0f, 200.0f
#define UV_GOSUPPORT_N		600.0f,   0.0f, 200.0f, 200.0f
#define UV_GOSUPPORT_H		600.0f, 200.0f, 200.0f, 200.0f

#define FCNT_FADE_MAX		(15.0f)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MovieFileTextureIosDelegater クラス

/*---------------------------------------------------------------------*//**
 * ビデオカメラデリゲートクラス
 *
**//*---------------------------------------------------------------------*/
@interface TitleSceneDelegater : NSObject <UIAlertViewDelegate>
{
	atar::TitleScene* _own;
	UIAlertView* _alert;
}

@property atar::TitleScene* own;
@property UIAlertView* alert;

@end

@implementation TitleSceneDelegater

@synthesize own = _own;
@synthesize alert = _alert;

/*---------------------------------------------------------------------*//**
	アラートビューのボタンクリックハンドラ
**//*---------------------------------------------------------------------*/
- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)index
{
	if(_own != nil)
	{
		_own->onClickAlertButton(index);
	}
	_alert = nil;
}

/*---------------------------------------------------------------------*//**
	アラートビューのボタンクリックハンドラ
**//*---------------------------------------------------------------------*/
- (IBAction)clickButton: (UIButton*)sender
{
	_own->onClickButton(sender.tag);
}

@end	// @implementation TitleSceneDelegater

//==========================================================================
// TitleSceneInner クラス

ATAR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 * Objective-C クラス内包クラス
 *
**//*---------------------------------------------------------------------*/
class TitleSceneInner
{
public:
	TitleSceneDelegater* _delg;
	
	TitleSceneInner(TitleScene* own)
		: _delg(nil)
	{
		_delg = [[TitleSceneDelegater alloc] init];
		_delg.own = own;
	}

	~TitleSceneInner()
	{
		_delg.own = nil;

		// 終了時にアラートダイアログがまだ出ているようなのであれば、消しておく
		if(_delg.alert != nil)
		{
			[_delg.alert dismissWithClickedButtonIndex:0 animated:NO];
		}
	
		_delg = nil;
	}
};

//==========================================================================
// TitleScene メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TitleScene::TitleScene()
	: _inner(0L)
	, _phase(PHASE_NULL)
	, _texTitle(0L)
	, _fcntFade(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TitleScene::~TitleScene()
{
	ASSERT(_inner == 0L);
	ASSERT(_texTitle == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool TitleScene::create(Texture* texGuiRef)
{
	// 内包クラスの作成
	_inner = new TitleSceneInner(this);
	
	// テクスチャの作成
	Texture* tex = new Texture();
	if((tex == 0L) || (!tex->create(FILEID_CONV_WD(SYSTEM_TITLE_SD_PNG), Texture::FMT_PNG)))
	{
		ASSERT(false);
		delete tex;
		return false;
	}
	_texTitle = tex;

	// その他変数の初期化
	_phase = PHASE_NULL;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void TitleScene::destroy()
{
	// テクスチャの削除
	if(_texTitle != 0L)
	{
		_texTitle->destroy();
		delete _texTitle;
		_texTitle = 0L;
	}

	// 内包クラスの削除
	delete _inner;
	_inner = 0L;
}

/*---------------------------------------------------------------------*//**
	シーン開始
**//*---------------------------------------------------------------------*/
void TitleScene::beginScene(void* param)
{
	EnlightAppDelegate* appdelg = ((OsDepModIos*)EtkBody::getBody()->getOsDepMod())->getEnlightAppDelegate();

	// ビューにボタンを追加する
	[appdelg createButtonOnView:appdelg.glView
		target:_inner->_delg
		action:@selector(clickButton:)
		uitag:(int)UITAG_START
		show:true
		frame:CGRectMake(RECT_START)
		title:NSLocalizedString(@"button_start", @"")
		titleFontSize:18
		normalTitleColor:[UIColor colorWithRed:0.2196 green:0.3294 blue:0.5294 alpha:1.0]
		highlightedTitleColor:[UIColor colorWithRed:0.5196 green:0.3294 blue:0.5294 alpha:1.0]
		normalBgImageName:@"UI_BUTTON_BACK_R.png"
		highlightedImageName:@"UI_BUTTON_BACK_P.png"];
	[appdelg createButtonOnView:appdelg.glView
		target:_inner->_delg
		action:@selector(clickButton:)
		uitag:(int)UITAG_HELP
		show:true
		frame:CGRectMake(RECT_HELP)
		title:NSLocalizedString(@"button_help", @"")
		titleFontSize:18
		normalTitleColor:[UIColor colorWithRed:0.2196 green:0.3294 blue:0.5294 alpha:1.0]
		highlightedTitleColor:[UIColor colorWithRed:0.5196 green:0.3294 blue:0.5294 alpha:1.0]
		normalBgImageName:@"UI_BUTTON_BACK_R.png"
		highlightedImageName:@"UI_BUTTON_BACK_P.png"];
	[appdelg createButtonOnView:appdelg.glView
		target:_inner->_delg
		action:@selector(clickButton:)
		uitag:(int)UITAG_GETMARKER
		show:true
		frame:CGRectMake(RECT_GETMARKER)
		title:NSLocalizedString(@"button_getmarker", @"")
		titleFontSize:18
		normalTitleColor:[UIColor colorWithRed:0.2196 green:0.3294 blue:0.5294 alpha:1.0]
		highlightedTitleColor:[UIColor colorWithRed:0.5196 green:0.3294 blue:0.5294 alpha:1.0]
		normalBgImageName:@"UI_BUTTON_BACK_R.png"
		highlightedImageName:@"UI_BUTTON_BACK_P.png"];
	[appdelg createButtonOnView:appdelg.glView
		target:_inner->_delg
		action:@selector(clickButton:)
		uitag:(int)UITAG_GOSUPPORT
		show:true
		frame:CGRectMake(RECT_GOSUPPORT)
		title:NSLocalizedString(@"button_gosupport", @"")
		titleFontSize:18
		normalTitleColor:[UIColor colorWithRed:0.2196 green:0.3294 blue:0.5294 alpha:1.0]
		highlightedTitleColor:[UIColor colorWithRed:0.5196 green:0.3294 blue:0.5294 alpha:1.0]
		normalBgImageName:@"UI_BUTTON_BACK_R.png"
		highlightedImageName:@"UI_BUTTON_BACK_P.png"];

	// その他変数の初期設定
	_phase = PHASE_STABLE;
	_fcntFade = -1.0f;
}

/*---------------------------------------------------------------------*//**
	シーン終了
**//*---------------------------------------------------------------------*/
void TitleScene::endScene(void* param)
{
	// ビューからボタンを削除する
	EnlightAppDelegate* appdelg = ((OsDepModIos*)EtkBody::getBody()->getOsDepMod())->getEnlightAppDelegate();
	[appdelg destroyAllButtonsOnView:appdelg.glView];
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void TitleScene::exec(ExecRes* res, const ExecCtx* ec)
{
	EtkExecCtx* eec = (EtkExecCtx*)ec;
	TouchPanel* touchp = eec->getTouchPanel(); 
	MaterialManager* mtrlmng = ((AtArBody*)AtArBody::getBody())->getMaterialManager();
	LoadingScreen* ldscr = ((AtArBody*)AtArBody::getBody())->getLoadingScreen();
	EnlightAppDelegate* appdelg = ((OsDepModIos*)EtkBody::getBody()->getOsDepMod())->getEnlightAppDelegate();
	
	// ボタンの有効性更新
	bool isShowButton = !eec->isOpenedLockGui() && (_phase == PHASE_STABLE);
	for(UIView* subview in [appdelg.glView subviews])
	{
		if(subview.hidden != (!isShowButton))
		{
			subview.hidden = !isShowButton;
		}
	}	

	// フェイズ処理
	switch(_phase)
	{
	case PHASE_START_DOWNLOAD_XML:
		mtrlmng->startFirstDownload();
		ldscr->begin();
		// 次へ
		_phase = PHASE_WAIT_DOWNLOAD_XML;
		break;
	case PHASE_WAIT_DOWNLOAD_XML:
		switch(mtrlmng->getStatus())
		{
		case MaterialManager::STATUS_SUCCEEDED_DOWNDLOAD:
			ldscr->end();
			// 次へ
			_phase = PHASE_NEXT;
			break;
		case MaterialManager::STATUS_FAILED_DOWNDLOAD:
			// アラートビュー（メッセージボックス）の表示
			#if SHOW_RETRY_ALERT_VIEW
			{
				// リトライするかをユーザーに尋ねる
				_inner->_delg.alert = [[UIAlertView alloc] initWithTitle:@"" message:NSLocalizedString(@"msg_retry_material_download", nil) delegate:_inner->_delg cancelButtonTitle:NSLocalizedString(@"button_retry", nil) otherButtonTitles:nil ];
				#if defined(_DEBUG)
					[_inner->_delg.alert addButtonWithTitle:NSLocalizedString(@"button_cancel", nil)];
				#endif
				[_inner->_delg.alert show];
			}
			#endif
			ldscr->end();
			// 次へ
			_phase = PHASE_WAIT_RETRY;
			break;
		}
		break;
	case PHASE_WAIT_RETRY:
		#if !SHOW_RETRY_ALERT_VIEW
		{
			if(!touchp->isReacted() && touchp->isTapRelease(0L, 0L))
			{
				// 次へ
				_phase = PHASE_START_DOWNLOAD_XML;
			}
		}
		#endif
		break;
	case PHASE_NEXT:
		if(_fcntFade <= 0.0f)
		{
			// シーン移動
			res->setGeneralValue1(AtArBody::SCENE_AR);
			res->setDone();
			touchp->setReacted(true);
			_phase = PHASE_NULL;
		}
		else
		{
			_fcntFade -= ec->getDeltaFrame();
			if(_fcntFade < 0.0f)	{	_fcntFade = 0.0f;	}
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void TitleScene::render(const RenderCtx* rc)
{
#if defined(_DEBUG) && 0
	ScreenTrace* st = EtkBody::getBody()->getScreenTrace();
	if(st != 0L)
	{
		st->traceFormat("--- TITLE ---");
	}
#endif
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void TitleScene::draw(const RenderCtx* rc)
{
	// 背景描画
//	ColorU8 col(245, 245, 245, 255);
	ColorU8 col(0, 0, 0, 255);
	RendererUtils::draw2dColorRect(rc->getRenderer(), 0.0f, 0.0f, EtkBody::getBody()->getLogicalWidth(), EtkBody::getBody()->getLogicalHeight(), &col, &col, &col, &col);

	// テクスチャ描画
	if(_fcntFade >= 0.0f)
	{
		f32 alpha = _fcntFade / FCNT_FADE_MAX;
		rc->getRenderer()->setColorAlpha((u8)(255.0f * alpha));
	}
	rc->getRenderer()->bindTexture(_texTitle);
	RendererUtils::draw2dTextureRect(rc->getRenderer(), 0.0f, 0.0f, EtkBody::getBody()->getLogicalWidth(), EtkBody::getBody()->getLogicalHeight(), 0.0f, 0.0f, Env_isWdResolution() ? TW_TITLE_WIDE : TW_TITLE_DEFAULT, Env_isWdResolution() ?  TH_TITLE_WIDE : TH_TITLE_DEFAULT);

#if defined(_DEBUG) && 0
	// 状態表示
	Font* font = EtkBody::getBody()->getGuiManager()->getFontSet()->getFont(AtArFontSet::JP);
	rdr->setSolidColorForce(255, 255, 255, 255);
	#define RECT_MSG 0.0f, 224.0f, (f32)EtkBody::getBody()->getLogicalWidth(), 32.0f
	VcString msg;
	switch(_phase)
	{
	case PHASE_WAIT_DOWNLOAD_XML:
		msg.set("DOWNLOADING");
		break;
	case PHASE_WAIT_RETRY:
		msg.set("RETRY ?");
		break;
	case PHASE_WAIT_START:
		msg.set("START ?");
		break;
	}
	EasyStringDrawer::draw(font, &msg, RECT_MSG, 16, EasyStringDrawer::F_ALIGN_CENTER, rc);
#endif

	// 描画設定を戻す
	if(_fcntFade >= 0.0f)	{	rc->getRenderer()->setColorAlpha(255);	}
}

/*---------------------------------------------------------------------*//**
	アラートボタンクリックハンドラ
**//*---------------------------------------------------------------------*/
void TitleScene::onClickAlertButton(int index)
{
	// リトライのアラートビューであろう
	switch(index)
	{
	default:	// リトライ
		_phase = PHASE_START_DOWNLOAD_XML;
		break;
	case 1:		// キャンセル（DEBUG）
		_phase = PHASE_NEXT;
		break;
	}
}

/*---------------------------------------------------------------------*//**
	アラートボタンクリックハンドラ
**//*---------------------------------------------------------------------*/
void TitleScene::onClickButton(int uitag)
{
	TRACELN("!");
	switch(uitag)
	{
	case UITAG_START:
		// 次へ
		#if OPTVER_NO_FIRSTLOAD
			_phase = PHASE_NEXT;
		#else
			_phase = PHASE_START_DOWNLOAD_XML;
		#endif
		_fcntFade = FCNT_FADE_MAX;	// フェード開始
		break;
	case UITAG_HELP:
		// ヘルプウインドウ表示
		{
			HelpWindow* wndHelp = ((AtArBody*)AtArBody::getBody())->getGui()->getHelpWindow();
			wndHelp->setContents(HelpWindow::CNTNS_HELP);
			wndHelp->showWindow(true);
		}
		break;
	case UITAG_GETMARKER:
		// マーカー取得方法ウインドウ表示
		{
			HelpWindow* wndHelp = ((AtArBody*)AtArBody::getBody())->getGui()->getHelpWindow();
			wndHelp->setContents(HelpWindow::CNTNS_GETMARKER);
			wndHelp->showWindow(true);
		}
		/*
		// 該当ホームページへブラウザでジャンプ
		{
			CcString url(URL_GETMARKER);
			EtkBody::getBody()->getOsDepMod()->gotoHp(&url);
		}
		*/
		break;
	case UITAG_GOSUPPORT:
		// お問い合わせウインドウ表示
		{
			HelpWindow* wndHelp = ((AtArBody*)AtArBody::getBody())->getGui()->getHelpWindow();
			NSURL* url = [NSURL URLWithString:@"tel:0-00-0000-0000"];
			if([[UIApplication sharedApplication] canOpenURL:url])
			{
				wndHelp->setContents(HelpWindow::CNTNS_GOSUPPORT_PHONE_YES);
			}
			else
			{
				wndHelp->setContents(HelpWindow::CNTNS_GOSUPPORT_PHONE_NO);
			}
			wndHelp->showWindow(true);
		}
		/*
		// 該当ホームページへブラウザでジャンプ
		{
			CcString url(URL_GOSUPPORT);
			EtkBody::getBody()->getOsDepMod()->gotoHp(&url);
		}
		*/
		break;
	}
}

/*---------------------------------------------------------------------*//**
	ボタン作成
**//*---------------------------------------------------------------------*/
Panel* TitleScene::createButton(Texture* texGuiRef, const RectS32* rectPanel, const RectF* uvPanelNormal, const RectF* uvPanelHover)
{
	ColorU8 col;

	Panel* pnl = new Panel();
	if(pnl == 0L)
	{
		ASSERT(false);
		return 0L;
	}
	if(!pnl->create(rectPanel))
	{
		ASSERT(false);
		return 0L;
	}
	pnl->setStateInfo(Panel::STATE_NORMAL, texGuiRef, uvPanelNormal, ColorU8::setout(&col, 255, 255, 255, 255));
	pnl->setStateInfo(Panel::STATE_HOVER, texGuiRef, uvPanelHover, ColorU8::setout(&col, 255, 255, 255, 255));

	return pnl;
}

ATAR_END_NS