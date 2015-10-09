/***********************************************************************//**
 *	SsCluster.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/01/24.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SsCluster.h"

// Friends
#include "SoumaScript.h"
#include "SoumaScriptContext.h"
#include "SoumaScriptUtils.h"
#include "SsUnit.h"
#include "../Souma.h"
#include "../../common/CalcUtils.h"
#include "../../effect/EleneEffect.h"
#include "../../effect/StatusChangeEffect.h"
#include "../../es/EsContext.h"
#include "../../es/EsObject.h"
#include "../../es/EsRunEnv.h"
#include "../../es/EsScopeProperty.h"
#include "../../es/EsSysClasses.h"
#include "../../es/EsSysPrototypes.h"
#include "../../body/Game.h"
#include "../../body/GameNotifier.h"
#include "../../item/ItemDef.h"
#include "../../magic/MagicCluster.h"
#include "../../magic/MagicSys.h"
#include "../../map/Map.h"
#include "../../map/PlacementObj.h"
#include "../../sound/GameSoundDef.h"
#include "../../spirit/Spirit.h"
#include "../../status/CharStat.h"
#include "../../status/FocusArray.h"
#include "../../status/StatusIfEx.h"
#include "../../status/TransStat.h"
#include "../../unit/char/CharUnit.h"
#include "../../unit/item/PutItemUnit.h"

// External
#include "../../../etk/particle/Particle.h"
#include "../../../etk/particle/ParticleSys.h"
#include "../../../etk/sound/SoundMngr.h"
#include "../../../tfw/lib/Matrix.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// 意識集中度
static const s32	CONCD_COMM_ATMAN		= CharStat::CONCD_EL_MAX * Elene::NUM_EL;	// アートマンと通信
static const s32	CONCD_COMM_GODS			= CharStat::CONCD_EL_MAX * Elene::EL_AE;	// ほとんど全ての神々と通信
static const s32	CONCD_COMM_AE			= CharStat::CONCD_EL_MAX * Elene::EL_AE;	// 空元素への操作
static const s32	CONCD_COMM_WI			= CharStat::CONCD_EL_MAX * Elene::EL_WI;	// 風元素への操作
static const s32	CONCD_COMM_FI			= CharStat::CONCD_EL_MAX * Elene::EL_FI;	// 火元素への操作
static const s32	CONCD_COMM_WA			= CharStat::CONCD_EL_MAX * Elene::EL_WA;	// 水元素への操作
static const s32	CONCD_COMM_SO			= CharStat::CONCD_EL_MAX * Elene::EL_SO;	// 地元素への操作

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SsCluster::BaseClass メソッド

/*---------------------------------------------------------------------*//**
	ソウマスクリプトクラスの作成
**//*---------------------------------------------------------------------*/
EsObject* SsCluster::FoundationClass::makeClassObject(EsContext* ctx, EsObject* objParent, s32 skwdOfs) const
{
	// クラスオブジェクト作成
	EsObject* objPtype;
	EsObject* objCtor;
	objParent->makeClassObject(&objPtype, &objCtor, ctx, this, 0L, SsCluster::callConstructor);	// ⇒ proto = JS_InitClass(cx, obj, NULL, &js_FunctionClass, Function, 1, function_props, function_methods, NULL, NULL);
	if(objPtype == 0L)
	{
		return 0L;
	}
	// 非静的プロパティを定義する
//	objPtype->defineClassProperty(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_MASTER_OBJ + skwdOfs), SsCluster::callGetMaster, g_funcDummyPropertySetter, -1, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED);
	// 非静的関数を定義する
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_GET_ELEMENT_QUANTITY + skwdOfs), callGetElementQuantity, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_EVOKE_ELEMENT + skwdOfs), callEvokeElement, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_EVOKE_ORIGINAL_ELEMENTS + skwdOfs), callEvokeOriginalElements, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_EVOKE_PRESET_ELEMENTS + skwdOfs), callEvokePresetElements, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_DISSOLVE + skwdOfs), callDissolve, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_SET_FOCUS_UNIT + skwdOfs), callSetFocusUnit, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_MATERIALIZE_CLUSTER + skwdOfs), callMaterialize, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_SYNTHESIZE + skwdOfs), callSynthesize, 0);

	return objPtype;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SsCluster::FoundationClass::FoundationClass(SoumaScriptKeywords::Keyword kwd)
	: SsBase::BaseClass(kwd, EsClass::F_HAS_PRIVATE, 0)
{
	_funcConstruct = SsCluster::callConstructor;
}


