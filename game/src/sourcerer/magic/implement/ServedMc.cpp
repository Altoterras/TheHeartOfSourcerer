/***********************************************************************//**
 *	ServedMc.cpp
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
#include "ServedMc.h"

// Friends
#include "../MagicSys.h"
#include "../../body/Game.h"
#include "../../map/Map.h"
#include "../../status/FocusArray.h"
#include "../../unit/Unit.h"
#include "../../unit/UnitManager.h"

// External
#include "../../../etk/particle/Particle.h"
#include "../../../etk/particle/ParticleSys.h"
#include "../../../tfw/collision/Gcol.h"
#include "../../../tfw/gcmn/Gcalc.h"
#include "../../../tfw/lib/Matrix.h"
#include "../../../tfw/lib/Vector.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

#define RATE_INTP_V_MIN	(0.15f)	// 速度補完率最小値（大きいほど対象に当たりやすい）

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ServedMc メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	初速度を設定する
**//*---------------------------------------------------------------------*/
void ServedMc::setInitialVelocity(const Vector3F* vel)
{
	MagicCluster::setInitialVelocity(vel);

	_speedDefault = vel->length();
}

/*---------------------------------------------------------------------*//**
	加速させる
**//*---------------------------------------------------------------------*/
void ServedMc::accelerate(const Vector3F* acc)
{
	MagicCluster::accelerate(acc);

	// 基本加速値を設定する
	if(_accDefault == 0.0f)
	{
		_accDefault = acc->length();
	}
}

