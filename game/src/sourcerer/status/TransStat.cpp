/***********************************************************************//**
 *	TransStat.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TransStat.h"

// Friends
#include "CharStat.h"
#include "../common/CalcUtils.h"
#include "../item/Item.h"
#include "../unit/Unit.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// マクロ

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TransStat メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	質および機能を算出する
**//*---------------------------------------------------------------------*/
void TransStat::calcQualityAndFunc(bool isSouma)
{
	_eeqf = _eestat.calcQualityFlags();
	_eeff = _eestat.calcFuncFlags(_eeqf, isSouma);
}

/*---------------------------------------------------------------------*//**
	機能フラグ（EleneStat::FF_*）を上書きする
**//*---------------------------------------------------------------------*/
bool TransStat::overrideFuncFlags(u32 eeff)
{
	_eeff = eeff;
	return true;
}

/*---------------------------------------------------------------------*//**
	否定的な作用かを判定する
**//*---------------------------------------------------------------------*/
bool TransStat::isNegativeOperation() const
{
	return EleneStat::isNegativeOperation(_eeff);
}

/*---------------------------------------------------------------------*//**
	スケーリング - 100 を等倍とした整数演算
**//*---------------------------------------------------------------------*/
void TransStat::scale(s32 ratio)
{
	_eestat.scale(ratio);
}

/*---------------------------------------------------------------------*//**
	スケーリング - 1.0f を等倍とした浮動小数点演算
**//*---------------------------------------------------------------------*/
void TransStat::scale(f32 ratio)
{
	_eestat.scale(ratio);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void TransStat::exec(const ExecCtx* ec)
{
	// 表示用結果値更新
	_eestat.exec(ec);
}

/*---------------------------------------------------------------------*//**
	設定
**//*---------------------------------------------------------------------*/
void TransStat::setupForMagicClusterMaking(Unit* unitOwnRef, const Elene* eleneOql, u16 flagsSub)
{
	// 基本設定値
	_unitOwnRef = unitOwnRef;
	_kind = KIND_SOUMA;		// 種類をソウマに
	_flagsSub = flagsSub;

	// 原質のみ指定で、現在値はゼロで設定
	Elene eeCurZero;
	_eestat.setup(eleneOql, &eeCurZero);

	// 質および機能を算出する
	calcQualityAndFunc(true);
}

/*---------------------------------------------------------------------*//**
	外部の武器を用いて、攻撃用として設定する
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
void TransStat::setupForWeaponAttack(Unit* unitOwnRef, const EleneStat* eestatWeapon, const CharStat* cstat)
{
	const Elene* eleneOwnWrp = cstat->getEleneStat()->getVeil(EleneStat::VEIL_WP)->getElene();
	const Elene* eleneOwnNql = cstat->getEleneStat()->getNql();
	const Elene* eleneOwnCql = cstat->getEleneStat()->getCql();

	// 攻撃元素化率
	f32 ratioEv = calcBaseAttackElementsRatio(cstat, 100);

	// 基本設定値
	_unitOwnRef = unitOwnRef;
	_eestat.copy(eestatWeapon);
	_eestat.scale(ratioEv);
	{
		// 瞬間結界は強制的に現在値を減少させ、攻撃を不利にする
		const Elene* eleneEf = cstat->getEleneStat()->getVeil(EleneStat::VEIL_BR)->getElene();
		for(int i = 0; i < Elene::NUM_EL; i++)
		{
			s32 eeCql = _eestat.getCql()->get(i);
			eeCql -= eleneEf->get(i);
			if(eeCql < 0)		{	eeCql = 0;		}
			_eestat.cql()->set(i, eeCql);
		}
	}
	Elene eleneCw(_eestat.calcCurrentAndWrapperElene());
	_kind = KIND_WEAPON;	// 種類を武器に

	// 基本数値算出
	f32 stbrandf, critical;
	s32 velwind;
	calcBaseAttackValue(&velwind, &critical, &stbrandf, cstat);
	s32 stbrandi = (s32)(stbrandf * 128.0f);	// 安定率を整数化（値は 128 を 100% としたもの）

	// 物理身体伝達率（想定値：20 ～ 50 %；値は 128 を 100% としたもの）
	s32 conduct = 64 - ((CharParam::VALUE_MAX - cstat->getCharParam()->getUnity()) * 38 / CharParam::VALUE_MAX);
	velwind = velwind * conduct / 128;

	// ＜風＞ 移動力によって風元素を増悪させ、激性にする
	_eestat.cql()->add(Elene::EL_WI, velwind);

	// ＜火＞ また、火元素も風にあおられて増悪する（鋭さが向上するイメージ）
	s32 eeFiAdd = eleneCw.getFire() / 128 * velwind / 512;
	eeFiAdd += cstat->getChargeCounter() * 8;
	_eestat.cql()->add(Elene::EL_FI, eeFiAdd);

	/*
	// ＜水＞ 自身の身体の水要素も武器を通して伝わる
	s32 eeWaAdd = (eleneWrp->getWater() / 64) + ((eleneCql->getWater() - eleneNql->getWater()) / 512) + (eleneNql->getWater() / 32);
	eeWaAdd = eeWaAdd * conduct / 128 * stbrandi / 128;
	_eestat.cql()->add(Elene::EL_WA, eeWaAdd);
	*/

	// ＜地＞ 自身の身体の地要素も武器を通して伝わる（体重をかけるイメージ）
	s32 eeSoAdd = (eleneOwnWrp->getSoil() / 16) + ((eleneOwnCql->getSoil() - eleneOwnNql->getSoil()) / 128) + (eleneOwnNql->getSoil() / 8);
	eeSoAdd = eeSoAdd * conduct / 128 * stbrandi / 128;
	_eestat.cql()->add(Elene::EL_SO, eeSoAdd);

	// 最終値更新
	_eestat.updateResultValue(true);

	// 五元素の質フラグを算出する
	_eeqf = _eestat.calcQualityFlags();
	// 五元素の機能フラグを算出する
	if(critical == 0.0f)	// 攻撃ミス
	{
		_eeff = EleneStat::FF_OP_MISS_ATTACK;
	}
	else					// 攻撃
	{
		_eeff = EleneStat::FF_OP_ATTACK;
	}
}

