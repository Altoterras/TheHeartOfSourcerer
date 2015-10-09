/***********************************************************************//**
 *  MagicClusterCreateParam.h
 *  Enlight Game Application
 *
 *  Created by Ryoutarou Kishi on 2009/12/18.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAGIC_MAGIC_CLUSTER_CREATE_PARAM_H_
#define _SRCR_MAGIC_MAGIC_CLUSTER_CREATE_PARAM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "MagicClusterType.h"
#include "../particle/GameParticleDef.h"
#include "../status/TransStat.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;

TFW_END_NS

SRCR_BEGIN_NS

class Unit;

/*---------------------------------------------------------------------*//**
 *	クラスター 作成パラメータ基底クラス
 *	
**//*---------------------------------------------------------------------*/
class MagicClusterCreateParam
{
	//======================================================================
	// メソッド
public:
	MagicClusterCreateParam(MagicClusterType::McType mctype, GameParticleDef::ParticleKind ptclkind)
		: _mctype(mctype)
		, _ptclkind(ptclkind)
		, _unitOwnrRef(0L)
		, _unitMainTrgRef(0L)
		, _elenePreset(0L)
		, _funcflagsPreset(0)
		, _pos(0L)
		, _radius(0.0f)
		, _radiusVel(0.0f)
		, _lifeframe(-1)
		, _numParticle(1)
		, _isNewPosVel(false)
		, _isShowFocusCursor(false)
		, _isNoOnlyTrgColl(false)
	{}

	//======================================================================
	// 変数
public:
	MagicClusterType::McType		_mctype;			// クラスター タイプ
	GameParticleDef::ParticleKind	_ptclkind;			// パーティクルの種類
	Unit*							_unitOwnrRef;		// オーナー ユニット
	Unit*							_unitMainTrgRef;	// ターゲット ユニット
	TransStat						_tstat;				// 転送ステータス
	const Elene*					_elenePreset;		// 既定の元素エネルギー量
	u32								_funcflagsPreset;	// 既定の機能フラグ
	const Vector3F*					_pos;				// 位置
	f32								_radius;			// 半径
	f32								_radiusVel;			// 半径増減速度
	s32								_lifeframe;			// 生存フレーム
	s32								_numParticle;		// 使用パーティクル数
	bool							_isNewPosVel;		// 位置・速度ベクトルを新規に持つ
	bool							_isShowFocusCursor;	// フォーカスカーソルを表示するかどうか
	bool							_isNoOnlyTrgColl;	// 対象のみに当たるのを無効化する
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAGIC_MAGIC_CLUSTER_CREATE_PARAM_H_
