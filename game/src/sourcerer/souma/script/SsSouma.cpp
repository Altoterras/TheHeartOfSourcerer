/***********************************************************************//**
 *	SsSouma.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/10/27.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SsSouma.h"

// Friends
#include "SoumaScript.h"
#include "SoumaScriptContext.h"
#include "SoumaScriptUtils.h"
#include "SsCluster.h"
#include "SsConsole.h"
#include "SsUnit.h"
#include "../Souma.h"
#include "../../char/CharDef.h"
#include "../../common/CalcUtils.h"
#include "../../body/Game.h"
#include "../../body/GameNotifier.h"
#include "../../gui/FocusCursor.h"
#include "../../gui/GameGui.h"
#include "../../magic/MagicCluster.h"
#include "../../magic/MagicSys.h"
#include "../../particle/GameParticleDef.h"
#include "../../particle/progptclmdl/implement/FanRangeModel.h"
#include "../../spirit/Spirit.h"
#include "../../status/CharStat.h"
#include "../../status/StatusIfEx.h"
#include "../../status/TransStat.h"
#include "../../unit/UnitManager.h"
#include "../../unit/char/CharUnit.h"

// External
#include "../../../es/EsArray.h"
#include "../../../es/EsContext.h"
#include "../../../es/EsKeyword.h"
#include "../../../es/EsObject.h"
#include "../../../es/EsRunEnv.h"
#include "../../../es/EsScopeProperty.h"
#include "../../../es/EsSysClasses.h"
#include "../../../es/EsSysPrototypes.h"
#include "../../../etk/particle/Particle.h"
#include "../../../etk/particle/ParticleSys.h"
#include "../../../tfw/collection/List.h"
#include "../../../tfw/lib/Matrix.h"
#include "../../../tfw/lib/Vector.h"
#include "../../../tfw/string/StringUtils.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SsSouma::BaseClass メソッド

/*---------------------------------------------------------------------*//**
	ソウマスクリプトクラスの作成
**//*---------------------------------------------------------------------*/
EsObject* SsSouma::FoundationClass::makeClassObject(EsContext* ctx, EsObject* objParent, s32 skwdOfs) const
{
	// クラスオブジェクト作成
	EsObject* objPtype;
	EsObject* objCtor;
	objParent->makeClassObject(&objPtype, &objCtor, ctx, this, 0L, SsSouma::callConstructor);	// ⇒ proto = JS_InitClass(cx, obj, NULL, &js_FunctionClass, Function, 1, function_props, function_methods, NULL, NULL);
	if(objPtype == 0L)
	{
		return 0L;
	}
	// 非静的プロパティを定義する
	///objPtype->defineClassProperty(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_MASTER_OBJ + skwdOfs), callGetMaster, g_funcDummyPropertySetter, -1, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED);
	// 非静的関数を定義する
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_CENTERING + skwdOfs), callCentering, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_MEDITATE + skwdOfs), callMeditate, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_GET_CONSDEPTH + skwdOfs), callGetConsDepth, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_GET_CONSOLE + skwdOfs), callGetConsole, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_CREATE_CLUSTER + skwdOfs), callCreateCluster, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_CREATE_INTENT + skwdOfs), callCreateIntent, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_GET_FOCUS_UNITS + skwdOfs), callGetFocusUnits, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_SET_FOCUS_UNIT + skwdOfs), callSetFocusUnit, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_GET_MASTER_UNIT + skwdOfs), callGetMasterUnit, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_ENUM_UNITS_BY_NAME + skwdOfs), callEnumUnitsByName, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_ENUM_AROUND_UNITS + skwdOfs), callEnumAroundUnits, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_ENUM_RECOGNIZED_UNITS + skwdOfs), callEnumRecognizedUnits, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_ENUM_SPIRIT_KNOWS_UNITS + skwdOfs), callEnumSpiritKnowsUnits, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_WAIT + skwdOfs), callWait, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_SLOW_CHANT + skwdOfs), callSlowChant, 0);

	return objPtype;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SsSouma::FoundationClass::FoundationClass(SoumaScriptKeywords::Keyword kwd)
	: SsBase::BaseClass(kwd, EsClass::F_HAS_PRIVATE, 0)
{
	_funcConstruct = SsSouma::callConstructor;
}


