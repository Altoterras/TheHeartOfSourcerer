/***********************************************************************//**
 *	IngameMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "IngameMode.h"

// Friends
#include "IngameRole.h"
#include "confront/ConfrontMode.h"
#include "menu/MenuMode.h"
#include "walking/WalkingMode.h"
#include "event/EventMode.h"
#include "worldmap/WorldMapMode.h"
#include "../../camera/WalkingCam.h"
#include "../../event/EventSys.h"
#include "../../body/Game.h"
#include "../../body/GameNotifier.h"

// External
#include "../../../etk/sg/CamSgnc.h"
#include "../../../etk/sg/SorSgm.h"
#include "../../../tfw/g3d/sg/node/SgNodeChain.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// IngameMode メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
IngameMode::IngameMode()
	: _wlkcam(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
IngameMode::~IngameMode()
{
	ASSERT(_wlkcam == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool IngameMode::create()
{
	if(!GameModeBase::create(GameMode::MODE_GAME_INGAME__START_, GameMode::MODE_GAME_INGAME__END_, new IngameRole()))
	{
		return false;
	}

	// モード登録
	registerMode((s16)GameMode::MODE_GAME_INGAME_WALKING, new WalkingMode());
	registerMode((s16)GameMode::MODE_GAME_INGAME_CONFRONT, new ConfrontMode());
	registerMode((s16)GameMode::MODE_GAME_INGAME_EVENT, new EventMode());
	registerMode((s16)GameMode::MODE_GAME_INGAME_MENU, new MenuMode());
	registerMode((s16)GameMode::MODE_GAME_INGAME_WORLDMAP, new WorldMapMode());

	// 専用カメラ作成
	WalkingCam* wlkcam = new WalkingCam();
	if(!wlkcam->create())
	{
		delete wlkcam;
		return false;
	}
	_wlkcam = wlkcam;
	Game::getGame()->setDefaultCamera(_wlkcam);	// ゲームにデフォルトカメラを設定する

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void IngameMode::destroy()
{
	// 専用カメラを削除する
	if(_wlkcam != 0L)
	{
		Game::getGame()->setDefaultCamera(0L);	// ゲームのデフォルトカメラを解除する
		_wlkcam->destroy();
		delete _wlkcam;
		_wlkcam = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool IngameMode::notifyBegin(void* objParamRef)
{
	// シーングラフ及びリソースプールに専用カメラを追加
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	if(sgm->addNode(_wlkcam, false))	// リソースプールに追加しないで自力管理する
	{
		CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
		nodecCam->switchVisible(_wlkcam->getId());	// カメラを有効化
	}

	// ストーリーへインゲームモード開始通知
	GameNotifier* gnotif = Game::getGame()->getGameNotifier(); ASSERT(gnotif != 0L);
	gnotif->notifyBeginIngameMode();

///	// 初期モード　→ GameModeBase::reserveDefaultChildModeDive で自動的に推移するように変更【2015/02/10 r-kishi】
///	changeChildMode((s16)GameMode::MODE_GAME_INGAME_WALKING, false);
///	Game::getGame()->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_WALKING);

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void IngameMode::notifyEnd(void* objParamRef)
{
	// シーングラフの専用カメラを削除
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	sgm->removeNode(_wlkcam);
}

/*---------------------------------------------------------------------*//**
	初期子モードを得る
**//*---------------------------------------------------------------------*/
short IngameMode::getDefaultChildMode()
{
	return (s16)GameMode::MODE_GAME_INGAME_WALKING;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
