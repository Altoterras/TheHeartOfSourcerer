/***********************************************************************//**
 *	Map.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Map.h"

// Friends
#include "ModelSwitch.h"
#include "PlacementObj.h"
#include "../common/CalcUtils.h"
#include "../episode/char/CharFactory.h"
#include "../episode/save/SaveStructure.h"
#include "../event/EventIdDef.h"
#include "../event/EventSys.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/GameRenderCtx.h"
#include "../body/Party.h"
#include "../file/GameFileIdDef.h"
#include "../unit/UnitManager.h"
#include "../unit/char/npc/EnemyUnit.h"
#include "../unit/char/npc/ExtraUnit.h"
#include "../unit/char/npc/MapObjUnit.h"
#include "../unit/char/pc/PcUnit.h"
#include "../unit/item/FieldItemUnit.h"
#include "../unit/item/PutItemUnit.h"
#include "../save/SaveSys.h"
#include "../save/SaveData.h"
#include "../spirit/Spirit.h"
#include "../spirit/SpiritManager.h"
#include "../spirit/char/npc/EnemySpirit.h"
#include "../spirit/char/npc/ExtraSpirit.h"

// External
#include "../../etk/sg/SorSgm.h"
#include "../../etk/sg/TbfDefineEx.h"
#include "../../etk/sound/SoundMngr.h"
#include "../../tfw/Tfw.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/collision/Gcol.h"
#include "../../tfw/common/ResourcePool.h"
#include "../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/serialize/Serializer.h"
#include "../../tfw/tbf/TbfDefine.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// テクスチャ情報
#define W_TEX				1024.0f
#define H_TEX				1024.0f
#define HW_GATE_MARK		32.0f
#define H_GATE_MARK			64.0f
#define UV_U_GATE_MARK		416
#define UV_V_GATE_MARK		960
#define UV_W_GATE_MARK		64
#define UV_H_GATE_MARK		64

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Map 静的メンバ変数

//==========================================================================
// Map メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	マップ読み込み
**//*---------------------------------------------------------------------*/
bool Map::loadMap(s32 mapid)
{
	bool isSucceeded = false;

	// 一時配置オブジェクト情報をクリアする
	_listPlaceObjTmp->removeAll();
	_ctrlevidAutoCntTmp = EventIdDef::CTRLEVID_MAPTMP__START_;	// 自動割当カウンタリセット

	// モデルを読み込む
	ShapeModel** mdlarr = 0L;
	s32 numModel = 0;
	u32 fileid = FILEID_FROM_MAP_MODEL(mapid);
	if(ModelLoader::loadModelsFromTbf(Game::getGame()->getTfw(), fileid, TbfDefineEx::MEXF_BG, ModelLoader::LFLAG_DISPOSE_SCENE_GRAPH, (Model***)&mdlarr, &numModel))
	{
		ASSERT(numModel > 0);
		// 読み込み後処理
		isSucceeded = loadMapAfter(mdlarr, numModel);
	}
	else
	{
		ASSERT(false);
	}
	delete[] mdlarr;

	// 現在のマップ番号を保存
	_mapidCur = mapid;

	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	マップ解放
**//*---------------------------------------------------------------------*/
void Map::releaseMap()
{
	SorSgm* sgm = Game::getGame()->getSceneGraph(); ASSERT(sgm != 0L);
	// シーングラフからモデルを除去する
	for(int incc = 0; incc < _numNccMsArr; incc++)
	{
		for(int imdl = 0; imdl < ModelSwitch::NUM_MODEL; imdl++)
		{
			Model* mdl = _msarrNcc[incc]->getModel(imdl);
			if((mdl != 0L) && mdl->isOnSg())
			{
				sgm->removeNode(mdl);
			}
		}
	}
	if(_unitMapRef != 0L)
	{
		for(int imdl = 0; imdl < _unitMapRef->getModelNum(); imdl++)	// 宣言モデルを走査するループ
		{
			Model* mdl = _unitMapRef->getModel(imdl);
			if((mdl != 0L) && mdl->isOnSg())
			{
				sgm->removeNode(mdl);
			}
		}
	}
	sgm->debug_print();

	// 一時配置オブジェクト情報をクリアする
	_listPlaceObjTmp->removeAll();	// unplacePlacementObj はどちらにしても下でユニットが削除されるので省略

	// ユニットマネージャ, スピリットマネージャ, イベントシステムからマップ依存ユニットを破棄
	UnitManager* unitmng = Game::getGame()->getUnitManager();	ASSERT(unitmng != 0L);
	SpiritManager* sprtmng = Game::getGame()->getSpiritManager();	ASSERT(sprtmng != 0L);
	EventSys* evsys = Game::getGame()->getEventSys();	ASSERT(evsys != 0L);
	{
		int i = 0;
		while(i < unitmng->getUnitNum())
		{
			Unit* unit = unitmng->getUnitFromIndex(i);
			if(unit->isMapDepend())
			{
				const Spirit* sprt = unit->getSpirit();
				if(sprt != 0L)
				{
					sprtmng->removeSpirit(sprt);
				}

				evsys->removeUnitReference(unit);

				unitmng->removeUnit(unit);
				if(i < unitmng->getUnitNum())	{	continue;	}	// 削除されたので同じインデックスでもう一度
			}
			i++;
		}
	}
	_unitMapRef = 0L;
	TRACE("{Map::releaseMap} remain unit-num : %d\n", unitmng->getUnitNum());

	// ニアカメラクリップのモデルスイッチの解放
	if(_msarrNcc != 0L)
	{
		for(int i = 0; i < _numNccMsArr; i++)
		{
			_msarrNcc[i]->destroy();
			delete _msarrNcc[i];
		}
		delete[] _msarrNcc;
		_msarrNcc = 0L;
		_numNccMsArr = 0;
	}

	// コリジョンへの参照を NULL 化
	_gcolMapRef = 0L;

	// リソースプールから未使用のリソースを削除する
	ResourcePool* respl = Game::getGame()->getResourcePool();
	if(respl != 0L)
	{
		//respl->removeAll();
		respl->removeUnusedSgEntityResource();
		respl->traceInfo();
	}

	// コンフィグをリセット
	_conf.clear();
	_mapidCur = 0;
}

/*---------------------------------------------------------------------*//**
	マップの BGM を再生する
**//*---------------------------------------------------------------------*/
void Map::playMapBgm()
{
	SoundMngr* sndmng = Game::getGame()->getSoundManager(); ASSERT(sndmng != 0L);

	// BGM 再生
	if(_conf._bgmid == 0)
	{
		sndmng->stopBgm();
	}
	else if(_conf._bgmid >= 1)
	{
		sndmng->playBgm(_conf._bgmid, -1, _conf._volumeBgm, false, SoundMngr::FRAME_FADE_OUT_DEFAULT, SoundMngr::FRAME_FADE_IN_DEFAULT, 0);
	}
}

/*---------------------------------------------------------------------*//**
	地面当たりより、平面に設定されている移動マップ ID を得る
**//*---------------------------------------------------------------------*/
s32 Map::getGcolPlaneMoveMapId(s32 idxPlane) const
{
	if((idxPlane < 0) || (_gcolMapRef->getPlaneNum() <= idxPlane))	{	return -1;	}

	u16 flagAtrb = _gcolMapRef->getPlaneAtrbFlag(idxPlane);
	if(TFW_IS_FLAG(flagAtrb, Gcol::Plane::ATRBF_MOVE_MAP_01))
	{
		return _conf._mapidMove[0];
	}
	else if(TFW_IS_FLAG(flagAtrb, Gcol::Plane::ATRBF_MOVE_MAP_02))
	{
		return _conf._mapidMove[1];
	}
	else if(TFW_IS_FLAG(flagAtrb, Gcol::Plane::ATRBF_MOVE_MAP_03))
	{
		return _conf._mapidMove[2];
	}
	else if(TFW_IS_FLAG(flagAtrb, Gcol::Plane::ATRBF_MOVE_MAP_04))
	{
		return _conf._mapidMove[3];
	}

	return 0;
}

/*---------------------------------------------------------------------*//**
	移動マップ ID を変更する
**//*---------------------------------------------------------------------*/
void Map::changeMoveMapId(s32 mapidOld, s32 mapidNew)
{
	for(int i = 0; i < MapConfig::NUM_MOVE_MAPID; i++)
	{
		if(_conf._mapidMove[i] == mapidOld)
		{
			_conf._mapidMove[i] = mapidNew;

			// 移動マップ ID を無効化する場合はゲートマークも無効化する
			if((mapidNew == 0) && (i < NUM_GATE_MAX))
			{
				_garrShowGate[i]._posCenter->set(F32_PMAX, F32_PMAX, F32_PMAX);
			}

			return;
		}
	}
}

/*---------------------------------------------------------------------*//**
	PlacementObj を配置する
**//*---------------------------------------------------------------------*/
bool Map::placePlacementObj(PlacementObj* pobj, bool isForcePlacement)
{
	// 確率判断
	if(!isForcePlacement)	// 強制配置時は確率判定無し
	{
		s32 randAppear = (s32)(100.0f * CalcUtils::randF());
		if(randAppear >= pobj->getAppearProbability())
		{
			pobj->setNoPlaceByProbability(true);	// 確率によって配置されなかった
			return true;	// 失敗ではない
		}
	}

	bool isSuccess = false;

	// マップに配置する
	if(pobj->getKind() == PlacementObj::KIND_EXTRA)
	{
		ExtraSpirit* spirit = new ExtraSpirit();
		ExtraUnit* unit = new ExtraUnit();
		if(spirit->create())
		{
			if(unit->create(spirit, pobj->getCharDefId()))
			{
				placeUnitFromPlacementObj(unit, pobj);
				unit->setPlacementObj(pobj);

				Game::getGame()->getSpiritManager()->addSpirit(spirit);
				Game::getGame()->getUnitManager()->addUnit(unit);
			
				isSuccess = true;	// 成功
			}
		}
		// 失敗
		if(!isSuccess)
		{
			delete spirit;
			delete unit;
		}
	}
	else if(pobj->getKind() == PlacementObj::KIND_MAPOBJ)
	{
		MapObjUnit* unit = new MapObjUnit();
		if(unit->create(pobj->getCharDefId()))
		{
			placeUnitFromPlacementObj(unit, pobj);
			unit->setPlacementObj(pobj);

			Game::getGame()->getUnitManager()->addUnit(unit);

			isSuccess = true;	// 成功
		}
		// 失敗
		if(!isSuccess)
		{
			delete unit;
		}
	}
	else if(pobj->getKind() == PlacementObj::KIND_ENEMY)
	{
		EnemySpirit* spirit;
		EnemyUnit* unit;
		if(CharFactory::newEnemy(&unit, &spirit, pobj->getCharDefId()))
		{
			if(spirit->create())
			{
				if(unit->create(spirit, pobj->getCharDefId()))
				{
					// オブジェクト配置
					placeUnitFromPlacementObj(unit, pobj);
					unit->setPlacementObj(pobj);

					// マネージャーに登録
					Game::getGame()->getSpiritManager()->addSpirit(spirit);
					Game::getGame()->getUnitManager()->addUnit(unit);

					isSuccess = true;	// 成功
				}
			}
			// 失敗
			if(!isSuccess)
			{
				delete spirit;
				delete unit;
			}
		}
	}
	else if(pobj->getKind() == PlacementObj::KIND_DROPITEM)
	{
		PutItemUnit* unit = new PutItemUnit();
		if(unit->create(pobj->getItemDefId(), pobj->eleneStat(), true))
		{
			// オブジェクト配置
			placeUnitFromPlacementObj(unit, pobj);
			unit->setPlacementObj(pobj);

			// マネージャーに登録
			Game::getGame()->getUnitManager()->addUnit(unit);

			isSuccess = true;	// 成功
		}
		// 失敗
		if(!isSuccess)
		{
			delete unit;
		}
	}
	else if(pobj->getKind() == PlacementObj::KIND_PICKUPITEM)
	{
		FieldItemUnit* unit = new FieldItemUnit();
		if(unit->create(pobj->getItemDefId(), pobj->eleneStat(), true))
		{
			// オブジェクト配置
			placeUnitFromPlacementObj(unit, pobj);
			unit->setPlacementObj(pobj);

			// マネージャーに登録
			Game::getGame()->getUnitManager()->addUnit(unit);

			isSuccess = true;	// 成功
		}
		// 失敗
		if(!isSuccess)
		{
			delete unit;
		}
	}

	// 成功時
	if(isSuccess)
	{
		pobj->setPlaced(true);	// 配置された
	}

	return isSuccess;
}

/*---------------------------------------------------------------------*//**
	PlacementObj から配置を解除する
**//*---------------------------------------------------------------------*/
bool Map::unplacePlacementObj(PlacementObj* pobj)
{
	// 確率によって配置されなかった（NoPlaceByProbability）ものはそのまま成功で返る
	if(pobj->isNoPlaceByProbability())
	{
		ASSERT(!pobj->isPlaced());
		pobj->setNoPlaceByProbability(false);	// フラグクリア
		return true;
	}

	bool isSuccess = false;

	// ユニットを無効化する
	UnitManager* unitmng = Game::getGame()->getUnitManager();	ASSERT(unitmng != 0L);
	for(s32 i = 0; i < unitmng->getUnitNum(); i++)
	{
		Unit* unit = unitmng->getUnitFromIndex(i);
		if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_NPC))
		{
			NpcCharUnit* npc = (NpcCharUnit*)unit;
			if(npc->getPlacementObj() == pobj)
			{
				npc->setPlacementObj(0L);	// 配置情報をクリアする
				npc->setEnable(false);		// 無効化する
				isSuccess = true;
			}
		}
	}

	pobj->setPlaced(false);	// 配置が解除された

	return isSuccess;
}

