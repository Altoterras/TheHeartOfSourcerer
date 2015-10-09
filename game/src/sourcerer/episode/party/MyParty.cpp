/***********************************************************************//**
 *	MyParty.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MyParty.h"

// Friends
#include "../FixedSourceIdDef.h"
#include "../StoryIdDef.h"
#include "../StoryManager.h"
#include "../char/pc/HelperUnit.h"
#include "../char/pc/MyUnit.h"
#include "../char/pc/PartnerUnit.h"
#include "../char/pc/HelperSpirit.h"
#include "../char/pc/MySpirit.h"
#include "../char/pc/PartnerSpirit.h"
#include "../data/FixedData.h"
#include "../save/SaveStructure.h"
#include "../spid/SpidDef.h"
#include "../worldmap/WorldMap.h"
#include "../../body/Game.h"
#include "../../body/MoveMap.h"
#include "../../gui/GameGui.h"
#include "../../gui/FocusCursor.h"
#include "../../gui/MoveCursor.h"
#include "../../gui/MoveCursorExtension.h"
#include "../../item/Item.h"
#include "../../item/ItemEquip.h"
#include "../../item/ItemManager.h"
#include "../../map/Map.h"
#include "../../source/Source.h"
#include "../../source/SourceManager.h"
#include "../../spirit/SpiritManager.h"
#include "../../status/EquipStat.h"
#include "../../status/StatusIfEx.h"
#include "../../status/TransStat.h"
#include "../../unit/Unit.h"
#include "../../unit/UnitManager.h"

// External
#include "../../../tfw/collision/Gcol.h"
#include "../../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MyParty メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	プレイヤーキャラクタが有効かどうかを得る
**//*---------------------------------------------------------------------*/
bool MyParty::isEnablePc(int ipc) const
{
	Unit* unit = _pcarr[ipc]._unitRef;
	if(unit != 0L)
	{
		return unit->isEnable();
	}
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	キャラクタ達を読み込む
**//*---------------------------------------------------------------------*/
bool MyParty::loadPcChars()
{
	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
	SpiritManager* sprtmng = Game::getGame()->getSpiritManager(); ASSERT(sprtmng != 0L);
	GameGui* gui = Game::getGame()->getGui(); ASSERT(gui != 0L);

	if(unitmng->getMyCharUnit() != 0L)	// 読み込み済み
	{
		return true;
	}

	// パートナーキャラ作成
	PartnerSpirit* sptrPc2 = new PartnerSpirit();
	PartnerUnit* unitPc2 = new PartnerUnit();
	if(!sptrPc2->create())
	{
		delete sptrPc2;
		sptrPc2 = 0L;
		ASSERT(false);
		return false;
	}
	if(!unitPc2->create(sptrPc2, this))
	{
		delete unitPc2;
		unitPc2 = 0L;
		ASSERT(false);
		return false;
	}

	// ヘルプキャラ作成
	HelperSpirit* sptrPc3 = new HelperSpirit();
	HelperUnit* unitPc3 = new HelperUnit();
	if(!sptrPc3->create())
	{
		delete sptrPc3;
		sptrPc3 = 0L;
		ASSERT(false);
		return false;
	}
	if(!unitPc3->create(sptrPc3, this))
	{
		delete unitPc3;
		unitPc3 = 0L;
		ASSERT(false);
		return false;
	}

	// 自キャラ作成
	MySpirit* sptrPc1 = new MySpirit();
	MyUnit* unitPc1 = new MyUnit();
	if(!sptrPc1->create())
	{
		delete sptrPc1;
		sptrPc1 = 0L;
		ASSERT(false);
		return false;
	}
	if(!unitPc1->create(sptrPc1, this))
	{
		delete unitPc1;
		unitPc1 = 0L;
		ASSERT(false);
		return false;
	}
	gui->getFocusCursor()->setOwnerSpirit(sptrPc1);

	// 配置（計算順序の都合で、PC1 を最初に追加）
	sprtmng->addSpirit(sptrPc1);
	sprtmng->addSpirit(sptrPc2);
	sprtmng->addSpirit(sptrPc3);
	unitmng->addMyCharUnit(unitPc1);
	unitmng->addUnit(unitPc2);
	unitmng->addUnit(unitPc3);
	_pcarr[PC_MY]._unitRef = unitPc1;
	_pcarr[PC_PTN]._unitRef = unitPc2;
	_pcarr[PC_HLP]._unitRef = unitPc3;

	// 最強装備に（Vol.1 のみかも）
	unitPc2->equipBest();
	unitPc2->updateEquipModelObjVisibility();

	// 有効化
	setEnablePcChar(PC_MY, true);
	setEnablePcChar(PC_PTN, true);
	setEnablePcChar(PC_HLP, true);

	// 移動カーソル拡張に操作ユニットに設定する
	MoveCursor* mvcsr = Game::getGame()->getGui()->getMoveCursor(); ASSERT(mvcsr != 0L);
	MoveCursorExtension* mcex = mvcsr->extension(); ASSERT(mcex != 0L);
	mcex->registerHandlingableUnit(unitPc1);
	mcex->registerHandlingableUnit(unitPc2);

	// デフォルトのリーダー設定
	setLeader(PC_MY);

	return true;
}

/*---------------------------------------------------------------------*//**
	リーダーになれるか調べる
**//*---------------------------------------------------------------------*/
bool MyParty::isValidLeader(s32 ipc)
{
	// ストーリー判定
	StoryManager* strymng = Game::getGame()->getStoryManager(); ASSERT(strymng != 0L);
	if(strymng->getCurrentStoryId() < StoryIdDef::STID_EP01_ASA_UNGETSU_BATTLE)
	{
		// 雲月再戦まではリーダーチェンジできない
		return false;
	}

	// ヘルプキャラはリーダーにできない
	if(ipc == PC_HLP)
	{
		return false;
	}

	return Party::isValidLeader(ipc);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MyParty::MyParty()
	: _srcmng(0L)
	, _itmmng(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MyParty::~MyParty()
{
	ASSERT(_srcmng == 0L);
	ASSERT(_itmmng == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MyParty::create()
{
	if(!Party::create(NUM_PC_MAX))	{	return false;	}

	// マ法ソース管理の作成
	SourceManager* splmng = new SourceManager();
	if(!splmng->create(FixedData::getFixedSourceDefTable()))
	{
		delete splmng;
		ASSERTM(false, "SourceManager::create failed.");
		return false;
	}
	_srcmng = splmng;

	// アイテム管理の作成
	ItemManager* itmmng = new ItemManager();
	if(!itmmng->create(FixedData::getItemDefTable()))
	{
		delete itmmng;
		ASSERTM(false, "ItemManager::create failed.");
		return false;
	}
	_itmmng = itmmng;

	// アイテムの追加
	#if defined(_DEBUG) && 1
	{
		// テスト石（癒し石１）
		{
			s32 itmidx = _itmmng->newItem(2024);
			// 水火風のベールを付帯
			Item* item = _itmmng->item(itmidx);
			CharStat cstat;
			cstat.setup(CharStat::CF_ITEM | CharStat::CF_IGNORE_ATTACK, false);
			cstat.bind(0L, item->eleneStat(), 0L);
			TransStat tstat;
			Elene eeOql(110000, 165000, 88000, 8800, 110000);
			Elene eeCql(110000, 165000, 88000, 88000, 110000);
			EleneStat eestatSkill(&eeOql, &eeCql);
			tstat.setupForSkill(0L, &eestatSkill);
#if 1
			StatusIfEx::influence(&cstat, &tstat, false, false);
#else
			StatusChangeResult scres;
			StatusInfluencer::influence(&scres, &cstat, &tstat);
#endif
			TRACE(
				"{MyParty::create} item 2024 veil1: %d,%d,%d,%d,%d\n",
				item->getEleneStat()->getVeil(0)->getElene()->getSoil(),
				item->getEleneStat()->getVeil(0)->getElene()->getWater(),
				item->getEleneStat()->getVeil(0)->getElene()->getFire(),
				item->getEleneStat()->getVeil(0)->getElene()->getWind(),
				item->getEleneStat()->getVeil(0)->getElene()->getAethel()	);
		}
		// テスト石（浄化石１）
		{
			s32 itmidx = _itmmng->newItem(2027);
			// 水火風のベールを付帯
			Item* item = _itmmng->item(itmidx);
			CharStat cstat;
			cstat.setup(CharStat::CF_ITEM | CharStat::CF_IGNORE_ATTACK, false);
			cstat.bind(0L, item->eleneStat(), 0L);
			TransStat tstat;
			Elene eeOql(110000, 165000, 88000, 8800, 110000);
			Elene eeCql(110000, 165000, 88000, 88000, 110000);
			EleneStat eestatSkill(&eeOql, &eeCql);
			tstat.setupForSkill(0L, &eestatSkill);
#if 1
			StatusIfEx::influence(&cstat, &tstat, false, false);
#else
			StatusChangeResult scres;
			StatusInfluencer::influence(&scres, &cstat, &tstat);
#endif
			TRACE(
				"{MyParty::create} item 2027 veil1: %d,%d,%d,%d,%d\n",
				item->getEleneStat()->getVeil(0)->getElene()->getSoil(),
				item->getEleneStat()->getVeil(0)->getElene()->getWater(),
				item->getEleneStat()->getVeil(0)->getElene()->getFire(),
				item->getEleneStat()->getVeil(0)->getElene()->getWind(),
				item->getEleneStat()->getVeil(0)->getElene()->getAethel()	);
		}
		
		_itmmng->newItem(2001);	// テスト石（水晶）
		_itmmng->newItem(2001);	// テスト石（水晶）
		_itmmng->newItem(2001);	// テスト石（水晶）
		_itmmng->newItem(2001);	// テスト石（水晶）
		_itmmng->newItem(3001);	// テスト草（雑草）
		_itmmng->newItem(3024);	// テスト草（癒し草）
		_itmmng->newItem(3027);	// テスト草（浄化草）
		_itmmng->newItem(2007);	// テスト石（火）
		_itmmng->newItem(2019);	// テスト石（水石）
		_itmmng->newItem(2028);	// テスト石（褐石）
		_itmmng->newItem(3013);	// テスト草（温草）
		_itmmng->newItem(3052);	// テスト草（昇り草）
		_itmmng->newItem(2050);	// テスト石（玄武石）
		_itmmng->newItem(3009);	// テスト草（火風草）
		_itmmng->newItem(2081);	// テスト石（大王石）
	}
	#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MyParty::destroy()
{
	// アイテム管理の削除
	if(_itmmng != 0L)
	{
		_itmmng->destroy();
		delete _itmmng;
		_itmmng = 0L;
	}

	// マ法ソース管理の削除
	if(_srcmng != 0L)
	{
		_srcmng->destroy();
		delete _srcmng;
		_srcmng = 0L;
	}

	Party::destroy();
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool MyParty::serialize(Serializer* ser) const
{
	// ソースデータ保存
	_srcmng->serialize(ser);
	// アイテムデータ保存
	_itmmng->serialize(ser);

	// PC のデータ保存
	MyUnit* unitPc1 = (MyUnit*)_pcarr[PC_MY]._unitRef;
	unitPc1->serialize(ser);
	PartnerUnit* unitPc2 = (PartnerUnit*)_pcarr[PC_PTN]._unitRef;
	unitPc2->serialize(ser);
	HelperUnit* unitPc3 = (HelperUnit*)_pcarr[PC_HLP]._unitRef;
	unitPc3->serialize(ser);

	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		SaveStructure::PcPartySaveStructure* pcptysvst = &svst->_pcptysvst;
		pcptysvst->_ipcLeader = (u8)getLeaderPcIndex();
	}
	else						// ダイレクトモード
	{
		SaveStructure::PcPartySaveStructure pcptysvst;
		pcptysvst._ipcLeader = (u8)getLeaderPcIndex();
		if(!ser->store(&pcptysvst, sizeof(SaveStructure::PcPartySaveStructure))) { return false; }
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアライズ
**//*---------------------------------------------------------------------*/
bool MyParty::deserialize(Serializer* ser)
{
	// ソースデータ読込
	_srcmng->deserialize(ser);
	// アイテムデータ読込
	_itmmng->deserialize(ser);

	// PC のデータ読込
	MyUnit* unitPc1 = (MyUnit*)_pcarr[PC_MY]._unitRef;
	unitPc1->deserialize(ser);
	PartnerUnit* unitPc2 = (PartnerUnit*)_pcarr[PC_PTN]._unitRef;
	unitPc2->deserialize(ser);
	HelperUnit* unitPc3 = (HelperUnit*)_pcarr[PC_HLP]._unitRef;
	unitPc3->deserialize(ser);

	// リーダーを得る
	s32 ipcLeader = 0;
	if(ser->isStructureMode())	// ストラクチャモード
	{
		const SaveStructure* svst = (SaveStructure*)ser->getStructure();
		const SaveStructure::PcPartySaveStructure* pcptysvst = &svst->_pcptysvst;
		ipcLeader = pcptysvst->_ipcLeader;
	}
	else						// ダイレクトモード
	{
		SaveStructure::PcPartySaveStructure pcptysvst;
		if(!ser->restore(&pcptysvst, sizeof(SaveStructure::PcPartySaveStructure))) { return false; }
		ipcLeader = pcptysvst._ipcLeader;
	}
	// ヘルパーキャラはリーダーになれない（古い仕様からの補正）
	if(ipcLeader == PC_HLP)
	{
		ipcLeader = PC_MY;
	}

	// リーダー設定
	if(ipcLeader != getLeaderPcIndex())
	{
		setLeader(ipcLeader);
	}

	// アイテム装備フラグの更新
	for(int i = 0; i < _itmmng->getItemNum(); i++)
	{
		_itmmng->item(i)->setEquiped(false);			// 装備フラグを一度クリアする
	}
	for(int j = 0; j < NUM_PC; j++)
	{
		ItemEquip* itmeq = _pcarr[j]._unitRef->itemEquip();
		if(itmeq == 0L)		{	continue;	}
		EquipStat* eqstat = itmeq->equipStat();
		if(eqstat == 0L)	{	continue;	}
		for(int i = 0; i < EquipPlace::NUM_EQP; i++)
		{
			Item* item = eqstat->equipItem(i);
			if(item == 0L)	{	continue;	}
			item->setEquiped(true);			// 装備フラグを再設定する
		}
	}

	// バージョン間互換性調整
	if(	(ser->getMajorVersionNumber() <= sourcerer::SaveStructure::VER_MAJOR_SAVE_STRUCTURE)
		&& (ser->getMinorVersionNumber() <= sourcerer::SaveStructure::VER_MINOR_SAVE_STRUCTURE_VER131)	)
	{
		// ソースリストの予約処理
		reserveDefaultSourceList();
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	規定のソースリスト予約を行う
**//*---------------------------------------------------------------------*/
void MyParty::reserveDefaultSourceList()
{
	// 安全のため、一度ソースをソートする
	_srcmng->sortSource();

	// 予約処理
	const s32 numReserveForUser = 45;
	const s32 numReserveForFixed = 50;
	ASSERT((numReserveForUser + numReserveForFixed) <= _srcmng->getSourceNum());
	_srcmng->reserveSourceFirst(numReserveForUser, numReserveForFixed);
}

/*---------------------------------------------------------------------*//**
	シリアライズ可能かを判定する
**//*---------------------------------------------------------------------*/
bool MyParty::isSerializableNow()
{
	for(int i = 0; i < _numPc; i++)
	{
		if(_pcarr[i]._unitRef == 0L)
		{
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	リーダーのマップ初期位置を算出する
**//*---------------------------------------------------------------------*/
bool MyParty::moveMapInitLeaderPos()
{
	Map* map = Game::getGame()->getMap();						if(map == 0L)		{	return false;	}
	MoveMap* mvmap = Game::getGame()->getMoveMap();				if(mvmap == 0L)		{	return false;	}
	Gcol* gcol = map->getGroundCollision();						if(gcol == 0L)		{	return false;	}
	StoryManager* strymng = Game::getGame()->getStoryManager();	if(strymng == 0L)	{	return false;	}
	WorldMap* wmap = strymng->worldMap();						if(wmap == 0L)		{	return false;	}

	// ワールドマップから隣接するマップ ID を得る
	s32 mapidPrev = wmap->getNeighborMapId(mvmap->getCurMapId(), mvmap->getPrevMapId());
	// 屋内から屋外に出たときなどはワールドマップからは得られない
	if(mapidPrev == -1)
	{
		mapidPrev = mvmap->getPrevMapId();	// 以前のマップ ID をそのまま検索用に使う
	}

	// step == 1 : 以前のマップ番号が移動フラグとして設定されている三角形に隣接している三角形に移動
	// step == 2 : 最初に見つかった入口フラグの場所からスタート
	for(int step = 1; step <= 2; step++)
	{
		for(int idxPlane = 0; idxPlane < gcol->getPlaneNum(); idxPlane++)
		{
			u16 flagAtrb = gcol->getPlaneAtrbFlag(idxPlane);
			// 入口フラグを探す
			if(TFW_IS_FLAG(flagAtrb, Gcol::Plane::ATRBF_ENTRANCE))
			{
				const Gcol::Plane* planeEntrance = gcol->getPlane(idxPlane);
				if(planeEntrance != 0L)
				{
					// 隣接三角形から以前のマップ番号と一致するものを探す（step == 1 の場合）
					u16 idxPlaneNeib[] = { planeEntrance->_plnidxNeib1, planeEntrance->_plnidxNeib2, planeEntrance->_plnidxNeib3 };
					for(int j = 0; j < 3; j++)
					{
						s32 mapidMove = (step == 1) ? map->getGcolPlaneMoveMapId(idxPlaneNeib[j]) : 0;
						if((mapidPrev == mapidMove) || (step == 2))
						{
							// 入口フラグのある三角形の中心から開始する
							const Vector3F* vp[] = { gcol->getVertex(planeEntrance->_vtxidx1), gcol->getVertex(planeEntrance->_vtxidx2), gcol->getVertex(planeEntrance->_vtxidx3) };
							Vector3F pos( ( vp[0]->x() + vp[1]->x() + vp[2]->x() ) / 3.0f, 0.0f, ( vp[0]->z() + vp[1]->z() + vp[2]->z() ) / 3.0f );
							pos.y() = gcol->getY(idxPlane, pos.x(), pos.z());
							// リーダー位置設定
							Unit* unitLeader = leaderPcUnit();
							if(unitLeader != 0L) { unitLeader->setPosition(&pos); }
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