//==========================================================================
// SsCluster メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool SsCluster::init()
{
	EsSysPrototypes::registerExternalPrototype(NsPrototype::getInstance());
	EsSysPrototypes::registerExternalPrototype(EnPrototype::getInstance());
	EsSysClasses::registerExternalClass(NsClass::getInstance(), true);
	EsSysClasses::registerExternalClass(EnClass::getInstance(), true);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es オブジェクトの作成
**//*---------------------------------------------------------------------*/
bool SsCluster::makeEsObject(EsObject** objMc, SoumaScriptContext* ssctx, MagicCluster* mc, s32 skwdOfs)
{
	const EsAtom* atom = SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_CLASS_NS_CLUSTER + skwdOfs);	// アトム取得
	EsValue valArg;
	SsBase::BaseVariables::setVariables(&valArg, mc);
	if(!ssctx->getEsContext()->getRunEnv()->makeClassInstance(objMc, atom, &valArg, 1))
	{
		ASSERT(false);
		return false;
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	Es オブジェクトからクラスタ取得
**//*---------------------------------------------------------------------*/
MagicCluster* SsCluster::getMagicCluster(EsObject* objMc)
{
	const EsClass* cls = objMc->getClass();
	if((cls != NsClass::getInstance()) && (cls != EnClass::getInstance()))
	{
		return 0L;
	}
	Variables v(objMc->slots()->getPrivateValue());
	return v.cluster();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	単元素呼び出し
**//*---------------------------------------------------------------------*/
s32 SsCluster::evokeElement(EsContext* ctx, Variables* v, s32 elTrg, s32 eeAdd)
{
	Unit* unitOwn = v->cluster()->getOwnerUnit();
	const CharStat* cstatOwn = unitOwn->getSpirit()->getCharStat();
	const CharParam* cprmOwn = cstatOwn->getCharParam();
	TransStat* tstatMc = v->cluster()->transStat();
	EleneStat* eestatMc = tstatMc->eleneStat();

	// 意識深さチェック
	s32 cons = cstatOwn->getConcentrateDepth();
	if(cstatOwn->getConcentrateDepth() < CharStat::CONCD_THRSLD_TRANCE)
	{
		return SoumaScript::RET_NE_CONCD;		// ＜失敗＞意識深さが足りない
	}

	// 増分元素量を得る
	s32 eleneEvokable = ((SoumaScriptContext*)ctx)->getSouma()->getEvokableElene()->get(elTrg);
	if(eeAdd > eleneEvokable)
	{
		eeAdd = eleneEvokable;
	}

	// 誤差乱数
	f32 rateRand = CalcUtils::randF();	// 0.0 ～ 1.0
	rateRand *= (CharParam::VALUE_MAX - cprmOwn->getStability()) * 0.000005f;	// 0.0 ～ 0.045
	switch(elTrg)
	{
	case Elene::EL_SO:	TFW_CLAMP(rateRand, 0.0f, 0.04f);	break;	// ＋4%
	case Elene::EL_WA:	TFW_CLAMP(rateRand, 0.0f, 0.02f);	break;	// ＋2%
	case Elene::EL_FI:	TFW_CLAMP(rateRand, 0.0f, 0.01f);	break;	// ＋1%
	default:			rateRand = 0.0f;					break;
	}

	// 意識深さによる誤差軽減（高い純性を出すのに必要）
	f32 rateConsCancel = 1.0f - (CharStat::CONCD_THRSLD_ZERO_ERROR - cons) / (f32)(CharStat::CONCD_THRSLD_ZERO_ERROR - CharStat::CONCD_THRSLD_TRANCE);
	if(rateConsCancel < 0.0f)
	{
		rateConsCancel = 0.0f;
	}
	rateRand *= rateConsCancel;

	// 誤差適用
	s32 eeRand = (s32)(eeAdd * rateRand);
	eeAdd += eeRand;

	// 対象元素を増やす
	eestatMc->cql()->add(elTrg, eeAdd);

	// 最終値更新
	eestatMc->updateResultValue(false);
	tstatMc->calcQualityAndFunc(true);

	return eeAdd;
	//return eestatMc->getCql()->get(elTrg);
}

/*---------------------------------------------------------------------*//**
	Es 関数 - コンストラクタ
**//*---------------------------------------------------------------------*/
bool SsCluster::callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis->slots()->setPrivateValue((numArg > 0) ? &vaArg[0] : 0L);

	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 元素量を返す
		SS: getelmquat a elm o
		JS: getElementQuantity(elm)
**//*---------------------------------------------------------------------*/
bool SsCluster::callGetElementQuantity(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.cluster() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}
	// 引数チェック
	if(numArg != 1)
	{	
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_NUM);
		return true;
	}
	if(!vaArg[0].isString())
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);
		return true;
	}

	// クラスタチェック
	if(!((SoumaScriptContext*)ctx)->getSouma()->checkCluster(v.cluster()->getId(), false))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_CLUSTER);
		return true;
	}

	// 対象の元素量を得る
	s32 retval = 0;
	s32 el = SoumaScriptUtils::matchElementNameKeyword(vaArg[0].getString(), true);	// 対象元素
	if(el != Elene::NUM_EL)
	{
		retval = v.cluster()->getTransStat()->getEleneStat()->getCql()->get(el) / Elene::ELENE_VALID_FIGURE;
	}

	valRet->setValue(retval);	// 戻り値の設定
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 単元素を直接呼び出す
		SS: lamelm a elm, add o
		JS: evokeElement(elm, add)
