/***********************************************************************//**
 *	ExplosionMc.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/02/17.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ExplosionMc.h"

// Friends
#include "../MagicSys.h"
#include "../../common/CalcUtils.h"
#include "../../body/Game.h"
#include "../../body/GameExecCtx.h"
#include "../../map/Map.h"
#include "../../particle/GameParticleDef.h"
#include "../../status/FocusArray.h"
#include "../../unit/Unit.h"
#include "../../unit/UnitManager.h"

// External
#include "../../../etk/particle/Particle.h"
#include "../../../etk/particle/ParticleSys.h"
#include "../../../tfw/collection/List.h"
#include "../../../tfw/lib/Matrix.h"
#include "../../../tfw/lib/Vector.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ExplosionMc メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ExplosionMc::ExplosionMc()
	: _radiusCur(0.0f)
	, _radiusMax(0.0f)
	, _radiusDelta(0.0f)
	, _listTrg(0L)
	, _isHitUnit(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ExplosionMc::~ExplosionMc()
{
	ASSERT(_listTrg == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ExplosionMc::create(MagicSys* mgcsysOwnrRef, u16 mcid, const MagicClusterCreateParam* mccprm)
{
	if(!MagicCluster::create(mgcsysOwnrRef, mcid, mccprm))
	{
		return false;
	}

	// 初期パラメータ
	_radiusCur = 0.0f;
	_radiusMax = mccprm->_radius;
	_radiusDelta = (mccprm->_radius / (f32)mccprm->_lifeframe) * 1.1f;	// 念のため少し多めに増加させる
	_isHitUnit = false;

	// ターゲットリストの作成
	_listTrg = new List<Unit*>;
	if(!TFW_IS_FLAG(_ctrlflags, CTRLF_NO_COLL))
	{
		UnitManager* unitmng = Game::getGame()->getUnitManager();
		for(s32 i = 0; i < unitmng->getUnitNum(); i++)
		{
			Unit* unit = unitmng->getUnitFromIndex(i);

			// 有効なモデルのみ対象
			if(!unit->isEnable())
			{
				continue;
			}

			// フォーカス可能なモデルのみ対象
			if(!unit->isEnableFocus())
			{
				continue;
			}

			// 影響を与えられないユニットを排除
			if(!unit->isCategory(Unit::UNITCAT_CHAR))
			{
				continue;
			}

			// 半径内に入ったかどうか
			const Vector3F* posUnit = unit->getPosition();
			if(	!TFW_IS_NEAR(_pos->x(), posUnit->x(), unit->getRadius() + _radiusMax) ||
				!TFW_IS_NEAR(_pos->y(), posUnit->y(), unit->getRadius() + _radiusMax) ||
				!TFW_IS_NEAR(_pos->z(), posUnit->z(), unit->getRadius() + _radiusMax) )
			{
				continue;
			}

			// 対象リストに追加
			_listTrg->addTail(unit);
		}
	}
	// 距離が近い順にソート
	CalcUtils::sortUnitArrayByDistanceAscending(_listTrg, _pos);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ExplosionMc::destroy()
{
	delete _listTrg;
	_listTrg = 0L;

	MagicCluster::destroy();
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理
**//*---------------------------------------------------------------------*/
void ExplosionMc::exec(const ExecCtx* ec)
{
	MagicCluster::exec(ec);
	if(!isValid())	{	return;	}

	// 半径増加
	if(_radiusCur < _radiusMax)	
	{
		_radiusCur += _radiusDelta * ec->getDeltaFrame();
		if(_radiusCur > _radiusMax)	{	_radiusCur = _radiusMax;	}
	}

	// ユニットへヒット
	if(_radiusCur > 0.0f)	// 念のため
	{
		f32 radiusCurSq = _radiusCur * _radiusCur;
		ListIterator<Unit*> itCur = _listTrg->iterator();
		while(itCur.has())
		{
			ListIterator<Unit*> itNext = itCur;
			itNext.next();

			Unit* unitTrg = itCur.object();

			// 半径内に入っているか判定
			Vector3F diff = *_pos - *unitTrg->getPosition();
			f32 distDiffSq = diff.lengthSq();
			if(distDiffSq > radiusCurSq)
			{
				break;	// 昇順ソート済みなので抜けてしまってかまわない
			}

			// 距離による強さ
			f32 ratio = 1.0f - (distDiffSq / (_radiusMax * _radiusMax));
			if(ratio > 1.0f)
			{
				ratio = 1.0f;
			}
			if(ratio > 0.0f)
			{
				TransStat tstat(*_tstat);
				if(ratio < 1.0f)
				{
					tstat.scale(ratio);
				}

				// ヒット処理
				hitUnit(ec, unitTrg, &tstat);
			}

			// 対象リストから外す
			_listTrg->remove(unitTrg);
			_isHitUnit = true;

			itCur = itNext;
		}
	}

	// 爆発エフェクト
	if((((GameExecCtx*)ec)->getBodyExecCount() & 3) == (::rand() & 5))
	{
		Vector3F pos(
			CalcUtils::randPlusMinusOneF() * _radiusCur,
			CalcUtils::randF() * _radiusCur * 0.5f,
			CalcUtils::randPlusMinusOneF() * _radiusCur );
		pos += *_pos;
		Game::getGame()->getParticleSys()->appearParticle(GameParticleDef::PTCLK_HIT_CLUSTER, &pos, GameParticleDef::CF_NO_CALC_VEL);
	}

	if(_listTrg->getCount() <= 0)
	{
		// 自ら消滅する
		disappearSelf(_isHitUnit ? DCF_COLL_UNIT : DCF_SELF_VANISH, 5.0f);
	}
}

/*---------------------------------------------------------------------*//**
	消滅時フレーム制御
**//*---------------------------------------------------------------------*/
void ExplosionMc::execExtinction(const ExecCtx* ec)
{
	MagicCluster::execExtinction(ec);

	f32 scale = _fcntExtinction;
	f32 alphaMul = 1.0f - _fcntExtinction / _frameExtinctionMax;

	if(_arrptclid[0] != 0)
	{
		Particle* ptcl = Game::getGame()->getParticleSys()->getParticle(_arrptclid[0]);
		ptcl->setScale(scale);				// スケール設定
		ptcl->setMulAlphaColor(alphaMul);	// パーティクルの乗算アルファ設定
	}
}

/*---------------------------------------------------------------------*//**
	パーティクルの出現
**//*---------------------------------------------------------------------*/
bool ExplosionMc::appearParticles(const MagicClusterCreateParam* mccprm)
{
	_arrptclid[0] = Game::getGame()->getParticleSys()->appearParticle(mccprm->_ptclkind, mccprm->_pos, GameParticleDef::CF_NO_CALC_VEL);
	return _arrptclid[0] != 0;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
