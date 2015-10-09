/***********************************************************************//**
 *	EmittedMc.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/04/03.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EmittedMc.h"

// Friends
#include "../../body/Game.h"
#include "../../unit/Unit.h"

// External
#include "../../../etk/particle/Particle.h"
#include "../../../etk/particle/ParticleSys.h"
#include "../../../tfw/lib/Vector.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EmittedMc メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EmittedMc::EmittedMc()
	: _radiusVel(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool EmittedMc::create(MagicSys* mgcsysOwnrRef, u16 mcid, const MagicClusterCreateParam* mccprm)
{
	// 半径増減速度を得る
	_radiusVel = mccprm->_radiusVel;

	// 親クラスの作成
	if(!ServedMc::create(mgcsysOwnrRef, mcid, mccprm))
	{
		return false;
	}

	// 初期半径を計算する
	BallMc::calcRadius();

	return true;
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理
**//*---------------------------------------------------------------------*/
void EmittedMc::exec(const ExecCtx* ec)
{
	ServedMc::exec(ec);
	if(!isValid())	{	return;	}

	// 半径の計算
	_radius += ec->getDeltaFrame() * _radiusVel;
	calcParticleRadius();
}

/*---------------------------------------------------------------------*//**
	消滅時フレーム制御
**//*---------------------------------------------------------------------*/
void EmittedMc::execExtinction(const ExecCtx* ec)
{
	BallMc::execExtinction(ec);

	f32 scale = _fcntExtinction;
	f32 alphaMul = 1.0f - _fcntExtinction / _frameExtinctionMax;

	if(_arrptclid[0] != 0)
	{
		Particle* ptcl = Game::getGame()->getParticleSys()->getParticle(_arrptclid[0]);
		ptcl->setScale(scale);				// スケール設定
		ptcl->setMulAlphaColor(alphaMul);	// パーティクルの乗算アルファ設定
	}
}

#if 0
/*---------------------------------------------------------------------*//**
	ユニットとの当たり
**//*---------------------------------------------------------------------*/
bool EmittedMc::collUnit(const ExecCtx* ec, Unit* unit)
{
	// 有効なモデルのみ対象
	if(!unit->isEnable())
	{
		return false;
	}

	// フォーカス可能なモデルのみ対象
	if(!unit->isEnableFocus())
	{
		return false;
	}

	// 自らに当たった場合は除外する
	if(_unitOwnrRef == unit)
	{
		return false;
	}

	// 影響を与えられないユニットを排除
	if(!unit->isCategory(Unit::UNITCAT_CHAR) && !unit->isCategory(Unit::UNITCAT_ITEM))
	{
		return false;
	}

	// 半径内に入ったかどうか
	const Vector3F* posUnit = unit->getCenterPos();
	if(	!TFW_IS_NEAR(_pos->x(), posUnit->x(), unit->getRadius() + _radius) ||
		!TFW_IS_NEAR(_pos->y(), posUnit->y(), unit->getRadius() + _radius) ||
		!TFW_IS_NEAR(_pos->z(), posUnit->z(), unit->getRadius() + _radius) )
	{
		return false;
	}

	// ヒット
	if(!hitUnit(ec, unit))
	{
		return false;
	}

	return true;
}
#endif

/*---------------------------------------------------------------------*//**
	半径を計算する
**//*---------------------------------------------------------------------*/
void EmittedMc::calcRadius()
{
	// ここでは半径を計算しない
}


////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
