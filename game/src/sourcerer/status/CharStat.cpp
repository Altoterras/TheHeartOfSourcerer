/***********************************************************************//**
 *	CharStat.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/18.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "CharStat.h"

// Friends
#include "EquipStat.h"
#include "TransStat.h"
#include "../common/CalcUtils.h"
#include "../episode/save/SaveStructure.h"
#include "../body/GameExecCtx.h"
#include "../item/Item.h"
#include "../souma/Souma.h"
#include "../spirit/Spirit.h"
#include "../unit/char/CharUnit.h"

// External
#include "../../tfw/serialize/Serializer.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// CharStat メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	アクションフレーム数を得る
**//*---------------------------------------------------------------------*/
f32 CharStat::getActFrame() const
{
	f32 af = 150.0f;	//【2012/03/31 r-kishi】Ver1.3.1: 188.0f
	af -= _chrprm.getActivity() * 0.01f;
	af -= (_eestat->getNql()->getWind() / (f32)Elene::ELEN_MAX) * 60.0f;
	if(af < 60.0f)	{	af = 60.0f;	}
	return af;
}

/*---------------------------------------------------------------------*//**
	重さ（g）を得る
**//*---------------------------------------------------------------------*/
s32 CharStat::getWeight() const
{
	return _eestat->getCql()->getWeight();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	瞑想中
**//*---------------------------------------------------------------------*/
void CharStat::dianaFrame()
{
	_flagsFrameNext |= FF_DIANA;
}

/*---------------------------------------------------------------------*//**
	詠唱中
**//*---------------------------------------------------------------------*/
void CharStat::calmingFrame()
{
	_flagsFrameNext |= FF_CALMING;
}

/*---------------------------------------------------------------------*//**
	走り中
**//*---------------------------------------------------------------------*/
void CharStat::runningFrame()
{
	_flagsFrameNext |= FF_RUNNING;
}

/*---------------------------------------------------------------------*//**
	防御中
**//*---------------------------------------------------------------------*/
void CharStat::guardingFrame()
{
	_flagsFrameNext |= FF_GUARDING;
}

/*---------------------------------------------------------------------*//**
	エネルギーダメージ
**//*---------------------------------------------------------------------*/
s32 CharStat::damageEnergy(s32 damageAdd, bool autoConcDepthDamage)
{
	// 攻撃無効の場合は抜ける
	if(TFW_IS_FLAG(_flagsCreate, CF_IGNORE_ATTACK))	{	return 0;	}

	if(damageAdd <= 0)	{	return 0;	}

	// マイナス値は回復
	if(damageAdd < 0)
	{
		return recoverEnergy(- damageAdd);
	}

	// 値のクリップ
	s32 damageEneNew = _damageEne + damageAdd;
	if(damageEneNew > (_lfeneMax - _damagePhys))	{	damageAdd = _lfeneMax - _damagePhys - _damageEne;	}

	// 新しいエネルギーダメージ値
	_damageEne += damageAdd;

	// 集中力低下
	if(autoConcDepthDamage && (_depthConc > 0))
	{
		s32 depthDamageConc = damageAdd * RATIO_LFENE_TO_CONCD_DEFAULT;
		if(depthDamageConc > 0)
		{
			// 低下防止
			s32 prevention = _chrprm.getUnity() * (8192 + _chrprm.getStability() * 32);
			depthDamageConc -= prevention;

			if(depthDamageConc > 0)
			{
				damageConcentrateDepth(depthDamageConc);
			}
		}
	}

	// 状態チェック
	checkStat();

	return - damageAdd;
}

/*---------------------------------------------------------------------*//**
	物理ダメージ
**//*---------------------------------------------------------------------*/
s32 CharStat::damagePhysics(s32 damageAdd)
{
	// 攻撃無効の場合は抜ける
	if(TFW_IS_FLAG(_flagsCreate, CF_IGNORE_ATTACK))	{	return 0;	}

	if(damageAdd <= 0)	{	return 0;	}

	// マイナス値は回復
	if(damageAdd < 0)
	{
		return recoverPhysics(- damageAdd);
	}

	// 値のクリップ
	s32 damagePhysNew = _damagePhys + damageAdd;
	if(damagePhysNew > _lfeneMax)	{	damageAdd = _lfeneMax - _damagePhys;	}

	// 新しい物理ダメージ値
	_damagePhys += damageAdd;

	// エネルギーダメージ値の補正
	if(_damageEne > (_lfeneMax - _damagePhys))
	{
		_damageEne = _lfeneMax - _damagePhys;
	}

	// 集中力低下
	if(_depthConc > 0)
	{
		s32 depthDamageConc = (s32)(damageAdd * calcLfDamageToConcdDamageRatio());
		if(depthDamageConc > 0)
		{
			// 低下防止
			s32 prevention = _chrprm.getUnity() * (1024 + _chrprm.getStability() * 32);
			depthDamageConc -= prevention;

			if(depthDamageConc > 0)
			{
				damageConcentrateDepth(depthDamageConc);
			}
		}
	}

	// 状態チェック
	checkStat();

	return - damageAdd;
}

/*---------------------------------------------------------------------*//**
	意識深さに対するダメージ（集中力低下）
**//*---------------------------------------------------------------------*/
void CharStat::damageConcentrateDepth(s32 depthDamageConc)
{
	if(_depthConc <= 0)			{	return;	}
	if(depthDamageConc <= 0)	{	return;	}

	// 低下処理
	if(depthDamageConc > 0)
	{
		if(_depthConc < depthDamageConc)	{	depthDamageConc = _depthConc;	}
		s32 depthConsNew = _depthConc - depthDamageConc;

		// 半覚醒状態では、集中度が半分より下に下がらない
		if(TFW_IS_FLAG(_flagsStat, SPSTAT_TRANS))
		{
			if(depthConsNew < CONCD_HENL_MIN)
			{
				depthConsNew = CONCD_HENL_MIN;
			}
		}

		_depthConc = depthConsNew;
	}
}

/*---------------------------------------------------------------------*//**
	生命エネルギーダメージから意識集中度ダメージ率計算
**//*---------------------------------------------------------------------*/
f32 CharStat::calcLfDamageToConcdDamageRatio() const
{
	f32 t = 1.0f - ((_chrprm.getUnity() - 100) * TFW_PI * 0.00005f);
	f32 k = t * t * t * t;
	if(t > 0)
	{
		k = (k * 9964.0f) + 36.0f;
	}
	else
	{
		k = (t * 80.0f) + 36.0f;
	}
	if(k < 8.0f)
	{
		k = 8.0f;
	}
	return k;

#if 0
	// 生命エネルギーダメージ → 意識集中度ダメージ率
	s32 ratioLfToConcd = CharStat::RATIO_LFENE_TO_CONCD_DEFAULT;
	ratioLfToConcd -= _chrprm.getUnity() << 3;	// ×8
	if(ratioLfToConcd < CharStat::RATIO_LFENE_TO_CONCD_MIN)
	{
		ratioLfToConcd = CharStat::RATIO_LFENE_TO_CONCD_MIN;
	}
	return ratioLfToConcd;
#endif
}

/*---------------------------------------------------------------------*//**
	エネルギー回復
**//*---------------------------------------------------------------------*/
s32 CharStat::recoverEnergy(s32 recvAdd)
{
	// マイナス値はダメージ
	if(recvAdd < 0)
	{
		return damageEnergy(- recvAdd);
	}

	// 値のクリップ
	s32 damageEneNew = _damageEne - recvAdd;
	if(damageEneNew < 0)	{	recvAdd = _damageEne;	}

	// 新しいエネルギーダメージ値
	_damageEne -= recvAdd;

	// 状態チェック
	checkStat();

	return recvAdd;
}

/*---------------------------------------------------------------------*//**
	物理ダメージ回復
**//*---------------------------------------------------------------------*/
s32 CharStat::recoverPhysics(s32 recvAdd)
{
	// マイナス値はダメージ
	if(recvAdd < 0)
	{
		return damagePhysics(- recvAdd);
	}

	// 値のクリップ
	s32 damagePhysNew = _damagePhys - recvAdd;
	if(damagePhysNew < 0)	{	recvAdd = _damagePhys;	}

	// 新しい物理ダメージ値
	_damagePhys -= recvAdd;

	// エネルギーダメージ値の補正
	if(_damageEne > (_lfeneMax - _damagePhys))
	{
		_damageEne = _lfeneMax - _damagePhys;
	}

	// 状態チェック
	checkStat();

	return recvAdd;
}

/*---------------------------------------------------------------------*//**
	蘇生
**//*---------------------------------------------------------------------*/
void CharStat::revive(bool isBarely)
{
	if(getStat() == STAT_ALIVE)	{	return;	}

	if(isBarely)	// 最小限（かろうじて蘇生）
	{
		if(_damagePhys >= _lfeneMax)
		{
			_damagePhys = _lfeneMax - 1;
		}
		if(_damageEne >= (_lfeneMax - _damagePhys))
		{
			_damageEne = _lfeneMax - _damagePhys - 1;
		}
	}
	else			// 全快
	{
		_damagePhys = 0;
		_damageEne = 0;
	}

	setStat(STAT_ALIVE);
}

/*---------------------------------------------------------------------*//**
	全快
**//*---------------------------------------------------------------------*/
void CharStat::recoverFull()
{
	// ダメージ回復
	_damagePhys = 0;
	_damageEne = 0;

	// 強制蘇生
	setStat(STAT_ALIVE);

	// 元素値リセット
	_eestat->reset();

	// 状態チェック
	checkStat();
}

/*---------------------------------------------------------------------*//**
	原質崩れ（ヴィクリティ）の自然回復処理
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
void CharStat::naturation(s32 eerecv, bool isNoDamane)
{
	bool isPure = true;
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		if(eerecv <= 0)	{	break;	}

		// 余剰量（ここでは根本原質と比較する）
		s32 badEl = _eestat->getCql()->get(i) - _eestat->getOql()->get(i);
		if(badEl <= 0)	{	continue;	}
		isPure = false;

		// 回復させる量
		s32 recvEl = TFW_MIN(eerecv, badEl);

		// 回復させるのに必要な体力エネルギー
		if(!isNoDamane)
		{
			s32 dmgEl = recvEl / 4;
			s32 eneRs = getEnergy();
			if(dmgEl >= (eneRs - 1))
			{
				dmgEl = eneRs - 1;
				recvEl = dmgEl * 4;
			}

			// エネルギーダメージ
			if(dmgEl > 0)
			{
				damageEnergy(dmgEl);
			}
		}

		// 原質に近づいていく
		if(recvEl > 0)
		{
			_eestat->cql()->sub(i, recvEl);
		}

		eerecv -= recvEl;
	}

	// 純性の場合、エネルギー回復
	if(isPure && (eerecv > 0))
	{
		recoverEnergy(eerecv / 64);
	}

	// 最終値更新
	_eestat->updateResultValue(false);

	// 状態チェック
	checkStat();
}

/*---------------------------------------------------------------------*//**
	結界限界値を算出する
**//*---------------------------------------------------------------------*/
void CharStat::calcVeilLimitElene(Elene* ee, s32 iveil)
{
	// 強化値の最大値（限界値）を得る
	ee->copy(_eestat->getNql());
	ee->scale((iveil == EleneStat::VEIL_BR) ? EleneStat::RATIO_EF_VEIL_MAX : EleneStat::RATIO_WP_VEIL_MAX);

	// 装備アイテムの場合は持ち主の元素量も含めることができる
	if(TFW_IS_FLAG(getCreateFlags(), CharStat::CF_ITEM))
	{
		if(getOwnerUnit() != 0L)
		{
			Unit* unitOwner = getOwnerUnit()->getOwnerUnit();
			if(unitOwner != 0L)
			{
				const Elene* eestatOwnerNql = unitOwner->getEleneStat()->getNql();
				ee->addSoil(eestatOwnerNql->getSoil() >> 3);		// >> 3 → ÷ 8
				ee->addWater(eestatOwnerNql->getWater() >> 3);
				ee->addFire(eestatOwnerNql->getFire() >> 3);
				ee->addWind(eestatOwnerNql->getWind() >> 3);
				ee->addAethel(eestatOwnerNql->getAethel() >> 3);
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	結界元素量を限界値でクリップする
**//*---------------------------------------------------------------------*/
void CharStat::clipVeilLimitElene(s32 iveil)
{
	Elene eeLimit;
	calcVeilLimitElene(&eeLimit, iveil);
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		if(_eestat->getVeil(iveil)->getElene()->get(i) > eeLimit.get(i))
		{
			_eestat->veil(iveil)->elene()->set(i, eeLimit.get(i));
		}
	}
}

/*---------------------------------------------------------------------*//**
	結界強化
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
bool CharStat::strengthenVeil(const Elene* input, s32 iveil, Unit* unitMaster)
{
	if((iveil < EleneStat::VEIL_WP) || (EleneStat::VEIL_BR < iveil))	{ return false; }

	// 瞬間結界の場合、別のマスターからの以前の瞬間結界はクリアする
	if(iveil == EleneStat::VEIL_BR)
	{
		if(_unitEfldMaster != unitMaster)
		{
			_eestat->veil(EleneStat::VEIL_BR)->clear();
		}
		_unitEfldMaster = unitMaster;
	}

	// 結界強化
	_eestat->veil(iveil)->strengthen(input);
	_eestat->updateResultValue(false);

	return true;
}

/*---------------------------------------------------------------------*//**
	結界弱体
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
bool CharStat::weakenVeil(const Elene* input, s32 iveil)
{
	if((iveil < EleneStat::VEIL_WP) || (EleneStat::VEIL_BR < iveil))	{ return false; }

	// 結界弱体
	_eestat->veil(iveil)->weaken(input);
	_eestat->updateResultValue(false);

	return true;
}

/*---------------------------------------------------------------------*//**
	結界リセット
**//*---------------------------------------------------------------------*/
void CharStat::resetVeil()
{
	for(int iveil = 0; iveil < EleneStat::NUM_VEIL; iveil++)
	{
		_eestat->veil(iveil)->clear();
	}
}

/*---------------------------------------------------------------------*//**
	チャージ
**//*---------------------------------------------------------------------*/
void CharStat::charge(u16 add)
{
	s32 rateChargeNew = (s32)_cntCharge + (s32)add;
	if(rateChargeNew > CHARGECNT_MAX)
	{
		_cntCharge = CHARGECNT_MAX;
	}
	else
	{
		_cntCharge = (u16)rateChargeNew;
	}
}

/*---------------------------------------------------------------------*//**
	チャージのリセット
**//*---------------------------------------------------------------------*/
void CharStat::resetCharge()
{
	_cntCharge = 0;
}

/*---------------------------------------------------------------------*//**
	意識深度のリセット
**//*---------------------------------------------------------------------*/
void CharStat::resetConcentrate()
{
	_depthConc = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
CharStat::CharStat(bool isNewEleneStat)
	: _flagsCreate(0)
	, _eestat(0L)
{
	if(isNewEleneStat)
	{
		TFW_SET_FLAG(_flagsCreate, CF_NEW_EESTAT, true);
		_eestat = new EleneStat();
	}

	clear();
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
CharStat::~CharStat()
{
	if(TFW_IS_FLAG(_flagsCreate, CF_NEW_EESTAT))
	{
		delete _eestat;
		_eestat = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void CharStat::copy(const CharStat* src)
{
	_flagsStat = src->_flagsStat;
	_flagsFrame = src->_flagsFrame;
	_flagsFrameNext = src->_flagsFrameNext;
	_flagsCreate = (_flagsCreate & CF_NEW_EESTAT) | (src->_flagsCreate & ~CF_NEW_EESTAT);	// CF_NEW_EESTAT 以外をコピー
	_lfeneMax = src->_lfeneMax;
	_damageEne = src->_damageEne;
	_damagePhys = src->_damagePhys;
	_depthConc = src->_depthConc;
	_depthConcDisp = src->_depthConcDisp;
	_lv = src->_lv;
	_experience = src->_experience;
	_fcntPeriodic = src->_fcntPeriodic;
	_eeqf = src->_eeqf;
	_rateExct = src->_rateExct;
	_rateDull = src->_rateDull;
	_cntCharge = src->_cntCharge;
	_cntChargeDisp = src->_cntChargeDisp;
	_chrprm.copy(&src->_chrprm);
	_eqstatRef = src->_eqstatRef;
	_unitSelfRef = src->_unitSelfRef;
	_unitEfldMaster = src->_unitEfldMaster;

	if(TFW_IS_FLAG(_flagsCreate, CF_NEW_EESTAT))
	{
		_eestat->copy(src->_eestat);
	}
	else
	{
		_eestat = src->_eestat;
	}
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void CharStat::clear()
{
	_flagsStat = 0;
	_flagsFrame = 0;
	_flagsFrameNext = 0;
	_lfeneMax = 0;
	_damageEne = 0;
	_damagePhys = 0;
	_depthConc = 0;
	_depthConcDisp = 0;
	_lv = 1;
	_experience = 0;
	_fcntPeriodic = 0.0f;
	_eeqf = 0;
	_rateExct = 0;
	_rateDull = 0;
	_cntCharge = 0;
	_cntChargeDisp = 0;
	_chrprm.clear();
	_eqstatRef = 0L;
	_unitSelfRef = 0L;
	_unitEfldMaster = 0L;

	if(TFW_IS_FLAG(_flagsCreate, CF_NEW_EESTAT))
	{
		_eestat->clear();
	}
	else
	{
		_eestat = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	初期設定
**//*---------------------------------------------------------------------*/
void CharStat::setup(const CharParam* chrprm, u8 flagsCreate, bool isPreserveBinding)
{
	EleneStat* eestatRefBk = _eestat;
	EquipStat* eqstatRefBk = _eqstatRef;
	Unit* unitSelfRefBk = _unitSelfRef;

	clear();

	if(isPreserveBinding)
	{
		_eestat = eestatRefBk;
		_eqstatRef = eqstatRefBk;
		_unitSelfRef = unitSelfRefBk;
	}

	_chrprm.copy(chrprm);
	_lfeneMax = _chrprm.getEnergy();

	TFW_SET_FLAG(_flagsCreate, flagsCreate, true);

	setStat(STAT_ALIVE);
}

/*---------------------------------------------------------------------*//**
	初期設定
**//*---------------------------------------------------------------------*/
void CharStat::setup(u8 flagsCreate, bool isPreserveBinding)
{
	EleneStat* eestatRefBk = _eestat;
	EquipStat* eqstatRefBk = _eqstatRef;
	Unit* unitSelfRefBk = _unitSelfRef;

	clear();

	if(isPreserveBinding)
	{
		_eestat = eestatRefBk;
		_eqstatRef = eqstatRefBk;
		_unitSelfRef = unitSelfRefBk;
	}

	TFW_SET_FLAG(_flagsCreate, flagsCreate, true);

	setStat(STAT_ALIVE);
}

/*---------------------------------------------------------------------*//**
	レベルアップ
**//*---------------------------------------------------------------------*/
void CharStat::levelup(const CharParam* chrprmNext)
{
	_chrprm.levelup(chrprmNext);
	_lfeneMax = _chrprm.getEnergy();
}

/*---------------------------------------------------------------------*//**
	エネルギー状態を関連付ける
**//*---------------------------------------------------------------------*/
void CharStat::bind(Unit* unitSelfRef, EleneStat* eestatRef, EquipStat* eqstatRef)
{
	_unitSelfRef = unitSelfRef;
	_eqstatRef = eqstatRef;	// こちらは NULL もありうる（特に敵とか）

	if(TFW_IS_FLAG(_flagsCreate, CF_NEW_EESTAT))
	{
		_eestat->copy(eestatRef);
	}
	else
	{
		_eestat = eestatRef;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void CharStat::exec(const ExecCtx* ec)
{
	// フレームフラグを更新
	_flagsFrame = _flagsFrameNext;
	_flagsFrameNext = 0;

	// 自動ステータス影響更新
	updateAutoInfluence(ec->getDeltaFrame());

	// 意識深度更新
	updateConcentrateDepth(ec->getDeltaFrame());
	///STRACE("c:%d\n", _depthConc);

	// エネルギー状態更新
	_eestat->exec(ec);

	// 間引き処理 - 10 フレームに１回処理する
	_fcntPeriodic += ec->getDeltaFrame();
	if(_fcntPeriodic >= 10.0f)
	{
		_fcntPeriodic -= 10.0f;
		TFW_SET_FLAG(_flagsFrameNext, FF_PERIODIC_EXEC, true);	// 次のフレームで多くの処理が実行される
	}

	// 瞬間結界の終了処理
	if(_unitEfldMaster != 0L)
	{
		bool isClearEfld = !_unitEfldMaster->isEnable();
		if(!isClearEfld && _unitEfldMaster->isCategory(Unit::UNITCAT_CHAR))
		{
			const CharStat* cstatMaster = ((CharUnit*)_unitEfldMaster)->getCharStat();
			const Spirit* sprtMaster = ((CharUnit*)_unitEfldMaster)->getSpirit();
			const Souma* soumaMaster = sprtMaster->getSouma();
			isClearEfld =
				(soumaMaster == 0L)
				|| ((soumaMaster != 0L) && !soumaMaster->isRunning())	// ソウマを詠唱していれば意識集中度が低くても良いように緩和【2012/03/29】 || ((soumaMaster != 0L) && (!soumaMaster->isRunning() || (cstatMaster->getConcentrateDepth() < CONCD_THRSLD_TRANCE)))
				|| (cstatMaster->getStat() != CharStat::STAT_ALIVE)
				|| TFW_IS_FLAG(cstatMaster->getSpStatFlags(), CharStat::SPSTAT_SLEEP);
		}
		if(isClearEfld)
		{
			_eestat->veil(EleneStat::VEIL_BR)->clear();
			_unitEfldMaster = 0L;
		}
	}
}


/*---------------------------------------------------------------------*//**
	シリアライズ
**//*---------------------------------------------------------------------*/
bool CharStat::serialize(Serializer* ser) const
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!storeToStructure(&svst)) { return false; }
		if(!ser->store(&svst, sizeof(SaveStructure))) { return false; }
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	デシリアイズ
**//*---------------------------------------------------------------------*/
bool CharStat::deserialize(Serializer* ser)
{
	if(ser->isStructureMode())	// ストラクチャモード
	{
		ASSERT(false);
		return false;
	}
	else						// ダイレクトモード
	{
		SaveStructure svst;
		if(!ser->restore(&svst, sizeof(SaveStructure))) { return false; }
		if(!restoreFromStructure(&svst))				{ return false; }

		// バージョン間互換性調整
		if(	(ser->getMajorVersionNumber() <= sourcerer::SaveStructure::VER_MAJOR_SAVE_STRUCTURE)
			&& (ser->getMinorVersionNumber() <= sourcerer::SaveStructure::VER_MINOR_SAVE_STRUCTURE_VER131)	)
		{
			// Ver1.3.1 依然とは意識深度の尺度が違う
			_depthConc *= 2;
		}

		return true;
	}
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャに保存する
**//*---------------------------------------------------------------------*/
bool CharStat::storeToStructure(SaveStructure* svst) const
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	svst->_flagsStat = _flagsStat;
	svst->_flagsFrame = _flagsFrame;
	svst->_flagsCreate = _flagsCreate;
	svst->_lfeneMax = _lfeneMax;
	svst->_damageEne = _damageEne;
	svst->_damagePhys = _damagePhys;
	svst->_depthConc = _depthConc;
	svst->_fcntPeriodic = _fcntPeriodic;
	svst->_cntCharge = _cntCharge;
	svst->_lv = _lv;
	svst->_experience = _experience;
	TFW_COPY_MEMORY(&svst->_chrprm, &_chrprm, sizeof(CharParam));
	_eestat->storeToStructure(&svst->_eessvst);

	return true;
}

/*---------------------------------------------------------------------*//**
	セーブストラクチャから読み込む
**//*---------------------------------------------------------------------*/
bool CharStat::restoreFromStructure(const SaveStructure* svst)
{
	ASSERT(sizeof(SaveStructure) == SaveStructure::SIZE_THIS);

	_flagsStat = svst->_flagsStat;
	_flagsFrame = svst->_flagsFrame;
	_flagsCreate = svst->_flagsCreate;
	_lfeneMax = svst->_lfeneMax;
	_damageEne = svst->_damageEne;
	_damagePhys = svst->_damagePhys;
	_depthConcDisp = _depthConc = svst->_depthConc;
	_fcntPeriodic = svst->_fcntPeriodic;
	_cntChargeDisp = _cntCharge = svst->_cntCharge;
	_lv = svst->_lv;
	_experience = svst->_experience;
	TFW_COPY_MEMORY(&_chrprm, &svst->_chrprm, sizeof(CharParam));
	_eestat->restoreFromStructure(&svst->_eessvst);

	// 元素バランスの正規化
	#if _ENLIGHT_V01_3_ABOVE	// Ver1.3 以上
		_eestat->normalize();
	#endif

	// 各ボリュームは既定経験値まで
	#if defined(_ENLIGHT_V01)
		if(_chrprm.getExperience() >= CharParam::EXP_MAX_VOL01)	{	_chrprm.setExperience(CharParam::EXP_MAX_VOL01);	}
	#endif

	// 現在の質の計算
	_eeqf = _eestat->calcQualityFlags(0L, &_rateExct, &_rateDull);

	// 瞬間結界マスターへの参照は現在保存しない仕様である
	_unitEfldMaster = 0L;
	_eestat->veil(EleneStat::VEIL_BR)->clear();

	return true;
}

/*---------------------------------------------------------------------*//**
	フレーム毎のステータス自然変更処理
**//*---------------------------------------------------------------------*/
void CharStat::updateAutoInfluence(f32 frate)
{
	ASSERT(_eestat != 0L);

	if(TFW_IS_FLAG(_flagsFrame, FF_PERIODIC_EXEC))	// 間引き処理フレームである
	{
		// 元素エネルギー自然治癒処理
		if(!TFW_IS_FLAG(_flagsFrame, FF_RUNNING) && (getEnergy() > 0))	// ただし走行中ダウン時は不可
		{
			s32 eerecv = _chrprm.getUnity() + (_chrprm.getStability() * 2);	// １フレームにおける回復可能能力
			eerecv = eerecv >> 2;

			naturation(eerecv, false);
		}

		// チャージ減衰
		if(_cntCharge > 0)
		{
			s32 ccsub = (_chrprm.getUnity() + _chrprm.getStability()) >> 8;	// 0 ～ 78
			ccsub = 100 - ccsub;
			s32 rateChargeNew = (s32)_cntCharge - ccsub;
			if(rateChargeNew > 0)	{	_cntCharge = (u16)rateChargeNew;	}
			else					{	_cntCharge = 0;						}
		}
	}

	// 表示用チャージ率更新
	if(_cntChargeDisp != _cntCharge)
	{
		s32 n = (s32)((_cntCharge - _cntChargeDisp) * 0.0625f * frate);
		_cntChargeDisp += n;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム毎の意識深度更新処理
**//*---------------------------------------------------------------------*/
void CharStat::updateConcentrateDepth(f32 frate)
{
	// +++ Adjustable Parameter >>>
	#define CONV_UNIF_TO_CONS(u_)			((u_) * 32768)			// 統合力を集中度に変換
	#define CONS_PURE_RATE_P1				(1.2f)					// 純性 +1 時の集中度ボーナス
	#define CONS_PURE_RATE_P2				(1.4f)					// 純性 +2 時の集中度ボーナス
	#define CONS_EXDL_RATE					(0.8f)					// 激性・鈍性時の集中度ペナルティ
	#define UNCONS_PURE_RATE_P1				(0.9f)					// 純性 +1 時の非集中度ボーナス
	#define UNCONS_PURE_RATE_P2				(0.8f)					// 純性 +2 時の非集中度ボーナス
	#define UNCONS_EXDL_RATE				(1.2f)					// 激性・鈍性時の非集中度ペナルティ
	#define DEFAULT_UNCONS					(-81920)				// デフォルトの非集中度
	#define CONV_UNIF_TO_UNCONS(u_)			(- ((u_) << 1))			// 統合力を非集中度に変換
	#define CONV_ACTV_TO_UNCONS(c_)			(- ((c_) >> 1))			// 変換力を非集中度に変換
	#define CONV_STAB_TO_UNCONS(s_)			(- ((s_) << 1))			// 安定力を非集中度に変換
	#define RECV_CONS_LAYER_TO_EERECV(l_)	((l_) >> 2)				// 意識深さレイヤーに対する元素回復量（>> 2 は ÷ 8 と同等）
	// +++ Adjustable Parameter <<<

	//STRACE("[%d] %d\n", _unitSelfRef->getId(), TFW_IS_FLAG(_flagsFrame, FF_DIANA));
	if(getEnergy() <= 0)
	{
		return;
	}
	if((_depthConc <= 0) && !TFW_IS_FLAG(_flagsFrame, FF_DIANA))
	{
		return;
	}

	s32 concAdd = 0;
	s32 concSub = 0;
	if(TFW_IS_FLAG(_flagsFrame, FF_DIANA))		// 瞑想中
	{
		// 集中力増加
		concAdd = CONV_UNIF_TO_CONS( _chrprm.getUnity() );
		// 現在の質によるボーナス・ペナルティ
		u32 eeqfPure = _eeqf & EleneStat::QF_MASK_PURE;
		if(eeqfPure == EleneStat::QF_PURE_3)			// 純性 +2
		{
			frate *= CONS_PURE_RATE_P2;
		}
		else if(eeqfPure == EleneStat::QF_PURE_2)		// 純性 +1
		{
			frate *= CONS_PURE_RATE_P1;
		}
		else if(
			((_eeqf & EleneStat::QF_EXCT_2) == EleneStat::QF_EXCT_2)	||		// 激性 +1 以上
			((_eeqf & EleneStat::QF_DULL_2) == EleneStat::QF_DULL_2)	)		// 鈍性 +1 以上	
		{
			frate *= CONS_EXDL_RATE;
		}
		// フレームレートをかける
		concAdd = (s32)(concAdd * frate);
	}
	else if(TFW_IS_FLAG(_flagsFrame, FF_CALMING))	// 静止中
	{
		// 集中力維持
	}
	else
	{
		// 集中力減少
		concSub = DEFAULT_UNCONS +
			CONV_UNIF_TO_UNCONS(_chrprm.getUnity()) + 
			CONV_ACTV_TO_UNCONS(_chrprm.getActivity()) +
			CONV_STAB_TO_UNCONS(_chrprm.getStability());
		if(concSub > 0)	{	concSub = 0;	}
		// 現在の質によるボーナス・ペナルティ
		u32 eeqfPure = _eeqf & EleneStat::QF_MASK_PURE;
		if(eeqfPure == EleneStat::QF_PURE_3)			// 純性 +2
		{
			frate *= UNCONS_PURE_RATE_P2;
		}
		else if(eeqfPure == EleneStat::QF_PURE_2)		// 純性 +1
		{
			frate *= UNCONS_PURE_RATE_P1;
		}
		else if(
			((_eeqf & EleneStat::QF_EXCT_2) == EleneStat::QF_EXCT_2)	||		// 激性 +1 以上
			((_eeqf & EleneStat::QF_DULL_2) == EleneStat::QF_DULL_2)	)		// 鈍性 +1 以上	
		{
			frate *= UNCONS_EXDL_RATE;
		}
		// フレームレートをかける
		concSub = (s32)(concSub * frate);
	}

#if 1
	// 現在の意識領域
	s32 layerConc = _depthConc >> 22;		// 0 ～ 11 意識レイヤー
	static const s32 CONCD_LAYER = 0xfffff;	// 2 の (31 - 11) 乗の値．11 右ビットシフトできる s32 最大値
#elif 1
	// 現在の元素領域
	s32 layerEl = (_depthConc / CONCD_EL_MAX) + 1;
	if(layerEl > CONCD_LAYER_NUM)	{	layerEl = CONCD_LAYER_NUM;	}
#else
	// 現在の元素領域
	s32 layerEl = _depthConc / CONCD_EL_MAX;
	if(layerEl >= CONCD_LAYER_NUM)	{	layerEl = CONCD_LAYER_NUM - 1;	}

	// 上位元素の分だけ進みが遅くなる
	s32 divr = 1;
	for(s32 i = 1; i <= layerEl; i++)
	{
		divr *= 2;
	}
#endif

	// 新しい意識深さ
	s32 depthConsNew = _depthConc;
	if(concAdd > 0)
	{
#if 1
		s32 layerWk = layerConc;
		while(concAdd > 0)
		{
			s32 consAddEl = concAdd;

			// 意識領域での最大消費量を超えている場合にクリップする
			s32 consAddElMax = CONCD_LAYER << layerWk;
			if(consAddEl > consAddElMax)
			{
				consAddEl = consAddElMax;
			}

			depthConsNew += consAddEl >> layerWk;
			concAdd -= consAddEl;
			layerWk++;
		}
#elif 1
		s32 layerElWk = layerEl;
		while((layerElWk < CONCD_LAYER_NUM) && (concAdd > 0))
		{
			s32 consAddEl = concAdd;

			// 元素領域での最大消費量を超えている場合にクリップする
			s32 consAddElMax = CONCD_EL_MAX * layerElWk;
			if(consAddEl > consAddElMax)
			{
				consAddEl = consAddElMax;
			}

			depthConsNew += consAddEl / layerElWk;
			concAdd -= consAddEl;
			layerElWk++;
		}
#else
		s32 layerElWk = layerEl;
		s32 divrWk = divr;
		while((layerElWk < CONCD_LAYER_NUM) && (concAdd > 0))
		{
			s32 consAddEl = concAdd;

			// 元素領域での最大消費量を超えている場合にクリップする
			s32 consAddElMax = CONCD_EL_MAX * divrWk;
			if(consAddEl > consAddElMax)
			{
				consAddEl = consAddElMax;
			}

			depthConsNew += consAddEl / divrWk;
			concAdd -= consAddEl;
			divrWk *= 2;
			layerElWk++;
		}
#endif

		if(depthConsNew > CONCD_TOTAL_MAX)
		{
			depthConsNew = CONCD_TOTAL_MAX;
		}
	}
	if(concSub < 0)
	{
#if 1
		s32 layerWk = layerConc;
		while((layerWk >= 0) && (concSub < 0))
		{
			s32 consSubEl = concSub;

			// 意識領域での最大消費量を超えている場合にクリップする
			s32 consSubElMax = - CONCD_LAYER << layerWk;
			if(consSubEl < consSubElMax)
			{
				consSubEl = consSubElMax;
			}

			depthConsNew += consSubEl >> layerWk;
			concSub -= consSubEl;
			layerWk--;
		}
#elif 1
		s32 layerElWk = layerEl;
		while((layerEl >= 0) && (concSub < 0))
		{
			s32 consSubEl = concSub;

			// 元素領域での最大消費量を超えている場合にクリップする
			s32 consSubElMax = - CONCD_EL_MAX * layerElWk;
			if(consSubEl < consSubElMax)
			{
				consSubEl = consSubElMax;
			}

			depthConsNew += consSubEl / layerElWk;
			concSub -= consSubEl;
			layerEl--;
		}
#else
		s32 layerElWk = layerEl;
		s32 divrWk = divr;
		while((layerEl >= 0) && (concSub < 0))
		{
			s32 consSubEl = concSub;

			// 元素領域での最大消費量を超えている場合にクリップする
			s32 consSubElMax = - CONCD_EL_MAX * divrWk;
			if(consSubEl < consSubElMax)
			{
				consSubEl = consSubElMax;
			}

			depthConsNew += consSubEl / divrWk;
			concSub -= consSubEl;
			divrWk /= 2;
			layerEl--;
		}
#endif

		if(depthConsNew < 0)
		{
			depthConsNew = 0;
		}
	}

	// 半覚醒状態では、集中度が半分より下に下がらない
	if(TFW_IS_FLAG(_flagsStat, SPSTAT_TRANS))
	{
		if(depthConsNew < CONCD_HENL_MIN)
		{
			depthConsNew = CONCD_HENL_MIN;
		}
	}

	// 新しい集中度
	_depthConc = depthConsNew;
	if(_depthConcDisp != _depthConc)
	{
		//_depthConcDisp = (_depthConcDisp + _depthConc) / 2;
		s32 n = (s32)((_depthConc - _depthConcDisp) * 0.0625f * frate);
		_depthConcDisp += n;
	}

	// 集中度による純化とエネルギー回復
#if 1
	s32 eerecv = RECV_CONS_LAYER_TO_EERECV(_chrprm.getUnity() << layerConc);
#elif 1
	s32 eerecv = RECV_CONS_LAYER_TO_EERECV(_chrprm.getUnity() * layerEl);
#else
	s32 eerecv = RECV_CONS_LAYER_TO_EERECV(_chrprm.getUnity() * divr);
#endif
	if(eerecv > 32767)	{	eerecv = 32767;	}
	if(eerecv > 0)
	{
		naturation(eerecv, true);
	}
}

/*---------------------------------------------------------------------*//**
	状態設定
**//*---------------------------------------------------------------------*/
void CharStat::setStat(Stat stat)
{
	TFW_SET_FLAG(_flagsStat, MASK_STAT, false);
	TFW_SET_FLAG(_flagsStat, stat, true);
}

/*---------------------------------------------------------------------*//**
	状態チェック
**//*---------------------------------------------------------------------*/
void CharStat::checkStat()
{
	// 状態
	if(!TFW_IS_FLAG(_flagsCreate, CF_ITEM))
	{
		s32 eneMax = _lfeneMax - _damagePhys;
		if(_damageEne >= eneMax)
		{
			_damageEne = eneMax;

			// 活動停止
			if(getStat() == STAT_ALIVE)
			{
				setStat(STAT_DOWN);
			}
		}
		else if(_damageEne < eneMax)
		{
			// 活動再開
			if(getStat() == STAT_DOWN)
			{
				setStat(STAT_ALIVE);
			}
		}
	}

	// 現在の質の計算
	_eeqf = _eestat->calcQualityFlags(0L, &_rateExct, &_rateDull);

	// 特殊状態
	{
		// 錯乱
		if((_eeqf & EleneStat::QF_MASK_EXCT) >= EleneStat::QF_EXCT_2)		// 激性 +1 以上
		{
			if(!TFW_IS_FLAG(_flagsStat, SPSTAT_CONFU))
			{
				TFW_SET_FLAG(_flagsStat, SPSTAT_CONFU, true);
			}
		}
		else
		{
			if(TFW_IS_FLAG(_flagsStat, SPSTAT_CONFU))
			{
				TFW_SET_FLAG(_flagsStat, SPSTAT_CONFU, false);
			}
		}
		// 睡眠
		if((_eeqf & EleneStat::QF_MASK_DULL) >= EleneStat::QF_DULL_3)		// 鈍性 +2 以上
		{
			if(!TFW_IS_FLAG(_flagsStat, SPSTAT_SLEEP))
			{
				TFW_SET_FLAG(_flagsStat, SPSTAT_SLEEP, true);
				TFW_SET_FLAG(_flagsStat, SPSTAT_HEAVY, false);
			}
		}
		else
		{
			if(TFW_IS_FLAG(_flagsStat, SPSTAT_SLEEP))
			{
				TFW_SET_FLAG(_flagsStat, SPSTAT_SLEEP, false);
			}

			// 倦怠
			if((_eeqf & EleneStat::QF_MASK_DULL) >= EleneStat::QF_DULL_1)	// 鈍性以上
			{
				if(!TFW_IS_FLAG(_flagsStat, SPSTAT_HEAVY))
				{
					TFW_SET_FLAG(_flagsStat, SPSTAT_HEAVY, true);
				}
			}
			else
			{
				if(TFW_IS_FLAG(_flagsStat, SPSTAT_HEAVY))
				{
					TFW_SET_FLAG(_flagsStat, SPSTAT_HEAVY, false);
				}
			}
		}
	}

}

#if defined(_DEBUG)
void CharStat::_debug_set(s32 damageEne, s32 damagePhys, s32 lfeneMax)
{
	_damageEne = damageEne;
	_damagePhys = damagePhys;
	_lfeneMax = lfeneMax;
}
#endif	// #if defined(_DEBUG)


////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