//==========================================================================
// SsSouma メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool SsSouma::init()
{
	EsSysPrototypes::registerExternalPrototype(NsPrototype::getInstance());
	EsSysPrototypes::registerExternalPrototype(EnPrototype::getInstance());
	EsSysClasses::registerExternalClass(NsClass::getInstance(), true);
	EsSysClasses::registerExternalClass(EnClass::getInstance(), true);
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	キーワード言語オフセットを得る
**//*---------------------------------------------------------------------*/
s32 SsSouma::getKeywordLangOffset(const EsObject* objThis)
{
	return (objThis->getClass()->getClassType() == EnClass::getInstance()->getClassType()) ? 1 : 0;
}

/*---------------------------------------------------------------------*//**
	ユニット Es 配列の作成
**//*---------------------------------------------------------------------*/
bool SsSouma::makeUnitEsArray(EsArray** objArray, const List<Unit*>* listUnits, EsContext* ctx, EsObject* objThis)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	return false;	}

	// 配列オブジェクトを作成
	const EsAtom* atomArray = ctx->getKeywordSet()->getStatic(EsKeywordSet::S_CLASS_ARRAY)->getAtom();	// アレイクラスのアトム取得
	if(!ctx->getRunEnv()->makeClassInstance((EsObject**)objArray, atomArray, 0L, 0))
	{
		ASSERT(false);
		return false;
	}

	// ユニット スクリプトクラスインスタンス作成準備
	s32 skwdOfs = getKeywordLangOffset(objThis);
	//const EsAtom* atomUnit = SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_CLASS_NS_UNIT + skwdOfs);	// ユニットクラスのアトム取得

	// 要素の設定
	EsValue* valArrElmTmp = new EsValue[listUnits->getCount()];
	u32 cnt = 0;
	for(ListIterator<Unit*> it = listUnits->iterator(); it.has(); it.next())
	{
		EsObject* objUnit;
		if(!SsUnit::makeEsObject(&objUnit, ((SoumaScriptContext*)ctx), it.object(), skwdOfs))
		{
			delete[] valArrElmTmp;
			return false;
		}
		valArrElmTmp[cnt++].setValue(objUnit);
		ASSERT(cnt <= listUnits->getCount());
	}
	EsArray::setArrayElements(ctx, *objArray, valArrElmTmp, 0, listUnits->getCount(), false);
	delete[] valArrElmTmp;

	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - コンストラクタ
**//*---------------------------------------------------------------------*/
bool SsSouma::callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	EsValue valVar;
	SoumaScriptContext* ssctx = (SoumaScriptContext*)exprm->_paramRun.getObject();
	SsBase::BaseVariables::setVariables(&valVar, ssctx->getSouma());
	objThis->slots()->setPrivateValue(&valVar);

	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 準備（瞑想開始）
		SS: edradin a o
		JS: centering()
