/***********************************************************************//**
 *	NpcCharUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "NpcCharUnit.h"

// Friends
#include "../../../common/CalcUtils.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../map/Map.h"
#include "../../../map/PlacementObj.h"

// External
#include "../../../../etk/sg/SorSgm.h"
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
// NpcCharUnit 静的メンバ変数

//==========================================================================
// NpcCharUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	制御用イベント ID を得る
**//*---------------------------------------------------------------------*/
u16 NpcCharUnit::getCtrlEvid() const
{
	if(_pobjRef == 0L)	{	return 0;	}
	return _pobjRef->getCtrlEvid();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	配置オブジェクト情報を設定する
**//*---------------------------------------------------------------------*/
void NpcCharUnit::setPlacementObj(PlacementObj* pobj)
{
	_pobjRef = pobj;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
NpcCharUnit::NpcCharUnit(UnitType unittype)
	: CharUnit(unittype)
	, _pobjRef(0L)
	, _fcntRandWalk(0.0f)
	, _walkstat(WSTAT_NULL)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
NpcCharUnit::~NpcCharUnit()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool NpcCharUnit::create(Model* mdlRef, Spirit* sprtRef, u16 chrdid, u16 unitflag, CharClass charcls)
{
	_fcntRandWalk = F32_PMAX;	// すぐに歩き出すように

	return CharUnit::create(mdlRef, sprtRef, chrdid, unitflag | UNITFLAG_NPC, charcls, true);
}

/*---------------------------------------------------------------------*//**
	ランダム歩行フレーム処理
**//*---------------------------------------------------------------------*/
void NpcCharUnit::execWalkingRandom(const ExecCtx* ec)
{
	if((_pobjRef != 0L) && (_pobjRef->getWalkingSpeed() > 0.0f))
	{
		// 一定フレーム数、もしくは進入禁止衝突にて、ランダムで方向転換
		_fcntRandWalk += ec->getDeltaFrame();
		if(_fcntRandWalk >= 150.0f || (_isHitKeepoutColl && (_fcntRandWalk >= 30.0f)))
		{
			_fcntRandWalk = 0.0f;

			// 歩行方向
			switch(rand() & 1)
			{
			case 0:
				_walkstat = WSTAT_NULL;
				break;
			case 1:
				_walkstat = WSTAT_WALK;
				{
					Vector3F pos(
						getPosition()->x() + ((CalcUtils::randF() * 200000.0f) - 100000.0f),
						getPosition()->y(),
						getPosition()->z() + ((CalcUtils::randF() * 200000.0f) - 100000.0f));
					setTargetPosition(&pos);
				}
				break;
			}
		}

		// 歩行処理
		switch(_walkstat)
		{
		case WSTAT_WALK:
			Vector2F accXz(
				_posTarget->x() - getPosition()->x(),
				_posTarget->z() - getPosition()->z()	);
			accXz.normalize();
			accXz.x() *= _pobjRef->getWalkingSpeed();
			accXz.y() *= _pobjRef->getWalkingSpeed();
			walk(&accXz, true);
			break;
		}
	}

	NpcCharUnit::execWalking(ec);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
