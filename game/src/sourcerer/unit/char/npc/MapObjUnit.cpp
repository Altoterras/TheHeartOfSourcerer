/***********************************************************************//**
 *	MapObjUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/16.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MapObjUnit.h"

// Friends
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../map/Map.h"
#include "../../../spirit/char/npc/ExtraSpirit.h"
#include "../../../status/CharStat.h"

// External
#include "../../../../etk/particle/Particle.h"
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../etk/sg/TbfDefineEx.h"
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

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MapObjUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MapObjUnit::MapObjUnit()
	: NpcCharUnit(UNITTYPE_MAPOBJ)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MapObjUnit::~MapObjUnit()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MapObjUnit::create(u16 chrdid)
{
	// モデルを読み込み、リソースプールとシーングラフに追加
	u32 fileid = FILEID_FROM_CHAR_MODEL(chrdid);
	#if defined(_DEBUG)
		if(!IdFile::isIdFileExist(fileid))	{	fileid = 311031;	}	// 存在しない場合は適当なキャラをとりあえず表示
	#endif
	//SorSgm* sgm = Game::getGame()->getSceneGraph();
	ShapeModel* mdl = 0L;
	if(!ModelLoader::loadModelFromTbf(Game::getGame()->getTfw(), fileid, TbfDefineEx::MEXF_CHAR, ModelLoader::LFLAG_DISPOSE_SCENE_GRAPH, (Model**)&mdl))
	{
		return false;
	}

	return NpcCharUnit::create(mdl, 0L, chrdid, UNITFLAG_DEPEND_MAP | UNITFLAG_CREATION_FOCUSABLE, CHARCLS_NULL);	// フォーカス可能に設定する
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