**//*---------------------------------------------------------------------*/
bool SsSouma::callCentering(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}

	// 準備
	if(!v.souma()->prepare())		{	((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_PREPARED);	return true;	}

	// 意識集中
	CharStat* cstat = v.souma()->ownerSpirit()->charStat(); ASSERT(cstat != 0L);
	cstat->dianaFrame();

	// 成否を返す
	valRet->setValue((s32)SoumaScript::RET_OK);	// 戻り値の設定
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 瞑想中
		SS: ia a o
		JS: meditate()
**//*---------------------------------------------------------------------*/
bool SsSouma::callMeditate(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}
	if(!v.souma()->isPrepared())	{	((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_PREPARED);	return true;	}

	// 瞑想
	v.souma()->meditate();

	// 集中度を返す
	CharStat* cstat = v.souma()->getOwnerSpirit()->charStat(); ASSERT(cstat != 0L);
	valRet->setValue((s32)cstat->getConcentrateDepth() / CharStat::CONCD_VALID_FIGURE);	// 戻り値の設定
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 術者の意識深さの取得
		SS: getconsnur a o
		JS: getConsDepth()
**//*---------------------------------------------------------------------*/
bool SsSouma::callGetConsDepth(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}
	if(!v.souma()->isPrepared())	{	((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_PREPARED);	return true;	}

	const CharStat* cstat = v.souma()->getOwnerSpirit()->charStat(); ASSERT(cstat != 0L);

	// 集中度を返す
	valRet->setValue((s32)cstat->getConcentrateDepth() / CharStat::CONCD_VALID_FIGURE);	// 戻り値の設定
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - コンソールの取得
		SS: getsola a o
		JS: getConsole()
**//*---------------------------------------------------------------------*/
bool SsSouma::callGetConsole(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}
	if(!v.souma()->isPrepared())	{	((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_PREPARED);	return true;	}

	// コンソール スクリプトクラスインスタンス作成
	EsObject* objConsole;
	if(!SsConsole::makeEsObject(&objConsole, ((SoumaScriptContext*)ctx), getKeywordLangOffset(objThis)))
	{
		valRet->setValue(SoumaScript::RET_SYS_ERROR);
		return false;
	}

	// 返答値にセット
	valRet->setValue(objConsole);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - アートマンから指定原質を持ったクラスタを出現させる
		SS: enluc a ae, wi, fi, wa, so o
		JS: belight(ae, wi, fi, wa, so)
**//*---------------------------------------------------------------------*/
bool SsSouma::callCreateCluster(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false;	}	// ソウマオブジェクトが存在しないのはNG
	if(!v.souma()->isPrepared())	{	((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_PREPARED);	return true;	}

	s32 retval = SoumaScript::RET_OTHER_PROBLEM;
	MagicCluster* mc = 0L;

	// 意識深さチェック
	const Unit* unitOwn = v.souma()->getOwnerUnit();
	const CharStat* cstatOwn = unitOwn->getSpirit()->getCharStat();
	//s32 cons = cstatOwn->getConcentrateDepth();
	if(cstatOwn->getConcentrateDepth() < CharStat::CONCD_THRSLD_TRANCE)
	{
		((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_NE_CONCD);		// ＜失敗＞意識深さが足りない
		return true;
	}

	do
	{
		// 引数の数チェック
		if(numArg != 5)
		{
			retval =  SoumaScript::RET_SC_INVALID_ARG_NUM;
			((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_NUM);
			break;
		}
#if 1
		s32 eeAe, eeWi, eeFi, eeWa, eeSo;
		if(!vaArg[0].toInt32(&eeAe, ctx))	{ retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break; }
		if(!vaArg[1].toInt32(&eeWi, ctx))	{ retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break; }
		if(!vaArg[2].toInt32(&eeFi, ctx))	{ retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break; }
		if(!vaArg[3].toInt32(&eeWa, ctx))	{ retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break; }
		if(!vaArg[4].toInt32(&eeSo, ctx))	{ retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break; }
		eeAe *= Elene::ELENE_VALID_FIGURE;
		eeWi *= Elene::ELENE_VALID_FIGURE;
		eeFi *= Elene::ELENE_VALID_FIGURE;
		eeWa *= Elene::ELENE_VALID_FIGURE;
		eeSo *= Elene::ELENE_VALID_FIGURE;
#else
		// 引数の型チェック
		if(vaArg[0].getType() != EsValue::VTYPE_INT)	{	retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break;	}
		if(vaArg[1].getType() != EsValue::VTYPE_INT)	{	retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break;	}
		if(vaArg[2].getType() != EsValue::VTYPE_INT)	{	retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break;	}
		if(vaArg[3].getType() != EsValue::VTYPE_INT)	{	retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break;	}
		if(vaArg[4].getType() != EsValue::VTYPE_INT)	{	retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break;	}
		// リクエストされた原質値を得る
		s32 eeAe = vaArg[0].getInt() * Elene::ELENE_VALID_FIGURE;	// 空
		s32 eeWi = vaArg[1].getInt() * Elene::ELENE_VALID_FIGURE;	// 風
		s32 eeFi = vaArg[2].getInt() * Elene::ELENE_VALID_FIGURE;	// 火
		s32 eeWa = vaArg[3].getInt() * Elene::ELENE_VALID_FIGURE;	// 水
		s32 eeSo = vaArg[4].getInt() * Elene::ELENE_VALID_FIGURE;	// 地
#endif

		#if 0
			// 増分元素量を得る
			Elene eleneInc;
			if(!v.souma()->getOwnerUnit()->getEvokableElene(&eleneInc, Unit::EVECAT_TOTAL))
			{
				retval = SoumaScript::RET_OTHER_PROBLEM;
				break;
			}
		#endif

		#if 0
			// 能力不足チェック
			if(	(eeAe > eleneMakableOql.getAethel()) ||
				(eeWi > eleneMakableOql.getWind()) ||
				(eeFi > eleneMakableOql.getFire()) ||
				(eeWa > eleneMakableOql.getWater()) ||
				(eeSo > eleneMakableOql.getSoil())	)
			{
				((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NE_ABILITY);	// ＜失敗＞作成能力が不足している
				return false;
			}
		#endif

		// クラスタ作成
		Elene eleneMakeOql(eeSo, eeWa, eeFi, eeWi, eeAe);
		u16 mcid;
		retval = v.souma()->makeCluster(&mcid, &eleneMakeOql, 0L, 0);
		if(retval != SoumaScript::RET_OK)
		{
			break;
		}
		MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
		mc = mgcsys->getCluster(mcid);
	}
	while(false);

	// エラーメッセージ
	if(retval != SoumaScript::RET_OK)
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, retval);
	}

	// クラスター スクリプトクラスインスタンス作成
	EsObject* objCluster;
	if(!SsCluster::makeEsObject(&objCluster, ((SoumaScriptContext*)ctx), mc, getKeywordLangOffset(objThis)))
	{
		valRet->setValue(SoumaScript::RET_SYS_ERROR);
		return false;
	}

	// 返答値にセット
	valRet->setValue(objCluster);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - アートマンから指定意図を持ったクラスタを出現させる [2014/09/08 new]
		SS: enwadna a form, intent, force, atrb, flags o
		JS: enword(form, intent, force, atrb, flags)
**//*---------------------------------------------------------------------*/
bool SsSouma::callCreateIntent(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if (v.souma() == 0L)	{ ((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false; }	// ソウマオブジェクトが存在しないのはNG
	if (!v.souma()->isPrepared())	{ ((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_PREPARED);	return true; }

	s32 retval = SoumaScript::RET_OTHER_PROBLEM;
	MagicCluster* mc = 0L;

	// 意識深さチェック
	const Unit* unitOwn = v.souma()->getOwnerUnit();
	const CharStat* cstatOwn = unitOwn->getSpirit()->getCharStat();
	//s32 cons = cstatOwn->getConcentrateDepth();
	if (cstatOwn->getConcentrateDepth() < CharStat::CONCD_THRSLD_TRANCE)
	{
		((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_NE_CONCD);		// ＜失敗＞意識深さが足りない
		return true;
	}

	do
	{
		VcString strForm, strIntent, strPower, strAtrb, strFlags;

		// 引数チェック
		if (numArg < 3)
		{
			retval = SoumaScript::RET_SC_INVALID_ARG_NUM;
			((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_NUM);
			break;
		}
		else
		{
#if 1
			if(!vaArg[0].toString(&strForm, ctx))	{ retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break; }
			if(!vaArg[1].toString(&strIntent, ctx))	{ retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break; }
			if(!vaArg[2].toString(&strPower, ctx))	{ retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break; }
			if(numArg >= 4)
			{
				vaArg[3].toString(&strAtrb, ctx);
				if (numArg >= 5)
				{
					vaArg[4].toString(&strFlags, ctx);
				}
			}
#else
			if(!vaArg[0].isString())	{ retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break; }
			strIntent = *vaArg[0].getString();
			if (vaArg[1].getType() != EsValue::VTYPE_STRING)	{ retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break; }
			strForce = *vaArg[1].getString();
			if (vaArg[2].getType() != EsValue::VTYPE_STRING)	{ retval = SoumaScript::RET_SC_INVALID_ARG_TYPE;	break; }
			strForm = *vaArg[2].getString();
			if (numArg >= 4)
			{
				const VcString* pstr = vaArg[3].getString();
				strAtrb = (pstr != 0L) ? *pstr : "";
				if (numArg >= 5)
				{
					pstr = vaArg[4].getString();
					strFlags = (pstr != 0L) ? *pstr : "";
				}
			}
#endif
		}

		// 形態
		u32 form = 0;
		switch(SoumaScriptUtils::matchElementNameKeyword(&strForm, true))
		{
		case Elene::EL_SO: form = EleneStat::FF_FORM_SOIL; break;
		case Elene::EL_WA: form = EleneStat::FF_FORM_WATER; break;
		case Elene::EL_FI: form = EleneStat::FF_FORM_FIRE; break;
		case Elene::EL_WI: form = EleneStat::FF_FORM_WIND; break;
		}
		if(form == 0)
		{
			((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_INVALID_FORM);	// <ERROR> 形態不正
			break;
		}

		// 基本機能値
		u32 op = SoumaScriptUtils::convOperationFlagFromString(&strIntent);
		if(op == 0)
		{
			((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_INVALID_FORM);	// <ERROR> 形態不正 - 仮【2014/10/02 r-kishi】
			break;
		}

		// エネルギー量
		s32 energy = 0;
		if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_WEAK, &strPower, true))	// 弱
		{
			energy = cstatOwn->getEnergy() / 10;
		}
		else if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_MIDDLE, &strPower, true))	// 中
		{
			energy = cstatOwn->getEnergy() / 4;
		}
		else if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_STRONG, &strPower, true))	// 強
		{
			energy = cstatOwn->getEnergy() / 2;
		}
		else if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_MIGHTY, &strPower, true))	// 強大
		{
			energy = cstatOwn->getEnergy() * 3 / 4;
		}
		else
		{
			VcStringArray strarrPowers = StringUtils::split(&strPower, ' ');
			if(strarrPowers.getCount() >= 2)
			{
				if(SoumaScriptUtils::matchPointOrPointsKeyword(strarrPowers.get(1), true))	// "point" or "points"
				{
					energy = StringUtils::toInteger(strarrPowers.get(0), 10);
				}
			}
		}
		if(energy == 0)
		{
			((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_INVALID_FORM);	// <ERROR> 形態不正 - 仮【2014/10/02 r-kishi】
			break;
		}

		// 属性フラグ
		u32 atrb = 0;
		VcStringArray strarrAtrbs = StringUtils::split(&strAtrb, ' ');
		for (int i = 0; i < strarrAtrbs.getCount(); i++)
		{
			switch(SoumaScriptUtils::matchElementNameKeyword(strarrAtrbs.get(i), true))
			{
			case Elene::EL_SO: atrb |= EleneStat::FF_ATRB_SOIL; break;
			case Elene::EL_WA: atrb |= EleneStat::FF_ATRB_WATER; break;
			case Elene::EL_FI: atrb |= EleneStat::FF_ATRB_FIRE; break;
			case Elene::EL_WI: atrb |= EleneStat::FF_ATRB_WIND; break;
			case Elene::EL_AE: atrb |= EleneStat::FF_ATRB_AETHEL; break;
			}
		}

		// 汎用フラグ
		u32 flags = 0;
		VcStringArray strarrFlags = StringUtils::split(&strFlags, ' ');
		for (int i = 0; i < strarrAtrbs.getCount(); i++)
		{
		}

		// 元素バランスを機能フラグから逆算する
		u32 funcflags = op | form | atrb | flags;
		Elene eleneMakeNql;
		Elene eleneMakeCql;
		SoumaScriptUtils::computeEleneBalanceFromIntent(&eleneMakeNql, &eleneMakeCql, funcflags, energy);

		// クラスタ作成
		u16 mcid;
		retval = v.souma()->makeCluster(&mcid, &eleneMakeNql, &eleneMakeCql, funcflags);
		if (retval != SoumaScript::RET_OK)
		{
			break;
		}
		MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
		mc = mgcsys->getCluster(mcid);
	}
	while (false);

	// エラーメッセージ
	if (retval != SoumaScript::RET_OK)
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, retval);
	}

	// クラスター スクリプトクラスインスタンス作成
	EsObject* objCluster;
	if (!SsCluster::makeEsObject(&objCluster, ((SoumaScriptContext*)ctx), mc, getKeywordLangOffset(objThis)))
	{
		valRet->setValue(SoumaScript::RET_SYS_ERROR);
		return false;
	}

	// 返答値にセット
	valRet->setValue(objCluster);
	return true;
}

#if 0
/*---------------------------------------------------------------------*//**
	Es 関数 - 指定された機能とエネルギー量から原質と目標元素量を算出したクラスタを出現させる
		SS: enlucpreset a func, ene o
		JS: belightPreset(func, ene)
**//*---------------------------------------------------------------------*/
bool SsSouma::callPreset(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if (v.souma() == 0L)	{ ((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return false; }
	if (!v.souma()->isPrepared())	{ ((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_PREPARED);	return true; }

	s32 retval = SoumaScript::RET_OTHER_PROBLEM;
	MagicCluster* mc = 0L;

	// 意識深さチェック
	const Unit* unitOwn = v.souma()->getOwnerUnit();
	const CharStat* cstatOwn = unitOwn->getSpirit()->getCharStat();
	//s32 cons = cstatOwn->getConcentrateDepth();
	if (cstatOwn->getConcentrateDepth() < CharStat::CONCD_THRSLD_TRANCE)
	{
		((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_NE_CONCD);		// ＜失敗＞意識深さが足りない
		return true;
	}

	// 引数チェック
	if (numArg != 2)
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_NUM);
		return true;
	}
	if (!vaArg[0].isNumber())
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);
		return true;
	}
	if (!vaArg[1].isNumber())
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);
		return true;
	}

	// 条件を得る
	s32 flagFunc = vaArg[0].getInt();
	u32 qfSpec = EleneStat::calcOperationFlagToQualityFlags(flagFunc);
	u32 qfExctSpec = qfSpec & EleneStat::QF_MASK_EXCT;
	u32 qfDullSpec = qfSpec & EleneStat::QF_MASK_DULL;
	s32 eneTotalSpec = vaArg[1].getInt();
	f32 eneEqOneF = eneTotalSpec * 0.2f;	// ÷5
	s32 eneEqOneI = (s32)eneEqOneF;

	// 試し変化量を設定する
	s32 eeDelta = eneEqOneI / 100;
	if (eeDelta <= 0)	{ eeDelta = 1; }

	// 元素バランスを作成する
	Elene eleneNql(eneEqOneI, eneEqOneI, eneEqOneI, eneEqOneI, eneEqOneI);
	Elene eleneCql;
	bool isCalcBalanceEnd = false;
	for (int safecnt = 0; safecnt < 100; safecnt++)
	{
		eleneCql.copy(&eleneNql);
		s32 eneRest = eneTotalSpec - eleneCql.sum();

		do
		{
			u32 qfWk = EleneStat::calcQualityFlags(0L, 0L, 0L, &eleneCql, &eleneNql);
			if (qfWk == qfSpec)	// 指定の質を作成できた
			{
				isCalcBalanceEnd = true;
				break;
			}

			// 余りエネルギー（eneRest）分で調整
			if (eneRest <= 0)
			{
				break;	// 調整できず
			}
			if ((EleneStat::QF_PURE_1 <= qfSpec) && (qfSpec <= EleneStat::QF_PURE_3))
			{
				eleneCql.addSoil(eeDelta);	// 地を増悪させて純性度を落とす
			}
			else
			{
				u32 qfExctWk = qfWk & EleneStat::QF_MASK_EXCT;
				if (qfExctWk != qfExctSpec)
				{
					if ((EleneStat::QF_EXCT_1 <= qfExctSpec) && (qfExctSpec <= EleneStat::QF_EXCT_3))
					{
						eleneCql.addFire(eeDelta);		// 火を増悪させて激性度を上げる
					}
				}
				else
				{
					u32 qfDullWk = qfWk & EleneStat::QF_MASK_DULL;
					if (qfDullWk != qfDullSpec)
					{
						if ((EleneStat::QF_DULL_1 <= qfDullSpec) && (qfDullSpec <= EleneStat::QF_DULL_3))
						{
							eleneCql.addSoil(eeDelta);	// 地を増悪させて鈍性度を上げる
						}
					}
				}
			}
			eneRest--;
		} while (true);
		if (isCalcBalanceEnd)	{ break; }	// バランス確定済み

		// 全体の原質を下げて調整しやすくする
		eleneNql.subSoil(eeDelta);
		eleneNql.subWater(eeDelta);
		eleneNql.subFire(eeDelta);
		eleneNql.subWind(eeDelta);
		eleneNql.subAethel(eeDelta);
		// 激性が求められている場合は、原質の火を落として調整しやすくする
		if ((EleneStat::QF_EXCT_1 <= qfExctSpec) && (qfExctSpec <= EleneStat::QF_EXCT_3))
		{
			eleneNql.subFire(eeDelta);
		}
		// 鈍性が求められている場合は、原質の地を落として調整しやすくする
		if ((EleneStat::QF_DULL_1 <= qfDullSpec) && (qfDullSpec <= EleneStat::QF_DULL_3))
		{
			eleneNql.subSoil(eeDelta);
		}
	}

	if (!isCalcBalanceEnd)
	{
		// 設定失敗
		retval = SoumaScript::RET_ELENE_BALANCE_FAILURE;	// <ERROR> 求められた元素バランスが成立しなかった
	}
	else
	{
		// クラスタ作成
		u16 mcid;
		retval = v.souma()->makeCluster(&mcid, &eleneNql);
		if (retval == SoumaScript::RET_OK)
		{
			MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
			mc = mgcsys->getCluster(mcid);
			// プリセット目標元素量を設定する
			mc->setPresetElene(&eleneCql);
		}
	}

	// 失敗時はエラーメッセージ設定
	if (retval != SoumaScript::RET_OK)
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, retval);
	}

	// クラスター スクリプトクラスインスタンス作成
	EsObject* objCluster;
	if (!SsCluster::makeEsObject(&objCluster, ((SoumaScriptContext*)ctx), mc, getKeywordLangOffset(objThis)))
	{
		valRet->setValue(SoumaScript::RET_SYS_ERROR);
		return false;
	}

	// 返答値にセット
	valRet->setValue(objCluster);
	return true;
}
#endif

