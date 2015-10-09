/***********************************************************************//**
 *	PcStatusWindowBase.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/01.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PcStatusWindowBase.h"

// Friends
#include "../../../episode/party/MyParty.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../gui/GameGui.h"
#include "../../../unit/char/pc/PcUnit.h"

// External
#include "../../../../tfw/lib/Rect.h"
#include "../../../../tfw/lib/Size.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gui/PanelText.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#if 1
#define X_BASE						((Game::getGame()->getLogicalWidth() - 400) / 2)
#define RECT_THIS					X_BASE, 48, 400, 272
#define RECT_BTN_LEFT				((X_BASE - 40) / 2), 144, 40, 32
#define RECT_BTN_RIGHT				X_BASE + 400 + ((X_BASE - 40) / 2), 144, 40, 32
#define RECT_BTN_ITEM				X_BASE + 300, 106, 60, 24
#define X_PC_NAME					X_BASE + 44
#define Y_PC_NAME					60
#define RECT_PC_ICON				X_BASE + 44, 82, 104, 104
#else
#define RECT_THIS					42, 48, 400, 272
#define RECT_BTN_LEFT				0, 144, 40, 32
#define RECT_BTN_RIGHT				440, 144, 40, 32
#define RECT_BTN_ITEM				340, 106, 60, 24
#define X_PC_NAME					84
#define Y_PC_NAME					60
#define RECT_PC_ICON				84, 82, 104, 104
#endif

#define W_TEX						(1024.0f)	// テクスチャ全体の幅
#define H_TEX						(1024.0f)	// テクスチャ全体の高さ
#define UV_THIS						128.0f / W_TEX, 448.0f / H_TEX, 400.0f / W_TEX, 272.0f / H_TEX
#define UV_LBTN_NRM					672.0f / W_TEX, 352.0f / H_TEX, 40.0f / W_TEX, 32.0f / H_TEX
#define UV_LBTN_HVR					672.0f / W_TEX, 384.0f / H_TEX, 40.0f / W_TEX, 32.0f / H_TEX
#define UV_RBTN_NRM					712.0f / W_TEX, 352.0f / H_TEX, 40.0f / W_TEX, 32.0f / H_TEX
#define UV_RBTN_HVR					712.0f / W_TEX, 384.0f / H_TEX, 40.0f / W_TEX, 32.0f / H_TEX

#define UV_PC1_ICON					328.0f / W_TEX, 720.0f / H_TEX, 104.0f / W_TEX, 104.0f / H_TEX
#define UV_PC2_ICON					440.0f / W_TEX, 720.0f / H_TEX, 104.0f / W_TEX, 104.0f / H_TEX
#define UV_PC3_ICON					552.0f / W_TEX, 720.0f / H_TEX, 104.0f / W_TEX, 104.0f / H_TEX

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PcStatusWindowBase メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PcStatusWindowBase::PcStatusWindowBase()
	: _ipc(MyParty::PC_MY)
	, _fontRef(0L)
	, _texRef(0L)
{
	ASSERT(NUM_PCKIND == MyParty::NUM_PC);

	for(int i = 0; i < NUM_BTN; i++)	{	_pnlBtn[i] = 0;		}
	for(int i = 0; i < NUM_PCKIND; i++)	{	_pcenb[i] = true;	}	// デフォルトで有効
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PcStatusWindowBase::~PcStatusWindowBase()
{
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_BTN; i++)	{	ASSERT(_pnlBtn[i] == 0L);	}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PcStatusWindowBase::create(Font* fontRef, Texture* texRef)
{
	// ウインドウの作成
	RectF32 rectThis(RECT_THIS);
	if(!Window::create(&rectThis))
	{
		return false;
	}

	// 値保存
	_fontRef = fontRef;
	_texRef = texRef;

	RectF32 rect;
	RectF32 uv;

	// 自身の設定
	setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_THIS));

	// 子パネル作成
	{
		_pnlBtn[BTN_LEFT] = new Panel();
		_pnlBtn[BTN_LEFT]->create(RectF32::setout(&rect, RECT_BTN_LEFT));
		_pnlBtn[BTN_LEFT]->setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_LBTN_NRM));
		_pnlBtn[BTN_LEFT]->setStateInfo(STATE_HOVER, _texRef, RectF32::setout(&uv, UV_LBTN_HVR));
	}
	{
		_pnlBtn[BTN_RIGHT] = new Panel();
		_pnlBtn[BTN_RIGHT]->create(RectF32::setout(&rect, RECT_BTN_RIGHT));
		_pnlBtn[BTN_RIGHT]->setStateInfo(STATE_NORMAL, _texRef, RectF32::setout(&uv, UV_RBTN_NRM));
		_pnlBtn[BTN_RIGHT]->setStateInfo(STATE_HOVER, _texRef, RectF32::setout(&uv, UV_RBTN_HVR));
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PcStatusWindowBase::destroy()
{
	// 子パネル削除
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] != 0)
		{
			_pnlBtn[i]->destroy();
			delete _pnlBtn[i];
			_pnlBtn[i] = 0L;
		}
	}

	Window::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void PcStatusWindowBase::execContents(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	// 親の制御
	Window::execContents(res, ec);

	// 子パネル制御
	TouchPanel* ui = gec->getTouchPanel();
	PointF32 ptTouch(S32_MIN, S32_MIN);
	PointF32 ptClick(S32_MIN, S32_MIN);
	bool isTouch = !ui->isReacted() ? ui->getTouchPosition(&ptTouch) : false;
	bool isClick = !ui->isReacted() ? ui->isClickRelease(&ptClick) : false;
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] == 0)	{	continue;	}
		if(isClick)
		{
			if(_pnlBtn[i]->hit(&ptClick, true))
			{
				switch(i)
				{
				case BTN_LEFT:
					prevPc();
					break;
				case BTN_RIGHT:
					nextPc();
					break;
				}
				ui->setReacted(true);
			}
		}
		else
		{
			_pnlBtn[i]->hit(&ptTouch, isTouch);
		}
		_pnlBtn[i]->exec(res, ec);
	}
}

/*---------------------------------------------------------------------*//**
	内容の描画
**//*---------------------------------------------------------------------*/
void PcStatusWindowBase::drawContents(const RenderCtx* rc, const RectF32* rectBase)
{
	Renderer* rdr = rc->getRenderer();
	const Party* party = Game::getGame()->getMyParty();
	const PcUnit* pcunit = party->getPcUnit(_ipc);
	const CharDef* cdef = pcunit->getCharDef();
	
	// 名前
	VcString wk;
	wk = *cdef->getName(Env_getLangId());
	wk += " ";
	wk += *cdef->getFamilyName(Env_getLangId());
	rdr->setSolidColor(127, 127, 127, 255);
	EasyStringDrawer::draw(_fontRef, &wk, (f32)X_PC_NAME, (f32)Y_PC_NAME, 16, rc);

	// 顔アイコン
	RectF32 rect(RECT_PC_ICON);
	const RectF32 uvPcIcon[3] =
	{
		RectF32( UV_PC1_ICON ),
		RectF32( UV_PC2_ICON ),
		RectF32( UV_PC3_ICON ),
	};
	rdr->setSolidColor(255, 255, 255, 255);
	rdr->bindTexture(_texRef);
	RendererUtils::draw2dTextureRect(rdr, &rect, &uvPcIcon[_ipc]);

	// 子パネル描画
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlBtn[i] != 0)	{	_pnlBtn[i]->draw(rc);	}
	}
}

