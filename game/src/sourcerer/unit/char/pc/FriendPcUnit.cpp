/***********************************************************************//**
 *	FriendPcUnit.cpp
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
#include "FriendPcUnit.h"

// Friends
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../map/Map.h"
#include "../../../spirit/Spirit.h"
#include "../../../episode/party/MyParty.h"

// External
#include "../../../../etk/sg/SorSgm.h"
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

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// FriendPcUnit 静的メンバ変数

//==========================================================================
// FriendPcUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void FriendPcUnit::notifyChangedMap()
{
	moveMapInitPos();	// 入口フラグによる移動

	getShapeModel()->setAnimId(ANIMID_STOP, 0, false, false, true, false, false);	// 停止モーションへ変更

	PcUnit::notifyChangedMap();
}

/*---------------------------------------------------------------------*//**
	対決状態変更通知
**//*---------------------------------------------------------------------*/
void FriendPcUnit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
	//spirit()->setFocusUnit(unitTrgRef);	// 注視ユニットの設定

	PcUnit::notifyChangedConfront(isConfront, confront, unitTrg, unitEntrant, isAddition);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
FriendPcUnit::FriendPcUnit(UnitType unittype)
	: PcUnit(unittype)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
FriendPcUnit::~FriendPcUnit()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool FriendPcUnit::create(Model* mdlRef, Spirit* sprtRef, Party* party, u16 chrdid, CharClass charcls)
{
	_eflags |= F_IGNORE_KEEPOUT;	// 進入禁止を無視する

	return PcUnit::create(mdlRef, sprtRef, party, chrdid, charcls);
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void FriendPcUnit::destroy()
{
	PcUnit::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void FriendPcUnit::exec(ExecRes* res, const ExecCtx* ec)
{
	// キャラユニットとしての移動処理など
	PcUnit::exec(res, ec);
}

/*---------------------------------------------------------------------*//**
	マップ初期位置へ移動
**//*---------------------------------------------------------------------*/
bool FriendPcUnit::moveMapInitPos()
{
	setPosition(_posTarget);
	return true;
}

/*---------------------------------------------------------------------*//**
	目標位置計算
**//*---------------------------------------------------------------------*/
void FriendPcUnit::calcTargetPos(const Vector3F* posPc1Diff)
{
	const CharUnit* unitMy = _partyRef->getPcUnit(MyParty::PC_MY); ASSERT(unitMy != 0L);

	// 自キャラが自分をフォーカスしているときはやめる
	if(!getSpirit()->isConfront())	// 非対決時のみ
	{
		const Unit* unitMyCharFocus = unitMy->getSpirit()->getFocusUnit();
		if(unitMyCharFocus != 0L)
		{
			if(unitMyCharFocus == this)					{	return;	}	// 自キャラがこちらをフォーカスしている
			if(unitMyCharFocus->getOwnerUnit() == this)	{	return;	}	// おそらく所持アイテムをフォーカスしている
		}
	}

	// PC 1 との位置差
	Matrix4F mtxRotY;
	mtxRotY.rotateY( unitMy->getDirection()->y() );
	Vector3F posTrg;
	mtxRotY.mult(&posTrg, posPc1Diff);
	posTrg += *unitMy->getPosition();

	// コリジョンをとる
	Gcol* gcol = Game::getGame()->getMap()->getGroundCollision();
	if(gcol != 0L)
	{
		for(int cntSafe = 1; cntSafe <= 10; cntSafe++)	// 進入禁止属性当たり時の滑り処理用のリトライループ
		{
			// 地面コリジョン平面を得る
			s32 idxPlane = gcol->getPlaneIndex(posTrg.x(), posTrg.z());
			if(idxPlane != -1)
			{
				// 地面属性フラグを得る
				u16 flagAtrb = gcol->getPlaneAtrbFlag(idxPlane);
				if(	Gcol::isMovablePlaneAtrbFlag(flagAtrb) )	// 進入可
				{
					break;	// OK !
				}
			}

			// 進入不可の場合は徐々に PC1 に近づける
			posTrg.set((posTrg.x() + unitMy->getPosition()->x()) * 0.5f, (posTrg.y() + unitMy->getPosition()->y()) * 0.5f, (posTrg.z() + unitMy->getPosition()->z()) * 0.5f);
		}
	}

	setTargetPosition(&posTrg);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
