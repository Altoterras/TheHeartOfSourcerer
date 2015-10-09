/***********************************************************************//**
 *	WorldMapMode.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/16.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "WorldMapMode.h"

// Friends
#include "../walking/TemplateRole.h"
#include "../../../camera/WorldMapCam.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/GameRenderCtx.h"
#include "../../../gui/GameFontSet.h"
#include "../../../gui/GameGui.h"
#include "../../../mode/GameMode.h"
#include "../../../episode/StoryManager.h"
#include "../../../episode/worldmap/WorldMap.h"

// External
#include "../../../../etk/sg/CamSgnc.h"
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../tfw/Tfw.h"
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/gcmn/Texture.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gcmn/Gcalc.h"
#include "../../../../tfw/lib/Point.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// WorldMapMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
WorldMapMode::WorldMapMode()
	: _wmapRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
WorldMapMode::~WorldMapMode()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool WorldMapMode::create()
{
	if(!GameModeBase::create(GameMode::MODE_TEMPLATE__START_, GameMode::MODE_TEMPLATE__END_, new TemplateRole()))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void WorldMapMode::destroy()
{
	// 世界地図を破棄する
	_wmapRef = 0L;

	GameModeBase::destroy();
}

/*---------------------------------------------------------------------*//**
	開始通知
**//*---------------------------------------------------------------------*/
bool WorldMapMode::notifyBegin(void* objParamRef)
{
	TRACE("{WorldMapMode::notifyBegin} called.\n");

	// GUI を一時的に隠す
	GameGui* gui = Game::getGame()->getGui();
	gui->setExecFlags(GameGui::EF_HIDE_TEMPORARY, true);

	// 世界地図を取得する
	_wmapRef = Game::getGame()->getStoryManager()->worldMap();

	// 世界地図に通知する
	_wmapRef->notifyBegin();

	return true;
}

/*---------------------------------------------------------------------*//**
	終了通知
**//*---------------------------------------------------------------------*/
void WorldMapMode::notifyEnd(void* objParamRef)
{
	TRACE("{WorldMapMode::notifyEnd} called.\n");

	// 世界地図に通知する
	_wmapRef->notifyEnd();

	// GUI の一時的隠しを戻す
	GameGui* gui = Game::getGame()->getGui();
	gui->setExecFlags(GameGui::EF_HIDE_TEMPORARY, false);
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void WorldMapMode::exec(ExecRes* res, const ExecCtx* ec)
{
	// 世界地図を制御する
	ExecRes resChild;
	_wmapRef->exec(&resChild, ec);
	if(resChild.isDone())
	{
		Game::getGame()->getGameMode()->changePrevEndMode(GameMode::MODE_GAME_INGAME_WORLDMAP);
	}

	GameModeBase::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void WorldMapMode::draw(const RenderCtx* rc)
{
	GameModeBase::draw(rc);

	// 世界地図を描画する
	_wmapRef->draw(rc);
}

/*---------------------------------------------------------------------*//**
	3D 描画
**//*---------------------------------------------------------------------*/
void WorldMapMode::render(const RenderCtx* rc)
{
	GameModeBase::render(rc);

	// 世界地図を描画する
	_wmapRef->render(rc);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