/*---------------------------------------------------------------------*//**
	自らの身体を武器として、攻撃用として設定する
	※ 計算式詳細は『ソウマ関数と計算式.odt』参照のこと
**//*---------------------------------------------------------------------*/
void TransStat::setupForSelfBodyAttack(Unit* unitOwnRef, const CharStat* cstat, u16 rateBody)
{
	// 攻撃元素化率
	f32 ratioEv = calcBaseAttackElementsRatio(cstat, rateBody);

	// 自身を武器化
	Elene eleneWeaponCql;
	Elene eleneWeaponNql;
	{
		const Elene* eleneNql = cstat->getEleneStat()->getNql();
		const Elene* eleneCql = cstat->getEleneStat()->getCql();
		const Elene* eleneWp = cstat->getEleneStat()->getVeil(EleneStat::VEIL_WP)->getElene();
		const Elene* eleneEf = cstat->getEleneStat()->getVeil(EleneStat::VEIL_BR)->getElene();
		for(int i = 0; i < Elene::NUM_EL; i++)
		{
			s32 eeNql = (s32)(eleneNql->get(i) * ratioEv);
			s32 eeCql = (s32)((eleneWp->get(i) + eleneCql->get(i)) * ratioEv);
			if(eeCql < eeNql)	{	eeCql = eeNql;	}
			eeCql -= eleneEf->get(i);	// 瞬間結界は強制的に現在値を減少させ、攻撃を不利にする
			if(eeCql < 0)		{	eeCql = 0;		}
			eleneWeaponCql.set(i, eeCql);
			eleneWeaponNql.set(i, eeNql);
		}
	}

	// 基本設定値
	_unitOwnRef = unitOwnRef;
	_eestat.setup(&eleneWeaponNql, &eleneWeaponCql);
	_kind = KIND_BODY;		// 種類を身体に

	// 基本数値算出
	f32 stbrandf, critical;
	s32 velwind;
	calcBaseAttackValue(&velwind, &critical, &stbrandf, cstat);

#if 0
	// 物理身体伝達率（想定値：20 ～ 50 %；値は 128 を 100% としたもの）
	s32 conduct = 64 - ((CharParam::VALUE_MAX - cstat->getCharParam()->getUnity()) * 38 / CharParam::VALUE_MAX);
	velwind = velwind * conduct / 16;

	// ＜風＞ 移動力によって風元素を増悪させ、激性にする
	_eestat.cql()->add(Elene::EL_WI, velwind);

	// ＜火＞ また、火元素も風にあおられて増悪する
	s32 eeFiAdd = _eestat.getFv()->getFire() / 128 * velwind / 512;
	_eestat.cql()->add(Elene::EL_FI, eeFiAdd);
#endif

	// 最終値更新
	_eestat.updateResultValue(true);

	// 五元素の質フラグを算出する
	_eeqf = _eestat.calcQualityFlags();
	// 五元素の機能フラグを算出する
	_eeff = _eestat.calcFuncFlags(_eeqf, false);
	if(critical == 0.0f)	// 攻撃ミス
	{
		TFW_SET_FLAG(_eeff, EleneStat::FF_MASK_OP, false);
		TFW_SET_FLAG(_eeff, EleneStat::FF_OP_MISS_ATTACK, true);
	}
}