/*---------------------------------------------------------------------*//**
	Es 関数 - 現在のフォーカスを得る
		SS: getfocmono a o
		JS: getFocusUnits
**//*---------------------------------------------------------------------*/
bool SsSouma::callGetFocusUnits(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
///	if(!v.souma()->isPrepared())	{	((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_PREPARED);	return true;	}

	// 範囲内のキャラクタユニットをリストアップする（現仕様では１つ）
	List<Unit*> listUnits;
	{
		// フォーカス取得
		GameGui* gui = Game::getGame()->getGui();	ASSERT(gui != 0L);
		FocusCursor* fccsr = gui->getFocusCursor(); ASSERT(fccsr != 0L);
		Unit* unitFocused = fccsr->getFocus(0);
		if((unitFocused != 0L) && unitFocused->isEnable() && unitFocused->isCategory(Unit::UNITCAT_CHAR))
		{
			listUnits.addTail(unitFocused);
		}
	}

	// 配列オブジェクトを作成
	EsArray* objArray;
	if(!makeUnitEsArray(&objArray, &listUnits, ctx, objThis))
	{
		ASSERT(false);
		valRet->setValue(SoumaScript::RET_SYS_ERROR);
		return false;
	}

	// 返答値にセット
	valRet->setValue(objArray);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - フォーカスを変更する
		SS: setfocmono a unit o
		JS: setFocusUnit(unit)
**//*---------------------------------------------------------------------*/
bool SsSouma::callSetFocusUnit(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
///	if(!v.souma()->isPrepared())	{	((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_PREPARED);	return true;	}

	// 引数の数チェック
	if(numArg != 1)
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_NUM);	// <ERROR> 引数の数が不正
		return true;
	}
	// 引数の型チェック
	if(vaArg[0].getType() != EsValue::VTYPE_OBJ)
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);	// <ERROR> 引数の型が不正
		return true;
	}

	// ユニットを得る（以前は整数値ハンドルからユニットを取得する形式だったが、オブジェクトから直接取得する形に変わった【2014/09/27 r-kishi】）
	Unit* unit = SsUnit::getUnit(vaArg[0].getObject());
	if(unit == 0L)
	{
		valRet->setValue(SoumaScript::RET_SYS_ERROR);	// 戻り値の設定
		return true;	// 失敗ではない
	}

	// フォーカスセット
	GameGui* gui = Game::getGame()->getGui();	ASSERT(gui != 0L);
	FocusCursor* fccsr = gui->getFocusCursor(); ASSERT(fccsr != 0L);
	fccsr->setFocus(0, unit);

	// 戻り値の設定
	valRet->setValue((s32)SoumaScript::RET_OK);	// <OK> 成功
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 唱者を得る
		SS: getorthormono a o
		JS: getMasterUnit()
