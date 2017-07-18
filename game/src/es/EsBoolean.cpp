/***********************************************************************//**
 *	EsBoolean.cpp
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
#include "EsBoolean.h"

// Friends
#include "EsContext.h"
#include "EsSysClasses.h"
#include "EsKeywordSet.h"
#include "EsObject.h"
#include "EsObjectSlots.h"
#include "EsRunEnv.h"
#include "EsStackFrame.h"
#include "EsValue.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス


//==========================================================================
// EsBoolean::EsBooleanPrototype メソッド

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool EsBoolean::EsBooleanPrototype::initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const
{
	EsObject* objPtype = EsBoolean::EsBooleanClass::makeClassObject(ctx, obj);
	*objPrototypeOut = objPtype;
	return objPtype != 0L;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsBoolean::EsBooleanPrototype::EsBooleanPrototype()
	: EsPrototype(EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_BOOLEAN)->getAtom(), EsSysPrototypes::PTYPEKIND_BOOLEAN)
{
}


//==========================================================================
// EsBoolean::EsBooleanClass メソッド

/*---------------------------------------------------------------------*//**
	ブーリアンスクリプトクラスの作成 ⇒ js_InitBooleanClass
**//*---------------------------------------------------------------------*/
EsObject* EsBoolean::EsBooleanClass::makeClassObject(EsContext* ctx, EsObject* objParent)
{
	// クラスオブジェクト作成
	EsObject* objPtype;
	EsObject* objCtor;
	objParent->makeClassObject(&objPtype, &objCtor, ctx, getInstance(), 0L, &callConstructor);	// ⇒ proto = JS_InitClass(cx, obj, NULL, &js_ArrayClass, js_Array, 1, array_props, array_methods, NULL, NULL);
	if((objPtype == 0L) || (objCtor == 0L))
	{
		return 0L;
	}
	// 非静的プロパティを定義する
	///objPtype->defineClassProperty();
	// 非静的関数を定義する ⇒ boolean_methods
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOSTRING)->getAtom(), callToString, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_VALUEOF)->getAtom(), callValueOf, 0);
	// 静的プロパティを定義する
	///objCtor->defineClassProperty();
	// 静的関数を定義する
	///objCtor->defineClassFunction();

	// オブジェクト初期化
	EsValue valFalse(false);
	objPtype->slots()->setPrivateValue(&valFalse);	// ⇒ STOBJ_SET_SLOT(proto, JSSLOT_PRIVATE, JSVAL_FALSE);
	return objPtype;
}

/*---------------------------------------------------------------------*//**
	ブーリアンオブジェクトクラスのコンストラクタ ⇒ js_BooleanClass
**//*---------------------------------------------------------------------*/
EsBoolean::EsBooleanClass::EsBooleanClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_BOOLEAN)->getAtom()
		, EsSysClasses::CLASSTYPE_BOOLEAN
		, EsClass::F_HAS_PRIVATE
		, EsSysPrototypes::PTYPEKIND_BOOLEAN
		, 0 )
{
	_funcConstruct = callConstructor;
}

/*---------------------------------------------------------------------*//**
	ブーリアンスクリプトクラスのコンストラクタ ⇒ Boolean
**//*---------------------------------------------------------------------*/
bool EsBoolean::EsBooleanClass::callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	// 値取得
	bool bval;
	if(numArg != 0)
	{
		vaArg[0].toBoolean(&bval, ctx);
	}
	else
	{
		bval = false;
	}

	// コンストラクタの場合は返答値に値をセットして返る
	if(!TFW_IS_FLAG(ctx->getTopStackFrame()->getFlags(), EsStackFrame::F_CONSTRUCTING))	// ⇒ JS_IsConstructing(cx)
	{
		valRet->setValue(bval);
		return true;
	}

	// 値保存
	EsValue valBool(bval);
	objThis->slots()->setPrivateValue(&valBool);
	return true;
}

/*---------------------------------------------------------------------*//**
	文字列化 ⇒ bool_toString
**//*---------------------------------------------------------------------*/
bool EsBoolean::EsBooleanClass::callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	EsValue valWk;
	if(!ctx->getRunEnv()->getPrimitiveThisObject(&valWk, getInstance(), valCallee, valThis, vaArg, numArg))
	{
		return false;
	}
	ASSERT(valWk.isBoolean());

	const VcString* str = valWk.getBoolean() ? EsKeywordSet::getStatic(EsKeywordSet::S_VALUE_TRUE)->getAtomString() : EsKeywordSet::getStatic(EsKeywordSet::S_VALUE_FALSE)->getAtomString();
	valRet->setValue(str);

	return true;
}

/*---------------------------------------------------------------------*//**
	オブジェクトの基本データ型の値を返す ⇒ bool_valueOf
**//*---------------------------------------------------------------------*/
bool EsBoolean::EsBooleanClass::callValueOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	return ctx->getRunEnv()->getPrimitiveThisObject(valRet, getInstance(), valCallee, valThis, vaArg, numArg);
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
