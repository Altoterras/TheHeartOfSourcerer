/***********************************************************************//**
 *	MapMod10181.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MapMod10181.h"

// Friends
#include "../../../episode/party/MyParty.h"
#include "../../../body/Game.h"
#include "../../../camera/WalkingCam.h"
#include "../../../mode/GameMode.h"
#include "../../../mode/ingame/IngameMode.h"
#include "../../../unit/char/pc/PcUnit.h"

// External
#include "../../../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapMod10181 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MapMod10181::MapMod10181()
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool MapMod10181::begin()
{
	return MapMod10000::begin();
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
bool MapMod10181::end()
{
	return MapMod10000::end();
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void MapMod10181::exec(ExecRes* res, const ExecCtx* ec)
{
	Mode* gmode = Game::getGame()->getGameMode()->getChildEndMode();
	if((gmode != 0L) && (gmode->getSelfModeId() == GameMode::MODE_GAME_INGAME_WALKING_NORMAL))
	{
		gmode = gmode->getParentMode();
		if((gmode != 0L) && (gmode->getSelfModeId() == GameMode::MODE_GAME_INGAME_WALKING))
		{
			gmode = gmode->getParentMode();
			if((gmode != 0L) && (gmode->getSelfModeId() == GameMode::MODE_GAME_INGAME))
			{
				IngameMode* igmode = (IngameMode*)gmode;
				WalkingCam* cam = igmode->getWalkingCam(); ASSERT(cam != 0L);
				Unit* unitMy = Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY); ASSERT(unitMy != 0L);

				// 制御を取得する
				cam->setOutControl(true);

				const f32 xGate = 5200.0f;								// ゲートの位置
				const f32 xWw = 10000.0f;								// 水の区の位置
				Vector3F posAim(unitMy->getPosition());					// 注視点．まずはキャラの位置を得る
				Vector3F posCam(&posAim);								// カメラ位置
				f32 rateToGate = (posAim.x() - xGate) / (xWw - xGate);	// ゲートまでの近さ率（0 近い ～ 1 遠い）
				posCam.x() += 5000.0f + (10000.0f * rateToGate);		// ゲートに行く程キャラに近づく
				posCam.y() += 50.0f + (200.0f * rateToGate);			// ゲートに行く程低い
				posAim.x() -= 100000.0f;
				posAim.y() += 8000.0f;
				cam->setLoc(&posCam);
				cam->setAim(&posAim);
			}
		}
	}

	MapMod10000::exec(res, ec);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
