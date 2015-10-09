/***********************************************************************//**
 *	SimpleSprite.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/09/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SimpleSprite.h"

// Friends
#include "../body/GameExecCtx.h"
#include "../event/matter/implement/SpriteMatter.h"
#include "../file/MlFileId.h"

// External
#include "../../tfw/lib/Rect.h"
#include "../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SimpleSprite メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	有効なスプライトかどうかを得る
**//*---------------------------------------------------------------------*/
bool SimpleSprite::isValid() const
{
	return _sprite != 0L;
}

/*---------------------------------------------------------------------*//**
	終了したかどうかを得る
**//*---------------------------------------------------------------------*/
bool SimpleSprite::isEnd() const
{
	if(_sprite == 0L)	{	return true;	}
	return _sprite->isEnd();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SimpleSprite::SimpleSprite()
	: _sprite(0L)
	, _frameCur(0.0f)
	, _isProhibitOtherUiReact(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SimpleSprite::~SimpleSprite()
{
	ASSERT(_sprite == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SimpleSprite::create(u32 fileid, f32 frameMin, bool fitScreen, u32 colorSide, bool isProhibitOtherUiReact)
{
	// 設定
	SpriteMatter::Setting setting;
	setting._mlfileidTex->create(fileid);
	setting._layout = fitScreen ? SpriteMatter::LAYOUT_FIT_SCREEN_KEEP_RATIO : SpriteMatter::LAYOUT_SPEC_RECT;
	setting._colorSide = fitScreen ? colorSide : 0;
	setting._isWait = true;
	setting._frameFadeOut = 15.0f;
	setting._frameFadeIn = 15.0f;
	setting._frameWaitMin = frameMin;
	setting._rect->set(0.0f, 0.0f, (f32)Game::getGame()->getStandardWidth(), (f32)Game::getGame()->getStandardHeight());
	setting._uv->set(0.0f, 0.0f, 0.9375f, 0.625f);

	// スプライトマター作成
	if(_sprite == 0L)	{	_sprite = new SpriteMatter();	}
	if(_sprite == 0L)	{	return false;	}
	_sprite->create(0L, 0);
	_sprite->setting(&setting);
	_sprite->begin(0);

	// その他の初期化
	_frameCur = 0.0f;
	_isProhibitOtherUiReact = isProhibitOtherUiReact;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SimpleSprite::destroy()
{
	// スプライトマター削除
	if(_sprite != 0L)
	{
		_sprite->destroy();
		delete _sprite;
		_sprite = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SimpleSprite::exec(const ExecCtx* ec)
{
	if(_sprite != 0L)
	{
		GameExecCtx* gec = (GameExecCtx*)ec;
		TouchPanel* ui = gec->getTouchPanel(); 

		// フレームカウンタ更新
		_frameCur += ec->getDeltaFrame();

		// スプライトマター制御
		_sprite->execImplement(0L, ec, _frameCur);

		// 他が反応しないように全ての操作を奪う
		if(_isProhibitOtherUiReact)	// 他での操作を禁止する
		{
			ui->setReacted(true);
		}
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void SimpleSprite::draw(const RenderCtx* rc)
{
	if(_sprite != 0L)
	{
		_sprite->draw(rc);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