/*---------------------------------------------------------------------*//**
	技として設定する
**//*---------------------------------------------------------------------*/
void TransStat::setupForSkill(Unit* unitOwnRef, const EleneStat* eestat)
{
	// 値の設定
	_unitOwnRef = unitOwnRef;
	_eestat.copy(eestat);
	_eestat.updateResultValue(true);
	_kind = KIND_SKILL;		// 種類を技に

	// 五元素の質フラグを算出する
	_eeqf = _eestat.calcQualityFlags();
	// 機能フラグを算出する
	_eeff = _eestat.calcFuncFlags(_eeqf, false);
}

/*---------------------------------------------------------------------*//**
	アイテム使用用として設定する
**//*---------------------------------------------------------------------*/
void TransStat::setupForUsingItem(Unit* unitOwnRef, const EleneStat* eestatItem)
{
	// 基本値の設定
	_unitOwnRef = unitOwnRef;
	_kind = KIND_ITEM;		// 種類をアイテムに

	// 現在量は、アイテムに付帯されたベールの量
	Elene eeCql;
	for(int ie = 0; ie < Elene::NUM_EL; ie++)
	{
		s32 e = eestatItem->getVeil(EleneStat::VEIL_WP)->getElene()->get(ie);

		// ベールがあるなら現在値も足す		【2015/06/01 r-kishi】
		if(e > 0)
		{
			e += eestatItem->getCql()->get(ie);
		}

		eeCql.add(ie, e);

	}

	// 最も小さな元素量を算出
	s32 ieMin = -1;
	s32 eeMinEe = S32_MAX;
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		if(eeMinEe > eeCql.get(i))
		{
			ieMin = i;
			eeMinEe = eeCql.get(i);
		}
	}
	// アイテムの原質に対しての率を算出
	ASSERT(ieMin != -1);
	if((ieMin < 0) || (Elene::NUM_EL <= ieMin))	{	return;	}
	f32 ratioItemOql = (f32)eeMinEe / (f32)eestatItem->getOql()->get(ieMin);

	// 原質作成
	Elene eeOql;
	for(int ie = 0; ie < Elene::NUM_EL; ie++)
	{
		eeOql.set(ie, (s32)(eestatItem->getOql()->get(ie) * ratioItemOql));
	}

	// 元素バランス作成
	_eestat.setup(&eeOql, &eeCql);
	_eestat.updateResultValue(true);

	// 五元素の質フラグを算出する
	_eeqf = _eestat.calcQualityFlags();
	// 機能フラグを算出する
	_eeff = _eestat.calcFuncFlags(_eeqf, false);
}

