/***********************************************************************//**
 *	SsUnit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/11/09.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SsUnit.h"

// Friends
#include "SoumaScript.h"
#include "SoumaScriptContext.h"
#include "SoumaScriptKeywords.h"
#include "../Souma.h"
#include "../../item/ItemEquip.h"
#include "../../unit/char/pc/PcUnit.h"
#include "../../status/CharStat.h"
#include "../../status/EquipPlace.h"

// External
#include "../../../es/EsContext.h"
#include "../../../es/EsObject.h"
#include "../../../es/EsScopeProperty.h"
#include "../../../es/EsSysClasses.h"
#include "../../../es/EsSysPrototypes.h"
#include "../../../es/EsRunEnv.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SsUnit::UnitClass メソッド

/*---------------------------------------------------------------------*//**
	ソウマスクリプトクラスの作成
**//*---------------------------------------------------------------------*/
EsObject* SsUnit::FoundationClass::makeClassObject(EsContext* ctx, EsObject* objParent, s32 skwdOfs) const
{
	// クラスオブジェクト作成
	EsObject* objPtype;
	EsObject* objCtor;
	objParent->makeClassObject(&objPtype, &objCtor, ctx, this, 0L, SsUnit::callConstructor);	// ⇒ proto = JS_InitClass(cx, obj, NULL, &js_FunctionClass, Function, 1, function_props, function_methods, NULL, NULL);
	if(objPtype == 0L)
	{
		return 0L;
	}
	// 非静的プロパティを定義する
	///objPtype->defineClassProperty(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_CONSDEPTH + skwdOfs), SsUnit::callGetConsdepth, g_funcDummyPropertySetter, -1, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED);
	// 非静的関数を定義する
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_GET_CONSDEPTH + skwdOfs), callGetConsdepth, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_GET_LIFE_ENERGY + skwdOfs), callGetLifeEnergy, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_GET_MAX_LIFE_ENERGY + skwdOfs), callGetMaxLifeEnergy, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_GET_ENERGY_DAMAGE + skwdOfs), callGetEnergyDamage, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_GET_PHYSICAL_DAMAGE + skwdOfs), callGetPhysicalDamage, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_IS_ENEMY + skwdOfs), callIsEnemy, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_IS_FRIEND + skwdOfs), callIsFriend, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_IS_ITEM + skwdOfs), callIsItem, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_IS_PUT_ITEM + skwdOfs), callIsPutItem, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_IS_MASTER + skwdOfs), callIsMaster, 0);
	objPtype->defineClassFunction(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_FUNCP_NS_GET_EQUIPED_UNIT + skwdOfs), callGetEquipedUnit, 0);
	// 静的プロパティを定義する
	objCtor->defineClassProperty(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_CONSTP_NS_ACCESSORY + skwdOfs), callAccessory, g_funcDummyPropertySetter, -1, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED);
	objCtor->defineClassProperty(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_CONSTP_NS_LEFT_HAND + skwdOfs), callLeftHand, g_funcDummyPropertySetter, -1, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED);
	objCtor->defineClassProperty(ctx, SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_CONSTP_NS_RIGHT_HAND + skwdOfs), callRightHand, g_funcDummyPropertySetter, -1, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED);

	return objPtype;
}

/*---------------------------------------------------------------------*//**
	ソウマスクリプトクラスのコンストラクタ
**//*---------------------------------------------------------------------*/
SsUnit::FoundationClass::FoundationClass(SoumaScriptKeywords::Keyword kwd)
	: SsBase::BaseClass(kwd, EsClass::F_HAS_PRIVATE, 0)
{
	_funcConstruct = SsUnit::callConstructor;
}


//==========================================================================
// SsUnit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool SsUnit::init()
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
bool SsUnit::makeEsObject(EsObject** objUnit, SoumaScriptContext* ssctx, Unit* unit, s32 skwdOfs)
{
	const EsAtom* atom = SoumaScriptKeywords::getKeyword(SoumaScriptKeywords::S_CLASS_NS_UNIT + skwdOfs);	// アトム取得
	EsValue valArg;
	SsBase::BaseVariables::setVariables(&valArg, unit);
	if(!ssctx->getEsContext()->getRunEnv()->makeClassInstance(objUnit, atom, &valArg, 1))
	{
		ASSERT(false);
		return false;
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	Es オブジェクトからユニット取得
**//*---------------------------------------------------------------------*/
Unit* SsUnit::getUnit(EsObject* objUnit)
{
	const EsClass* cls = objUnit->getClass();
	if((cls != NsClass::getInstance()) && (cls != EnClass::getInstance()))
	{
		return 0L;
	}
	Variables v(objUnit->slots()->getPrivateValue());
	return v.unit();
}

/*---------------------------------------------------------------------*//**
	キーワード言語オフセットを得る
**//*---------------------------------------------------------------------*/
s32 SsUnit::getKeywordLangOffset(const EsObject* objThis)
{
	return (objThis->getClass()->getClassType() == EnClass::getInstance()->getClassType()) ? 1 : 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	Es 関数 - コンストラクタ
**//*---------------------------------------------------------------------*/
bool SsUnit::callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis->slots()->setPrivateValue((numArg > 0) ? &vaArg[0] : 0L);

	return true;
}

/*---------------------------------------------------------------------*//**
	Es アクセサ - アクセサリ
		asorio / accessory
**//*---------------------------------------------------------------------*/
bool SsUnit::callAccessory(EsValue* valRet, EsContext* ctx, EsObject* objThis, const EsValue* id)
{
	valRet->setValue((s32)EQPC_ACCSR);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es アクセサ - 左手
		hara / leftHand
**//*---------------------------------------------------------------------*/
bool SsUnit::callLeftHand(EsValue* valRet, EsContext* ctx, EsObject* objThis, const EsValue* id)
{
	valRet->setValue((s32)EQPC_LHAND);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es アクセサ - 右手
		fura / rightHand
**//*---------------------------------------------------------------------*/
bool SsUnit::callRightHand(EsValue* valRet, EsContext* ctx, EsObject* objThis, const EsValue* id)
{
	valRet->setValue((s32)EQPC_RHAND);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - ユニットの意識深さを得る
		getconsnur / getConsDepth
**//*---------------------------------------------------------------------*/
bool SsUnit::callGetConsdepth(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.unit() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// 意識深さを取得、返答値に設定する
	if(v.unit()->isCategory(Unit::UNITCAT_CHAR))
	{
		valRet->setValue(((CharUnit*)v.unit())->getCharStat()->getConcentrateDepth() / CharStat::CONCD_VALID_FIGURE);
	}
	else
	{
		valRet->setValue((s32)0);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - ユニットの残り生命エネルギー量を得る
		getcuilna / getLifeEnergy
**//*---------------------------------------------------------------------*/
bool SsUnit::callGetLifeEnergy(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.unit() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// 残り生命エネルギーを取得、返答値に設定する
	if(v.unit()->isCategory(Unit::UNITCAT_CHAR))
	{
		valRet->setValue(((CharUnit*)v.unit())->getCharStat()->getEnergy());
	}
	else
	{
		valRet->setValue((s32)0);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - ユニットの最大生命エネルギー量を得る
		getgemcuilna / getMaxLifeEnergy
**//*---------------------------------------------------------------------*/
bool SsUnit::callGetMaxLifeEnergy(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.unit() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// 最大生命エネルギーを取得、返答値に設定する
	if(v.unit()->isCategory(Unit::UNITCAT_CHAR))
	{
		valRet->setValue(((CharUnit*)v.unit())->getCharStat()->getMaxEnergy());
	}
	else
	{
		valRet->setValue((s32)0);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - ユニットのエネルギーダメージ量を得る
		getnadano / getEnergyDamage
**//*---------------------------------------------------------------------*/
bool SsUnit::callGetEnergyDamage(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.unit() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// 最大生命エネルギーを取得、返答値に設定する
	if(v.unit()->isCategory(Unit::UNITCAT_CHAR))
	{
		valRet->setValue(((CharUnit*)v.unit())->getCharStat()->getEnergyDamage());
	}
	else
	{
		valRet->setValue((s32)0);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - ユニットの物理ダメージ量を得る
		getmonotudano / getPhysicalDamage
**//*---------------------------------------------------------------------*/
bool SsUnit::callGetPhysicalDamage(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.unit() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// 最大生命エネルギーを取得、返答値に設定する
	if(v.unit()->isCategory(Unit::UNITCAT_CHAR))
	{
		valRet->setValue(((CharUnit*)v.unit())->getCharStat()->getPhysicsDamage());
	}
	else
	{
		valRet->setValue((s32)0);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - ユニットは敵である
		hagoth / isEnemy
**//*---------------------------------------------------------------------*/
bool SsUnit::callIsEnemy(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.unit() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// 敵かどうかを取得、返答値に設定する
	valRet->setValue(v.unit()->getUnitType() == UNITTYPE_ENEMY_CHAR);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - ユニットは味方である
		hamelon / isFriend
**//*---------------------------------------------------------------------*/
bool SsUnit::callIsFriend(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.unit() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// 味方かどうかを取得、返答値に設定する
#if 0
	valRet->setValue(TFW_IS_FLAG(v.unit()->getUnitFlags(), Unit::UNITFLAG_PC) && (((SoumaScriptContext*)ctx)->getSouma()->getOwnerUnit() != v.unit()));
#else
	const Unit* unitThis = ((SoumaScriptContext*)ctx)->getSouma()->getOwnerUnit();
	valRet->setValue(unitThis->isFriendsSide(v.unit()) && (unitThis != v.unit()));
#endif
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - ユニットはアイテムである
		hamotol / isItem
**//*---------------------------------------------------------------------*/
bool SsUnit::callIsItem(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.unit() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// アイテムかどうかを取得、返答値に設定する
	valRet->setValue(v.unit()->isCategory(Unit::UNITCAT_ITEM));
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - ユニットは置かれたアイテムである
		haposumotol / isPutItem
**//*---------------------------------------------------------------------*/
bool SsUnit::callIsPutItem(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.unit() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// 置かれたアイテムかどうかを取得、返答値に設定する
	valRet->setValue(v.unit()->getUnitType() == UNITTYPE_PUT_ITEM);
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - ユニットは術者である
		haorthor / isMaster
**//*---------------------------------------------------------------------*/
bool SsUnit::callIsMaster(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.unit() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// 術者かどうかを取得、返答値に設定する
	valRet->setValue(((SoumaScriptContext*)ctx)->getSouma()->getOwnerUnit() == v.unit());
	return true;
}

/*---------------------------------------------------------------------*//**
	Es 関数 - 装備ユニットを得る
		getsokuitmono / getEquipedUnit
**//*---------------------------------------------------------------------*/
bool SsUnit::callGetEquipedUnit(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	Variables v(objThis->slots()->getPrivateValue());	if(v.unit() == 0L)	{	((SoumaScriptContext*)ctx)->errorFatal(valRet, SoumaScript::RET_SC_INVALID_CALL);	return true;	}
	// 引数チェック
	if(numArg != 1)
	{	
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_NUM);
		return true;
	}
#if 1
	int eqpc = EQPC_NULL;
	if(vaArg[0].isString())
	{
		VcString strEqpc;
		vaArg[0].toString(&strEqpc, ctx);
		if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_LEFT_HAND, &strEqpc, true)) { eqpc = EQPC_LHAND; }
		else if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_RIGHT_HAND, &strEqpc, true)) { eqpc = EQPC_RHAND; }
		else if(SoumaScriptKeywords::match(SoumaScriptKeywords::S_GENEW_NS_ACCESSORY, &strEqpc, true)) { eqpc = EQPC_ACCSR; }
	}
	else if(!vaArg[0].toInt32(&eqpc, ctx))
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);
		return true;
	}
#else
	if(!vaArg[0].isInt())
	{
		((SoumaScriptContext*)ctx)->errorMessage(valRet, SoumaScript::RET_SC_INVALID_ARG_TYPE);
		return true;
	}
#endif
	// 装備ユニットを得る
	Unit* unitThis = v.unit(); 
	EsObject* objUnit = 0;
	if(unitThis->isCategory(Unit::UNITCAT_CHAR) && TFW_IS_FLAG(unitThis->getUnitFlags(), Unit::UNITFLAG_PC))
	{
		PcUnit* unitPc = (PcUnit*)unitThis;
		const ItemEquip* itmeq = unitPc->getItemEquip();
		if(itmeq != 0L)
		{
			s32 ieqp = EquipPlace::NUM_EQP;
			switch(eqpc)
			{
			case EQPC_LHAND:	ieqp = EquipPlace::EQP_LHAND;	break;
			case EQPC_RHAND:	ieqp = EquipPlace::EQP_RHAND;	break;
			case EQPC_ACCSR:	ieqp = EquipPlace::EQP_ACCSR;	break;
			}
			if(ieqp != EquipPlace::NUM_EQP)
			{
				const Item* item = itmeq->getEquipStat()->getEquipItem(ieqp);
				if(item != 0L)
				{
					// 装備品は存在する
					Unit* unitWep = itmeq->getEquipUnit(ieqp);
					if(!makeEsObject(&objUnit, ((SoumaScriptContext*)ctx), unitWep, getKeywordLangOffset(objThis)))
					{
						return false;
					}
				}
			}
		}
	}
	// 返答値にセットして終了
	if(objUnit != 0L)
	{
		valRet->setValue(objUnit);
	}
	else
	{
		valRet->setNull();
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
