/***********************************************************************//**
 *	MapconfMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MapconfMatter.h"

// Friends
#include "../EventMatterCreateParam.h"
#include "../EventMatterType.h"
#include "../../EventSys.h"
#include "../../../body/Game.h"
#include "../../../map/Map.h"
#include "../../../map/PlacementObj.h"
#include "../../../unit/UnitManager.h"
#include "../../../save/SaveData.h"
#include "../../../save/SaveSys.h"
#include "../../../spirit/SpiritManager.h"
#include "../../../episode/data/FixedData.h"

// External
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapconfMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MapconfMatter::MapconfMatter()
	: EventMatter(EMTYPE_MAPCONF)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MapconfMatter::~MapconfMatter()
{
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MapconfMatter::destroy()
{
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool MapconfMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
{
	if(!EventMatter::setupFromXmlNode(node, cparam))	{	return false;	}

	s32 iwk;
	const XmlNode* nodeWk;
	s32* iarrWk = 0L;
	s32 numArr = 0;

	Map* map = Game::getGame()->getMap();
	MapConfig* mconf = map->mapConfig();
	SaveSys* svdat = Game::getGame()->getSaveSys();

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// マップコンフィグ

	// 開始イベント
	if(XmlParserUtils::analyzeChildData(&iwk, node, "StartupEvent") != 0L)	{	mconf->_evidStartup = iwk;	}

	// マップ移動設定
	if(XmlParserUtils::analyzeChildData(&iwk, node, "MoveMap1") != 0L)		{	mconf->_mapidMove[0] = iwk;	}
	if(XmlParserUtils::analyzeChildData(&iwk, node, "MoveMap2") != 0L)		{	mconf->_mapidMove[1] = iwk;	}
	if(XmlParserUtils::analyzeChildData(&iwk, node, "MoveMap3") != 0L)		{	mconf->_mapidMove[2] = iwk;	}
	if(XmlParserUtils::analyzeChildData(&iwk, node, "MoveMap4") != 0L)		{	mconf->_mapidMove[3] = iwk;	}

	// BGM
	nodeWk = XmlParserUtils::analyzeChildData(&iwk, node, "BgmId");
	if(nodeWk != 0L)
	{
		mconf->_bgmid = iwk;
		mconf->_volumeBgm = 1.0f;	// デフォルトボリューム

		f32 fwk;
		if(XmlParserUtils::analyzeAtrb(&fwk, nodeWk, "volume"))	{	mconf->_volumeBgm = fwk;	}
	}

	// マップ種別（屋内、フィールド...など）
	VcString strwk;
	if(XmlParserUtils::analyzeChildData(&strwk, node, "MapKind") != 0L)
	{
		if(strwk.equalsIgnoreCase("Indoor"))		{	mconf->_mapkind = MapConfig::MAPKIND_INDOOR;	}
		else if(strwk.equalsIgnoreCase("Field"))	{	mconf->_mapkind = MapConfig::MAPKIND_FIELD;		}
	}

	// フォグ
	nodeWk = XmlParserUtils::findChild(node, "Fog");
	if(nodeWk != 0L)
	{
		if(XmlParserUtils::analyzeAtrb(&strwk, nodeWk, "mode"))
		{
			if(strwk.equalsIgnoreCase("linear"))	{	mconf->_fogmode = GL_LINEAR;	}
			else if(strwk.equalsIgnoreCase("exp"))	{	mconf->_fogmode = GL_EXP;		}
			else if(strwk.equalsIgnoreCase("exp2"))	{	mconf->_fogmode = GL_EXP2;		}
		}

		f32 fwk;
		if(XmlParserUtils::analyzeAtrb(&fwk, nodeWk, "density"))	{	mconf->_fogdensity = fwk;	}
		if(XmlParserUtils::analyzeAtrb(&fwk, nodeWk, "start"))		{	mconf->_fogstart = fwk;		}
		if(XmlParserUtils::analyzeAtrb(&fwk, nodeWk, "end"))		{	mconf->_fogend = fwk;		}

		Vector4F vwk;
		if(XmlParserUtils::analyzeAtrb(&vwk, nodeWk, "color"))
		{
			mconf->_fogcolor[0] = vwk.r();
			mconf->_fogcolor[1] = vwk.g();
			mconf->_fogcolor[2] = vwk.b();
			mconf->_fogcolor[3] = vwk.a();
		}
	}

	// 元素バランス
	nodeWk = XmlParserUtils::findChild(node, "EleneEff");
	if(nodeWk != 0L)
	{
		// 原質
		if(XmlParserUtils::analyzeAtrb(&iarrWk, &numArr, nodeWk, "nql"))
		{
			Elene ee;
			for(int i = 0; (i < Elene::NUM_EL) && (i < numArr); i++)
			{
				ee.set(i, iarrWk[i] * 100);
			}

			if(mconf->_eestatEff.isInitialValue())
			{
				mconf->_eestatEff.setup(&ee, &ee);
			}
			else
			{
				mconf->_eestatEff.nql()->copy(&ee);
			}

			delete[] iarrWk;
			iarrWk = 0L;
		}

		// 現在の質
		if(XmlParserUtils::analyzeAtrb(&iarrWk, &numArr, nodeWk, "cql"))
		{
			Elene ee;
			for(int i = 0; (i < Elene::NUM_EL) && (i < numArr); i++)
			{
				ee.set(i, iarrWk[i] * 100);
			}

			if(mconf->_eestatEff.isInitialValue())
			{
				mconf->_eestatEff.setup(&ee, &ee);
			}
			else
			{
				mconf->_eestatEff.cql()->copy(&ee);
			}

			delete[] iarrWk;
			iarrWk = 0L;
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// 固定配置オブジェクト

	s32 cntPobj = 1;

	const XmlNode* nodeChild = node->getChildNode();
	while(nodeChild != 0L)
	{
		if(nodeChild->getName()->equals("PlaceObj"))
		{
			// 配置オブジェクト情報を作成してマップコンフィグに追加する
			PlacementObj* pobj = 0L;

			// 配置情報を作成する
			pobj = new PlacementObj();
			if(pobj != 0L)
			{
				pobj->setupFromXmlNode(nodeChild, PlacementObj::UCASE_MAP_CONF, (u8)cntPobj++, (const EventMatterCreateParam*)cparam);	// XML ノードからセットアップ

				// マップコンフィグに追加する
				mconf->addPlacementObjSetting(pobj);
			}
		}
		nodeChild = nodeChild->getSiblingNode();
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
