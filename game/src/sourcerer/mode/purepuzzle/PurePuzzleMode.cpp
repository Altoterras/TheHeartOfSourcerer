/***********************************************************************//**
 *	PurePuzzleMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/02/07.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PurePuzzleMode.h"

// Friends
#include "PrpzlPlayMode.h"
#include "menu/PrpzlMenuMode.h"
#include "../../purepuzzle/PrpzlOperationData.h"
#include "../../purepuzzle/PrpzlPerf_Enlight.h"
#include "../../purepuzzle/PurePuzzle.h"
#include "../../body/Game.h"
#include "../../body/GameRenderCtx.h"

// External
#include "../../../etk/sg/CamSgnc.h"
#include "../../../etk/sg/SorSgm.h"
#include "../../../tfw/Tfw.h"
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/lib/Color.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PurePuzzleMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PurePuzzleMode::PurePuzzleMode()
	: _opdat(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PurePuzzleMode::~PurePuzzleMode()
{
	ASSERT(_opdat == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool PurePuzzleMode::create()
{
	if(!GameModeBase::create(GameMode::MODE_GAME_PRPZL__START_, GameMode::MODE_GAME_PRPZL__END_, 0L))
	{
		return false;
	}

	// パズル動作データの作成
	_opdat = new PrpzlOperationData();

	// モード登録
	registerMode((s16)GameMode::MODE_GAME_PRPZL_MENU, new PrpzlMenuMode(_opdat));
	registerMode((s16)GameMode::MODE_GAME_PRPZL_PLAY, new PrpzlPlayMode(_opdat));

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void PurePuzzleMode::destroy()
{
	// パズル動作データを削除する
	if(_opdat != 0L)
	{
		delete _opdat;
		_opdat = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool PurePuzzleMode::notifyBegin(void* objParamRef)
{
///	// 初期モード　→ GameModeBase::reserveDefaultChildModeDive で自動的に推移するように変更【2015/02/10 r-kishi】
///	changeChildMode((s16)GameMode::MODE_GAME_PRPZL_MENU, false);

	return true;
}

/*---------------------------------------------------------------------*//**
	初期子モードを得る
**//*---------------------------------------------------------------------*/
short PurePuzzleMode::getDefaultChildMode()
{
	return (s16)GameMode::MODE_GAME_PRPZL_MENU;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