/*---------------------------------------------------------------------*//**
	キャラの有効・無効設定
**//*---------------------------------------------------------------------*/
void PcStatusWindowBase::setEnablePc(s32 ipc, bool isEnable)
{
	_pcenb[ipc] = isEnable;

	// 有効数が１以下の場合は左右キャラクタ変更ボタンを無効化する
	s32 cntEnabledPc = 0;
	for(int i = 0; i < NUM_PCKIND; i++)
	{
		if(_pcenb[i])	{	cntEnabledPc++;	}
	}
	if(cntEnabledPc <= 1)
	{
		_pnlBtn[BTN_LEFT]->setEnable(false);
		_pnlBtn[BTN_RIGHT]->setEnable(false);
	}
}

/*---------------------------------------------------------------------*//**
	次のキャラへ
**//*---------------------------------------------------------------------*/
void PcStatusWindowBase::nextPc()
{
	s32 ipcNew = _ipc;
	do
	{
		ipcNew++;
		if(ipcNew >= NUM_PCKIND)	{	ipcNew = 0;	}
		if(_pcenb[ipcNew])
		{
			_ipc = ipcNew;
			onChangePc();
			break;
		}
	}
	while(ipcNew != _ipc);
}

/*---------------------------------------------------------------------*//**
	前のキャラへ
**//*---------------------------------------------------------------------*/
void PcStatusWindowBase::prevPc()
{
	s32 ipcNew = _ipc;
	do
	{
		ipcNew--;
		if(ipcNew < 0)	{	ipcNew = NUM_PCKIND - 1;	}
		if(_pcenb[ipcNew])
		{
			_ipc = ipcNew;
			onChangePc();
			break;
		}
	}
	while(ipcNew != _ipc);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
