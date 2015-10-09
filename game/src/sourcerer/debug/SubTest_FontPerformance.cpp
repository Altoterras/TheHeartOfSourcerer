/***********************************************************************//**
 *	SubTest_FontPerformance.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SubTest_FontPerformance.h"

// Friends
#include "../body/Game.h"
#include "../gui/GameFontSet.h"

// External
#include "../../tfw/g2d/font/Font.h"
#include "../../tfw/g2d/StringDrawer.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/string/VcString.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラスメソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SubTest_FontPerformance::SubTest_FontPerformance()
	: _strdraw(0L)
	, _fontRef(0L)
	, _str(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SubTest_FontPerformance::~SubTest_FontPerformance()
{
	ASSERT(_strdraw == 0L);
	ASSERT(_str == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SubTest_FontPerformance::create()
{
	_strdraw = new StringDrawer();
	if(!_strdraw->create())
	{
		return false;
	}

	_fontRef = Game::getGame()->getFontSet()->getFont(GameFontSet::JP);

	_str = new VcString();
	for(int j = 0; j < 10; j++)
	{
		for(int k = '!'; k <= '~'; k++)
		{
			_str->add((char)k);
		}
	}

	RectF32 rect(0, 0, Game::getGame()->getLogicalWidth(), Game::getGame()->getLogicalHeight());
	_strdraw->setString(_str);
	_strdraw->setRectangle(&rect);
	_strdraw->setFont(_fontRef);
	_strdraw->setFontSize(16);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SubTest_FontPerformance::destroy()
{
	delete _str;
	_str = 0L;

	if(_strdraw != 0L)
	{
		_strdraw->destroy();
		delete _strdraw;
		_strdraw  = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void SubTest_FontPerformance::exec(ExecRes* res, const ExecCtx* ec)
{
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void SubTest_FontPerformance::render(const RenderCtx* rc)
{
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void SubTest_FontPerformance::draw(const RenderCtx* rc)
{
	rc->getRenderer()->setSolidColor(255, 255, 255, 255);

	_strdraw->draw(rc);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
