/***********************************************************************//**
 *	ExtraUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/22.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ExtraUnit.h"

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

#define CNT_DETECT_INVALID		(-1)	// 対象検出無効値
#define CNT_DETECT_START		(15)	// 対象検出開始カウンタ値
#define CNT_DETECT_MOVE			(0)		// 検出時移動カウンタ値

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ExtraUnit 静的メンバ変数

//==========================================================================
// ExtraUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	探知対象を設定する
**//*---------------------------------------------------------------------*/
void ExtraUnit::setDetectTarget(Unit* unitTrg)
{
	if(getThisSpirit()->getFocusUnit() == unitTrg)	{	return;	}	// 設定済み

	getThisSpirit()->setFocusUnit(unitTrg, false);	// 注視ユニットの設定

	_cntDetect = CNT_DETECT_START;	// 検出カウンタのセット
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ExtraUnit::ExtraUnit()
	: NpcCharUnit(UNITTYPE_EXTRA_CHAR)
	, _cntDetect(CNT_DETECT_INVALID)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ExtraUnit::~ExtraUnit()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ExtraUnit::create(Spirit* sprtRef, u16 chrdid)
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

	///_eflags |= F_IGNORE_KEEPOUT;	// 進入禁止を無視する

	return NpcCharUnit::create(mdl, sprtRef, chrdid, UNITFLAG_DEPEND_MAP | UNITFLAG_CREATION_FOCUSABLE, CHARCLS_NULL);	// フォーカス可能に設定する
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void ExtraUnit::exec(ExecRes* res, const ExecCtx* ec)
{
	// 対象検出
	if(_cntDetect >= 0)
	{
		if(_cntDetect == CNT_DETECT_MOVE)
		{
			if(getThisSpirit()->getFocusUnit() != 0L)
			{
				// 移動ターゲット位置の設定
				*_posTarget = *getThisSpirit()->getFocusUnit()->getPosition();
			}
		}

		if(_cntDetect > 0)	{	_cntDetect--;	}
	}

	// 仮．本来は ExtraRole からやるが、現状まだないので
	NpcCharUnit::execWalking(ec);

	// キャラユニットとしての移動処理など
	NpcCharUnit::exec(res, ec);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