**//*---------------------------------------------------------------------*/
bool SsSouma::callGetMasterUnit(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}

	// ユニット スクリプトクラスインスタンス作成
	EsObject* objUnit;
	if(!SsUnit::makeEsObject(&objUnit, ((SoumaScriptContext*)ctx), v.souma()->ownerUnit(), getKeywordLangOffset(objThis)))
	{
		valRet->setValue(SoumaScript::RET_SYS_ERROR);
		return false;
	}

	// 返答値にセット
	valRet->setValue(objUnit);
	return true;

#if 0	// eval ライク
	EsStackFrame* sfrmCaller = ctx->getScriptedCallerFrame();

	EsObject* objScope = 0L;
	if(sfrmCaller != 0L)
	{
		objScope = sfrmCaller->makeScopeChainObject(ctx);	// ⇒ parent = js_GetScopeChain(cx, caller);
		if(objScope == 0L)
		{
			return false;
		}
	}

	VcString strScript("function(){ return new EnUnit; }");

	// スクリプト解析
	Es es;
	if(!es.parse(ctx, &strScript, sfrmCaller, EsBytecodeBuilder::BF_OPT_NAME_REF_ON_SCOPE))	// ⇒ JSCompiler::compileScript(cx, scopeobj, caller, principals, tcflags, JSSTRING_CHARS(str), JSSTRING_LENGTH(str), NULL, file, line, str);
	{
		return false;
	}

	if(sfrmCaller != 0L)
	{
		objScope = sfrmCaller->getScopeChainObject();
	}

	// ※※※ 実装保留 ※※※ ⇒ js_CheckPrincipalsAccess

	// 実行開始
	if(!es.prepareRun(ctx, sfrmCaller, EsStackFrame::F_EVAL, val, ctx->getRunParameter()))	// ⇒ js_Execute(cx, scopeobj, script, caller, JSFRAME_EVAL, rval);
	{
		return false;
	}

	// 返り値を実行未終了値に設定する
	val->setUnfinished();