**//*---------------------------------------------------------------------*/
bool SsCluster::callEvokeElement(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.cluster() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}
	// 引数チェック
	if(numArg != 2)
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_NUM);
		return true;
	}
	if(!vaArg[0].isString())
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);
		return true;
	}
	if(!vaArg[1].isNumber())
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);
		return true;
	}

	// クラスタチェック
	if(!((SoumaScriptContext*)ctx)->getSouma()->checkCluster(v.cluster()->getId(), true))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_CLUSTER);
		return false;
	}

	// 対象の元素量を得る
	s32 retval = 0;
	s32 elTrg = SoumaScriptUtils::matchElementNameKeyword(vaArg[0].getString(), true);			// 対象元素
	s32 eeAdd = (s32)vaArg[1].getNumber() * Elene::ELENE_VALID_FIGURE;		// 追加元素量
	if(elTrg != Elene::NUM_EL)
	{
		if(eeAdd > 0)
		{
			retval = evokeElement(ctx, &v, elTrg, eeAdd);
		}
	}

	// 戻り値の設定
	if(retval >= 0)	// 成功
	{
		valRet->setValue(retval / Elene::ELENE_VALID_FIGURE);
	}
	else			// エラー
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, retval);
		return false;
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 原質値まで全元素を呼び出す
		SS: lamprimelm a o
		JS: evokeOriginalElements()
**//*---------------------------------------------------------------------*/
bool SsCluster::callEvokeOriginalElements(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.cluster() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}

	// クラスタチェック
	if(!((SoumaScriptContext*)ctx)->getSouma()->checkCluster(v.cluster()->getId(), true))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_CLUSTER);
		return false;
	}

	EleneStat* eestatMc = v.cluster()->transStat()->eleneStat();
	
	bool okAll = true;
	for(int i = 0; i < Elene::NUM_EL; i++)
	{
		s32 eeTrg = eestatMc->getOql()->get(i);
		s32 eeAdd = eeTrg - eestatMc->getCql()->get(i);
		if(eeAdd > 0)
		{
			s32 eeCur = evokeElement(ctx, &v, i, eeAdd);
			if(eeCur == SoumaScript::RET_NE_CONCD)
			{
				((SoumaScriptContext*)ctx)->errorMistake(valRet, eeCur);
				return false;
			}
			else if(eeCur < eeTrg)
			{
				okAll = false;
			}
		}
	}

	// 戻り値の設定
	valRet->setValue(okAll);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 既定の元素量値まで全元素を呼び出す
		SS: lampresetelm a o
		JS: evokePresetElements()
