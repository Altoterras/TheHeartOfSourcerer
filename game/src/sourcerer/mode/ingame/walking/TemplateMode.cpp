/***********************************************************************//**
 *	TemplateMode.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TemplateMode.h"

// Friends
#include "TemplateRole.h"
#include "../../../body/Game.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TemplateMode メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TemplateMode::TemplateMode()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool TemplateMode::create()
{
	if(!GameModeBase::create(GameMode::MODE_TEMPLATE__START_, GameMode::MODE_TEMPLATE__END_, new TemplateRole()))
	{
		return false;
	}

	// モード登録
	registerMode((s16)GameMode::MODE_TEMPLATE_DUMMY_1, new GameModeBase());
	registerMode((s16)GameMode::MODE_TEMPLATE_DUMMY_2, new GameModeBase());

	return true;
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool TemplateMode::notifyBegin(void* objParamRef)
{
///	// 初期モード　→ GameModeBase::reserveDefaultChildModeDive で自動的に推移するように変更【2015/02/10 r-kishi】
///	changeChildMode((s16)GameMode::MODE_TEMPLATE_DUMMY_1, false);

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void TemplateMode::notifyEnd(void* objParamRef)
{
}

/*---------------------------------------------------------------------*//**
	初期子モードを得る
**//*---------------------------------------------------------------------*/
short TemplateMode::getDefaultChildMode()
{
	return (s16)GameMode::MODE_TEMPLATE_DUMMY_1;
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void TemplateMode::exec(ExecRes* res, const ExecCtx* ec)
{
	GameModeBase::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void TemplateMode::render(const RenderCtx* rc)
{
	GameModeBase::render(rc);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