/*---------------------------------------------------------------------*//**
	直接指定機能として設定する
**//*---------------------------------------------------------------------*/
void TransStat::setupForDirectFunc(Unit* unitOwnRef, const EleneStat* eestat, u32 eeff)
{
	// 値の設定
	_unitOwnRef = unitOwnRef;
	_eestat.copy(eestat);
	_eestat.updateResultValue(true);
	_kind = KIND_DIRECT;	// 種類をダイレクトに

	// 五元素の質フラグを算出する
	_eeqf = _eestat.calcQualityFlags();
	// 機能フラグを設定する
	_eeff = eeff;
}

/*---------------------------------------------------------------------*//**
	基本攻撃元素率を算出する
**//*---------------------------------------------------------------------*/
f32 TransStat::calcBaseAttackElementsRatio(const CharStat* cstat, u16 rateWeaponEffective)
{
	// 武器有効度
	f32 ratioEv = rateWeaponEffective * 0.01f;
		
	// 変換力
	ratioEv *= 0.4f + (cstat->getCharParam()->getActivity() / (f32)CharParam::VALUE_DEFAULT) * 0.1f;

	// 安定度
	f32 rr = 1.0f - (cstat->getCharParam()->getStability() / (f32)CharParam::VALUE_MAX);
	rr *= CalcUtils::randPlusMinusOneF() * 0.05f;	// ± 5%
	ratioEv *= (1.0f + rr);

	// 溜めによる加勢
	if(cstat->getChargeCounter() > 0)
	{
		ratioEv += (f32)cstat->getChargeCounter() / (f32)CharStat::CHARGECNT_MAX * 0.5f;
	}

	return ratioEv;
}

/*---------------------------------------------------------------------*//**
	基本攻撃用の値を算出する
**//*---------------------------------------------------------------------*/
void TransStat::calcBaseAttackValue(s32* velwind, f32* critical, f32* stbrandf, const CharStat* cstat)
{
	// 減衰乱数（想定値 = 弱:75 ～ 強:100 %）
	*stbrandf = 1.0f - (CalcUtils::randF() * (f32)(CharParam::VALUE_MAX - cstat->getCharParam()->getStability()) / (f32)CharParam::VALUE_MAX) * 0.25f;

	// 錯乱チェック
	// ※『興奮と倦怠による影響.ods』参照
	*critical = 1.0f;
	if(cstat->getExcitednessRate() > 150)
	{
		s32 exct = cstat->getExcitednessRate() - (cstat->getCharParam()->getUnity() / 128);	// 集中度が高いと抑えられる
		if(exct > 150)
		{
#if 1
			s32 hitscore = 127 - (exct - 150);
			if(hitscore < 32)	{	hitscore = 32;	}	// 25% 以下にはしない
			s32 randscore = ::rand() & 127;
			if(hitscore >= randscore)
			{
				hitscore = exct - 170;
				randscore = ::rand() & 127;
				if(hitscore >= randscore)
				{
					*critical = 2.0f;	// クリティカルヒット
				}
				else
				{
					*critical = 1.0f;	// 通常攻撃
				}
			}
			else
			{
				*critical = 0.0f;	// 攻撃ミス
			}
#else
			f32 hitrate = 1.0f - (exct - 150) * 0.005f;	// 興奮度による確率低下
			s32 hitscore = (s32)(hitrate * RAND_MAX);
			s32 randscore = ::rand();
			if(hitscore >= randscore)
			{
				if(TFW_IS_FLAG(cstat->getSpStatFlags(), CharStat::SPSTAT_CONFU))	// 錯乱状態
				{
					*critical = 2.0f;	// 当たれば攻撃２倍
				}
			}
			else
			{
				*critical = 0.0f;	// 攻撃ミス
			}
#endif
		}
	}

	// 速度（想定値：1000 ～ 10000 cm/s）
	s32 velo = cstat->getEleneStat()->getNql()->getWind() / 32;
	// 速度→風元素
	*velwind = (s32)( (f32)(velo * cstat->getCharParam()->getActivity()) * (*stbrandf) * (*critical));
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