**//*---------------------------------------------------------------------*/
bool SsCluster::callEvokePresetElements(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if (v.cluster() == 0L)	{ ((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false; }

	// クラスタチェック
	if(!((SoumaScriptContext*)ctx)->getSouma()->checkCluster(v.cluster()->getId(), true))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_CLUSTER);
		return false;
	}

	EleneStat* eestatMc = v.cluster()->transStat()->eleneStat();
	const Elene* elenePreset = v.cluster()->getPresetElene();

	// 既定の元素量が無い場合は原質値を目標値とする
	if (elenePreset == 0L)
	{
		elenePreset = eestatMc->getOql();
	}
	ASSERT(elenePreset != 0L);

	bool okAll = true;
	for (int i = 0; i < Elene::NUM_EL; i++)
	{
		s32 eeTrg = elenePreset->get(i);
		s32 eeAdd = eeTrg - eestatMc->getCql()->get(i);
		if (eeAdd > 0)
		{
			s32 eeCur = evokeElement(ctx, &v, i, eeAdd);
			if(eeCur == SoumaScript::RET_NE_CONCD)
			{
				((SoumaScriptContext*)ctx)->errorMistake(valRet, eeCur);
				return false;
			}
			else if(eeCur < eeTrg)
			{
				okAll = false;
			}
		}
	}

	// 戻り値の設定
	valRet->setValue(okAll);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 溶解
		SS: disona a o
		JS: dissolve()
**//*---------------------------------------------------------------------*/
bool SsCluster::callDissolve(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.cluster() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}

	// クラスタチェック
	if(!((SoumaScriptContext*)ctx)->getSouma()->checkCluster(v.cluster()->getId(), true))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_CLUSTER);
		return false;
	}

	// ベールの追加値と限界値
	Elene eeWpAdd;	// 現在値合計
	Elene eeFrAdd;	// 種族特徴率
	Elene eleneVeilLimit(*v.cluster()->getTransStat()->getEleneStat()->getNql());
	eleneVeilLimit.scale(EleneStat::RATIO_WP_VEIL_MAX);

	// フォーカスされている置かれたアイテムの元素を回収し、置かれたアイテムを消滅させる
	s32 limit, add;
	Map* map = Game::getGame()->getMap(); ASSERT(map != 0L);
	const FocusArray* focusarr = v.cluster()->getFocusArray();
	for(int i = 0; i < focusarr->getCount(); i++)
	{
		const Unit* unitTrg = focusarr->getUnit(i);
		if((unitTrg != 0L) && unitTrg->isEnable() && (unitTrg->getUnitType() == UNITTYPE_PUT_ITEM))
		{
			const EleneStat* eestatTrg = unitTrg->getEleneStat();
			PutItemUnit* ditmu = (PutItemUnit*)unitTrg;

			const ItemDef* itmdf = ditmu->getItemDef();
			const Elene* fr = itmdf->getFamilyRatio();
			for(int j = 0; j < Elene::NUM_EL; j++)
			{
				// 現在値合計
				limit = eleneVeilLimit.get(i);
				add = eestatTrg->getCql()->get(j);
				if((eeWpAdd.get(j) + add) > limit)	{	eeWpAdd.set(j, limit);	}
				else								{	eeWpAdd.add(j, add);	}
				// 種族特徴率
				eeFrAdd.add(j, fr->get(j));
			}

			// 置かれたアイテム消滅
			PlacementObj* pobj = ditmu->getPlacementObj();
			ditmu->setPlacementObj(0L);
			map->removeTemporaryPlacementObjSetting(pobj);
			ditmu->setEnable(false);	// 無効化

			// 消化済みユニット数のインクリメント
			v.cluster()->addDissolvedUnitCount();
		}
	}
	///v.cluster()->focusArray()->clear();	// フォーカスクリア

	// 種族特徴率は平均化する
	f32 rr = (focusarr->getCount() > 0) ? (1.0f / (f32)focusarr->getCount()) : 0.0f;
	for(int j = 0; j < Elene::NUM_EL; j++)
	{
		eeFrAdd.set(j, (s32)(eeFrAdd.get(j) * rr));
	}

	// ベール付加
	if((eeFrAdd.sum() > 0) || (eeWpAdd.sum() > 0))
	{
		// ベール化
		if(eeWpAdd.sum() > 0)	{	v.cluster()->transStat()->eleneStat()->veil(EleneStat::VEIL_WP)->strengthen(&eeWpAdd);	}
		if(eeFrAdd.sum() > 0)	{	v.cluster()->transStat()->eleneStat()->veil(EleneStat::VEIL_BR)->strengthen(&eeFrAdd);	}

		// パーティクス・SE 再生
		ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
		ptclsys->appearParticle(GameParticleDef::PTCLK_CURE, v.cluster()->getPosition(), 0);
		Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_ENE_CURE, false, 1.0f, v.cluster()->getPosition(), 0L);
	}

	valRet->setValue(SoumaScript::RET_OK);	// 戻り値の設定
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - フォーカスをセットする
		SS: setfocmono a unit reset o
		JS: setFocusUnit(unit, reset)
