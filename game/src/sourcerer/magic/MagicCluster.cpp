/***********************************************************************//**
 *	MagicCluster.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MagicCluster.h"

// Friends
#include "MagicSys.h"
#include "MagicInfluencer.h"
#include "../common/CalcUtils.h"
#include "../effect/EleneEffect.h"
#include "../effect/StatusChangeEffect.h"
#include "../body/GameExecCtx.h"
#include "../status/CharStat.h"
#include "../status/FocusArray.h"
#include "../status/StatusIfEx.h"
#include "../status/TransStat.h"
#include "../unit/char/CharUnit.h"
#include "../unit/item/PutItemUnit.h"
#include "../unit/item/EquipmentItemUnit.h"

// External
#include "../../etk/particle/Particle.h"
#include "../../etk/particle/ParticleSys.h"
#include "../../tfw/framemod/ExecCtx.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/lib/Vector.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MagicCluster 静的定数

const f32 MagicCluster::SPEED_MC_MIN = 0.033f;
const f32 MagicCluster::SPEED_MC_MAX = 50.0f;

//==========================================================================
// MagicCluster メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	位置を設定する
**//*---------------------------------------------------------------------*/
void MagicCluster::setPosition(const Vector3F* pos)
{
	_pos->copy(pos);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	持続フレーム数を設定する
**//*---------------------------------------------------------------------*/
void MagicCluster::setLifeFrame(f32 frame)
{
	_frameLife = frame;
	_fcntElapsed = 0.0f;
}

/*---------------------------------------------------------------------*//**
	初速度を設定する
**//*---------------------------------------------------------------------*/
void MagicCluster::setInitialVelocity(const Vector3F* vel)
{
	_vel->copy(vel);
}

/*---------------------------------------------------------------------*//**
	最大速さを設定する
**//*---------------------------------------------------------------------*/
void MagicCluster::setMaxSpeed(f32 speed)
{
	_speedMax = speed;
}

/*---------------------------------------------------------------------*//**
	加速させる
**//*---------------------------------------------------------------------*/
void MagicCluster::accelerate(const Vector3F* acc)
{
	_vel->x() += acc->x();
	_vel->y() += acc->y();
	_vel->z() += acc->z();

	f32 speedSq = _vel->lengthSq();
	if(speedSq > (_speedMax * _speedMax))
	{
		f32 speed = ::sqrtf(speedSq);
		_vel->normalize();
		*_vel *= speed;
	}
}

/*---------------------------------------------------------------------*//**
	フォーカスユニットを追加する
**//*---------------------------------------------------------------------*/
void MagicCluster::addFocusUnit(Unit* unit, bool isReset)
{
	// リセット
	if(isReset)
	{
		for(int i = 0; i < _focusarr->getMaxCount(); i++)
		{
			_focusarr->setUnit(i, 0L);
		}
	}

	// null は追加なし
	if(unit == 0L)
	{
		return;
	}

	// 重複チェック
	for(int i = 0; i < _focusarr->getMaxCount(); i++)
	{
		if(_focusarr->getUnit(i) == unit)
		{
			return;
		}
	}

	// 追加
	s32 cnt = _focusarr->getCount();
	_focusarr->setUnit(cnt, unit);
}

/*---------------------------------------------------------------------*//**
	色を設定する
**//*---------------------------------------------------------------------*/
void MagicCluster::setColor(const ColorU8* color)
{
	// 色の設定		※とりあえず、直接パーティクルに設定する 【2011/01/07】
	for(int i = 0; i < NUM_PTCL_MAX; i++)
	{
		if(_arrptclid[i] == 0)	{	break;		}
		Particle* ptcl = Game::getGame()->getParticleSys()->getParticle(_arrptclid[i]);
		if(ptcl == 0)			{	continue;	}
		ptcl->setColor(color);
	}
}

/*---------------------------------------------------------------------*//**
	位置等を反映する
**//*---------------------------------------------------------------------*/
void MagicCluster::reflectPosture()
{
}

/*---------------------------------------------------------------------*//**
	既定の元素量を設定する
**//*---------------------------------------------------------------------*/
void MagicCluster::setPresetElene(const Elene* elenePreset)
{
	if(_elenePreset == 0L)
	{
		_elenePreset = new Elene();
	}
	if(_elenePreset != 0L)
	{
		_elenePreset->copy(elenePreset);
	}
}

/*---------------------------------------------------------------------*//**
	実体化する
**//*---------------------------------------------------------------------*/
bool MagicCluster::checkHealth()
{
	// クラスタの質と機能を算出する
	_tstat->calcQualityAndFunc(true);

	// 既定の元素量が設定されている場合はフラグを上書きする
	if(_funcflagsPreset != 0)
	{
		// ただし、元素過少や元素過大は認めない
		u32 funcOp = _tstat->getFuncFlags() & EleneStat::FF_MASK_OP;
		if((funcOp != EleneStat::FF_OP_IMMATURE) && (funcOp != EleneStat::FF_OP_OVERBREAK))
		{
			u32 flagsFuncNew = (EleneStat::FF_MASK_EFF & _tstat->getFuncFlags()) | (_funcflagsPreset & (EleneStat::FF_MASK_OP | EleneStat::FF_MASK_ATRB | EleneStat::FF_MASK_FORM));
			if(!_tstat->overrideFuncFlags(flagsFuncNew)) { return false; }
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	実体化する
**//*---------------------------------------------------------------------*/
bool MagicCluster::materialize(u32 form)
{
	return MagicInfluencer::materialize(form, _mcid);
}

/*---------------------------------------------------------------------*//**
	合成する
**//*---------------------------------------------------------------------*/
bool MagicCluster::synthesize(MagicCluster* mcMul)
{
	return MagicInfluencer::synthesize(this->_mcid, mcMul->_mcid, this->getDissolvedUnitCount());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MagicCluster::MagicCluster()
	: _mcid(0)
	, _confflags(0)
	, _ctrlflags(0)
	, _dcflags(0)
	, _cntDissolvedUnit(0)
	, _pos(0L)
	, _vel(0L)
	, _speedMax(SPEED_MC_MAX)
	, _fcntElapsed(0.0f)
	, _frameLife(0.0f)
	, _fcntExtinction(0.0f)
	, _frameExtinctionMax(0.0f)
	, _tstat(0L)
	, _elenePreset(0L)
	, _funcflagsPreset(0)
	, _focusarr(0L)
	, _unitOwnrRef(0L)
{
	for(int i = 0; i < NUM_PTCL_MAX; i++)	{	_arrptclid[i] = 0;	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MagicCluster::~MagicCluster()
{
	ASSERT(_pos == 0L);
	ASSERT(_tstat == 0L);
	ASSERT(_elenePreset == 0L);
	ASSERT(_focusarr == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MagicCluster::create(MagicSys* mgcsysOwnRef, u16 mcid, const MagicClusterCreateParam* mccprm)
{
	// 参照を保存する
	_unitOwnrRef = mccprm->_unitOwnrRef;

	// 生存フレーム
	setLifeFrame((f32)mccprm->_lifeframe);

	// パーティクルを出す
	appearParticles(mccprm);
	Particle* ptclFirst = (_arrptclid[0] != 0) ? Game::getGame()->getParticleSys()->getParticle(_arrptclid[0]) : 0L;

	// 位置・速度を自分で管理するかどうか
	_confflags = 0;
	TFW_SET_FLAG(_confflags, CONFF_HAS_POS_VEL, mccprm->_isNewPosVel);
	if(TFW_IS_FLAG(_confflags, CONFF_HAS_POS_VEL) || (ptclFirst == 0L))
	{
		// 位置・速度ベクトルを作成する
		_pos = new Vector3F();
		_vel = new Vector3F();
	}
	else
	{
		// パーティクルから取得する
		_pos = ptclFirst->position();
		_vel = ptclFirst->velocity();
	}
	if(mccprm->_pos != 0L)	{	_pos->copy(mccprm->_pos);	}

	// 伝達ステータスを作成する
	_tstat = new TransStat(mccprm->_tstat);

	// 既定の元素量を設定する
	if (mccprm->_elenePreset != 0L)
	{
		setPresetElene(mccprm->_elenePreset);
	}
	// 既定の機能フラグを設定する
	_funcflagsPreset = mccprm->_funcflagsPreset;

	// フォーカス配列を作成する
	if(mccprm->_isShowFocusCursor)
	{
		TFW_SET_FLAG(_confflags, CONFF_SHOW_FC, true);
		_focusarr = new FocusArray(NUM_FOCUS_MAX, true);
	}
	else
	{
		_focusarr = new FocusArray(NUM_FOCUS_MAX, false);
	}

	// デフォルトのフォーカス
	_focusarr->setUnit(0, mccprm->_unitMainTrgRef);

	// 有効化
	TFW_SET_FLAG(_ctrlflags, CTRLF_VALID, true);

	// ID の設定
	_mcid = mcid;
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MagicCluster::destroy()
{
	_mcid = 0;

	// フォーカス配列を削除する
	delete _focusarr;
	_focusarr = 0L;

	// 既定の元素量を削除する
	delete _elenePreset;
	_elenePreset = 0L;

	// 伝達ステータスを削除する
	delete _tstat;
	_tstat = 0L;
	// 位置・速度ベクトルを削除する
	if(TFW_IS_FLAG(_confflags, CONFF_HAS_POS_VEL))
	{
		delete _vel;
		delete _pos;
	}
	_vel = 0L;
	_pos = 0L;

	// パーティクルを消す
	for(int i = 0; i < NUM_PTCL_MAX; i++)
	{
		if(_arrptclid[i] == 0)	{	break;	}
		Game::getGame()->getParticleSys()->disappearParticle(_arrptclid[i]);
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void MagicCluster::exec(const ExecCtx* ec)
{
	if(!isValid())
	{
		if(TFW_IS_FLAG(_ctrlflags, CTRLF_EXTINCTION))
		{
			execExtinction(ec);
		}
		return;
	}

	///if(!((GameExecCtx*)ec)->isLockOperating())	// 操作ロック（イベント等）以外
	{
		#if 0
			// 自然減衰
			if((_unitOwnrRef != 0L) && _unitOwnrRef->isCategory(Unit::UNITCAT_CHAR))
			{
				Elene* eeCql = _tstat->eleneStat()->cql();
				const Elene* eeNql = _tstat->getEleneStat()->getNql();
				f32 rc = (f32)(CharParam::VALUE_MAX - ((CharUnit*)_unitOwnrRef)->getCharStat()->getCharParam()->getUnity());
				bool isUpdate = false;
				for(int i = 0; i < Elene::NUM_EL; i++)
				{
					s32 en = eeCql->get(i);
					if(en <= 0)		{	continue;	}
					f32 re = en / (f32)Elene::ELEN_MAX;
					re *= re;								// 放物線を描く（大きい元素量ほど割合が大きい）
					re *= rc;								// 減衰量
					re *= (1.0f + i * 0.1f);				// 上位元素ほど減衰しやすい
					if(eeNql->get(i) <= en)
					{
						re *= 0.1f;							// 原質値より大きければ安定する
					}
					if(re <= 0.0f)	{	continue;	}
					en -= (s32)re;
					if(en < 0)	{	en = 0;	}
					eeCql->set(i, en);
					isUpdate = true;
					STRACE("[%d] %d, (re=%f, rc=%f)\n", i, en, re, rc);
				}
				if(isUpdate)
				{
					_tstat->eleneStat()->updateResultValue();
				}
			}
		#endif

		// 生存時間による自動消滅
		if(_frameLife >= 0)
		{
			if(_fcntElapsed > _frameLife)
			{
				disappearSelf(DCF_ZERO_LIFE, 0.0f);	// 消滅する
			}
		}

		// フォーカスの有効性チェック
		_focusarr->updateValidity();

		// エネルギー状態更新
		ASSERT(_tstat != 0L);
		_tstat->exec(ec);

		// フレーム更新
		_fcntElapsed += ec->getDeltaFrame();
	}
}

/*---------------------------------------------------------------------*//**
	消滅時フレーム制御
**//*---------------------------------------------------------------------*/
void MagicCluster::execExtinction(const ExecCtx* ec)
{
	_fcntExtinction += ec->getDeltaFrame();
	if(_fcntExtinction > _frameExtinctionMax)
	{
		_fcntExtinction = _frameExtinctionMax;
		TFW_SET_FLAG(_ctrlflags, CTRLF_EXTINCTION, false);
	}
}

/*---------------------------------------------------------------------*//**
	ユニットにヒットした
**//*---------------------------------------------------------------------*/
bool MagicCluster::hitUnit(const ExecCtx* ec, Unit* unit, const TransStat* tstat)
{
	bool res = false;

	// 元素エフェクト
	EleneEffect::on(tstat, unit->getCenterPos());

	// ステータス影響エフェクト
#if 0
	StatusChangeResult scres;
#endif
	if(unit->isCategory(Unit::UNITCAT_CHAR))
	{
		CharUnit* cunit = (CharUnit*)unit;
		CharStat* cstat = cunit->charStat(); ASSERT(cstat != 0L);
#if 1
		res = StatusIfEx::influence(cstat, tstat, true, true);
#else
		scres.setParties(cunit, this);
		StatusInfluencer::influence(&scres, cstat, tstat);
		StatusChangeEffect::on(&scres);
#endif

		// ★★★トロフィー処理挿入場所候補★★★
	}
	else if(unit->getUnitType() == UNITTYPE_EQUIPMENT_ITEM)
	{
		EquipmentItemUnit* iunit = (EquipmentItemUnit*)unit;
		CharStat* cstat = iunit->charStat(); ASSERT(cstat != 0L);
#if 1
		res = StatusIfEx::influence(cstat, tstat, true, true);
#else
		scres.setParties(iunit, this);
		StatusInfluencer::influence(&scres, cstat, tstat);
		StatusChangeEffect::on(&scres);
#endif
	}
	else if(unit->getUnitType() == UNITTYPE_PUT_ITEM)
	{
		PutItemUnit* diunit = (PutItemUnit*)unit;
		CharStat* cstat = diunit->charStat(); ASSERT(cstat != 0L);
#if 1
		res = StatusIfEx::influence(cstat, tstat, true, true);
#else
		scres.setParties(diunit, this);
		StatusInfluencer::influence(&scres, cstat, tstat);
		StatusChangeEffect::on(&scres);
#endif
	}

#if 1
	return res;
#else
	if(scres.isValid())
	{
		MagicInfluencer::procResult(&scres);
		return true;
	}

	return false;
#endif
}

/*---------------------------------------------------------------------*//**
	自律的な消滅
**//*---------------------------------------------------------------------*/
void MagicCluster::disappearSelf(u16 dcflags, f32 frameExtinction)
{
	TFW_SET_FLAG(_ctrlflags, CTRLF_VALID, false);
	TFW_SET_FLAG(_dcflags, dcflags, true);

	_fcntExtinction = 0.0f;
	_frameExtinctionMax = frameExtinction;
	if(_frameExtinctionMax > 0.0f)
	{
		TFW_SET_FLAG(_ctrlflags, CTRLF_EXTINCTION, true);
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