#endif
}

/*---------------------------------------------------------------------*//**
	Es 関数 - ユニットを得る
		SS: enummonoparesta a name o
		JS: enumUnitsByName(name)
**//*---------------------------------------------------------------------*/
bool SsSouma::callEnumUnitsByName(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// 引数を得る
	const VcString* nameArg = 0L;
	if((numArg > 0) && vaArg[0].isString())
	{
		nameArg = vaArg[0].getString();
	}

	// ユニット検索
	List<Unit*> listUnits;
	if(nameArg != 0L)
	{
		UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
		for(s32 i = 0; i < unitmng->getUnitNum(); i++)
		{
			Unit* unitWk = unitmng->getUnitFromIndex(i);
			if(unitWk->isEnable() && unitWk->isCategory(Unit::UNITCAT_CHAR))
			{
				const CharDef* cdef = ((CharUnit*)unitWk)->getCharDef();
				if(cdef != 0L)
				{
					for(s32 j = 0; j < NUM_LANGID; j++)
					{
						if(cdef->getName((LangId)j)->equalsIgnoreCase(nameArg))
						{
							listUnits.addTail(unitWk);
							break;	// j のループは抜ける
						}
					}
				}
			}
		}
	}

	// 配列オブジェクトを作成
	EsArray* objArray;
	if(!makeUnitEsArray(&objArray, &listUnits, ctx, objThis))
	{
		ASSERT(false);
		valRet->setValue(SoumaScript::RET_SYS_ERROR);
		return false;
	}

	// 返答値にセット
	valRet->setValue(objArray);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 周囲のユニットを得る
		SS: enumosmono a o
		JS: enumAroundUnits()
**//*---------------------------------------------------------------------*/
bool SsSouma::callEnumAroundUnits(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
///	if(!v.souma()->isPrepared())	{	((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_PREPARED);	return false;	}

	// 有効範囲の二乗を得る
	f32 lenEffctv = v.souma()->getOwnerSpirit()->getCharStat()->getEleneStat()->getCql()->getEffectiveRange();
	f32 lenEffctvSq = lenEffctv * lenEffctv;

	// エネルギー消費
	s32 lfeneCost = (s32)lenEffctv;
#if 1
	if(!StatusIfEx::wasteSoumaFunctionEnergy(v.souma()->ownerSpirit()->charStat(), lfeneCost, false))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NE_ENEG);		// <ERROR> エネルギー不足
		return true;
	}
