/***********************************************************************//**
 *	HelperUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/10/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "HelperUnit.h"

// Friends
#include "MyUnit.h"
#include "../../party/MyParty.h"
#include "../../../episode/save/SaveStructure.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../map/Map.h"
#include "../../../status/CharStat.h"
#include "../../../status/EleneStat.h"
#include "../../../episode/char/CharIdDef.h"

// External
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../etk/sg/TbfDefineEx.h"
#include "../../../../tfw/collision/Gcol.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/lib/Matrix.h"
#include "../../../../tfw/lib/Point.h"
#include "../../../../tfw/g3d/sg/node/implement/Camera.h"
#include "../../../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/ui/TouchPanel.h"
#include "../../../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define VEL_MIN				(150.0f / (f32)FRAMERATE)	// 移動最小速さ
#define VEL_MAX				(460.0f / (f32)FRAMERATE)	// 標準の移動最大速さ
#define RATIO_RUN			(1.0f)						// 標準の走り速さ率
#define RATIO_FRICT_NRM		(0.5f)						// 標準の速さ摩擦率（非加速フレーム時）
#define RATIO_FRICT_ACC		(0.5f)						// 標準の速さ摩擦率（加速フレーム時）

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// HelperUnit 静的メンバ変数

//==========================================================================
// HelperUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	フレンド関係にあるユニットの配列を得る
**//*---------------------------------------------------------------------*/
UnitArray HelperUnit::getFriends() const
{
	UnitArray uarr;
	CharUnit* unitPc1 = _partyRef->pcUnit(MyParty::PC_MY);
	if((unitPc1 != 0L) && unitPc1->isEnable())
	{
		uarr.addUnit(unitPc1);
	}
	CharUnit* unitPc2 = _partyRef->pcUnit(MyParty::PC_PTN);
	if((unitPc2 != 0L) && unitPc2->isEnable())
	{
		uarr.addUnit(unitPc2);
	}
	return uarr;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
HelperUnit::HelperUnit()
	: PcUnit(UNITTYPE_HELPER_CHAR)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
HelperUnit::~HelperUnit()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool HelperUnit::create(Spirit* sprtRef, Party* party)
{
	// モデルを読み込み、リソースプールに追加（シーングラフには追加しない）
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	ShapeModel* mdl = 0L;
	if(!ModelLoader::loadModelFromTbf(Game::getGame()->getTfw(), CHARACTER_CHAR1103_TBMB, TbfDefineEx::MEXF_CHAR, ModelLoader::LFLAG_NULL, (Model**)&mdl))
	{
		return false;
	}

	// 親クラスによる作成
	if(!PcUnit::create(mdl, sprtRef, party, CharIdDef::CHARID_PC3, CHARCLS_ENHANCER))
	{
		return false;
	}

	// 基本速さパラメータの設定
	setVelocityParam(VEL_MAX, RATIO_RUN, RATIO_FRICT_NRM, RATIO_FRICT_ACC);
	setVelocityRange(VEL_MIN, VEL_MAX);

	// 半径、中央位置の調整
	_heightCenter += 110.0f;

	return true;
}

/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool HelperUnit::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		SaveStructure* svst = (SaveStructure*)ser->getStructure();
		SaveStructure::PcSaveStructure* pcsvst = &svst->_pcsvst[2];
		
		CharUnit::storeToStructure(&pcsvst->_cusvst);
	}
	else						// ダイレクトモード
	{
		CharUnitSaveStructure cusvst;
		CharUnit::storeToStructure(&cusvst);
		ser->store(&cusvst, sizeof(CharUnitSaveStructure));
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	デシリアイズ
**//*---------------------------------------------------------------------*/
bool HelperUnit::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		const SaveStructure* svst = (SaveStructure*)ser->getStructure();
		const SaveStructure::PcSaveStructure* pcsvst = &svst->_pcsvst[2];

		CharUnit::restoreFromStructure(&pcsvst->_cusvst);
	}
	else						// ダイレクトモード
	{
		CharUnitSaveStructure cusvst;
		ser->restore(&cusvst, sizeof(CharUnitSaveStructure));
		CharUnit::restoreFromStructure(&cusvst);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	移動カーソルによって操作されているかどうか
**//*---------------------------------------------------------------------*/
bool HelperUnit::isOperatedForMoving()
{
	return false;	// 操作されることはない
}

/*---------------------------------------------------------------------*//**
	目標位置計算
**//*---------------------------------------------------------------------*/
void HelperUnit::calcTargetPos()
{
	Vector3F posLeaderDiff(100.0f, 0.0f, -75.0f);	// ヘルプキャラはリーダーの左後ろにいる
	PcUnit::calcTargetPosRelLeader(&posLeaderDiff);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
