/***********************************************************************//**
 *	MapMod10000.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/15.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MapMod10000.h"

// Friends
#include "../../MapIdDef.h"
#include "../../../episode/char/pc/PartnerUnit.h"
#include "../../../episode/party/MyParty.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/MoveMap.h"
#include "../../../map/Map.h"
#include "../../../map/MapConfig.h"
#include "../../../sound/GameSoundDef.h"

// External
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/g3d/sg/node/implement/Camera.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapMod10000 メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MapMod10000::MapMod10000()
	: _sechWaterSrrnd(0)
{
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool MapMod10000::begin()
{
	if(!MapMod::begin())	{	return false;	}

	// コルポの武器・盾をコメサット村の内外で有効／無効化する
	Party* party = Game::getGame()->getMyParty(); ASSERT(party != 0L);
	PartnerUnit* unitPc2 = (PartnerUnit*)party->pcUnit(MyParty::PC_PTN);
	if((unitPc2 != 0L) && unitPc2->isEnable())
	{
		unitPc2->updateEquipModelObjVisibility();
	}

	// 水の区と寺院橋マップは水のせせらぎを鳴らす
	// ※将来的には派生クラスでの実装も検討すること
	if(_sechWaterSrrnd == 0)
	{
		s32 mapid = Game::getGame()->getMoveMap()->getCurMapId();
		if(	(mapid == MapIdDef::MAPID_EP01_BASE_VILLAGE_WATER_WARD) ||
			(mapid == MapIdDef::MAPID_EP01_BASE_VILLAGE_TEMPLE_BRIDGE)	)
		{
			_sechWaterSrrnd = playSurroundingsSe(GameSoundDef::SE_WATER_MURMUR, 0.253f, true, false);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
bool MapMod10000::end()
{
	// 水のせせらぎを止める
	if(_sechWaterSrrnd != 0)
	{
		SoundMngr* sndmng = Game::getGame()->getSoundManager(); ASSERT(sndmng != 0L);
		if(sndmng != 0L)
		{
			sndmng->stopSe(_sechWaterSrrnd);
		}
		_sechWaterSrrnd = 0;
	}

	return MapMod::end();
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void MapMod10000::exec(ExecRes* res, const ExecCtx* ec)
{
	MapMod::exec(res, ec);

	// 鳥のさえずり
	do
	{
		// フレームと乱数で頻度を間引く
		GameExecCtx* gec = (GameExecCtx*)ec;
		if((gec->getBodyExecCount() & 0x1f) != 0)											{	break;	}
		if((::rand() & 7) != 0)																{	break;	}

		// イベント中は除外
		if(gec->isLockOperating())															{	break;	}
		// 対決中は除外
		if(Game::getGame()->isConfront())													{	break;	}

		// 室内は除外
		if(Game::getGame()->getMap()->getMapConfig()->_mapkind == MapConfig::MAPKIND_INDOOR)	{	break;	}

		// SE を決める
		u16 sedid = 0;
		switch(::rand() & 7)	// 0 ～ 7
		{
		case 1:	case 2:	sedid = GameSoundDef::SE_BIRDSONG_1;	break;
		case 3: case 4:	sedid = GameSoundDef::SE_BIRDSONG_2;	break;
		case 5:			sedid = GameSoundDef::SE_BIRDSONG_3;	break;
		}
		if(sedid == 0L)																		{	break;	}

		playSurroundingsSe(sedid, 0.2f, false, true);
	}
	while(false);
}

/*---------------------------------------------------------------------*//**
	環境 SE を鳴らす
**//*---------------------------------------------------------------------*/
s32 MapMod10000::playSurroundingsSe(u16 seid, f32 volume, bool isLoop, bool isPosRandom)
{
	// 鳴らす位置を決める
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	if(sgm == 0L)													{	return 0;	}
	Camera* cam = sgm->getVisibleCamera();
	if(cam == 0L)													{	return 0;	}
	if(cam->getLoc() == 0L)											{	return 0;	}
	Vector3F pos, vel;
	pos.copy(cam->getLoc());
	if(isPosRandom)
	{
		pos.x() += (::rand() & 0x7ff) - 1023;	// -1023m ～ 1024m
		pos.z() += (::rand() & 0x7ff) - 1023;	// -1023m ～ 1024m
	}

	// SE を鳴らす
	SoundMngr* sndmng = Game::getGame()->getSoundManager(); ASSERT(sndmng != 0L);
	if(sndmng == 0L)												{	return 0;	}
	s32 sech = sndmng->playSe(seid, isLoop, volume, &pos, &vel);

	return sech;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