#else
	if(!StatusInfluencer::wasteSoumaFunctionEnergy(0L, v.souma()->ownerSpirit()->charStat(), lfeneCost, false))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NE_ENEG);		// <ERROR> エネルギー不足
		return true;
	}
	v.souma()->ownerSpirit()->notifySoumaAction(Souma::SNOTIFID_WASTE_ENE, 0L, 0L);	// スピリットへ通知
#endif

	// 範囲内のユニットをリストアップする
	List<Unit*> listUnits;
	CalcUtils::collectUnitRoundRange(&listUnits, v.souma()->getOwnerUnit()->getPosition(), Unit::UNITFLAG_CHAR | Unit::UNITFLAG_PUT_ITEM, lenEffctvSq);

	// 配列オブジェクトを作成
	EsArray* objArray;
	if(!makeUnitEsArray(&objArray, &listUnits, ctx, objThis))
	{
		ASSERT(false);
		valRet->setValue(SoumaScript::RET_SYS_ERROR);
		return false;
	}

	// エフェクト表示
	ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
	u16 ptclid = ptclsys->appearParticle(GameParticleDef::PTCLK_PROG_FAN_RANGE, v.souma()->getOwnerUnit()->getPosition(), 0);
	if(ptclid != 0)
	{
		Particle* ptcl = ptclsys->getParticle(ptclid);	ASSERT(ptcl != 0L);
		FanRangeModel* mdl = (FanRangeModel*)ptcl->getModel();	ASSERT(mdl != 0L);
		mdl->setConfig(lenEffctv, TFW_DPI, 0.0f, 16, 30);
	}

	// 返答値にセット
	valRet->setValue(objArray);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 認識できる（扇状の周囲の）ユニットを得る
		SS: enumanekmono a dir o
		JS: enumRecognizedUnits(dir)