**//*---------------------------------------------------------------------*/
bool SsCluster::callSetFocusUnit(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.cluster() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}
	// 引数の数チェック
	if(numArg == 0)	{	((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_NUM);	return true;	}
	// 引数の型チェック
	if(!vaArg[0].isObject() && !vaArg[0].isNull())	{	((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);	return true;	}
	if((numArg >= 2) && !vaArg[1].isBoolean())		{	((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);	return true;	}

	// クラスタチェック
	if(!((SoumaScriptContext*)ctx)->getSouma()->checkCluster(v.cluster()->getId(), true))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_CLUSTER);
		return false;
	}

	// フォーカス追加
	Unit* unit = vaArg[0].isObject() ? SsUnit::getUnit(vaArg[0].getObject()) : 0L;
	bool isReset = (numArg >= 2) ? vaArg[1].getBoolean() : false;	// デフォルトは追加
	v.cluster()->addFocusUnit(unit, isReset);

	// エフェクト表示
	if(unit != 0L)
	{
		ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
		u16 ptclid = ptclsys->appearParticle(GameParticleDef::PTCLK_SOUMA_LOCKON, unit->getCenterPos(), 0);
		if(ptclid != 0)
		{
			Particle* ptcl = ptclsys->getParticle(ptclid);
			if(ptcl != 0L)
			{
				ptcl->setScale(0.5f);
			}
		}
	}

	valRet->setValue(SoumaScript::RET_OK);	// 戻り値の設定
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - クラスタを実体化
		SS: materaizad a o
		JS: materialize()
**//*---------------------------------------------------------------------*/
bool SsCluster::callMaterialize(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.cluster() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}

	bool isHappened = false;
	Souma* souma = ((SoumaScriptContext*)ctx)->getSouma();
	TransStat* tstatMc = v.cluster()->transStat();
	u16 mcid = v.cluster()->getId();

	// クラスタチェック
	if(!souma->checkCluster(mcid, true))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_CLUSTER);
		return false;
	}

	// フォーカスされていない場合は自身がフォーカスしている対象にする
	const FocusArray* focusarr = v.cluster()->getFocusArray();
	if(focusarr->getCount() <= 0)
	{
		Unit* unitTrg = souma->ownerSpirit()->focusUnit();	// 自身のフォーカス対象
		if((unitTrg != 0L) && unitTrg->isEnable())
		{
			v.cluster()->addFocusUnit(unitTrg, false);
		}
	}

	// エネルギー消費
#if 1
	if(!StatusIfEx::wasteSoumaMaterializeEnergy(souma->ownerSpirit()->charStat(), tstatMc->getEleneStat()->getCql(), false))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NE_ENEG);		// <ERROR> エネルギー不足
		return false;
	}
