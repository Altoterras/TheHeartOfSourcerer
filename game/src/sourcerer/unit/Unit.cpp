/***********************************************************************//**
 *	Unit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Unit.h"

// Friends
#include "UnitSaveStructure.h"
#include "../body/Game.h"
#include "../body/GameNotifier.h"
#include "../spirit/Spirit.h"

// External
#include "../../tfw/common/IdManager.h"
#include "../../tfw/g3d/sg/node/implement/Model.h"
#include "../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../tfw/lib/Vector.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Unit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	表示中かどうかを得る
**//*---------------------------------------------------------------------*/
bool Unit::isShow() const
{
	// １つでも表示中のものがあれば true で返す
	for(int i = 0; i < _numMdl; i++)
	{
		if(_mdlarrRef[i]->isShow())
		{
			return true;
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	指定カテゴリかどうかを得る
**//*---------------------------------------------------------------------*/
bool Unit::isCategory(UnitCategory ucat) const
{
	switch(ucat)
	{
	default:
		return false;
	case UNITCAT_CHAR:
		return TFW_IS_FLAG(_unitflags, UNITFLAG_CHAR);
	case UNITCAT_ITEM:
		return TFW_IS_FLAG(_unitflags, UNITFLAG_ITEM);
	}
}

/*---------------------------------------------------------------------*//**
	味方かどうかを得る
**//*---------------------------------------------------------------------*/
bool Unit::isFriendsSide(const Unit* unitTrg) const
{
	if(TFW_IS_FLAG(this->getUnitFlags(), Unit::UNITFLAG_PC))
	{
		return TFW_IS_FLAG(unitTrg->getUnitFlags(), Unit::UNITFLAG_PC);
	}
	else if(this->getUnitType() == UNITTYPE_ENEMY_CHAR)
	{
		return unitTrg->getUnitType() == UNITTYPE_ENEMY_CHAR;
	}
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	有効・無効設定
**//*---------------------------------------------------------------------*/
void Unit::setEnable(bool isEnable)
{
	TFW_SET_FLAG(_aflags, AF_ENABLE, isEnable);

	setShow(isEnable);
}

/*---------------------------------------------------------------------*//**
	位置を設定
**//*---------------------------------------------------------------------*/
void Unit::setPosition(const Vector3F* pos)
{
	_pos->copy(pos);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Unit::Unit(UnitType unittype)
	: _unittype((u8)unittype)
	, _aflags(0)
	, _unitid(0)
	, _mdlarrRef(0L)
	, _numMdl(0)
	, _unitflags(0)
	, _pos(0L)
	, _radius(0.0f)
{
	// メモリ改ざんバグチェック用
	#if defined(_DEBUG)
		__pad_0010[0] = 0;
	#endif
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Unit::~Unit()
{
	ASSERT(_mdlarrRef == 0L);
	ASSERT(_pos == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Unit::create(Model* mdlRef, Spirit* sprtRef, u16 unitflags)
{
	return create(&mdlRef, 1, sprtRef, unitflags);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Unit::create(Model** mdlarrRef, s32 numModel, Spirit* sprtRef, u16 unitflags)
{
	// モデル参照配列を作成する
	s32 cntMdl = 0;
	for(int i = 0; i < numModel; i++)
	{
		if(mdlarrRef[i] != 0L)	{	cntMdl++;	}
	}
	_numMdl = cntMdl;
	if(_numMdl > 0)
	{
		_mdlarrRef = new Model*[_numMdl];
		cntMdl = 0;
		for(int i = 0; i < _numMdl; i++)
		{
			if(mdlarrRef[i] != 0L)
			{
				_mdlarrRef[cntMdl] = mdlarrRef[i];
				_mdlarrRef[cntMdl]->setUsed(true);	// 使用フラグを立てる
				cntMdl++;
			}
		}
	}

	// モデル半径を求める
	f32 r = 1.0f;	// デフォルト 1cm
	for(int i = 0; i < _numMdl; i++)
	{
		if((_mdlarrRef[i] != 0L) && (_mdlarrRef[i]->getModelClass() == Model::MODELCLASS_SHAPE))
		{
			ShapeModel* smdl = (ShapeModel*)_mdlarrRef[i];
			ASSERT(smdl != 0L);
			f32 rBb;
			smdl->getAllShapesBoundingBox(0L, &rBb);
			if(r < rBb)
			{
				// 外包する球から内包する球の半径に変換（だいたい；本来は (1/√2)×(1/√2) か）
				r = rBb * 0.6f;
			}
		}
	}
	_radius = r;

	// 位置ベクトルを作成する
	_pos = new Vector3F();

	// フラグを保存する
	_unitflags = unitflags;
	if(TFW_IS_FLAG(_unitflags, UNITFLAG_CREATION_FOCUSABLE))	{	_aflags |= AF_FOCUSABLE;	}
	if(TFW_IS_FLAG(_unitflags, UNITFLAG_CREATION_HIDDEN))		{	_aflags |= AF_HIDDEN;		}
	if(TFW_IS_FLAG(_unitflags, UNITFLAG_CREATION_HIDN_CLICK))	{	_aflags |= AF_HIDN_CLICK;	}
	
	// ユニット ID を得る
	_unitid = Game::getGame()->getIdManager()->getNewId();
	ASSERT(_unitid != 0L);

	// 有効化
	setEnable(true);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Unit::destroy()
{
	// 無効化
	setEnable(false);

	// ユニット ID を解放する
	ASSERT(_unitid != 0L);
	Game::getGame()->getIdManager()->releaseId(_unitid);

	// 位置ベクトルを削除する
	delete _pos;
	_pos = 0L;

	// モデルの使用フラグを降ろす
	for(int i = 0; i < _numMdl; i++)
	{
		_mdlarrRef[i]->setUsed(false);	// 使用フラグを下ろす
	}

	// モデル参照配列を削除する
	delete[] _mdlarrRef;
	_mdlarrRef = 0L;
}

/*---------------------------------------------------------------------*//**
	マップ変更前通知
**//*---------------------------------------------------------------------*/
void Unit::notifyChangeMapBefore()
{
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void Unit::notifyChangedMap()
{
}

/*---------------------------------------------------------------------*//**
	対決状態変更通知
**//*---------------------------------------------------------------------*/
void Unit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
}

/*---------------------------------------------------------------------*//**
	イベントゲームモード変更通知
**//*---------------------------------------------------------------------*/
void Unit::notifyChangedEventMode(bool isEventMode)
{
}

/*---------------------------------------------------------------------*//**
	ステータス影響発生通知
**//*---------------------------------------------------------------------*/
void Unit::notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat)
{
	GameNotifier* gnotif = Game::getGame()->getGameNotifier(); ASSERT(gnotif != 0L);
	gnotif->notifyUnitInfluenceStat(this, scres, tstat);
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャに保存する
**//*---------------------------------------------------------------------*/
bool Unit::storeToStructure(UnitSaveStructure* svst) const
{
	ASSERT(sizeof(UnitSaveStructure) == UnitSaveStructure::SIZE_THIS);

	svst->_aflags = _aflags;

	svst->_pos.copy(_pos);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool Unit::restoreFromStructure(const UnitSaveStructure* svst)
{
	ASSERT(sizeof(UnitSaveStructure) == UnitSaveStructure::SIZE_THIS);
	
	u16 aflags = svst->_aflags;
	setEnable( TFW_IS_FLAG(aflags, AF_ENABLE) );
	setGuiFocused( TFW_IS_FLAG(aflags, AF_GUI_FOCUSED) );
	setEnableFocus( TFW_IS_FLAG(aflags, AF_FOCUSABLE) );
	setHidden( TFW_IS_FLAG(aflags, AF_HIDDEN) );
	setHiddenClick( TFW_IS_FLAG(aflags, AF_HIDN_CLICK) );

	setPosition(&svst->_pos);

	return true;
}

/*---------------------------------------------------------------------*//**
	表示設定
**//*---------------------------------------------------------------------*/
void Unit::setShow(bool isShow)
{
	for(int i = 0; i < _numMdl; i++)
	{
		if(!TFW_IS_FLAG(_mdlarrRef[i]->getModelFlag(), Model::MFLAG_NO_USE))	// DEFTYPE_DECLARE, DEFTYPE_ALTERNATIVE を除外
		{
			_mdlarrRef[i]->setShow(isShow);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