/*---------------------------------------------------------------------*//**
	PlacementObj による配置がなされているかを調べる
**//*---------------------------------------------------------------------*/
bool Map::isPlacePlacementObj(const PlacementObj* pobj)
{
	UnitManager* unitmng = Game::getGame()->getUnitManager();	ASSERT(unitmng != 0L);
	for(s32 i = 0; i < unitmng->getUnitNum(); i++)
	{
		Unit* unit = unitmng->getUnitFromIndex(i);
		if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_NPC))
		{
			NpcCharUnit* npc = (NpcCharUnit*)unit;
			if(npc->getPlacementObj() == pobj)
			{
				return true;
			}
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	配置オブジェクト情報を一時リストに追加し、必要があれば実際に配置する
**//*---------------------------------------------------------------------*/
bool Map::addTemporaryPlacementObjSetting(PlacementObj* pobjEntr, bool isPlace)
{
	bool isSuccess = false;

	do
	{
		// 自動割当制御イベント ID
		if(pobjEntr->getCtrlEvid() == 0)
		{
			pobjEntr->setCtrlEvid(_ctrlevidAutoCntTmp);
			_ctrlevidAutoCntTmp++;
			ASSERT(_ctrlevidAutoCntTmp <= EventIdDef::CTRLEVID_MAPTMP__END_);
		}

		// リストの最大数確認
		ASSERT(_listPlaceObjTmp != 0L);
		if(_listPlaceObjTmp->getCount() >= NUM_TEMPPOBJ_MAX)
		{
			break;	// 失敗
		}

		// 実際に配置
		if(isPlace)
		{
			if(!placePlacementObj(pobjEntr, false))
			{
				break;	// 失敗
			}
		}

		// リストに追加
		_listPlaceObjTmp->addTail(pobjEntr);
		isSuccess = true;	// 成功
	}
	while(false);

	// 失敗時は管理委譲された配置オブジェクト情報を削除する
	if(!isSuccess)
	{
		delete pobjEntr;
	}

	return isSuccess;
}

/*---------------------------------------------------------------------*//**
	配置オブジェクト情報を一時リストから削除する
**//*---------------------------------------------------------------------*/
bool Map::removeTemporaryPlacementObjSetting(const PlacementObj* pobj)
{
	ListIterator<PlacementObj*> itCur = _listPlaceObjTmp->iterator();
	while(itCur.has())
	{
		ListIterator<PlacementObj*> itNext = itCur;
		itNext.next();

		PlacementObj* pobjWk = itCur.object();
		if(pobjWk == pobj)
		{
			_listPlaceObjTmp->removeNode(itCur.node());
			return true;
		}

		itCur = itNext;
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	配置オブジェクト一時リストの総数を得る
**//*---------------------------------------------------------------------*/
s32 Map::getTemporaryPlacementObjCount() const
{
	return _listPlaceObjTmp->getCount();
}

/*---------------------------------------------------------------------*//**
	移動可能なランダムな位置を計算する
**//*---------------------------------------------------------------------*/
bool Map::calcRandomMovablePosition(Vector3F* posOut, const Vector3F* posCenter, f32 radiusRange)
{
	for(int cntSafe = 1; cntSafe <= 10; cntSafe++)	// 進入禁止属性当たり時の滑り処理用のリトライループ
	{
		f32 x = posCenter->x() + (radiusRange * CalcUtils::randPlusMinusOneF());
		f32 z = posCenter->z() + (radiusRange * CalcUtils::randPlusMinusOneF());

		// 地面コリジョン平面を得る
		s32 idxPlane = _gcolMapRef->getPlaneIndex(x, z);
		if(idxPlane != -1)
		{
			// 地面属性フラグを得る
			u16 flagAtrb = _gcolMapRef->getPlaneAtrbFlag(idxPlane);
			if(	Gcol::isMovablePlaneAtrbFlag(flagAtrb) )	// 進入可
			{
				posOut->x() = x;
				posOut->y() = _gcolMapRef->getY(idxPlane, x, z);
				posOut->z() = z;
				return true;
			}
		}
	}
	
	// 諦めて中央を返す
	posOut->copy(posCenter);
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Map::Map()
	: _mapidCur(0)
	, _unitMapRef(0L)
	, _gcolMapRef(0L)
	, _msarrNcc(0L)
	, _numNccMsArr(0)
	, _listPlaceObjTmp(0L)
	, _ctrlevidAutoCntTmp(0)
	, _eflags(0)
	, _thetaGateMark(0.0f)
{
	TFW_ZERO_MEMORY(_uvarrGateMark, sizeof(f32) * 8);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Map::~Map()
{
	ASSERT(_unitMapRef == 0L);
	ASSERT(_gcolMapRef == 0L);
	ASSERT(_msarrNcc == 0L);
	ASSERT(_listPlaceObjTmp == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Map::create(Texture* texSignRef)
{
	// 一時用配置オブジェクト情報を作成
	_listPlaceObjTmp = new List<PlacementObj*>(true);

	// ゲート記号テクスチャを保存
	_texMarkRef = texSignRef;

	// ゲート表示位置をクリア
	for(s32 igate = 0; igate < NUM_GATE_MAX; igate++)
	{
		_garrShowGate[igate]._posCenter->set(F32_PMAX, F32_PMAX, F32_PMAX);
	}

	// ゲート記号テクスチャ座標
	RectF32 uv;
	Gcalc::calcTexUv(&uv, UV_U_GATE_MARK, UV_V_GATE_MARK, UV_W_GATE_MARK, UV_H_GATE_MARK, W_TEX, H_TEX, Env_get2drRate());
	_uvarrGateMark[0] = uv._v[0];				_uvarrGateMark[1] = - uv._v[1];
	_uvarrGateMark[2] = uv._v[0] + uv._v[2];	_uvarrGateMark[3] = - uv._v[1];
	_uvarrGateMark[4] = uv._v[0];				_uvarrGateMark[5] = - uv._v[1] - uv._v[3];
	_uvarrGateMark[6] = uv._v[0] + uv._v[2];	_uvarrGateMark[7] = - uv._v[1] - uv._v[3];

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Map::destroy()
{
	releaseMap();

	// 一時用配置オブジェクト情報を削除
	delete _listPlaceObjTmp;
	_listPlaceObjTmp = 0L;
}

/*---------------------------------------------------------------------*//**
	マップ読み込み後処理
**//*---------------------------------------------------------------------*/
bool Map::loadMapAfter(ShapeModel** mdlarr, s32 numModel)
{
	// マップモデルのアニメーションを設定
	for(int i = 0; i < numModel; i++)
	{
		if(mdlarr[i]->getAnimSet() != 0L)	// アニメーションがある
		{
			mdlarr[i]->setAnimId(1, 0, false, false, true, false, false);
		}
	}

	// ユニット化してマネージャに追加
	Unit* unit = new Unit(UNITTYPE_NULL);
	if(!unit->create((Model**)mdlarr, numModel, 0L, Unit::UNITFLAG_DEPEND_MAP))
	{
		ASSERT(false);
		return false;
	}
	_unitMapRef = unit;
	UnitManager* unitmng = Game::getGame()->getUnitManager();	ASSERT(unitmng != 0L);
	unitmng->addUnit(unit);

	// 地面コリジョンを得る
	Gcol* gcol = mdlarr[0]->getGcol();
	if(gcol == 0L)
	{
		ASSERT(false);
		return false;
	}
	_gcolMapRef = gcol;

	// カメラの移動範囲を算出
	if(_gcolMapRef != 0L)
	{
		/*
		f32 xMin = F32_PMAX;
		f32 xMax = F32_MIN;
		f32 zMin = F32_PMAX;
		f32 zMax = F32_MIN;
		for(int i = 0; i < gcol->getVertexNum(); i++)
		{
			const Vector3F* vtx = gcol->getVertex(i);
			if(xMin > vtx->x())	{	xMin = vtx->x();	}
			if(xMax < vtx->x())	{	xMax = vtx->x();	}
			if(zMin > vtx->z())	{	zMin = vtx->z();	}
			if(zMax < vtx->z())	{	zMax = vtx->z();	}
		}
		const f32 MARGIN_CAM = 1000.0f;		// 当たりの端からのマージン
		_conf._xCamMoveMin = xMin + MARGIN_CAM;
		_conf._xCamMoveMax = xMax - MARGIN_CAM;
		_conf._zCamMoveMin = zMin + MARGIN_CAM;
		_conf._zCamMoveMax = zMax - MARGIN_CAM;
		*/
		const f32 MARGIN_CAM = 1000.0f;		// 当たりの端からのマージン
		_conf._xCamMoveMin = _gcolMapRef->getVertexMin()->x() + MARGIN_CAM;
		_conf._xCamMoveMax = _gcolMapRef->getVertexMax()->x() - MARGIN_CAM;
		_conf._zCamMoveMin = _gcolMapRef->getVertexMin()->z() + MARGIN_CAM;
		_conf._zCamMoveMax = _gcolMapRef->getVertexMax()->z() - MARGIN_CAM;
	}

	// ゲート表示位置を算出
	if(_gcolMapRef != 0L)
	{
		Vector3F posWk[NUM_GATE_MAX];
		s32 num[NUM_GATE_MAX];
		TFW_ZERO_MEMORY(num, sizeof(s32) * NUM_GATE_MAX);

		for(s32 ipl = 0; ipl < _gcolMapRef->getPlaneNum(); ipl++)
		{
			const Gcol::Plane* pl = _gcolMapRef->getPlane(ipl);
			if(TFW_IS_FLAG(pl->_flagAtrb, Gcol::Plane::ATRBFMASK_MOVEMAP))
			{
				// 中央を求める
				const Vector3F* v1 = _gcolMapRef->getVertex(pl->_vtxidx1);
				const Vector3F* v2 = _gcolMapRef->getVertex(pl->_vtxidx2);
				const Vector3F* v3 = _gcolMapRef->getVertex(pl->_vtxidx3);
				Vector3F vC((v1->x() + v2->x() + v3->x()) / 3.0f, (v1->y() + v2->y() + v3->y()) / 3.0f, (v1->z() + v2->z() + v3->z()) / 3.0f);

				// どのゲートか？
				s32 igate = 0;
				if(TFW_IS_FLAG(pl->_flagAtrb, Gcol::Plane::ATRBF_MOVE_MAP_02))
				{
					igate = 1;
				}
				else if(TFW_IS_FLAG(pl->_flagAtrb, Gcol::Plane::ATRBF_MOVE_MAP_03))
				{
					igate = 2;
				}
				else if(TFW_IS_FLAG(pl->_flagAtrb, Gcol::Plane::ATRBF_MOVE_MAP_04))
				{
					igate = 3;
				}

				// まずは足し算
				posWk[igate] += vC;
				num[igate]++;
			}
		}

		// ゲート表示位置を設定する
		for(s32 igate = 0; igate < NUM_GATE_MAX; igate++)
		{
			if(num[igate] > 0)
			{
				_garrShowGate[igate]._posCenter->set(
					posWk[igate].x() / (f32)num[igate],
					posWk[igate].y() / (f32)num[igate],
					posWk[igate].z() / (f32)num[igate]	);
			}
			else
			{
				_garrShowGate[igate]._posCenter->set(
					F32_PMAX,
					F32_PMAX,
					F32_PMAX	);
			}
		}
	}

	// ニアカメラクリップの準備処理
	if(!prepareNcc())
	{
		ASSERT(false);
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ニアカメラクリップの準備処理
**//*---------------------------------------------------------------------*/
bool Map::prepareNcc()
{
	List<ModelSwitch*>	listMs(false);

	// ニアカメラ変更モデルのモデル
	for(int imdlNcc = 0; imdlNcc < _unitMapRef->getModelNum(); imdlNcc++)
	{
		ShapeModel* mdlNcc = (ShapeModel*)_unitMapRef->getModel(imdlNcc);
		// ニアカメラ変更モデルフラグで探す
		if(TFW_IS_FLAG(mdlNcc->getExFlags(), TbfDefineEx::MEXF_ATRBFLAG_NCC))
		{
			ShapeModel* mdlChg = 0L;	// 変更後のモデルへのポインタ

			// 変更後のモデルを探す
			for(int imdlDcl = 0; imdlDcl < _unitMapRef->getModelNum(); imdlDcl++)	// 宣言モデルを走査するループ
			{
				ShapeModel* mdlDcl = (ShapeModel*)_unitMapRef->getModel(imdlDcl);
				// 宣言タイプモデルでかつ同じ定義名のシェイプのモデルを探す
				if((mdlDcl->getDefType() == ShapeModel::DEFTYPE_DCL) && mdlNcc->isSameDefNameShape(mdlDcl))
				{
					for(int imdlChg = 0; imdlChg < _unitMapRef->getModelNum(); imdlChg++)	// 代替モデルを走査するループ
					{
						ShapeModel* mdlAlt = (ShapeModel*)_unitMapRef->getModel(imdlChg);
						// 定義タイプが代替で同名のモデルを探す
						if(mdlAlt->getDefType() == ShapeModel::DEFTYPE_ALT)
						{

							// 名前が一致した場合に代替モデルを設定する
							if(mdlDcl->isSameDefNameShape(mdlAlt))
							{
								mdlChg = mdlAlt;
								break;
							}

						}
					}

					break;
				}
			}

			if(mdlChg != 0L)	// 変更後のモデルが見つかった場合
			{
				// モデルのコピーを作成する
				ShapeModel* mdlDep = (ShapeModel*)mdlChg->clone();	// コピーを作成
				mdlDep->setPosture(mdlNcc->getPosition(), mdlNcc->getRotate(), mdlNcc->getScale());	// Ncc の位置をコピー

				// シーングラフ及びリソースプールに追加する
				SorSgm* sgm = Game::getGame()->getSceneGraph();	ASSERT(sgm != 0L);
				sgm->addNode(mdlDep, true);

				// モデルスイッチを作成する
				ModelSwitch* ms = new ModelSwitch();
				ms->create(mdlNcc, mdlDep);
				listMs.addTail(ms);
			}
			else				// 変更後のモデルが見つからなかった場合
			{
				// モデルスイッチを作成する
				ModelSwitch* ms = new ModelSwitch();
				ms->create(mdlNcc, 0L);
				listMs.addTail(ms);
			}
		}
	}

	// モデルスイッチリストを配列化
	_msarrNcc = listMs.toArray();
	_numNccMsArr = listMs.getCount();

	return true;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Map::exec(ExecRes* res, const ExecCtx* ec)
{
	GameExecCtx* gec = (GameExecCtx*)ec;

	// 操作ロックフラグ更新
	TFW_SET_FLAG(_eflags, EF_LOCK_OP, gec->isLockOperating());

	// ニアカメラクリップ処理
	Unit* unitMyChar = Game::getGame()->getGameMyParty()->leaderPcUnit();
	if(unitMyChar != 0L)
	{
		const Vector3F* posMainChar = unitMyChar->getPosition();

		for(int i = 0; i < _numNccMsArr; i++)
		{
			Model* mdlMap = _msarrNcc[i]->getAnyModel();
			if(mdlMap == 0L)	{	continue;	}
			const Vector3F* posMapMdl = mdlMap->getPosition();
			if((posMapMdl != 0L) && (posMainChar->z() < posMapMdl->z()))
			{
				_msarrNcc[i]->switchModel(1);
			}
			else
			{
				_msarrNcc[i]->switchModel(0);
			}

		}
	}

	// ゲート記号の回転角を更新
	_thetaGateMark += 0.025f * ec->getDeltaFrame();
	if(_thetaGateMark > TFW_PI)	{	_thetaGateMark -= TFW_DPI;	}
}

/*---------------------------------------------------------------------*//**
	描画
**//*---------------------------------------------------------------------*/
void Map::render(const RenderCtx* rc)
{
	// ゲート位置を表示
	if(!isCloseGate() && !TFW_IS_FLAG(_eflags, EF_LOCK_OP))
	{
		GameRenderCtx* grc = (GameRenderCtx*)rc;
		Renderer* rdr = rc->getRenderer();

		#if 0	// 黄色いソリッドな三角▼描画
		
			rdr->bindTexture(0L);
			rdr->setSolidColor(255, 255, 0, 255);
			rdr->setAlphaBlend(false);
			rdr->setCulling(false);
			for(s32 igate = 0; igate < NUM_GATE_MAX; igate++)
			{
				if(_garrShowGate[igate]._posCenter->x() != F32_PMAX)
				{
					Vector3F vtx[3];
					f32 theta = grc->getFrameCount() * 0.1f;
					f32 fsin = ::sinf(theta);
					f32 fcos = ::cosf(theta);
					vtx[0].set(_garrShowGate[igate]._posCenter);
					vtx[1].set(vtx[0].x() - 20.0f * fcos, vtx[0].y() + 50.0f, vtx[0].z() - 20.0f * fsin);
					vtx[2].set(vtx[0].x() + 20.0f * fcos, vtx[0].y() + 50.0f, vtx[0].z() + 20.0f * fsin);
					RendererUtils::render3dTriangles(rdr, vtx, 0L, 0L, 0L, 3, RendererUtils::TRIANGLES);
				}
			}
			rdr->setCulling(true);
		
		#else

			// テクスチャ設定
			rdr->setEnableTexture(true);
			rdr->bindTexture(_texMarkRef);
			// 加算アルファ合成
			rdr->setAlphaBlend(true);
			rdr->setAlphaBlendFunc(Renderer::AFUNC_ADD);
			// 背面カリングなし
			rdr->setCulling(false);
			// 基本色
			rdr->setSolidColor(255, 255, 255, 255);

			// 回転する板ポリゴンを各ゲートごとに描画
			f32 fsin = ::sinf(_thetaGateMark);
			f32 fcos = ::cosf(_thetaGateMark);
			f32 xr = HW_GATE_MARK * fcos;
			f32 zr = HW_GATE_MARK * fsin;
			for(s32 igate = 0; igate < NUM_GATE_MAX; igate++)
			{
				if(_garrShowGate[igate]._posCenter->x() != F32_PMAX)
				{
					const Vector3F* center = _garrShowGate[igate]._posCenter;
					Vector3F vtx[4];
					f32 x1 = center->x() - xr;
					f32 x2 = center->x() + xr;
					f32 z1 = center->z() - zr;
					f32 z2 = center->z() + zr;
					f32 y1 = center->y() + H_GATE_MARK;
					f32 y2 = center->y();
					vtx[0].set(x1, y1, z1);
					vtx[1].set(x2, y1, z2);
					vtx[2].set(x1, y2, z1);
					vtx[3].set(x2, y2, z2);
					RendererUtils::render3dTriangles(rdr, vtx, 0L, (const Vector2F*)_uvarrGateMark, 0L, 4, RendererUtils::TRIANGLE_STRIP);
				}
			}

			// 背面カリングあり
			rdr->setCulling(false);

		#endif
	}
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool Map::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();

		svst->_gamesvst._ctrlevidAutoCntMapTmp = _ctrlevidAutoCntTmp;	// マップ一時配置用自動割当制御イベント ID カウンタの保存

		// 一時配置オブジェクトの状態を保存する
		if(!svst->storePlacementObjData(_listPlaceObjTmp))
		{
			return false;
		}
	}
	else						// ダイレクトモード
	{
		ser->store(&_ctrlevidAutoCntTmp);	// マップ一時配置用自動割当制御イベント ID カウンタの保存

		// 一時配置オブジェクトの状態を保存する
		s32 numPobj = _listPlaceObjTmp->getCount();
		ser->store(&numPobj);
		for(ListIterator<PlacementObj*> it = _listPlaceObjTmp->iterator(); it.has(); it.next())
		{
			const PlacementObj* pobj = it.object();
			pobj->serialize(ser);
		}
	}

	// マップコンフィグの保存
	_conf.serialize(ser);

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool Map::deserialize(Serializer* ser)
{
	// 一時配置オブジェクトをクリア
	_listPlaceObjTmp->removeAll();

	// リスタート時はマップを読まない
	if(TFW_IS_FLAG(ser->getGeneralFlags(), SaveData::SRF_RESTARTING))
	{
		return true;
	}

	if(ser->isStructureMode())	// ストラクチャモード
	{
		const SaveStructure* svst = (SaveStructure*)ser->getStructure();

		_ctrlevidAutoCntTmp = svst->_gamesvst._ctrlevidAutoCntMapTmp;	// マップ一時配置用自動割当制御イベント ID カウンタの読み込み

		// 一時配置オブジェクトの状態を読み込む
		for(int idx = 0; idx < SaveStructure::NUM_POBJ_MAX; idx++)
		{
			const PlacementObj::SaveStructure* pobjsvst = &svst->_pobjsvst[idx];
			if(pobjsvst->_ucase == PlacementObj::UCASE_MAP_TMP)
			{
				PlacementObj* pobj = new PlacementObj();
				pobj->restoreFromStructure(pobjsvst);
				_listPlaceObjTmp->addTail(pobj);
			}
		}
	}
	else						// ダイレクトモード
	{
		ser->restore(&_ctrlevidAutoCntTmp);	// マップ一時配置用自動割当制御イベント ID カウンタの読み込み

		// 一時配置オブジェクトの状態を読み込む
		s32 numPobj = 0;
		ser->restore(&numPobj);
		for(int i = 0; i < numPobj; i++)
		{
			PlacementObj* pobj = new PlacementObj();
			pobj->deserialize(ser);
			_listPlaceObjTmp->addTail(pobj);
		}
	}

	// 一時配置オブジェクトの配置
	for(ListIterator<PlacementObj*> it = _listPlaceObjTmp->iterator(); it.has(); it.next())
	{
		PlacementObj* pobj = it.object();
		if(pobj->isPlaced())
		{
			placePlacementObj(pobj, true);
		}
	}

	// マップコンフィグの読み込み
	_conf.deserialize(ser);
	// 配置を再現する
	_conf.placePlacementObjToMap(this, true);

	return true;
}

/*---------------------------------------------------------------------*//**
	マップ開始時イベント実行
**//*---------------------------------------------------------------------*/
void Map::beginMapStartEvent(u32 evparam)
{
	// フォグの設定
	//	※	現状では、有効化・無効化は適時行うが、詳細なフォグの設定はこのタイミングでしか行わない．
	//		従って、スクリプトで値を変更すると、次のマップまで継続される．
	Renderer* rdr = Game::getGame()->getTfw()->getRenderer();
	if(_conf._fogmode != 0)	// フォグ有効
	{
		rdr->setFogWhole(true);
		::glFogf(GL_FOG_MODE, (f32)_conf._fogmode);
		if((_conf._fogmode == GL_EXP) || (_conf._fogmode == GL_EXP2))
		{
			::glFogf(GL_FOG_DENSITY, _conf._fogdensity);
		}
		else if(_conf._fogmode == GL_LINEAR)
		{
			::glFogf(GL_FOG_START, _conf._fogstart);
			::glFogf(GL_FOG_END, _conf._fogend);
		}
		::glFogfv(GL_FOG_COLOR, _conf._fogcolor);
		::glHint(GL_FOG_HINT, GL_DONT_CARE);
	}
	else
	{
		rdr->setFogWhole(false);
	}

	// 開始時イベントの実行
	if(_conf._evidStartup != 0)
	{
		EventSys* evsys = Game::getGame()->getEventSys(); ASSERT(evsys != 0L);
		EvCause evcause(EventCause::CID_MAPSTART);
		evsys->begin(_conf._evidStartup, evparam, &evcause);
	}
}

/*---------------------------------------------------------------------*//**
	配置情報の場所にユニットを配置する
**//*---------------------------------------------------------------------*/
bool Map::placeUnitFromPlacementObj(Unit* unit, PlacementObj* pobj)
{
	// 配置
	Vector3F pos = *pobj->getPosition();
	if(pobj->getPositionEvid() != 0)
	{
		const Gcol::Plane* plane = _gcolMapRef->getPlaneFromEventId(pobj->getPositionEvid());
		if(plane != 0L)
		{
			const Vector3F* v1 = _gcolMapRef->getVertex(plane->_vtxidx1);
			const Vector3F* v2 = _gcolMapRef->getVertex(plane->_vtxidx2);
			const Vector3F* v3 = _gcolMapRef->getVertex(plane->_vtxidx3);
			if((v1 != 0L) && (v2 != 0L) && (v3 != 0L))
			{
				pos.x() = (v1->x() + v2->x() + v3->x()) * 0.3333333333333f;
				pos.y() = (v1->y() + v2->y() + v3->y()) * 0.3333333333333f;
				pos.z() = (v1->z() + v2->z() + v3->z()) * 0.3333333333333f;
			}
		}
	}
	f32 rRandomPos = pobj->getPositionRandomRadius();
	if(rRandomPos != 0.0f)
	{
		calcRandomMovablePosition(&pos, &pos, rRandomPos);	// 乱数配置
	}
	if(!TFW_IS_FLAG(pobj->getPlaceFlags(), PlacementObj::PLACEF_NO_Y_COLL))
	{
		adjustCollY(&pos);
	}
	unit->setPosition(&pos);

	// 向き
	if(unit->isCategory(Unit::UNITCAT_CHAR))
	{
		((CharUnit*)unit)->setDirection(pobj->getDirection());
	}

	// 無効設定
	if(!pobj->isEnable())
	{
		unit->setEnable(false);	// 配置はするが無効（後で有効にするパターン）
	}
	// 隠し設定
	if(pobj->isHidden())
	{
		unit->setHidden(true);
	}
	// フォーカス無効設定
	if(!pobj->isEnableFocus())
	{
		unit->setEnableFocus(false);
	}

	// 起動イベントを配置時から進めるセーブフラグ ID が設定されている場合は、起動イベントを進める
	if(pobj->getDefaultNextCallEvSfid() != 0)
	{
		SaveData* svdat = Game::getGame()->getSaveSys()->getSaveData();
		if(svdat->getSaveF(pobj->getDefaultNextCallEvSfid()))
		{
			if(pobj->getActiveCallEviIndex() == 0)	// 現在最初のイベント起動の場合
			{	
				pobj->nextCallEvi(false);
			}
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	Y 位置をコリジョンから調節する
**//*---------------------------------------------------------------------*/
bool Map::adjustCollY(Vector3F* pos)
{
	if(_gcolMapRef == 0L)
	{
		return false;
	}
	s32 idxPlane = _gcolMapRef->getPlaneIndex(pos->x(), pos->z());
	if(idxPlane == -1)
	{
		return false;
	}
	pos->y() = _gcolMapRef->getY(idxPlane, pos->x(), pos->z());
	return true;
}


//==========================================================================
// Map::Gate メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Map::Gate::Gate()
{
	_posCenter = new Vector3F();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Map::Gate::~Gate()
{
	delete _posCenter;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