#else
	if(!StatusInfluencer::wasteSoumaMaterializeEnergy(0L, souma->ownerSpirit()->charStat(), tstatMc->getEleneStat()->getCql(), false))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NE_ENEG);		// <ERROR> エネルギー不足
		return false;
	}
	souma->ownerSpirit()->notifySoumaAction(Souma::SNOTIFID_WASTE_ENE, 0L, 0L);	// スピリットへ通知
#endif

	// クラスタ形成確認
	u32 funcOp = tstatMc->getFuncFlags() & EleneStat::FF_MASK_OP;

	// 既定の元素量がある場合はその元素量以下は認めない
	const Elene* elenePreset = v.cluster()->getPresetElene();
	if(elenePreset != 0L)
	{
		for(int i = 0; i < Elene::NUM_EL; i++)
		{
			if(tstatMc->getEleneStat()->getCql()->get(i) < elenePreset->get(i))
			{
				funcOp = EleneStat::FF_OP_IMMATURE;		// 元素量過小である
				break;
			}
		}
	}

	// 元素量エラー
	if(funcOp == EleneStat::FF_OP_IMMATURE)		// 元素過小
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_ELENE_IMMATURE);	// <ERROR> 元素過小（未成熟）
		return false;
	}
	if(funcOp == EleneStat::FF_OP_OVERBREAK)	// 元素過大
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_ELENE_OVERBREAK);	// <ERROR> 元素過大（破裂）
		return false;
	}

	// クラスタ実体化
	u32 funcForm = tstatMc->getFuncFlags() & EleneStat::FF_MASK_FORM;
	if(funcForm == 0)
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_INVALID_FORM);	// <ERROR> 形態不正
	}
	else
	{
		// 実体化
		isHappened = v.cluster()->materialize(funcForm);
	}
	if(!isHappened)
	{
		// 現クラスタ消滅
		MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
		mgcsys->disappearCluster(mcid, MagicCluster::DCF_IMMATURE_SOUMA);
	}
	// ソウマ実体化成功通知
	GameNotifier* gnotif = Game::getGame()->getGameNotifier(); ASSERT(gnotif != 0L);
	gnotif->notifyDischargeSouma(souma, mcid);
	// ソウマシステム内のクラスタの保持を手放す
	souma->releaseCluster(mcid);

	// キャラクタのアニメーション変更
	if(souma->getOwnerUnit()->isCategory(Unit::UNITCAT_CHAR))
	{
		((CharUnit*)souma->ownerUnit())->changeAnim(CharUnit::ANIMID_MAGIC_EMIT_1, false);
		((CharUnit*)souma->ownerUnit())->changeAnim(CharUnit::ANIMID_MAGIC_CHANT_1, true, false, true);	// EMIT の次のアニメを予約
	}
	// SE 再生
	Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_SERVE_MCLUSTER, false, 1.0f, souma->getOwnerUnit()->getCenterPos(), 0L);

	// スピリットへ通知
	souma->ownerSpirit()->notifySoumaAction(Souma::SNOTIFID_SERVE, 0L, 0L);
	if(!isHappened)
	{
		souma->ownerSpirit()->notifySoumaAction(Souma::SNOTIFID_NOT_HAPPENED, 0L, 0L);
	}

	// 戻り値の設定
	valRet->setValue((s32)SoumaScript::RET_OK);	// <OK> 成功
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - クラスタを合成・アイテム化
		SS: awaraizad a synthnm clus o
		JS: synthesize(synthnm clus)
