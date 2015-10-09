/***********************************************************************//**
 *	MovieScene.cpp
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
#include "MovieScene.h"

// Friends
#include "../body/AtArBody.h"
#include "../body/MaterialManager.h"
#include "../body/MovieFileTextureIos.h"
#include "../file/AtArFileIdDef.h"
#include "../gui/AtArFontSet.h"
#import "../../ios/EnlightAppDelegate.h"
#include "../../ios/OsDepModIos.h"
#import "../../ios/MainGlView.h"
#import "../../ios/MoviePlaybackView.h"

// External
#include "../../etk/body/EtkExecCtx.h"
#include "../../etk/gui/EtkGui.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/string/VcString.h"

// Library

////////////////////////////////////////////////////////////////////////////

ATAR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define TFW_RECTF_TO_CGRECT(rect)	CGRectMake((rect)->x(), (rect)->y(), (rect)->w(), (rect)->h())

////////////////////////////////////////////////////////////////////////////
// 定数

#define YOFS_MORE_BTN_IPAD		(0)
#define YOFS_MORE_BTN_IPHONE	(-32)
#define W_MORE_BTN				(256)
#define H_MORE_BTN				(95)
#define YOFS_BACK_BTN_IPAD		(128)
#define YOFS_BACK_BTN_IPHONE	(95)
#define W_BACK_BTN				(256)
#define H_BACK_BTN				(64)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MovieFileTextureIosDelegater クラス

ATAR_END_NS

/*---------------------------------------------------------------------*//**
 * ビデオカメラデリゲートクラス
 *
**//*---------------------------------------------------------------------*/
@interface MovieSceneDelegater : NSObject
{
	atar::MovieScene* _own;
}

@property atar::MovieScene* own;

@end

@implementation MovieSceneDelegater

@synthesize own = _own;

/*---------------------------------------------------------------------*//**
	アラートビューのボタンクリックハンドラ
**//*---------------------------------------------------------------------*/
- (IBAction)clickButton: (UIButton*)sender
{
	_own->onClickButton((int)sender.tag);
}

@end	// @implementation MovieSceneDelegater

ATAR_BEGIN_NS

//==========================================================================
// MovieSceneInner クラス

/*---------------------------------------------------------------------*//**
 * Objective-C クラス内包クラス
 *
**//*---------------------------------------------------------------------*/
class MovieSceneInner
{
public:
	MovieSceneDelegater* _delg;
	
	MovieSceneInner(MovieScene* own)
		: _delg(nil)
	{
		_delg = [[MovieSceneDelegater alloc] init];
		_delg.own = own;
	}

	~MovieSceneInner()
	{
		_delg = nil;
	}
};

