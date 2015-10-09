/***********************************************************************//**
 *	HelpWindow.cpp
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/07/26.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HelpWindow.h"

// Friends
#include "../../../file/AtArFileIdDef.h"

// External
#include "../../../../etk/body/EtkBody.h"
#include "../../../../etk/body/EtkExecCtx.h"
#include "../../../../etk/body/OsDepMod.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/gcmn/Texture.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gcmn/View.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

ATAR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 配置場所
#define RECT_BACK			5, 3, 48, 48
#define RECT_PREV_A32		5, (EtkBody::getBody()->getLogicalHeight() / 2) - 25, 48, 48
#define RECT_NEXT_A32		EtkBody::getBody()->getLogicalWidth() - 48 - 5, (EtkBody::getBody()->getLogicalHeight() / 2) - 25, 48, 48
#define RECT_PREV_A43		5, 3, 48, 48
#define RECT_NEXT_A43		EtkBody::getBody()->getLogicalWidth() - 50 - 5, 3, 48, 48


// テクスチャ UV 値
#define W_GUI_TEX			(1024.0f)
#define H_GUI_TEX			(1024.0f)
#define UV_BACK_N			  0.0f,   0.0f, 200.0f, 200.0f
#define UV_BACK_H			  0.0f, 200.0f, 200.0f, 200.0f
#define UV_PREV_N			200.0f,   0.0f, 200.0f, 200.0f
#define UV_PREV_H			200.0f, 200.0f, 200.0f, 200.0f
#define UV_NEXT_N			400.0f,   0.0f, 200.0f, 200.0f
#define UV_NEXT_H			400.0f, 200.0f, 200.0f, 200.0f
#define W_CONTENTS_TEX		(2048.0f)
#define H_CONTENTS_TEX		(2048.0f)
#define UV_CONTENTS_TEX		  0.0f,   0.0f, 2048.0f, 1536.0f

// フェードカウンタ
#define FCNT_FADE			(30.0f)

////////////////////////////////////////////////////////////////////////////
// クラス


////////////////////////////////////////////////////////////////////////////
// 定数

//==========================================================================
// HelpWindow メソッド

// 内容テクスチャファイル ID
const u32 HelpWindow::FILEID_CONTENTS_TEX[NUM_CNTNS][NUM_PAGE] =
{
	// CNTNS_NULL
	{ 0, 0 },
	// CNTNTS_HELP
	{ AR_HELP_PAGE1_SD_PNG, AR_HELP_PAGE2_SD_PNG },
	// CNTNS_GETMARKER
	{ AR_GETMARKER_PAGE1_SD_PNG, AR_GETMARKER_PAGE2_SD_PNG },
	// CNTNS_GOSUPPORT_PHONE_NO
	{ AR_GOSUPPORT_PHONE_NO_SD_PNG, 0 },
	// CNTNS_GOSUPPORT_PHONE_YES
	{ AR_GOSUPPORT_PHONE_YES_SD_PNG, 0 },
};

// 内容リンク矩形
const s32 HelpWindow::RECT_CONTENTS_LINK[NUM_CNTNS][NUM_PAGE][NUM_LINK][4] =
{
	// CNTNS_NULL
	{
		{	{   0,   0,   0,   0 },	{   0,   0,   0,   0 },	},
		{	{   0,   0,   0,   0 },	{   0,   0,   0,   0 },	},
	},
	// CNTNTS_HELP
	{
		{	{   0,   0,   0,   0 },	{   0,   0,   0,   0 },	},
		{	{   0,   0,   0,   0 },	{   0,   0,   0,   0 },	},
	},
	// CNTNS_GETMARKER
	{
		{	{   0, 256, 480,  48 },	{   0,   0,   0,   0 },	},
		{	{   0, 256, 480,  48 },	{   0,   0,   0,   0 },	},
	},
	// CNTNS_GOSUPPORT_PHONE_NO
	{
		{	{   0, 172, 464,  48},	{   0,   0,   0,   0 },	},
		{	{   0,   0,   0,   0 },	{   0,   0,   0,   0 },	},
	},
	// CNTNS_GOSUPPORT_PHONE_YES
	{
		{	{   0, 100, 240,  48 },	{   0, 188, 464,  48 },	},
		{	{   0,   0,   0,   0 },	{   0,   0,   0,   0 },	},
	},
};

// 内容リンク URL
const char* HelpWindow::URL_CONTENTS_LINK[NUM_CNTNS][NUM_PAGE][NUM_LINK] =
{
	// CNTNS_NULL
	{
		{	"",	"",	},
		{	"",	"",	},
	},
	// CNTNTS_HELP
	{
		{	"",	"",	},
		{	"",	"",	},
	},
	// CNTNS_GETMARKER
	{
		{	"http://www.pancrase.co.jp/hybrid-ar/pdf/logo_marker.pdf", "", },
		{	"http://www.pancrase.co.jp/hybrid-ar/pdf/magazine_list.pdf", "", },
	},
	// CNTNS_GOSUPPORT_NO
	{
		{	"http://www.pancrase.co.jp/hybrid-ar/", ""	},
		{	"",	"",	},
	},
	// CNTNS_GOSUPPORT_YES
	{
		{	"tel:03-5339-9198",	"http://www.pancrase.co.jp/hybrid-ar/",	},
		{	"",	"",	},
	},
};

//==========================================================================
// HelpWindow メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HelpWindow::HelpWindow()
	: _texGuiRef(0L)
	, _isDone(false)
	, _isInputDone(false)
	, _contents(CNTNS_NULL)
	, _idxCurPage(0)
	, _fcntFade(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HelpWindow::~HelpWindow()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HelpWindow::create(Font* fontRef, Texture* texGuiRef)
{
	RectS32 rect;
	RectF uv1, uv2;

	// ウインドウの作成
	rect.set(0, 0, EtkBody::getBody()->getLogicalWidth(), EtkBody::getBody()->getLogicalHeight());
	if(!Window::create(&rect))
	{
		return false;
	}

	// ボタンの作成
	f32 aspectScr = EtkBody::getBody()->getScreenWidth() / (f32)EtkBody::getBody()->getScreenHeight();
	if(!_btn[BTN_BACK].create(
		texGuiRef,
		RectS32::setout(&rect, RECT_BACK),
		Gcalc::calcTexUv(&uv1, UV_BACK_N, W_GUI_TEX, H_GUI_TEX, 1.0f),
		Gcalc::calcTexUv(&uv2, UV_BACK_H, W_GUI_TEX, H_GUI_TEX, 1.0f)))
	{	ASSERT(false);	return false;	}
	if(!_btn[BTN_PREV].create(
		texGuiRef,
		(aspectScr >= ETK_ASPECT_3_2) ? RectS32::setout(&rect, RECT_PREV_A32) : RectS32::setout(&rect, RECT_PREV_A43),
		Gcalc::calcTexUv(&uv1, UV_PREV_N, W_GUI_TEX, H_GUI_TEX, 1.0f),
		Gcalc::calcTexUv(&uv2, UV_PREV_H, W_GUI_TEX, H_GUI_TEX, 1.0f)))
	{	ASSERT(false);	return false;	}
	if(!_btn[BTN_NEXT].create(
		texGuiRef,
		(aspectScr >= ETK_ASPECT_3_2) ? RectS32::setout(&rect, RECT_NEXT_A32) : RectS32::setout(&rect, RECT_NEXT_A43),
		Gcalc::calcTexUv(&uv1, UV_NEXT_N, W_GUI_TEX, H_GUI_TEX, 1.0f),
		Gcalc::calcTexUv(&uv2, UV_NEXT_H, W_GUI_TEX, H_GUI_TEX, 1.0f)))
	{	ASSERT(false);	return false;	}

	// その他の変数初期化
	_texGuiRef = texGuiRef;		// テクスチャ参照を保存
	_isDone = false;			// 終了フラグをクリア
	_isInputDone = false;		// 入力フラグをクリア
	_contents = CNTNS_HELP;		// デフォルト設定
	_idxCurPage = 0;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void HelpWindow::destroy()
{
	// ボタンの削除
	for(int i = 0; i < NUM_BTN; i++)
	{
		_btn[i].destroy();
	}
	
	// ページテクスチャの削除
	for(int i = 0; i < NUM_PAGE; i++)
	{
		if(_page[i]._tex == 0L)	{	continue;	}
		_page[i]._tex->destroy();
		delete _page[i]._tex;
		_page[i]._tex = 0L;
	}

	Window::destroy();
}

/*---------------------------------------------------------------------*//**
	内容のフレーム制御
**//*---------------------------------------------------------------------*/
void HelpWindow::execContents(ExecRes* res, const ExecCtx* ec)
{
	EtkExecCtx* eec = (EtkExecCtx*)ec;
	TouchPanel* touchp = eec->getTouchPanel();

	// タッチをボタンに伝達
	PointS32 ptN, ptS;
	bool isActive = (touchp->getTouchingNum() == 1) && !touchp->isReacted();	// シングル
	touchp->getTouchStartPosition(&ptS);
	touchp->getTouchPosition(&ptN);
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_btn[i]._isEnable)
		{
			_btn[i]._pnl->hit(&ptN, &ptS, isActive);
		}
	}
	
	// 操作
	if(touchp->isTapRelease(&ptN, &ptS) && !touchp->isReacted() && (_fcntFade == 0.0f))
	{
		for(int i = 0; i < NUM_BTN; i++)
		{
			if(_btn[i]._isEnable)
			{
				if(_btn[i]._pnl->hitTest(&ptN) && _btn[i]._pnl->hitTest(&ptS))
				{
					switch(i)
					{
					case BTN_BACK:
						// フェードアウト開始
						_fcntFade = - FCNT_FADE;
						break;
					case BTN_PREV:
						if(_idxCurPage > 0)
						{
							_idxCurPage--;
							updateButtonsState();
							
							_page[PAGE_1]._xOffset = - EtkBody::getBody()->getLogicalWidth();
							_page[PAGE_2]._xOffset = - EtkBody::getBody()->getLogicalWidth();
						}
						break;
					case BTN_NEXT:
						if(_idxCurPage < (NUM_PAGE - 1))
						{
							_idxCurPage++;
							updateButtonsState();
							
							_page[PAGE_1]._xOffset = EtkBody::getBody()->getLogicalWidth();
							_page[PAGE_2]._xOffset = EtkBody::getBody()->getLogicalWidth();
						}
						break;
					}
					
					touchp->setReacted(true);	// UI 反応済み
					break;
				}
			}
		}
		
		if(!touchp->isReacted())
		{
			for(int j = 0; j < NUM_LINK; j++)
			{
				if(_link[_idxCurPage][j]._url->getLength() <= 0)	{	continue;	}
				if(_link[_idxCurPage][j]._rect->isPointIn(&ptN) && _link[_idxCurPage][j]._rect->isPointIn(&ptS))
				{
					EtkBody::getBody()->getOsDepMod()->gotoHp(_link[_idxCurPage][j]._url);
				}
			}
		}
	}
	
	// ボタンのフレーム処理
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_btn[i]._isEnable)
		{
			_btn[i]._pnl->exec(res, ec);
		}
	}
	
	// ページのフレーム処理
	for(int i = 0; i < NUM_PAGE; i++)
	{
		// ページ移動アニメーション
		if(_page[i]._xOffset != 0.0f)
		{
			_page[i]._xOffset *= 0.75f;
			if(TFW_IS_NEAR(_page[i]._xOffset, 0.0f, 1.0f))
			{
				_page[i]._xOffset = 0.0f;
			}
		}
	}
	
	// フェードカウンタの更新
	if(_fcntFade > 0.0f)		// フェードイン
	{
		_fcntFade -= ec->getDeltaFrame();
		if(_fcntFade < 0.0f)	{	_fcntFade = 0.0f;	}
	}
	else if(_fcntFade < 0.0f)	// フェードアウト
	{
		_fcntFade += ec->getDeltaFrame();
		if(_fcntFade > 0.0f)	{	_fcntFade = 0.0f;	}
		if(_fcntFade >= 0.0f)
		{
			// ウインドウを閉じて終了
			showWindow(false);
		}
	}
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void HelpWindow::drawContents(const RenderCtx* rc, const RectS32* rectBase)
{
	RectF vtx, uv;
	ColorU8 col;

	rc->getRenderer()->setSolidColor(255, 255, 255, 255);

	// フェードのアルファ値
	f32 falphaFade = (_fcntFade > 0.0f) ? (FCNT_FADE - _fcntFade) / FCNT_FADE : (_fcntFade < 0.0f) ? (- _fcntFade) / FCNT_FADE : 1.0f;

	// 下地表示
	col.set(255, 255, 255, (falphaFade > 0.5f) ? 255 : (u8)(255.0f * falphaFade * 2.0f));
	RendererUtils::draw2dColorRect(rc->getRenderer(), RectF::setout(&vtx, 0.0f, 0.0f, EtkBody::getBody()->getStandardWidth(), EtkBody::getBody()->getStandardHeight()), &col);

	// ページの描画
	col.set(255, 255, 255, (u8)(255.0f * falphaFade));
	const View* view = EtkBody::getBody()->getTfw()->getView();
	f32 xofsScr = (view->getAspect() - (1024.0f / 768.0f)) * EtkBody::getBody()->getStandardHeight() * 0.5f;	// iPad 解像度を基準にしたスクリーン位置オフセット
	for(int i = 0; i < NUM_PAGE; i++)
	{
		s32 wScr = EtkBody::getBody()->getScreenWidth();
		s32 wLScr = EtkBody::getBody()->getLogicalWidth();
		vtx.x() = ((i - _idxCurPage) * EtkBody::getBody()->getLogicalWidth()) + _page[i]._xOffset + xofsScr;
		vtx.y() = 0.0f;
		vtx.w() = EtkBody::getBody()->getLogicalWidth() - xofsScr - xofsScr;
		vtx.h() = EtkBody::getBody()->getLogicalHeight();
		rc->getRenderer()->bindTexture(_page[i]._tex);
		Gcalc::calcTexUv(&uv, UV_CONTENTS_TEX, W_CONTENTS_TEX, H_CONTENTS_TEX, 1.0f);
		RendererUtils::draw2dColorTextureRect(rc->getRenderer(), &vtx, &uv, &col, &col, &col, &col);
	}

	// ボタンの描画
	if(_fcntFade == 0.0f)
	{
		for(int i = 0; i < NUM_BTN; i++)
		{
			if(_btn[i]._isEnable)
			{
				_btn[i]._pnl->draw(rc);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	ボタンの状態更新
**//*---------------------------------------------------------------------*/
void HelpWindow::updateButtonsState()
{
	// 1 ページのみの場合
	int numPage = 0;
	for(int i = 0; i < NUM_PAGE; i++)
	{
		if(_page[i]._tex != 0L)
		{
			numPage++;
		}
	}
	if(numPage <= 1)
	{
		if(!_btn[BTN_BACK]._pnl->isEnable()){	_btn[BTN_BACK]._pnl->setEnable(true);	}
		if(_btn[BTN_PREV]._pnl->isEnable()){	_btn[BTN_PREV]._pnl->setEnable(false);	}
		if(_btn[BTN_NEXT]._pnl->isEnable()){	_btn[BTN_NEXT]._pnl->setEnable(false);	}
		return;
	}

	// 戻るボタンの更新
	f32 aspectScr = EtkBody::getBody()->getScreenWidth() / (f32)EtkBody::getBody()->getScreenHeight();
	if(aspectScr >= ETK_ASPECT_3_2)
	{
		if(!_btn[BTN_BACK]._pnl->isEnable()){	_btn[BTN_BACK]._pnl->setEnable(true);	}
	}
	else
	{
		if(_idxCurPage <= 0)
		{
			if(!_btn[BTN_BACK]._pnl->isEnable()){	_btn[BTN_BACK]._pnl->setEnable(true);	}
		}
		else
		{
			if(_btn[BTN_BACK]._pnl->isEnable()){	_btn[BTN_BACK]._pnl->setEnable(false);	}
		}
	}

	// 左右ボタンの更新
	if(_idxCurPage <= 0)
	{
		if(_btn[BTN_PREV]._pnl->isEnable()){	_btn[BTN_PREV]._pnl->setEnable(false);	}
		if(!_btn[BTN_NEXT]._pnl->isEnable()){	_btn[BTN_NEXT]._pnl->setEnable(true);	}
	}
	else if(_idxCurPage >= (NUM_PAGE - 1))
	{
		if(!_btn[BTN_PREV]._pnl->isEnable()){	_btn[BTN_PREV]._pnl->setEnable(true);	}
		if(_btn[BTN_NEXT]._pnl->isEnable()){	_btn[BTN_NEXT]._pnl->setEnable(false);	}
	}
	else
	{
		if(!_btn[BTN_PREV]._pnl->isEnable()){	_btn[BTN_PREV]._pnl->setEnable(true);	}
		if(!_btn[BTN_NEXT]._pnl->isEnable()){	_btn[BTN_NEXT]._pnl->setEnable(true);	}
	}
}

/*---------------------------------------------------------------------*//**
	表示状態の変更通知
**//*---------------------------------------------------------------------*/
void HelpWindow::onShowWindow(bool isShow)
{
	Window::onShowWindow(isShow);

	if(!isShow)
	{
		// ページテクスチャの削除
		for(int i = 0; i < NUM_PAGE; i++)
		{
			if(_page[i]._tex == 0L)	{	continue;	}
			_page[i]._tex->destroy();
			delete _page[i]._tex;
			_page[i]._tex = 0L;
		}
		
		return;
	}

	// ページテクスチャの読み込み
	for(int i = 0; i < NUM_PAGE; i++)
	{
		if(FILEID_CONTENTS_TEX[_contents][i] == 0)	{	continue;	}
		
		Texture* tex = new Texture();
		if(!tex->create(FILEID_CONV_RD( FILEID_CONTENTS_TEX[_contents][i] ), Texture::FMT_PNG))
		{
			delete tex;
			ASSERTM(false, "{HelpWindow::create} Texture::create failed.");
			tex = 0L;
		}
		_page[i]._tex = tex;
		_page[i]._xOffset = 0.0f;
	}
	
	// リンクの設定
	for(int i = 0; i < NUM_PAGE; i++)
	{
		for(int j = 0; j < NUM_LINK; j++)
		{
			_link[i][j]._rect->_v[0] = RECT_CONTENTS_LINK[_contents][i][j][0];
			_link[i][j]._rect->_v[1] = RECT_CONTENTS_LINK[_contents][i][j][1];
			_link[i][j]._rect->_v[2] = RECT_CONTENTS_LINK[_contents][i][j][2];
			_link[i][j]._rect->_v[3] = RECT_CONTENTS_LINK[_contents][i][j][3];
			_link[i][j]._url->set(URL_CONTENTS_LINK[_contents][i][j]);
		}
	}

	// 値をクリア
	_isDone = false;				// 終了フラグをクリア
	_isInputDone = false;			// 入力フラグをクリア
	_idxCurPage = 0;
	
	// フェードイン開始
	_fcntFade = FCNT_FADE;

	// ボタンの状態を更新する
	updateButtonsState();
}

//==========================================================================
// HelpWindow::Button メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HelpWindow::Button::Button()
	: _pnl(0L)
	, _isEnable(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HelpWindow::Button::~Button()
{
	ASSERT(_pnl == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HelpWindow::Button::create(Texture* texGuiRef, const RectS32* rectPanel, const RectF* uvPanelNormal, const RectF* uvPanelHover)
{
	ColorU8 col;

	_pnl = new Panel();
	if(!_pnl->create(rectPanel))
	{
		ASSERT(false);
		return false;
	}
	_pnl->setStateInfo(Panel::STATE_NORMAL, texGuiRef, uvPanelNormal, ColorU8::setout(&col, 255, 255, 255, 255));
	_pnl->setStateInfo(Panel::STATE_HOVER, texGuiRef, uvPanelHover, ColorU8::setout(&col, 255, 255, 255, 255));

	_isEnable = true;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void HelpWindow::Button::destroy()
{
	if(_pnl != 0L)
	{
		_pnl->destroy();
		delete _pnl;
		_pnl = 0L;
	}
}

//==========================================================================
// HelpWindow::Page メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HelpWindow::Page::Page()
	: _tex(0L)
	, _xOffset(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HelpWindow::Page::~Page()
{
	ASSERT(_tex == 0L);
}

//==========================================================================
// HelpWindow::Link メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HelpWindow::Link::Link()
	: _rect(new RectS32())
	, _url(new VcString()
	)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HelpWindow::Link::~Link()
{
	delete _rect;
	delete _url;
}

////////////////////////////////////////////////////////////////////////////

ATAR_END_NS
