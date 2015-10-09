/***********************************************************************//**
 *	MoveMap.cpp
 *	Enlight MoveMap Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/03.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// マクロ

#define ES_TEST			(1)
#define DEBUG_FULLMODE	(0)

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MoveMap.h"

// Friends
#include "Game.h"
#include "GameNotifier.h"
#include "../camera/GameCustomCamClass.h"
#include "../camera/WalkingCam.h"
#include "../confront/Confront.h"
#include "../episode/save/SaveStructure.h"
#include "../event/EventIdDef.h"
#include "../event/EventSys.h"
#include "../magic/MagicSys.h"
#include "../map/Map.h"
#include "../save/SaveData.h"
#include "../save/SaveSys.h"
#include "../unit/UnitManager.h"

// External
#include "../../etk/effect/ScreenEffect.h"
#include "../../etk/particle/ParticleSys.h"
#include "../../etk/sg/SorSgm.h"
#include "../../etk/sound/SoundMngr.h"
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MoveMap メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	マップ移動
**//*---------------------------------------------------------------------*/
bool MoveMap::changeMap(s32 mapid, u32 evparamMapStart)
{
	return changeMap(mapid, evparamMapStart, false);
}

/*---------------------------------------------------------------------*//**
	マップ移動予約
**//*---------------------------------------------------------------------*/
void MoveMap::reserveChangeMap(s32 mapid)
{
	_mapidReserved = mapid;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MoveMap::MoveMap()
	: _mapidCur(0)
	, _mapidPrev(0)
	, _mapidReserved(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MoveMap::~MoveMap()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MoveMap::create()
{
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MoveMap::destroy()
{
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void MoveMap::exec()
{
	// マップ移動予約を処理
	if(_mapidReserved != 0)
	{
		// 戦闘時は戦闘を強制終了
		if(Game::getGame()->isConfront())
		{
			Game::getGame()->getConfront()->end(0L);
		}

		// マップ移動
		changeMap(_mapidReserved, 0);
		_mapidReserved = 0;
	}
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool MoveMap::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		svst->_gamesvst._mapidCur = _mapidCur;
		svst->_gamesvst._mapidPrev = _mapidPrev;
	}
	else						// ダイレクトモード
	{
		ser->store(&_mapidCur);
		ser->store(&_mapidPrev);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool MoveMap::deserialize(Serializer* ser)
{
	// リスタート時はマップを読まない
	if(TFW_IS_FLAG(ser->getGeneralFlags(), SaveData::SRF_RESTARTING))
	{
		return true;
	}

	s32 mapid = 0;

	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		mapid = svst->_gamesvst._mapidCur;
		_mapidPrev = svst->_gamesvst._mapidPrev;
	}
	else						// ダイレクトモード
	{
		ser->restore(&mapid);
		ser->restore(&_mapidPrev);
	}

	// マップ反映
	if(mapid != 0)
	{
		changeMap(mapid, 0, true);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	マップ移動
**//*---------------------------------------------------------------------*/
bool MoveMap::changeMap(s32 mapid, u32 evparamMapStart, bool isDeserializeCase)
{
	GameNotifier* gnotif = Game::getGame()->getGameNotifier(); ASSERT(gnotif != 0L);
	EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
	Map* map = Game::getGame()->getMap(); ASSERT(map != 0L);
	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
	SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
	SoundMngr* sndmng = Game::getGame()->getSoundManager(); ASSERT(sndmng != 0L);
	ScreenEffect* screff = Game::getGame()->getSceneScreenEffect(); ASSERT(screff != 0L);
	MagicSys* mgcsys = Game::getGame()->getMagicSys(); ASSERT(mgcsys != 0L);
	ParticleSys* ptclsys = Game::getGame()->getParticleSys(); ASSERT(ptclsys != 0L);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// マップ終了処理

 	// マップ変更前通知
	if(!gnotif->notifyChangeMapBefore(&mapid, _mapidCur))
	{
		// 返答が false の場合はマップ移動を中止する
		return true;	// 失敗ではない
	}

	// マップイベントの強制終了
	evsys->terminateEventRange(EventIdDef::EVID_MAP__START_, EventIdDef::EVID_MAP__END_);

	// マップ解放
	map->releaseMap();

	// SE ロード
	sndmng->registerSeIngame();	// 何度呼んでも良い設計になっている

	// SE 全停止
	sndmng->stopAllSe();

	// マジッククラスタ全消滅
	mgcsys->disappearAllCluster();

	// パーティクル全消滅
	ptclsys->disappearAllParticle();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// マップ開始処理

	// マップロード
	if(!map->loadMap(mapid))
	{
		ASSERT(false);
		return false;
	}

	ASSERT(unitmng->getMyCharUnit() != 0L);

	// マップイベントロード
	if(!evsys->loadMap(mapid))
	{
		ASSERT(false);
		return false;
	}
	// 配置オブジェクトの配置
	if(!isDeserializeCase)	// デシリアライズ時は、Map::deserialize で保存された配置を再現する
	{
		map->mapConfig()->placePlacementObjToMap(map, false);
	}

	// 現在のマップ番号の設定
	_mapidPrev = _mapidCur;
	_mapidCur = mapid;

	// マップ変更後通知
	gnotif->notifyChangedMap(isDeserializeCase);

	// マップ開始イベント実行
	map->beginMapStartEvent(evparamMapStart);

	// BGM 再生
	map->playMapBgm();

	// カメラ設定
	Camera* camVisible = Game::getGame()->getSceneGraph()->getVisibleCamera();
	if((camVisible != 0L) && (camVisible->getCameraClass() == GameCustomCamClass::CAMCLS_WALKING))
	{
		((WalkingCam*)camVisible)->resetVisibleCounter();
	}

	// フェードイン
	ColorU8 colBlack(0, 0, 0, 255);
	screff->setColor1(&colBlack);
	screff->setMaxFrame(1 * FRAMERATE);
	screff->start(ScreenEffect::MODE_FADE_IN);

	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