**//*---------------------------------------------------------------------*/
bool SsCluster::callSynthesize(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.cluster() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}
	// 引数の数チェック
	if(numArg < 2)	{	((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_NUM);	return true;	}
	// 引数の型チェック
//	if(!vaArg[0].isString())	{	((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);	return true;	}
	if(!vaArg[1].isObject())	{	((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);	return true;	}

	// 合成メソッドを得る
#if 1
	const VcString* nameSynthMethod;
	if(!vaArg[0].toString(&nameSynthMethod, ctx)) { ((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE); return true; }
#else
	const VcString* nameSynthMethod = vaArg[0].getString();
	if(	!nameSynthMethod->equalsIgnoreCase("motol") &&
		!nameSynthMethod->equalsIgnoreCase("item")	)
#endif
	if(!SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_ITEM, nameSynthMethod, true))
	{
		valRet->setValue(SoumaScript::RET_SYS_ERROR);	// 戻り値の設定
		return false;
	}

	// 対象ユニットを得る
	MagicCluster* clusMul = getMagicCluster(vaArg[1].getObject());
	if(clusMul == 0L)
	{
		valRet->setValue(SoumaScript::RET_SYS_ERROR);	// 戻り値の設定
		return false;
	}

	Souma* souma = ((SoumaScriptContext*)ctx)->getSouma();
	TransStat* tstatMc = v.cluster()->transStat();
	u16 mcidThis = v.cluster()->getId();
	u16 mcidMul = clusMul->getId();

	// クラスタチェック
	if(!souma->checkCluster(mcidThis, true) || !souma->checkCluster(mcidMul, true))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_CLUSTER);
		return false;
	}

	// フォーカスされていない場合は自身がフォーカスしている対象にする
	const FocusArray* focusarr = v.cluster()->getFocusArray();
	if(focusarr->getCount() <= 0)
	{
		Unit* unitTrg = souma->ownerSpirit()->focusUnit();	// 自身のフォーカス対象
		if((unitTrg != 0L) && unitTrg->isEnable())
		{
			v.cluster()->addFocusUnit(unitTrg, false);
		}
	}

	// エネルギー消費
#if 1
	if(!StatusIfEx::wasteSoumaMaterializeEnergy(souma->ownerSpirit()->charStat(), tstatMc->getEleneStat()->getCql(), false))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NE_ENEG);		// <ERROR> エネルギー不足
		return false;
	}
#else
	if(!StatusInfluencer::wasteSoumaMaterializeEnergy(0L, souma->ownerSpirit()->charStat(), tstatMc->getEleneStat()->getCql(), false))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NE_ENEG);		// <ERROR> エネルギー不足
		return false;
	}
	souma->ownerSpirit()->notifySoumaAction(Souma::SNOTIFID_WASTE_ENE, 0L, 0L);	// スピリットへ通知
#endif

	// クラスタ形成確認
	u32 funcOp = tstatMc->getFuncFlags() & EleneStat::FF_MASK_OP;
	if(funcOp == EleneStat::FF_OP_IMMATURE)		// 元素過小
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_ELENE_IMMATURE);		// <ERROR> 元素過小（未成熟）
		return false;
	}
	if(funcOp == EleneStat::FF_OP_OVERBREAK)	// 元素過大
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_ELENE_OVERBREAK);		// <ERROR> 元素過大（破裂）
		return false;
	}

	// クラスタ合成・実体化
	v.cluster()->synthesize(clusMul);
	// ソウマ実体化成功通知
	GameNotifier* gnotif = Game::getGame()->getGameNotifier(); ASSERT(gnotif != 0L);
	gnotif->notifyDischargeSouma(souma, mcidThis);
	// ソウマシステム内のクラスタの保持を手放す
	souma->releaseCluster(mcidThis);
	souma->releaseCluster(mcidMul);
	// クラスタ消滅
	MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
	mgcsys->disappearCluster(mcidThis, MagicCluster::DCF_SELF_VANISH);
	mgcsys->disappearCluster(mcidMul, MagicCluster::DCF_SELF_VANISH);

	// キャラクタのアニメーション変更
	if(souma->getOwnerUnit()->isCategory(Unit::UNITCAT_CHAR))
	{
		((CharUnit*)souma->ownerUnit())->changeAnim(CharUnit::ANIMID_MAGIC_EMIT_1, false);
		((CharUnit*)souma->ownerUnit())->changeAnim(CharUnit::ANIMID_MAGIC_CHANT_1, true, false, true);	// EMIT の次のアニメを予約
	}
	// SE 再生
	Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_SERVE_MCLUSTER, false, 1.0f, souma->getOwnerUnit()->getCenterPos(), 0L);

	// スピリットへ通知
	souma->ownerSpirit()->notifySoumaAction(Souma::SNOTIFID_SERVE, 0L, 0L);

	// 戻り値の設定
	valRet->setValue((s32)SoumaScript::RET_OK);	// <OK> 成功
	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
