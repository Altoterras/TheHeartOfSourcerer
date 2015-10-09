/***********************************************************************//**
 *	EquipmentItemUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EquipmentItemUnit.h"

// Friends
#include "../../episode/data/FixedData.h"
#include "../../item/Item.h"
#include "../../item/ItemDef.h"
#include "../../item/ItemDefTbl.h"
#include "../../body/Game.h"
#include "../../model/spid/Spid.h"
#include "../../status/CharStat.h"

// External
#include "../../../etk/sg/SorSgm.h"
#include "../../../tfw/Tfw.h"
#include "../../../tfw/g3d/HrAnimObjCtx.h"
#include "../../../tfw/g3d/sg/node/implement/LookAtCamera.h"
#include "../../../tfw/gcmn/View.h"
#include "../../../tfw/lib/Matrix.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EquipmentItemUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	被フォーカス変更
**//*---------------------------------------------------------------------*/
void EquipmentItemUnit::setGuiFocused(bool isEnable)
{
	Unit::setGuiFocused(isEnable);

	if(isEnable)
	{
		if(_haocSpRef != 0L)
		{
			// フォーカスされているときのみ行列を保存するように設定
			_haocSpRef->setFlags(HrAnimObjCtx::F_SAVE_MV_MTX, isEnable);
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EquipmentItemUnit::EquipmentItemUnit()
	: ItemUnit(UNITTYPE_EQUIPMENT_ITEM)
	, _itmRef(0L)
	, _unitOwnerRef(0L)
	, _haocSpRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EquipmentItemUnit::~EquipmentItemUnit()
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EquipmentItemUnit::create(Item* item, Unit* owner, u16 spid)
{
	// 値の設定
	_itmRef = item;
	_unitOwnerRef = owner;

	// SPID より装備の物理的場所を得る
	if(spid != 0L)
	{
		if(Spid::find(&_haocSpRef, spid, owner))
		{
			// 行列を保存するように設定
			_haocSpRef->setFlags(HrAnimObjCtx::F_SAVE_MV_MTX, true);
		}
	}

	// アイテムユニット作成
	u16 itmdid = (_itmRef->getItemDef() != 0L) ? _itmRef->getItemDef()->getItemDefId() : 0;
	if(!ItemUnit::create(itmdid, _itmRef->eleneStat(), UNITFLAG_EQUIPMENT_ITEM | UNITFLAG_CREATION_FOCUSABLE | UNITFLAG_CREATION_HIDDEN))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void EquipmentItemUnit::destroy()
{
	ASSERT(this->_unittype == UNITTYPE_EQUIPMENT_ITEM);
	_itmRef = 0L;
	_unitOwnerRef = 0L;

	ItemUnit::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void EquipmentItemUnit::exec(ExecRes* res, const ExecCtx* ec)
{
	ItemUnit::exec(res, ec);

	// 位置を得る
	do
	{
		// 階層アニメオブジェクトから得る
		//if((_haocSpRef != 0L) && isGuiFocused())	// 重い処理なのでフォーカスされているときのみ
		if(_haocSpRef != 0L)
		{
			const Matrix4F* mtxMv = _haocSpRef->getModelViewMatrix();
			if(mtxMv != 0L)
			{
				SorSgm* sgm = Game::getGame()->getSceneGraph(); ASSERT(sgm != 0L);
				View* view = Game::getGame()->getTfw()->getView();
				Camera* cam = sgm->getVisibleCamera();
				if(cam != 0L)
				{
					const Matrix4F* mtxCamInv = cam->getViewInverseMatrix();
					if(mtxCamInv != 0L)
					{
						Matrix4F mtxRotInv;
						switch(view->getRotZ())
						{
						case TFW_DISPF_RANG_90:
							mtxRotInv.setRotateZ(TFW_DEG_TO_RAD(90.0f));
							break;
						case TFW_DISPF_RANG_180:
							mtxRotInv.setRotateZ(TFW_DEG_TO_RAD(180.0f));
							break;
						case TFW_DISPF_RANG_270:
							mtxRotInv.setRotateZ(TFW_DEG_TO_RAD(270.0f));
							break;
						default:
							break;
						}
						Vector3F pos(mtxMv->_v[3][0], mtxMv->_v[3][1], mtxMv->_v[3][2]);
						mtxRotInv.mult(&pos, &pos);
						mtxCamInv->mult(&pos, &pos);
						_pos->copy(&pos);
						break;
					}
				}
			}
		}
		
		// オーナーユニットから得る
		if(_unitOwnerRef != 0L)
		{
			_pos->copy(_unitOwnerRef->getPosition());
			break;
		}
	}
	while(false);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