//==========================================================================
// MovieScene メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MovieScene::MovieScene()
	: _inner(0L)
	, _phase(PHASE_NULL)
	, _idxMatchedMaterial(-1)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MovieScene::~MovieScene()
{
	ASSERT(_inner == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MovieScene::create()
{
	// 内包クラスの作成
	_inner = new MovieSceneInner(this);

	// その他変数の初期化
	_phase = PHASE_WAIT_MOVIEEND;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MovieScene::destroy()
{
	destroyScene();

	// 内包クラスの削除
	delete _inner;
	_inner = 0L;
}

/*---------------------------------------------------------------------*//**
	シーン開始
**//*---------------------------------------------------------------------*/
void MovieScene::beginScene(void* param)
{
	// マッチした素材インデックスを得る
	ExecRes* res = (ExecRes*)param;
	_idxMatchedMaterial = res->getGeneralValue2();

	// ムービー再生の開始
	MovieFileTextureIos* mtex = ((AtArBody*)AtArBody::getBody())->getMovieFileTexture();
	mtex->play();
	
	// ビューを切り替える
	EnlightAppDelegate* appdelg = ((OsDepModIos*)EtkBody::getBody()->getOsDepMod())->getEnlightAppDelegate();
	[appdelg changeActiveView: appdelg.moviePlaybackView];
	
	// プレイヤーをセット
	appdelg.moviePlaybackView.player = mtex->getPlayer();
	
	// ビューにボタンを追加
	RectF rect;
	// もっと見るボタン
	{
		/*
		UIButton* btn = [UIButton buttonWithType:UIButtonTypeCustom];
		if([btn respondsToSelector:@selector(setAttributedTitle)])	// iOS6 以上
		{
			NSMutableAttributedString* title = [[NSMutableAttributedString alloc] initWithString:NSLocalizedString(@"button_more_view", @"")];
			[title addAttributes:[NSDictionary dictionaryWithObject:[UIFont boldSystemFontOfSize:20] forKey:NSFontAttributeName] range:NSMakeRange(0, title.length)];
			[title addAttributes:[NSDictionary dictionaryWithObject:[UIColor colorWithRed:0.2196 green:0.3294 blue:0.5294 alpha:1.0] forKey:NSForegroundColorAttributeName] range:NSMakeRange(0, title.length)];
			[btn setAttributedTitle:title forState:UIControlStateNormal];
		}
		else
		{
			[btn setTitle:NSLocalizedString(@"button_more_view", @"") forState:UIControlStateNormal];
			[btn setTitleColor:[UIColor colorWithRed:0.2196 green:0.3294 blue:0.5294 alpha:1.0] forState:UIControlStateNormal];
			[btn setTitleColor:[UIColor colorWithRed:0.5196 green:0.3294 blue:0.5294 alpha:1.0] forState:UIControlStateHighlighted];
			[btn.titleLabel setFont:[UIFont fontWithName:@"HiraKakuProN-W6" size:20]];
		}
		[btn setFrame: TFW_RECTF_TO_CGRECT(makeButtonRect(&rect, UITAG_MORE_VIEW))];
		[btn setTag: (int)UITAG_MORE_VIEW];
		[btn addTarget:_inner->_delg action:@selector(clickButton:) forControlEvents:UIControlEventTouchUpInside];
		[btn setBackgroundImage:[UIImage imageNamed:@"UI_BUTTON_MORE_R.png"] forState:UIControlStateNormal];
		[btn setBackgroundImage:[UIImage imageNamed:@"UI_BUTTON_MORE_P.png"] forState:UIControlStateHighlighted];
		btn.hidden = true;	// まずは無効のまま追加
		[appdelg.moviePlaybackView addSubview: btn];
		*/
		[appdelg createButtonOnView:appdelg.moviePlaybackView
			target:_inner->_delg
			action:@selector(clickButton:)
			uitag:(int)UITAG_MORE_VIEW
			show:false
			frame:TFW_RECTF_TO_CGRECT(makeButtonRect(&rect, UITAG_MORE_VIEW))
			title:NSLocalizedString(@"button_more_view", @"")
			titleFontSize:18
			normalTitleColor:[UIColor colorWithRed:0.2196 green:0.3294 blue:0.5294 alpha:1.0]
			highlightedTitleColor:[UIColor colorWithRed:0.5196 green:0.3294 blue:0.5294 alpha:1.0]
			normalBgImageName:@"UI_BUTTON_MORE_R.png"
			highlightedImageName:@"UI_BUTTON_MORE_P.png"];
	}
	// 戻るボタン
	{
		/*
		UIButton* btn = [UIButton buttonWithType:UIButtonTypeCustom];
		if([btn respondsToSelector:@selector(setAttributedTitle)])	// iOS6 以上
		{
			NSMutableAttributedString* title = [[NSMutableAttributedString alloc] initWithString:NSLocalizedString(@"button_back", @"")];
			[title addAttributes:[NSDictionary dictionaryWithObject:[UIFont boldSystemFontOfSize:18] forKey:NSFontAttributeName] range:NSMakeRange(0, title.length)];
			[title addAttributes:[NSDictionary dictionaryWithObject:[UIColor colorWithRed:0.2196 green:0.3294 blue:0.5294 alpha:1.0] forKey:NSForegroundColorAttributeName] range:NSMakeRange(0, title.length)];
			[btn setAttributedTitle:title forState:UIControlStateNormal];
		}
		else
		{
			[btn setTitle:NSLocalizedString(@"button_back", @"") forState:UIControlStateNormal];
			[btn setTitleColor:[UIColor colorWithRed:0.2196 green:0.3294 blue:0.5294 alpha:1.0] forState:UIControlStateNormal];
			[btn setTitleColor:[UIColor colorWithRed:0.5196 green:0.3294 blue:0.5294 alpha:1.0] forState:UIControlStateHighlighted];
			[btn.titleLabel setFont:[UIFont fontWithName:@"HiraKakuProN-W6" size:18]];
		}
		[btn setFrame: TFW_RECTF_TO_CGRECT(makeButtonRect(&rect, UITAG_BACK))];
		[btn setTag: (int)UITAG_BACK];
		[btn addTarget:_inner->_delg action:@selector(clickButton:) forControlEvents:UIControlEventTouchUpInside];
		[btn setBackgroundImage:[UIImage imageNamed:@"UI_BUTTON_BACK_R.png"] forState:UIControlStateNormal];
		[btn setBackgroundImage:[UIImage imageNamed:@"UI_BUTTON_BACK_P.png"] forState:UIControlStateHighlighted];
		btn.hidden = true;	// まずは無効のまま追加
		[appdelg.moviePlaybackView addSubview: btn];
		*/
		[appdelg createButtonOnView:appdelg.moviePlaybackView
			target:_inner->_delg
			action:@selector(clickButton:)
			uitag:(int)UITAG_BACK
			show:false
			frame:TFW_RECTF_TO_CGRECT(makeButtonRect(&rect, UITAG_BACK))
			title:NSLocalizedString(@"button_back", @"")
			titleFontSize:18
			normalTitleColor:[UIColor colorWithRed:0.2196 green:0.3294 blue:0.5294 alpha:1.0]
			highlightedTitleColor:[UIColor colorWithRed:0.5196 green:0.3294 blue:0.5294 alpha:1.0]
			normalBgImageName:@"UI_BUTTON_BACK_R.png"
			highlightedImageName:@"UI_BUTTON_BACK_P.png"];
	}

	// その他変数の初期設定
	_phase = PHASE_WAIT_MOVIEEND;
}

/*---------------------------------------------------------------------*//**
	シーン終了
**//*---------------------------------------------------------------------*/
void MovieScene::endScene(void* param)
{
	destroyScene();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void MovieScene::exec(ExecRes* res, const ExecCtx* ec)
{
	EtkExecCtx* eec = (EtkExecCtx*)ec;

	switch(_phase)
	{
	case PHASE_WAIT_MOVIEEND:
		{
			EnlightAppDelegate* appdelg = ((OsDepModIos*)EtkBody::getBody()->getOsDepMod())->getEnlightAppDelegate();
			MovieFileTextureIos* mtex = ((AtArBody*)AtArBody::getBody())->getMovieFileTexture();
			if(mtex->isFinished())
			{
				MaterialManager* mtrlmng = ((AtArBody*)AtArBody::getBody())->getMaterialManager();
				const MaterialManager::ArMaterialInfo* armi = mtrlmng->getArMaterial(_idxMatchedMaterial);
				// ボタンを表示
				RectF rect;
				for(UIView* subview in [appdelg.moviePlaybackView subviews])
				{
					if((subview.tag == UITAG_MORE_VIEW) && (armi != 0L) && (armi->_url->getLength() <= 0))
					{
						continue;	// 「もっと見る」ボタンの URL が空
					}
					subview.hidden = false;
					[subview setFrame: TFW_RECTF_TO_CGRECT(makeButtonRect(&rect, subview.tag))];	// 矩形再設定
				}
				// 次へ
				_phase = PHASE_WAIT_ANSWER;
			}
			else if(((eec->getBodyExecCount() & 7) == 0) && !mtex->isPlaying())
			{
				// 再生が止まってしまっている、もしくは再生に失敗している場合は再度再生する
				TRACELN("{MovieScene::exec} force re-play!");
					mtex->play();
			}
		}
		break;
	case PHASE_WAIT_ANSWER:
		if(_idxMatchedMaterial == -1)	// 戻る
		{
			// シーン移動
			res->setGeneralValue1(AtArBody::SCENE_AR);
			res->setDone();
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void MovieScene::render(const RenderCtx* rc)
{
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void MovieScene::draw(const RenderCtx* rc)
{
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void MovieScene::onClickButton(int tag)
{
	switch(tag)
	{
	case UITAG_MORE_VIEW:	// もっと見る
		if(_idxMatchedMaterial != -1)
		{
			MaterialManager* mtrlmng = ((AtArBody*)AtArBody::getBody())->getMaterialManager();
			const MaterialManager::ArMaterialInfo* armi = mtrlmng->getArMaterial(_idxMatchedMaterial);
			if(armi != 0L)
			{
				EtkBody::getBody()->getOsDepMod()->gotoHp(armi->_url);
				return;
			}
		}
		_idxMatchedMaterial = -1;
		break;
	case UITAG_BACK:	// 戻る
		_idxMatchedMaterial = -1;
		break;
	}
}

/*---------------------------------------------------------------------*//**
	シーン破棄
**//*---------------------------------------------------------------------*/
void MovieScene::destroyScene()
{
	EnlightAppDelegate* appdelg = ((OsDepModIos*)EtkBody::getBody()->getOsDepMod())->getEnlightAppDelegate();

	// ビューからボタンを削除する
	/*
	for(UIView* subview in [appdelg.moviePlaybackView subviews])
	{
		[subview removeFromSuperview];
	}
	*/
	[appdelg destroyAllButtonsOnView:appdelg.moviePlaybackView];
	
	// プレイヤーを外す
	if(appdelg.moviePlaybackView.player != nil)
	{
		appdelg.moviePlaybackView.player = nil;
	}

	// ビューを切り替える
	if(appdelg.mainViewController.view == appdelg.moviePlaybackView)
	{
		[appdelg changeActiveView:appdelg.glView];
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
RectF* MovieScene::makeButtonRect(RectF* rect, int tag)
{
	EnlightAppDelegate* appdelg = ((OsDepModIos*)EtkBody::getBody()->getOsDepMod())->getEnlightAppDelegate();
	f32 aspectScr = EtkBody::getBody()->getScreenWidth() / (f32)EtkBody::getBody()->getScreenHeight();
	CGRect rectParent = appdelg.moviePlaybackView.bounds;
//	rectParent = [appdelg.moviePlaybackView convertRect:appdelg.moviePlaybackView.bounds fromView:appdelg.moviePlaybackView];
	switch(tag)
	{
	case UITAG_MORE_VIEW:
		rect->x() = (rectParent.size.width - W_MORE_BTN) * 0.5f;
		rect->y() = ((rectParent.size.height - H_MORE_BTN) * 0.5f) + ((aspectScr < ETK_ASPECT_3_2) ? YOFS_MORE_BTN_IPAD : YOFS_MORE_BTN_IPHONE);
		rect->w() = W_MORE_BTN;
		rect->h() = H_MORE_BTN;
		break;
	case UITAG_BACK:
		rect->x() = (rectParent.size.width - W_BACK_BTN) * 0.5f;
		rect->y() = ((rectParent.size.height - H_BACK_BTN) * 0.5f) + ((aspectScr < ETK_ASPECT_3_2) ? YOFS_BACK_BTN_IPAD : YOFS_BACK_BTN_IPHONE);
		rect->w() = W_BACK_BTN;
		rect->h() = H_BACK_BTN;
		break;
	}
	return rect;
}

////////////////////////////////////////////////////////////////////////////

ATAR_END_NS