**//*---------------------------------------------------------------------*/
bool SsSouma::callEnumRecognizedUnits(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
///	if(!v.souma()->isPrepared())	{	((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NO_PREPARED);	return false;	}

	// 引数を得る
	f32 angleDir = 0.0f;
	if((numArg > 0) && vaArg[0].isNumber())
	{
		angleDir = (f32)vaArg[0].getNumber() * TFW_PI / 180.0f;
	}

	// 有効範囲の二乗を得る
	f32 lenEffctv = v.souma()->getOwnerSpirit()->getCharStat()->getEleneStat()->getCql()->getEffectiveRange();
	f32 lenEffctvSq = lenEffctv * lenEffctv;

	// エネルギー消費
	s32 lfeneCost = (s32)lenEffctv / 4;
#if 1
	if(!StatusIfEx::wasteSoumaFunctionEnergy(v.souma()->ownerSpirit()->charStat(), lfeneCost, false))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NE_ENEG);		// <ERROR> エネルギー不足
		return true;
	}
#else
	if(!StatusInfluencer::wasteSoumaFunctionEnergy(0L, v.souma()->ownerSpirit()->charStat(), lfeneCost, false))
	{
		((SoumaScriptContext*)ctx)->errorMistake(valRet, SoumaScript::RET_NE_ENEG);		// <ERROR> エネルギー不足
		return true;
	}
	v.souma()->ownerSpirit()->notifySoumaAction(Souma::SNOTIFID_WASTE_ENE, 0L, 0L);	// スピリットへ通知
#endif

	/*
	// 範囲判定用の向きベクトルを算出する
	Vector3F dirRange(0.0f, 0.0f, 1.0f);
	const Vector3F* dirChar = v.souma()->getOwnerUnit()->getDirection();
	angleDir += dirChar->y();
	Matrix4F mtxRot;
	mtxRot.rotateY(angleDir);
	mtxRot.mult(&dirRange, &dirRange);
	*/

	// 範囲内のユニットをリストアップする
	List<Unit*> listUnits;
	angleDir += v.souma()->getOwnerUnit()->getDirection()->y();
	CalcUtils::collectUnitFanRange(
		&listUnits,
		v.souma()->getOwnerUnit()->getPosition(),
		angleDir,
		Unit::UNITFLAG_CHAR | Unit::UNITFLAG_PUT_ITEM,
		lenEffctvSq, TFW_COS45	);	// ±45°以内

	// 配列オブジェクトを作成
	EsArray* objArray;
	if(!makeUnitEsArray(&objArray, &listUnits, ctx, objThis))
	{
		ASSERT(false);
		valRet->setValue(SoumaScript::RET_SYS_ERROR);
		return false;
	}

	// エフェクト表示
	ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
	u16 ptclid = ptclsys->appearParticle(GameParticleDef::PTCLK_PROG_FAN_RANGE, v.souma()->getOwnerUnit()->getPosition(), 0);
	if(ptclid != 0)
	{
		Particle* ptcl = ptclsys->getParticle(ptclid);	ASSERT(ptcl != 0L);
		FanRangeModel* mdl = (FanRangeModel*)ptcl->getModel();	ASSERT(mdl != 0L);
		mdl->setConfig(lenEffctv, TFW_HPI, TFW_HPI - angleDir, 8, 120);
	}

	// 返答値にセット
	valRet->setValue(objArray);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 聖霊の知る対象ユニットを得る
		SS: enumfaesirumono a o
		JS: enumSpiritKnowsUnits()
**//*---------------------------------------------------------------------*/
bool SsSouma::callEnumSpiritKnowsUnits(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}

	// 範囲内のキャラクタユニットをリストアップする（現仕様では１つ）
	List<Unit*> listUnits;
	{
		// 無意識フォーカスユニットを取得
		GameNotifier* gnotif = Game::getGame()->getGameNotifier(); ASSERT(gnotif != 0L);
		Unit* unitKnows = 0L;
		if(!gnotif->notifyFindUnconsciousFocus(&unitKnows))
		{
			// 見つからなかった
			unitKnows = v.souma()->ownerUnit();	// 術者自身
		}
		if(unitKnows->isEnable())
		{
			listUnits.addTail(unitKnows);
		}
	}

	// 配列オブジェクトを作成
	EsArray* objArray;
	if(!makeUnitEsArray(&objArray, &listUnits, ctx, objThis))
	{
		ASSERT(false);
		valRet->setValue(SoumaScript::RET_SYS_ERROR);
		return false;
	}

	// 返答値にセット
	valRet->setValue(objArray);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 数秒待つ
		SS: deri a sec o
		JS: wait(sec)
**//*---------------------------------------------------------------------*/
bool SsSouma::callWait(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{ ((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true; }

	// 引数を得る
	f32 frmWait = 1.0f * FRAMERATE;	// デフォルト 1 秒
	if((numArg > 0) && vaArg[0].isNumber())
	{
		frmWait = (f32)vaArg[0].getNumber() * FRAMERATE;
	}

	// 待機を設定する
	v.souma()->wait(frmWait);

	// 戻り値の設定
	valRet->setValue((s32)SoumaScript::RET_OK);	// <OK> 成功
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 詠唱を遅くする
		SS: tarudlina a o
		JS: slowChant()
**//*---------------------------------------------------------------------*/
bool SsSouma::callSlowChant(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.souma() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}

	// 詠唱を遅くする
	v.souma()->slowChant();

	// 戻り値の設定
	valRet->setValue((s32)SoumaScript::RET_OK);	// <OK> 成功
	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
