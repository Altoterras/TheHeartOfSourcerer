/***********************************************************************//**
 *	WalkingMode.h
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
#include "WalkingMode.h"

// Friends
#include "WalkingRole.h"
#include "../MagicTypingMode.h"
#include "../IngameMode.h"
#include "../ItemUsingMode.h"
#include "../../../camera/WalkingCam.h"
#include "../../../body/Game.h"
#include "../../../body/Party.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/MoveCursor.h"
#include "../../../save/SaveSys.h"
#include "../../../unit/char/pc/PcUnit.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// オートセーブまでのフレーム数
#define FRAME_MODE_BEGIN		(15)
#define FRAME_CHANGED_MAP		(3)

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// WalkingMode メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
WalkingMode::WalkingMode()
	: _frameSafeSave(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool WalkingMode::create()
{
	if(!GameModeBase::create(GameMode::MODE_GAME_INGAME_WALKING__START_, GameMode::MODE_GAME_INGAME_WALKING__END_, new WalkingRole()))
	{
		return false;
	}

	// モード登録
	registerMode((s16)GameMode::MODE_GAME_INGAME_WALKING_NORMAL, new GameModeBase());
	registerMode((s16)GameMode::MODE_GAME_INGAME_WALKING_MAGICTYPING, new MagicTypingMode());
	registerMode((s16)GameMode::MODE_GAME_INGAME_WALKING_ITEMUSING, new ItemUsingMode());

	return true;
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool WalkingMode::notifyBegin(void* objParamRef)
{
	// ロールコマンド設定
	getSelfRole()->reset();
	getSelfRole()->stackCommand(WalkingRole::STABLE, 0L);
	getSelfRole()->begin();

///	// 初期モード　→ GameModeBase::reserveDefaultChildModeDive で自動的に推移するように変更【2015/02/10 r-kishi】
///	changeChildMode((s16)GameMode::MODE_GAME_INGAME_WALKING_NORMAL, false);

	// セーフセーブカウンタを設定
	_frameSafeSave = FRAME_MODE_BEGIN;

	/*
	// 移動カーソルをリセットする
	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
	mvcsr->reset();
	*/

	// リーダー不在の場合はいるメンバーをリーダーに
	setSafeLeader();

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void WalkingMode::notifyEnd(void* objParamRef)
{
}

/*---------------------------------------------------------------------*//**
	初期子モードを得る
**//*---------------------------------------------------------------------*/
short WalkingMode::getDefaultChildMode()
{
	return (s16)GameMode::MODE_GAME_INGAME_WALKING_NORMAL;
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void WalkingMode::exec(ExecRes* res, const ExecCtx* ec)
{
	GameModeBase::exec(res, ec);

	IngameMode* modeParent = (IngameMode*)getParentMode();
	if(modeParent->getWalkingCam()->isInputOutRange())	// ユーザーがモードから抜けようとした
	{
		// ワールドマップモードへ
		TRACE("{WalkingMode::exec} change to WORLDMAP mode.\n");
		Game::getGame()->getGameMode()->changeEndMode(GameMode::MODE_GAME_INGAME_WORLDMAP);
	}
	else
	{
		// オートセーブ処理
		if(_frameSafeSave > 0.0f)
		{
			_frameSafeSave -= ec->getDeltaFrame();
			if(_frameSafeSave < 0.0f)	{	_frameSafeSave = 0.0f;	}

			if(_frameSafeSave == 0.0f)
			{
				// 中断用データをセーブする
				SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
				if(svsys != 0L)	{	svsys->saveResumeAuto();	}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void WalkingMode::render(const RenderCtx* rc)
{
	GameModeBase::render(rc);
}

/*---------------------------------------------------------------------*//**
	マップ変更後通知
**//*---------------------------------------------------------------------*/
void WalkingMode::notifyChangedMap(bool isDeserializeCase)
{
	GameModeBase::notifyChangedMap(isDeserializeCase);

	if(!isDeserializeCase)
	{
		// セーフセーブカウンタを設定
		_frameSafeSave = FRAME_CHANGED_MAP;
	}

	// リーダー不在の場合はいるメンバーをリーダーに
	setSafeLeader();
}

/*---------------------------------------------------------------------*//**
	リーダー不在の場合はいるメンバーをリーダーにする
**//*---------------------------------------------------------------------*/
void WalkingMode::setSafeLeader()
{
	Party* party = Game::getGame()->getMyParty();
	if(!party->getLeaderPcUnit()->isEnable())
	{
		for(int i = 0; i < party->getPcNum(); i++)
		{
			if(party->setLeader(i))	{	break;	}
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
