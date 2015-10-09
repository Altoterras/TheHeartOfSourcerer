/***********************************************************************//**
 *	PauseScreen.cpp
 *	Envelopment Terras Kit
 *
 *  Created by Ryoutarou Kishi on 2012/08/30.
 *  Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PauseScreen.h"

// Friends
#include "EtkExecCtx.h"

// External
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/Texture.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PauseScreen メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	開始処理
**//*---------------------------------------------------------------------*/
void PauseScreen::start()
{
	_step = STEP_FADEIN;
	_fcntStep = 0.0f;

	// 最初のポーズでテクスチャを読み込む
	if(_tex == 0L)
	{
		loadTexture();
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PauseScreen::PauseScreen()
	: _step(STEP_NULL)
	, _fcntStep(0.0f)
	, _tex(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PauseScreen::~PauseScreen()
{
	ASSERT(_tex == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PauseScreen::create()
{
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PauseScreen::destroy()
{
	deleteTexture();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void PauseScreen::exec(ExecRes* res, const ExecCtx* ec)
{
	_fcntStep += ec->getDeltaFrame();

	switch(_step)
	{
	case STEP_NULL:
		return;
	case STEP_FADEIN:
		if(_fcntStep >= 15.0f)
		{
			_step = STEP_STABLE;
			_fcntStep = 0.0f;
			return;
		}
		break;
	case STEP_STABLE:
		{
			EtkExecCtx* eec = (EtkExecCtx*)ec;
			TouchPanel* touchp = eec->getTouchPanel();
			if(touchp->isTapRelease(0L, 0L))
			{
				_step = STEP_FADEOUT;
				_fcntStep = 0.0f;
			}
		}
		break;
	case STEP_FADEOUT:
		if(_fcntStep >= 15.0f)
		{
			_step = STEP_NULL;	// 終了
			_fcntStep = 0.0f;
			return;
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void PauseScreen::deleteTexture()
{
	if(_tex != 0L)
	{
		_tex->destroy();
		delete _tex;
		_tex = 0L;
	}
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS