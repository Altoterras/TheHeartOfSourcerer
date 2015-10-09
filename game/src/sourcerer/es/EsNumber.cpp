/***********************************************************************//**
 *	EsNumber.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/08/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsNumber.h"

// Friends
#include "EsContext.h"
#include "EsError.h"
#include "EsSysClasses.h"
#include "EsKeywordSet.h"
#include "EsObject.h"
#include "EsObjectSlots.h"
#include "EsRunEnv.h"
#include "EsScopeProperty.h"
#include "EsStackFrame.h"

// External
#include "../../tfw/lib/TypeUtils.h"
#include "../../tfw/string/StringUtils.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス
//==========================================================================
// EsNumber::EsF64AtomHashOp メソッド

/*---------------------------------------------------------------------*//**
	エントリのサイズを返す
**//*---------------------------------------------------------------------*/
u32 EsNumber::EsF64AtomHashOp::getEntrySize()
{
	return sizeof(EsAtom::EsAtomHashEntry);
}

/*---------------------------------------------------------------------*//**
	エントリバッファを確保する
**//*---------------------------------------------------------------------*/
void* EsNumber::EsF64AtomHashOp::newEntryBuffer(u32 numEntry)
{
	EsAtom::EsAtomHashEntry* arr = new EsAtom::EsAtomHashEntry[numEntry];
	return arr;
}

/*---------------------------------------------------------------------*//**
	エントリバッファを削除する
**//*---------------------------------------------------------------------*/
void EsNumber::EsF64AtomHashOp::deleteEntryBuffer(void* entrybuf)
{
	EsAtom::EsAtomHashEntry* arr = (EsAtom::EsAtomHashEntry*)entrybuf;
	delete[] arr;
}

/*---------------------------------------------------------------------*//**
	ハッシュコードを算出する
**//*---------------------------------------------------------------------*/
u32 EsNumber::EsF64AtomHashOp::hashCode(const void* key)
{
	const f64* d = (f64*)key;
	u32 h = TFW_F64_TO_RAWU32_HI(*d) ^ TFW_F64_TO_RAWU32_LO(*d);
	return h;
}

/*---------------------------------------------------------------------*//**
	エントリが同一かどうか判定する
**//*---------------------------------------------------------------------*/
bool EsNumber::EsF64AtomHashOp::matchEntry(HashEntry* entry, const void* key)
{
	const EsAtom* atom = ((EsAtom::EsAtomHashEntry*)entry)->getAtom();
	const f64 d1 = atom->getDouble()->_v;
	const f64 d2 = *(f64*)key;
	if(TFW_F64_IS_NAN(d1))
	{
		return TFW_F64_IS_NAN(d2);
	}
	if(TFW_F64_IS_NAN(d2))
	{
		return false;
	}
	return d1 == d2;
}

/*---------------------------------------------------------------------*//**
	エントリ初期化
**//*---------------------------------------------------------------------*/
bool EsNumber::EsF64AtomHashOp::initEntry(HashEntry* entry, const void* key)
{
	EsAtom::EsAtomHashEntry* ae = (EsAtom::EsAtomHashEntry*)entry;
	EsAtom* atom = new EsAtom((f64*)key);
	ae->setAtom(atom);
	return true;
}

/*---------------------------------------------------------------------*//**
	エントリ破棄
**//*---------------------------------------------------------------------*/
void EsNumber::EsF64AtomHashOp::cleanupEntry(HashEntry* entry)
{
	EsAtom::EsAtomHashEntry* ae = (EsAtom::EsAtomHashEntry*)entry;
	delete ae->getAtom();
	entry->clear();
}


//==========================================================================
// EsNumber::EsNumberPrototype メソッド

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberPrototype::initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const
{
	EsObject* objPtype = EsNumber::EsNumberClass::makeClassObject(ctx, obj);
	*objPrototypeOut = objPtype;
	return objPtype != 0L;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsNumber::EsNumberPrototype::EsNumberPrototype()
	: EsPrototype(EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_NUMBER)->getAtom(), EsSysPrototypes::PTYPEKIND_NUMBER)
{
}


//==========================================================================
// EsNumber::EsNumberClass メソッド

/*---------------------------------------------------------------------*//**
	数値スクリプトクラスの作成 ⇒ js_InitNumberClass
**//*---------------------------------------------------------------------*/
EsObject* EsNumber::EsNumberClass::makeClassObject(EsContext* ctx, EsObject* objParent)
{
	// グローバルオブジェクトに数値関連関数を定義する ⇒ JS_DefineFunctions(cx, obj, number_functions)
	if(!objParent->addFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ISNAN)->getAtom(), callIsNan, 0, EsExtParam()))				{	return 0L;	}
	if(!objParent->addFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ISFINITE)->getAtom(), callIsFinite, 0, EsExtParam()))		{	return 0L;	}
	if(!objParent->addFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PARSEFLOAT)->getAtom(), callParseFloat, 0, EsExtParam()))	{	return 0L;	}
	if(!objParent->addFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PARSEINT)->getAtom(), callParseInt, 0, EsExtParam()))		{	return 0L;	}

	// クラスオブジェクト作成
	EsObject* objPtype;
	EsObject* objCtor;
	objParent->makeClassObject(&objPtype, &objCtor, ctx, getInstance(), 0L, &callConstructor);	// ⇒ proto = JS_InitClass(cx, obj, NULL, &js_ArrayClass, js_Array, 1, array_props, array_methods, NULL, NULL);
	if((objPtype == 0L) || (objCtor == 0L))
	{
		return 0L;
	}

	EsValue valWk;
	// 非静的関数定義 ⇒ number_methods
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOSTRING)->getAtom(), callToString, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_VALUEOF)->getAtom(), callValueOf, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOFIXED)->getAtom(), callToFixed, 0);	
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOEXPONENTIAL)->getAtom(), callToExponential, 0);	
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOPRECISION)->getAtom(), callToPrecision, 0);	
	// 静的特殊定数アクセサ定義 ⇒ JS_DefineConstDoubles(cx, ctor, number_constants)
	if(!objCtor->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_NAN)->getAtom(), 0L, 0L, EsValue::setout(&valWk, TypeUtils::getF64NaN()), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))								{	return 0L;	}
	if(!objCtor->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_POSITIVE_INFINITY)->getAtom(), 0L, 0L, EsValue::setout(&valWk, TypeUtils::getF64PositiveInfinity()), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))	{	return 0L;	}
	if(!objCtor->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_NEGATIVE_INFINITY)->getAtom(), 0L, 0L, EsValue::setout(&valWk, TypeUtils::getF64NegativeInfinity()), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))	{	return 0L;	}
	if(!objCtor->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_MAX_VALUE)->getAtom(), 0L, 0L, EsValue::setout(&valWk, (f64)1.7976931348623157E+308), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))					{	return 0L;	}
	if(!objCtor->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_MIN_VALUE)->getAtom(), 0L, 0L, EsValue::setout(&valWk, (f64)0), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))										{	return 0L;	}
	// オブジェクトにNaN（ECMA 15.1.1.1）, Infinity（ECMA 15.1.1.2） アクセサ定義
	if(!objParent->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_INFINITY)->getAtom(), 0L, 0L, EsValue::setout(&valWk, TypeUtils::getF64PositiveInfinity()), 0, EsScopeProperty::ATRBF_PERMANENT, 0))	{	return 0L;	}

	// オブジェクト初期化
	EsValue valZero((s32)0);
	objPtype->slots()->setPrivateValue(&valZero);	// ⇒ STOBJ_SET_SLOT(proto, JSSLOT_PRIVATE, JSVAL_ZERO);
	return objPtype;
}

/*---------------------------------------------------------------------*//**
	数値オブジェクトクラスのコンストラクタ ⇒ js_NumberClass
**//*---------------------------------------------------------------------*/
EsNumber::EsNumberClass::EsNumberClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_NUMBER)->getAtom()
		, EsSysClasses::CLASSTYPE_NUMBER
		, EsClass::F_HAS_PRIVATE
		, EsSysPrototypes::PTYPEKIND_NUMBER
		, 0 )
{
	_funcConstruct = callConstructor;
}

/*---------------------------------------------------------------------*//**
	数値スクリプトクラスのコンストラクタ ⇒ Number
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberClass::callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	// 値取得
	f64 fval;
	EsValue valRes;
	if(numArg != 0)
	{
		vaArg[0].toNumber(&fval, &valRes, ctx);
		if(valRes.isNull())
		{
			return false;
		}
		EsValue valTrue(true);
		if(valRes.equalsStrict(&valTrue))
		{
			valRes.setNumber(fval);
		}
		else
		{
			ASSERT(valRes.isInt() || valRes.isDouble());
		}
		vaArg[0].copy(&valRes);
	}
	else
	{
		valRes.setValue(0);
		fval = 0;
	}

	// コンストラクタの場合は返答値に値をセットして返る
	if(!TFW_IS_FLAG(ctx->getTopStackFrame()->getFlags(), EsStackFrame::F_CONSTRUCTING))	// ⇒ JS_IsConstructing(cx)
	{
		valRet->copy(&valRes);
		return true;
	}

	// 値保存
	objThis->slots()->setPrivateValue(&valRes);	// ⇒　STOBJ_SET_SLOT(obj, JSSLOT_PRIVATE, v);
	return true;
}

/*---------------------------------------------------------------------*//**
	NaN 判定 ⇒ num_isNaN
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberClass::callIsNan(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg == 0)
	{
		valRet->setValue(true);
		return true;
	}

	// 値取得
	f64 fval;
	vaArg[0].toNumber(&fval, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	// 判定
	valRet->setValue(TFW_F64_IS_NAN(fval));
	return true;
}

/*---------------------------------------------------------------------*//**
	有限値判定 ⇒ num_isFinite
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberClass::callIsFinite(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg == 0)
	{
		valRet->setValue(true);
		return true;
	}

	// 値取得
	f64 fval;
	vaArg[0].toNumber(&fval, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	// 判定
	valRet->setValue(TFW_F64_IS_FINITE(fval));
	return true;
}

/*---------------------------------------------------------------------*//**
	浮動小数点数値解析 ⇒ num_parseFloat
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberClass::callParseFloat(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg == 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());
		return true;
	}

	// 文字列化
	const VcString* cstr;
	if(!vaArg[0].toString(&cstr, ctx) || (cstr == 0L))
	{
		return false;
	}

	// 浮動小数点数値化
	s32 indexParseEnd = 0;
	f64 fval = StringUtils::toDouble(&indexParseEnd, cstr, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_INFINITY)->getAtomString());
	if(indexParseEnd == 0)	// 数の解析は行われなかった
	{
		valRet->setValue(TypeUtils::getF64NaN());
		return true;
	}

	// 値保存
	valRet->setNumber(fval);	// ⇒ js_NewNumberInRootedValue(cx, d, vp);
	return true;
}

/*---------------------------------------------------------------------*//**
	整数解析 ⇒ num_parseInt
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberClass::callParseInt(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg == 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());
		return true;
	}

	// 文字列化
	const VcString* cstr;
	if(!vaArg[0].toString(&cstr, ctx) || (cstr == 0L))
	{
		return false;
	}

	// 整数化
	s32 indexParseEnd = 0;
	s32 ival = StringUtils::toInteger(&indexParseEnd, cstr);
	if(indexParseEnd == 0)	// 数の解析は行われなかった
	{
		valRet->setValue(TypeUtils::getF64NaN());
		return true;
	}

	// 値保存
	valRet->setNumber(ival);	// ⇒ js_NewNumberInRootedValue(cx, d, vp);
	return true;
}

/*---------------------------------------------------------------------*//**
	文字列化 ⇒ num_toString
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberClass::callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	// 値を得る
	EsValue valWk;
	if(!ctx->getRunEnv()->getPrimitiveThisObject(&valWk, getInstance(), valCallee, valThis, vaArg, numArg))
	{
		return false;
	}
	ASSERT(valWk.isNumber());

	// 基数を得る
	s32 radix = 10;
	if((numArg != 0) && !vaArg[0].isVoid())	// ⇒ argc != 0 && !JSVAL_IS_VOID(vp[2])
	{
		if(!vaArg[0].toInt32(&radix, ctx))
		{
			return false;
		}
		if((radix < 2) || (radix > 36))
		{
			ctx->addRunError(EsError::ID_BAD_RADIX);
			return false;
		}
	}

	// 文字列化
	VcString strWk;
	if(radix == 10)
	{
		// ⇒ str = js_NumberToString(cx, d);
		if(valWk.isInt())			{	VcString::format(&strWk, "%d", valWk.getInt());		}
		else if(valWk.isDouble())	{	VcString::format(&strWk, "%f", valWk.getDouble());	}
	}
	else
	{
		f64 fval = valWk.isInt() ? (f64)valWk.getInt() : valWk.getDouble();
		strWk = StringUtils::toString(fval, radix);
	}
	const VcString* cstr = ctx->newString(&strWk);
	if(cstr == 0L)
	{
		return false;
	}

	// 値保存
	valRet->setValue(cstr);	// ⇒ *vp = STRING_TO_JSVAL(str);
	return true;
}

/*---------------------------------------------------------------------*//**
	オブジェクトの基本データ型の値を返す ⇒ num_valueOf
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberClass::callValueOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(valThis->isNumber())	// ⇒ v = vp[1]; if (JSVAL_IS_NUMBER(v))
	{
		valRet->copy(valThis);
		return true;
	}

	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ obj = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}
	if(!ctx->instanceOf(objThis, getInstance(), &vaArg[0]))	{	return false;	}

	// 値取得
	valRet->copy(objThis->getSlots()->getPrivateValue());	// *vp = OBJ_GET_SLOT(cx, obj, JSSLOT_PRIVATE);
	return true;
}

/*---------------------------------------------------------------------*//**
	小数点以下の桁数を指定して変換した文字列を返す ⇒ num_toFixed
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberClass::callToFixed(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	return convertTo(ctx, valCallee, valThis, vaArg, numArg, valRet, 3, 3, -20, 100, 0);
}

/*---------------------------------------------------------------------*//**
	指定された桁数で指数形式の文字列を返す ⇒ num_toExponential
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberClass::callToExponential(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	return convertTo(ctx, valCallee, valThis, vaArg, numArg, valRet, 2, 2, 0, 100, 1);
}

/*---------------------------------------------------------------------*//**
	指定した精度にした文字列を返す ⇒ num_toPrecision
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberClass::callToPrecision(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if((numArg == 0) || vaArg[0].isVoid())
	{
		return callToString(ctx, objThis, valCallee, valThis, vaArg, numArg, valRet, exprm);
	}
	return convertTo(ctx, valCallee, valThis, vaArg, numArg, valRet, 2, 2, 1, 100, 0);
}

/*---------------------------------------------------------------------*//**
	文字列変換 ⇒ num_to
**//*---------------------------------------------------------------------*/
bool EsNumber::EsNumberClass::convertTo(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, s32 modeArgZero, s32 modeArgOne, s32 precisionMin, s32 precisionMax, s32 precisionOffset)
{
	// 値を得る
	EsValue valWk;
	if(!ctx->getRunEnv()->getPrimitiveThisObject(&valWk, getInstance(), valCallee, valThis, vaArg, numArg))
	{
		return false;
	}
	ASSERT(valWk.isNumber());
	f64 fval = valWk.isInt() ? (f64)valWk.getInt() : valWk.getDouble();

	// 精度を得る
	s32 precision;
	if(numArg == 0)
	{
		precision = 0;
		modeArgOne = modeArgZero;
	}
	else
	{
		f64 fpwk;
		vaArg[0].toNumber(&fpwk, &vaArg[0], ctx);
		if(vaArg[0].isNull())
		{
			return false;
		}
		precision = EsValue::convF64ToS32(fpwk);
		if((precision < precisionMin) || (precision > precisionMax))
		{
			ctx->addRunError(EsError::ID_INVALID_PRECISION_RANGE);
			return false;
		}
	}

	// 文字列化し、返答値に設定
	VcString strWk = StringUtils::toString(fval, modeArgOne, precision + precisionOffset);
	const VcString* cstr = ctx->newString(&strWk);
	if(cstr == 0L)
	{
		return false;
	}
	valRet->setValue(cstr);
	return true;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