/*---------------------------------------------------------------------*//**
	位置等を反映する
**//*---------------------------------------------------------------------*/
void ServedMc::reflectPosture()
{
	BallMc::reflectPosture();
	applyParticlePosture();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ServedMc::ServedMc()
	: _numParticle(0)
	, _posAfter(0L)
	, _accDefault(0.0f)
	, _speedDefault(0.0f)
	, _leaveOwner(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ServedMc::~ServedMc()
{
	ASSERT(_posAfter == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ServedMc::create(MagicSys* mgcsysOwnrRef, u16 mcid, const MagicClusterCreateParam* mccprm)
{
	// パーティクル数を得る
	_numParticle = TFW_MIN(mccprm->_numParticle, NUM_PTCL_MAX);

	// 残像位置配列の作成
	if(_numParticle > 1)
	{
		_posAfter = new Vector3F[_numParticle - 1];
		if(_posAfter == 0L)		{	return false;	}
	}

	// 親クラスの作成
	if(!BallMc::create(mgcsysOwnrRef, mcid, mccprm))
	{
		return false;
	}

	// フォーカスが設定されている場合はフォーカスターゲットのみに当たる
	if((mccprm->_unitMainTrgRef != 0L) && !mccprm->_isNoOnlyTrgColl)
	{
		TFW_SET_FLAG(_ctrlflags, MagicCluster::CTRLF_ONLY_TRG_COLL, true);	// ターゲットのみ
	}

	// 残像位置の初期位置を設定
	if(_posAfter != 0L)
	{
		for(int i = 0; i < _numParticle - 1; i++)
		{
			_posAfter[i].copy(_pos);
		}
	}

	// パーティクルの位置等設定
	applyParticlePosture();

	// その他の初期化
	_leaveOwner = false;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ServedMc::destroy()
{
	delete[] _posAfter;
	_posAfter = 0L;

	BallMc::destroy();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void ServedMc::exec(const ExecCtx* ec)
{
	BallMc::exec(ec);
	if(!isValid())	{	return;	}

	// 移動処理
	Gcol* gcol = Game::getGame()->getMap()->getGroundCollision();
	Unit* unitTrg = _focusarr->unit(0);
	//if(_cntExec++ > 0)
	{
		// 移動方向をターゲット方向に修正
		if(unitTrg != 0L)
		{
			#if 0
				Vector3F diff = *unitTrg->getCenterPos() - *_pos;
				f32 lenlenDiff = diff.lengthSq();		// 残りの距離（の２乗）
				diff.normalize();						// 方向を得る
				f32 lenVel = _vel->length();
				f32 advLenVel = lenVel * ec->getDeltaFrame();
				if((advLenVel * advLenVel) > lenlenDiff)	// 進む距離が残り距離より長い場合
				{
					lenVel *= 0.5f;	// 適当に減速
				}
				*_vel = diff * lenVel;
				//Vector3F acc = diff * (lenVel * 0.5f);
				//*_vel *= 0.5f;
				//accelerate(&acc);
			#else
				// ターゲット方向に向きを変える
				Vector3F dif = *unitTrg->getCenterPos() - *_pos;
				Vector3F difu = dif;
				difu.normalize();
				Vector3F velu = *_vel;
				velu.normalize();
				//if((velu.dot(&difu) >= 0.866f) || (_fcntElapsed > 0.0f)/*(_fcntElapsed >= (_frameLife * 0.5f))*/)	// 内積が±30°以下、又は寿命が半分以下なら
				{
					// 強制的にターゲットへ方向を向ける
					f32 speed = _vel->length();
					f32 rateLife = RATE_INTP_V_MIN + (_fcntElapsed / _frameLife);	// 寿命が残り少ないほど正確に
					if(rateLife > 1.0f) { rateLife = 1.0f; }
					rateLife = rateLife * rateLife;				// 寿命が残り少ないほどより正確に（二次曲線のカーブ）
					f32 irateLife = 1.0f - rateLife;
					_vel->_v[0] = ((velu._v[0] * irateLife) + (difu._v[0] * rateLife)) * speed;
					_vel->_v[1] = ((velu._v[1] * irateLife) + (difu._v[1] * rateLife)) * speed;
					_vel->_v[2] = ((velu._v[2] * irateLife) + (difu._v[2] * rateLife)) * speed;
				}
				Vector3F acc = difu;
				acc *= _accDefault * ec->getDeltaFrame();
				accelerate(&acc);
				// 速度制限
				f32 speedSq = _vel->lengthSq();
				f32 dlenSq = dif.lengthSq();
				if(speedSq >(_speedDefault * _speedDefault))	// 基本速さよりも速い
				{
					_vel->normalize();
					*_vel *= _speedDefault;
					speedSq = _vel->lengthSq();
				}
				if(dlenSq < speedSq)	// １フレームあたりに進む距離が残り距離より長い場合
				{
					_vel->normalize();
					*_vel *= ::sqrtf(dlenSq);
				}
			#endif
		}

		// 速度を適用
		_pos->x() += _vel->x() * ec->getDeltaFrame();
		_pos->y() += _vel->y() * ec->getDeltaFrame();
		_pos->z() += _vel->z() * ec->getDeltaFrame();

		// 当たり
		if(!TFW_IS_FLAG(_ctrlflags, CTRLF_NO_COLL))
		{
			// 地面コリジョン平面を得る
			s32 idxPlane = gcol->getPlaneIndex(_pos->x(), _pos->z());
			if(idxPlane == -1)
			{
				// マップ外に出たとする
				TRACE("{ServedMc::exec} null plane col.\n");
				disappearSelf(DCF_OUT_MAP, 3.0f);	// 自ら消滅する
			}
			else
			{
				// 地面の高さを得る
				f32 y = gcol->getY(idxPlane, _pos->x(), _pos->z());
				if(y != INVALID_FLOAT)
				{
					// 跳ね返り処理
					if((_pos->y() - _radius) < y)	// 地面に潜った
					{
						//_pos->y() = - _vel->y() * ec->getDeltaFrame();
						_pos->y() = y + _radius;
					}
				}
			}
		}
	}
	
	// 対象に当たったかどうか
	if(!TFW_IS_FLAG(_ctrlflags, CTRLF_NO_COLL))
	{
		if(TFW_IS_FLAG(_ctrlflags, CTRLF_ONLY_TRG_COLL))	// ターゲットのみ当たりをとる
		{
			if(unitTrg != 0L)
			{
				collUnit(ec, unitTrg);
			}
		}
		else
		{
			bool collSelf = false;
			UnitManager* unitmng = Game::getGame()->getUnitManager();
			for(s32 i = 0; i < unitmng->getUnitNum(); i++)
			{
				Unit* unit = unitmng->getUnitFromIndex(i);
				if(unit == _unitOwnrRef) { collSelf = true; }
				if(collUnit(ec, unit)) { break; }
			}
			if(!collSelf && !_leaveOwner)
			{
				_leaveOwner = true;	// 一度離れた
			}
		}
	}
	
	// パーティクルの位置等設定
	applyParticlePosture();

	// 位置で残像させる
	if(_posAfter != 0L)
	{
		for(int i = _numParticle - 2; i >= 1; i--)
		{
			_posAfter[i].copy(&_posAfter[i - 1]);
		}
		_posAfter[0].copy(_pos);
	}
}

/*---------------------------------------------------------------------*//**
	消滅時フレーム制御
**//*---------------------------------------------------------------------*/
void ServedMc::execExtinction(const ExecCtx* ec)
{
	BallMc::execExtinction(ec);

	f32 scale = (_radiusPtcl * RATIO_RADIUS_TO_SCALE) + (_fcntExtinction * 0.5f);
	f32 alphaMul = calcMulAlpha();
	alphaMul *= 1.0f - _fcntExtinction / _frameExtinctionMax;

	for(int i = 0; i < _numParticle; i++)
	{
		if(_arrptclid[i] != 0)
		{
			Particle* ptcl = Game::getGame()->getParticleSys()->getParticle(_arrptclid[i]);
			ptcl->setScale(scale);				// スケール設定
			ptcl->setMulAlphaColor(alphaMul);	// パーティクルの乗算アルファ設定
		}

		alphaMul *= 0.8f;	// 徐々に薄く
	}
}

/*---------------------------------------------------------------------*//**
	パーティクルの出現
**//*---------------------------------------------------------------------*/
bool ServedMc::appearParticles(const MagicClusterCreateParam* mccprm)
{
	// パーティクルの出現
	for(int i = 0; i < _numParticle; i++)
	{
		_arrptclid[i] = Game::getGame()->getParticleSys()->appearParticle(mccprm->_ptclkind, mccprm->_pos, GameParticleDef::CF_NO_CALC_VEL | Particle::CF_EXT_COLOR);
		if(_arrptclid[i] == 0)	{	return false;	}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ユニットとの当たり
**//*---------------------------------------------------------------------*/
bool ServedMc::collUnit(const ExecCtx* ec, Unit* unit)
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

#if 1
	// 一度離れるまでは味方に当たらない（但し、ターゲット指定していない場合のみ）
	if(!TFW_IS_FLAG(_ctrlflags, CTRLF_ONLY_TRG_COLL) && !_leaveOwner && _unitOwnrRef->isFriendsSide(unit))
	{
		return false;
	}
#elif 0
	// 一度離れるまでは唱者自らに当たらない（但し、ターゲット指定していない場合）
	if(!TFW_IS_FLAG(_ctrlflags, CTRLF_ONLY_TRG_COLL) && !_leaveOwner && (unit == _unitOwnrRef))
	{
		return false;
	}
#elif 0
	// 初期数フレームは当たらない
	if(!TFW_IS_FLAG(_ctrlflags, CTRLF_ONLY_TRG_COLL))
	{
		ASSERT(_tstat != 0L);
		if(EleneStat::isNegativeOperation(_tstat->getFuncFlags()))	// 否定的な作用の場合
		{
			if(_fcntElapsed <= 5.0f)
			{
				return false;
			}
		}
		else
		{
			if(_fcntElapsed < 2.0f)
			{
				return false;
			}
		}
	}
#elif 0
	// 初期フレーム中に自らに当たった場合は除外する
	if((_unitOwnrRef == unit) && !TFW_IS_FLAG(_ctrlflags, CTRLF_ONLY_TRG_COLL))
	{
		if(_fcntElapsed <= 5.0f)
		{
			return false;
		}
	}
#elif 0
	// 初期半分のフレームに自らに当たった場合は除外する
	if((_unitOwnrRef == unit) && !TFW_IS_FLAG(_ctrlflags, CTRLF_ONLY_TRG_COLL))
//	if((TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_PC) || TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_EQUIPMENT_ITEM)) && !TFW_IS_FLAG(_ctrlflags, CTRLF_ONLY_TRG_COLL))
	{
		if(_fcntElapsed <= _frameLife / 2)
		{
			return false;
		}
	}
#elif 0
//return false;
	if((_unitOwnrRef == unit) && !TFW_IS_FLAG(_ctrlflags, CTRLF_ONLY_TRG_COLL))
	{
//TRACE("_fcntElapsed=%f\n", _fcntElapsed);
		if(_fcntElapsed <= 6.0f)
		{
			return false;
		}
	}
#endif

	// 影響を与えられないユニットを排除
	if(!unit->isCategory(Unit::UNITCAT_CHAR) && !unit->isCategory(Unit::UNITCAT_ITEM))
	{
		return false;
	}

	// 半径内に入ったかどうか
	const Vector3F* posUnit = unit->getCenterPos();
#if 1
	f32 rc = unit->getRadius() + _radius;
	if( !TFW_IS_NEAR(_pos->x(), posUnit->x(), rc) ||
		!TFW_IS_NEAR(_pos->y(), posUnit->y(), rc) ||
		!TFW_IS_NEAR(_pos->z(), posUnit->z(), rc))
	{
		return false;
	}
#elif 0	// Y 方向の判定は甘くする
	f32 rc = unit->getRadius() + _radius;
	f32 rcy = rc * 1.7f;
	if(!TFW_IS_NEAR(_pos->x(), posUnit->x(), rc) ||
		!TFW_IS_NEAR(_pos->y(), posUnit->y(), rcy) ||
		!TFW_IS_NEAR(_pos->z(), posUnit->z(), rc))
	{
		return false;
	}
#else
	f32 range = _tstat->getEleneStat()->getCql()->getEffectiveRange();
	if(	!TFW_IS_NEAR(_pos->x(), posUnit->x(), unit->getRadius() + range) ||
		!TFW_IS_NEAR(_pos->y(), posUnit->y(), unit->getRadius() + range) ||
		!TFW_IS_NEAR(_pos->z(), posUnit->z(), unit->getRadius() + range) )
	{
		return false;
	}
#endif

	// ヒット
	if(!hitUnit(ec, unit))
	{
		return false;
	}

	// 自ら消滅する
	disappearSelf(DCF_COLL_UNIT, 10.0f);

	return true;
}

/*---------------------------------------------------------------------*//**
	乗算アルファ値を算出する
**//*---------------------------------------------------------------------*/
f32 ServedMc::calcMulAlpha()
{
	f32 alphaMul = 1.0f;
	if(_frameLife >= 0)
	{
		f32 lifeframeThld = 7 * _frameLife / 8;
		if(_fcntElapsed > lifeframeThld)
		{
			alphaMul = 1.0f - ((_fcntElapsed - lifeframeThld) / (f32)(_frameLife - lifeframeThld));
		}
	}
	return alphaMul;
}

/*---------------------------------------------------------------------*//**
	パーティクルの姿勢反映
**//*---------------------------------------------------------------------*/
void ServedMc::applyParticlePosture()
{
	f32 scale = _radiusPtcl * RATIO_RADIUS_TO_SCALE;
	f32 alphaMul = calcMulAlpha();
	f32 rotz = 0.0f;
	if(TFW_IS_FLAG(_ctrlflags, CTRLF_DIR_TO_ROTZ))
	{
		Vector3F vn(*_vel);
		vn.normalize();
		if(vn.z() <= 0.0f)	// -90°～ 90°
		{
			rotz = ::atanf(vn.x() / vn.z());
		}
		else
		{
			rotz = Gcalc::adjustAngle(TFW_PI + ::atanf(vn.x() / vn.z()));
		}
	}
	for(int i = 0; i < _numParticle; i++)
	{
		if(_arrptclid[i] != 0)
		{
			Vector3F* pos = (i == 0) ? _pos : &_posAfter[i - 1];
			Particle* ptcl = Game::getGame()->getParticleSys()->getParticle(_arrptclid[i]);
			ptcl->setScale(scale);				// スケール設定
			ptcl->position()->copy(pos);		// 位置設定
			ptcl->setMulAlphaColor(alphaMul);	// パーティクルの乗算アルファ設定
			if(TFW_IS_FLAG(_ctrlflags, CTRLF_DIR_TO_ROTZ)) { ptcl->setRotateZ(rotz); }	// 方向を Z 軸回転に反映させる

			ptcl->reflectPosture(true);	// 位置反映
		}

		alphaMul *= 0.8f;	// 徐々に薄く
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
