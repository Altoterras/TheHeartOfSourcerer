/***********************************************************************//**
 *	EsSysClasses.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2010/06/01.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsSysClasses.h"

// Friends
#include "Es.h"
#include "EsArray.h"
#include "EsAtomList.h"
#include "EsBlock.h"
#include "EsBoolean.h"
#include "EsBytecodeBuilder.h"
#include "EsContext.h"
#include "EsError.h"
#include "EsFunction.h"
#include "EsInterlang.h"
#include "EsIterator.h"
#include "EsKeywordSet.h"
#include "EsMath.h"
#include "EsNumber.h"
#include "EsObject.h"
#include "EsResolver.h"
#include "EsRunEnv.h"
#include "EsProgramCounter.h"
#include "EsScope.h"
#include "EsScopeProperty.h"
#include "EsStackFrame.h"
#include "EsStackRegister.h"
#include "EsString.h"
#include "EsValue.h"
#include "EsWith.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsSysClasses 定数

// 標準クラス配列 ⇒ standard_class_atoms
const EsSysClasses::FixedClassType EsSysClasses::FIXED_STANDARD_CLASSES[NUM_FIXED_STANDARD_CLASSES] =
{
	CLASSTYPE_FUNC,
	CLASSTYPE_OBJECT,
	CLASSTYPE_ARRAY,
	CLASSTYPE_BOOLEAN,
//	CLASSTYPE_DATE,
	CLASSTYPE_MATH,
	CLASSTYPE_NUMBER,
	CLASSTYPE_STRING,
//	CLASSTYPE_ERROR,
//	CLASSTYPE_REGEXP,
};

//==========================================================================
// EsSysClasses 変数

Array<const EsClass*> EsSysClasses::_arrExtClasses(true, false);
Array<const EsClass*> EsSysClasses::_arrExtStdClasses(true, false);

//==========================================================================
// EsSysClasses メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	クラスインスタンスを得る
**//*---------------------------------------------------------------------*/
const EsClass* EsSysClasses::getClass(u16 clstype)
{
	// 固定クラス
	switch(clstype)
	{
	case CLASSTYPE_OBJECT:		return EsObject::EsObjectClass::getInstance();
	case CLASSTYPE_FUNC:		return EsFunction::EsFunctionClass::getInstance();
	case CLASSTYPE_CALL:		return EsCallClass::getInstance();
	case CLASSTYPE_BLOCK:		return EsBlock::EsBlockClass::getInstance();
	case CLASSTYPE_WITH:		return EsWith::EsWithClass::getInstance();
	case CLASSTYPE_DECLENV:		return EsDeclEnvClass::getInstance();
	case CLASSTYPE_NUMBER:		return EsNumber::EsNumberClass::getInstance();
	case CLASSTYPE_BOOLEAN:		return EsBoolean::EsBooleanClass::getInstance();
	case CLASSTYPE_STRING:		return EsString::EsStringClass::getInstance();
	case CLASSTYPE_ARGUMENTS:	return EsArgumentsClass::getInstance();
	case CLASSTYPE_ARRAY:		return EsArray::EsArrayClass::getInstance();
	case CLASSTYPE_ITERATOR:	return EsIterator::EsIteratorClass::getInstance();
	case CLASSTYPE_SCRIPT:		return EsScriptClass::getInstance();
	case CLASSTYPE_GLOBAL:		return EsGlobalClass::getInstance();
	case CLASSTYPE_MATH:		return EsMath::EsMathClass::getInstance();
	}

	// 拡張クラス
	for(u32 i = 0; i < _arrExtClasses.getCount(); i++)
	{
		const EsClass* cls = _arrExtClasses.getAt(i);
		if(cls->getClassType() == clstype)
		{
			return cls;
		}
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	新しいクラスタイプ値を生成する
**//*---------------------------------------------------------------------*/
u16 EsSysClasses::generateClassType(const EsAtom* atomName)
{
	u16 clstype = CLASSTYPE__CUSTOM__;
	for(u32 i = 0; i < _arrExtClasses.getCount(); i++)
	{
		const EsClass* clsWk = _arrExtClasses.getAt(i);
		if(clsWk == 0L)	{	continue;	}
		if(clsWk->getNameAtom() == atomName)
		{
			return clsWk->getClassType();
		}
		if(clstype <= clsWk->getClassType())
		{
			clstype = clsWk->getClassType() + 1;
		}
	}
	return clstype;
}

/*---------------------------------------------------------------------*//**
	グローバススコープのクラスタイプかを得る ⇒ IsCacheableNonGlobalScope
**//*---------------------------------------------------------------------*/
bool EsSysClasses::isNonGlobalScope(u16 clstype)
{
	switch(clstype)
	{
	case CLASSTYPE_CALL:
	case CLASSTYPE_BLOCK:
	case CLASSTYPE_DECLENV:
		return true;
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	標準クラスの数を得る
**//*---------------------------------------------------------------------*/
s32 EsSysClasses::getStandardClassNum()
{
	return NUM_FIXED_STANDARD_CLASSES + _arrExtStdClasses.getCount();
}

/*---------------------------------------------------------------------*//**
	標準クラスを得る
**//*---------------------------------------------------------------------*/
const EsClass* EsSysClasses::getStandardClass(s32 index)
{
	if((0 <= index) && (index < NUM_FIXED_STANDARD_CLASSES))
	{
		return getClass(FIXED_STANDARD_CLASSES[index]);
	}
	index -= NUM_FIXED_STANDARD_CLASSES;
	if((0 <= index) && ((u32)index <_arrExtStdClasses.getCount()))
	{
		return _arrExtStdClasses.getAt(index);
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	拡張クラスを登録する
**//*---------------------------------------------------------------------*/
void EsSysClasses::registerExternalClass(const EsClass* cls, bool isStandardClass)
{
	_arrExtClasses.add(cls);
	if(isStandardClass)
	{
		_arrExtStdClasses.add(cls);
	}
}


//==========================================================================
// EsSysClasses::EsCallClass メソッド

/*---------------------------------------------------------------------*//**
	追加の予約スロット数を得る ⇒ call_reserveSlots
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsCallClass::getAdditionalReservedSlotNum(u16* slotnumOut, EsObject* obj) const
{
	EsFunction* func = obj->getCallObjectFunction();	// ⇒ fun = GetCallObjectFunction(obj);
	if(func == 0L)	{	return false;	}
	*slotnumOut = func->getLocalVarNum(EsFunction::LVK_ARG) + func->getLocalVarNum(EsFunction::LVK_VAR);	// ⇒ fun->countArgsAndVars();
	return true;
}

/*---------------------------------------------------------------------*//**
	コールオブジェクトクラスのコンストラクタ ⇒ js_CallClass
**//*---------------------------------------------------------------------*/
EsSysClasses::EsCallClass::EsCallClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_CALL)->getAtom()
		, CLASSTYPE_CALL
		, EsClass::F_HAS_PRIVATE | EsClass::F_RESOLVE_OUT_PARAM | EsClass::F_IS_ANONYMOUS
		, EsSysPrototypes::PTYPEKIND_NULL
		, NUM_DEFAULT_RESERVED_SLOTS )
{
	_funcEnumerate = callEnumerate;
	_funcResolv = callResolve;
	_funcConvert = callConvert;
}

/*---------------------------------------------------------------------*//**
	列挙 ⇒ call_enumerate
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsCallClass::callEnumerate(EsContext* ctx, EsObject* obj)
{
	EsFunction* func = obj->getCallObjectFunction();
	EsAtomList alistVarArg;
	func->enumLocalAtoms(&alistVarArg, EsFunction::LVK_VAR);
	func->enumLocalAtoms(&alistVarArg, EsFunction::LVK_ARG);
	for(int i = 0; i < alistVarArg.getCount(); i++)
	{
		const EsAtom* atom = alistVarArg.getAtom(i);
		if(atom == 0L)
		{
			continue;
		}

		EsScopeProperty* sprop;
		EsObject* objParent;
		EsValue valAtom(atom);
		if(!obj->findPropertyBase(&sprop, &objParent, ctx, &valAtom))
		{
			return false;
		}

		ASSERT(sprop != 0L);
		ASSERT(obj == objParent);
		objParent->dropPropertyOv(ctx, sprop);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ARGUMENTS 取得 ⇒ GetCallArguments
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsCallClass::funcGetArguments(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id)
{
	if(obj->getClass()->getClassType() != EsSysClasses::CLASSTYPE_CALL)
	{
		return true;
	}

	EsFunction* func = obj->getCallObjectFunction();	// ⇒ fun = GetCallObjectFunction(obj);
	EsStackFrame* sfrm = (EsStackFrame*)obj->slots()->privateData();	// ⇒ fp = (JSStackFrame *) JS_GetPrivate(cx, obj);

	if((sfrm != 0L) && TFW_IS_FLAG(sfrm->getOverrideFlags(), EsStackFrame::convPropsidToBits(EsFunction::PROPSID_CALL_ARGUMENTS)))	// ⇒ if (fp && !TEST_OVERRIDE_BIT(fp, CALL_ARGUMENTS))
	{
		EsObject* objArgs = sfrm->makeArgsObject(ctx);
		if(objArgs == 0L)
		{
			return false;
		}
		val->setValue(objArgs);
	}
	else
	{
		val->copy(func->getSlots()->getValue(EsFunction::EsFunctionClass::getInstance()->getCallArgumentsSlotIndex()));
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ARGUMENTS 設定 ⇒ SetCallArguments
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsCallClass::funcSetArguments(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val)
{
	if(obj->getClass()->getClassType() != EsSysClasses::CLASSTYPE_CALL)
	{
		return true;
	}

	EsFunction* func = obj->getCallObjectFunction();	// ⇒ fun = GetCallObjectFunction(obj);
	EsStackFrame* sfrm = (EsStackFrame*)obj->slots()->privateData();	// ⇒ fp = (JSStackFrame *) JS_GetPrivate(cx, obj);

	if(sfrm != 0L)
	{
		TFW_SET_FLAG(*sfrm->overrideFlags(), EsStackFrame::convPropsidToBits(EsFunction::PROPSID_CALL_ARGUMENTS), true);
	}

	func->slots()->setValue(EsFunction::EsFunctionClass::getInstance()->getCallArgumentsSlotIndex(), val);

	return true;
}

/*---------------------------------------------------------------------*//**
	引数取得 ⇒ js_GetCallArg
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsCallClass::funcGetArg(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id)
{
	if(obj->getClass()->getClassType() != EsSysClasses::CLASSTYPE_CALL)
	{
		return true;
	}

	EsFunction* func = obj->getCallObjectFunction();	// ⇒ fun = GetCallObjectFunction(obj);
	EsStackFrame* sfrm = (EsStackFrame*)obj->slots()->privateData();	// ⇒ fp = (JSStackFrame *) JS_GetPrivate(cx, obj);

	ASSERT(id->isInt() && (id->getInt() <= 0xffff));
	u32 idx = (u16)id->getInt();
	ASSERT(idx < func->getLocalVarNum(EsFunction::LVK_ARG));

	if(sfrm == 0L)
	{
		idx += NUM_DEFAULT_RESERVED_SLOTS;
		return obj->getSlots()->getReservedSlotValue(val, ctx, idx);
	}

	val->copy(sfrm->getArg(idx));
	return true;
}

/*---------------------------------------------------------------------*//**
	引数取得 ⇒ SetCallArg
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsCallClass::funcSetArg(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val)
{
	if(obj->getClass()->getClassType() != EsSysClasses::CLASSTYPE_CALL)
	{
		return true;
	}

	EsFunction* func = obj->getCallObjectFunction();	// ⇒ fun = GetCallObjectFunction(obj);
	EsStackFrame* sfrm = (EsStackFrame*)obj->slots()->privateData();	// ⇒ fp = (JSStackFrame *) JS_GetPrivate(cx, obj);

	ASSERT(id->isInt() && (id->getInt() <= 0xffff));
	u32 idx = (u16)id->getInt();
	ASSERT(idx < func->getLocalVarNum(EsFunction::LVK_ARG));

	if(sfrm == 0L)
	{
		idx += NUM_DEFAULT_RESERVED_SLOTS;
		return obj->slots()->setReservedSlotValue(ctx, idx, val);
	}

	sfrm->setArg(idx, val);
	return true;
}

/*---------------------------------------------------------------------*//**
	変数取得 ⇒ js_GetCallVar
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsCallClass::funcGetVar(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id)
{
	if(obj->getClass()->getClassType() != EsSysClasses::CLASSTYPE_CALL)
	{
		return true;
	}

	EsFunction* func = obj->getCallObjectFunction();	// ⇒ fun = GetCallObjectFunction(obj);
	EsStackFrame* sfrm = (EsStackFrame*)obj->slots()->privateData();	// ⇒ fp = (JSStackFrame *) JS_GetPrivate(cx, obj);

	ASSERT(id->isInt() && (id->getInt() <= 0xffff));
	u32 idx = (u16)id->getInt();
	ASSERT(idx < func->getLocalVarNum(EsFunction::LVK_VAR));

	if(sfrm == 0L)
	{
		idx += NUM_DEFAULT_RESERVED_SLOTS;
		idx += func->getLocalVarNum(EsFunction::LVK_VAR);
		return obj->getSlots()->getReservedSlotValue(val, ctx, idx);
	}

	val->copy(sfrm->getSlot(idx));
	return true;
}

/*---------------------------------------------------------------------*//**
	変数取得 ⇒ js_GetCallVarChecked
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsCallClass::funcGetVarChecked(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id)
{
	if(!funcGetVar(val, ctx, obj, id))
	{
		return false;
	}

	return EsFunction::checkEscapingClosure(ctx, obj, val);
}

/*---------------------------------------------------------------------*//**
	変数設定 ⇒ SetCallVar
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsCallClass::funcSetVar(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val)
{
	if(obj->getClass()->getClassType() != EsSysClasses::CLASSTYPE_CALL)
	{
		return true;
	}

	EsFunction* func = obj->getCallObjectFunction();	// ⇒ fun = GetCallObjectFunction(obj);
	EsStackFrame* sfrm = (EsStackFrame*)obj->slots()->privateData();	// ⇒ fp = (JSStackFrame *) JS_GetPrivate(cx, obj);

	ASSERT(id->isInt() && (id->getInt() <= 0xffff));
	u32 idx = (u16)id->getInt();
	ASSERT(idx < func->getLocalVarNum(EsFunction::LVK_VAR));

	if(sfrm == 0L)
	{
		idx += NUM_DEFAULT_RESERVED_SLOTS;
		idx += func->getLocalVarNum(EsFunction::LVK_VAR);
		return obj->slots()->setReservedSlotValue(ctx, idx, val);
	}

	sfrm->setSlot(idx, val);
	return true;
}

/*---------------------------------------------------------------------*//**
	解決 ⇒ call_resolve
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsCallClass::callResolve(EsObject** objOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv)
{
	ASSERT(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_CALL);
	ASSERT(obj->getSlots()->getPrototypeObject() == 0L);

	if(!id->isString())
	{
		return true;
	}

	EsValue* valCallee = obj->slots()->value(EsSysClasses::EsCallClass::getInstance()->getCalleeSlotIndex());
	if(valCallee->isVoid())
	{
		return true;
	}
	ASSERT(valCallee->isObject());
	EsFunction* func = (EsFunction*)valCallee->getObject()->slots()->privateData();

	EsValue valIdAtom;
	if(!ctx->convValueToStringId(&valIdAtom, id))
	{
		return false;
	}
	ASSERT(valIdAtom.isAtom());

	EsFunction::LocalVarKind lvk;
	u32 slotidx;
	func->findLocalVar(&lvk, &slotidx, valIdAtom.getAtom());	// ⇒ localKind = js_LookupLocal(cx, fun, JSID_TO_ATOM(id), &slot)
	if((lvk != EsFunction::LVK_NULL) && (lvk != EsFunction::LVK_UPVAR))
	{
		ASSERT(slotidx <= 0xffff);

		EsPropertyGetterFunc funcGetter = 0L;
		EsPropertySetterFunc funcSetter = 0L;
		u8 atrbf = EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_ENUMERATE | EsScopeProperty::ATRBF_SHARED;
		if(lvk == EsFunction::LVK_ARG)
		{
			ASSERT(slotidx < func->getLocalVarNum(EsFunction::LVK_ARG));
			funcGetter = funcGetArg;
			funcSetter = funcSetArg;
		}
		else
		{
			ASSERT((lvk == EsFunction::LVK_VAR) || (lvk == EsFunction::LVK_CONST));
			ASSERT(slotidx < func->getLocalVarNum(EsFunction::LVK_VAR));
			funcGetter = funcGetVar;
			funcSetter = funcSetVar;
			if(lvk == EsFunction::LVK_CONST)
			{
				TFW_SET_FLAG(atrbf, EsScopeProperty::ATRBF_READONLY, true);
			}

			EsValue valVar;
			EsValue valSlotIdx((s32)slotidx);
			if(!funcGetVar(&valVar, ctx, obj, &valSlotIdx))
			{
				return false;
			}
			if(valVar.isFunctionObject())
			{
				EsObject* objFunc = valVar.getObject();
				const EsFunction* funcWk = (const EsFunction*)objFunc->getSlots()->getPrivateData();
				if(funcWk->isNeedsWrapperClosure())
				{
					funcGetter = funcGetVarChecked;
				}
			}
		}

		EsValue valVoid;
		if(!obj->definePropertyNative(
			ctx,
			id,
			&valVoid,
			funcGetter,
			funcSetter,
			atrbf,
			EsScopeProperty::OPTF_HAS_SHORTID,
			(s16)slotidx,
			true	))
		{
			return false;
		}

		*objOut = obj;
		return true;
	}

	if(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ARGUMENTS)->getAtom() == id->getAtom())
	{
		EsValue valVoid;
		if(!obj->definePropertyNative(
			ctx,
			id,
			&valVoid,
			funcGetArguments,
			funcSetArguments,
			EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED,
			0,
			0,
			true	))
		{
			return false;
		}

		*objOut = obj;
		return true;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	変換 ⇒ call_convert
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsCallClass::callConvert(EsValue* val, EsContext* ctx, EsObject* obj, EsLogicalType ltype)
{
	if(ltype == LTYPE_FUNC)
	{
		EsStackFrame* sfrm = (EsStackFrame*)obj->slots()->privateData();	// ⇒ fp = (JSStackFrame *) JS_GetPrivate(cx, obj);
		if(sfrm != 0L)
		{
			ASSERT(sfrm->getFunction() != 0L);
			val->setValue(sfrm->getCalleeObject());
		}
	}
	return false;
}


//==========================================================================
// EsSysClasses::EsDeclEnvClass メソッド

/*---------------------------------------------------------------------*//**
	宣言型環境オブジェクトクラスのコンストラクタ ⇒ js_DeclEnvClass
**//*---------------------------------------------------------------------*/
EsSysClasses::EsDeclEnvClass::EsDeclEnvClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_OBJECT)->getAtom()
		, CLASSTYPE_DECLENV
		, EsClass::F_HAS_PRIVATE
		, EsSysPrototypes::PTYPEKIND_OBJECT
		, 0 )
{
}

/*---------------------------------------------------------------------*//**
	アクセサ取得実装 ⇒ CalleeGetter
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsDeclEnvClass::callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id)
{
	return EsFunction::checkEscapingClosure(ctx, obj, val);
}


//==========================================================================
// EsSysClasses::EsArgumentsClass メソッド

/*---------------------------------------------------------------------*//**
	宣言型環境オブジェクトクラスのコンストラクタ ⇒ js_ArgumentsClass
**//*---------------------------------------------------------------------*/
EsSysClasses::EsArgumentsClass::EsArgumentsClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_OBJECT)->getAtom()
		, CLASSTYPE_ARGUMENTS
		, EsClass::F_HAS_PRIVATE | EsClass::F_RESOLVE_OUT_PARAM
		, EsSysPrototypes::PTYPEKIND_OBJECT
		, 1 )
{
	_funcGetProp = callGetProperty;
	_funcSetProp = callSetProperty;
	_funcDelProp = callDelProperty;
	_funcEnumerate = callEnumerate;
	_funcResolv = callResolve;
}

/*---------------------------------------------------------------------*//**
	アクセサ取得 ⇒ args_getProperty
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsArgumentsClass::callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id)
{
	if(!id->isInt())
	{
		return true;
	}
	EsStackFrame* sfrm = (EsStackFrame*)obj->getPrivateData(ctx, EsSysClasses::getClass(EsSysClasses::CLASSTYPE_ARGUMENTS), 0L);
	if(sfrm == 0L)
	{
		return true;
	}
	ASSERT(sfrm->getArgsObject() != 0L);

	s32 slotidx = id->getInt();
	switch(slotidx)
	{
	case EsFunction::PROPSID_ARGS_CALLEE:
		if(!TFW_IS_FLAG(sfrm->getOverrideFlags(), EsStackFrame::convPropsidToBits(EsFunction::PROPSID_ARGS_CALLEE)))	// ⇒ TEST_OVERRIDE_BIT(fp, slot)
		{
			if(sfrm->getFunction()->isNeedsWrapperClosure())
			{
				ctx->addRunError(EsError::ID_LEAK_OPTIMIZED_CLOSURE);
				return false;
			}
			val->setValue(sfrm->getCalleeObject());
		}
		break;

	case EsFunction::PROPSID_ARGS_LENGTH:
		val->setValue((s32)sfrm->getArgNum());
		break;

	default:
		if((0 <= slotidx) && ((u32)slotidx < sfrm->getArgNum()) && !sfrm->isArgDeleted(ctx, slotidx))
		{
			val->copy(sfrm->getArg(slotidx));
		}
		break;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	アクセサ設定 ⇒ args_setProperty
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsArgumentsClass::callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val)
{
	if(!id->isInt())
	{
		return true;
	}
	EsStackFrame* sfrm = (EsStackFrame*)obj->getPrivateData(ctx, EsSysClasses::getClass(EsSysClasses::CLASSTYPE_ARGUMENTS), 0L);
	if(sfrm == 0L)
	{
		return true;
	}
	ASSERT(sfrm->getArgsObject() != 0L);

	s32 slotidx = id->getInt();
	switch(slotidx)
	{
	case EsFunction::PROPSID_ARGS_CALLEE:
	case EsFunction::PROPSID_ARGS_LENGTH:
		TFW_SET_FLAG(*sfrm->overrideFlags(), EsStackFrame::convPropsidToBits(slotidx), true);	// ⇒ SET_OVERRIDE_BIT(fp, slot)
		break;

	default:
		if(	sfrm->getFunction()->isInterpreted() &&
			(0 <= slotidx) && ((u32)slotidx < sfrm->getArgNum()) &&
			!sfrm->isArgDeleted(ctx, slotidx))
		{
			sfrm->setArg(slotidx, val);
		}
		break;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	アクセサ削除 ⇒ args_delProperty
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsArgumentsClass::callDelProperty(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val)
{
	if(!id->isInt())
	{
		return true;
	}
	EsStackFrame* sfrm = (EsStackFrame*)obj->getPrivateData(ctx, EsSysClasses::getClass(EsSysClasses::CLASSTYPE_ARGUMENTS), 0L);
	if(sfrm == 0L)
	{
		return true;
	}
	ASSERT(sfrm->getArgsObject() != 0L);

	s32 slotidx = id->getInt();
	switch(slotidx)
	{
	case EsFunction::PROPSID_ARGS_CALLEE:
	case EsFunction::PROPSID_ARGS_LENGTH:
		TFW_SET_FLAG(*sfrm->overrideFlags(), EsStackFrame::convPropsidToBits(slotidx), true);	// ⇒ SET_OVERRIDE_BIT(fp, slot)
		break;

	default:
		if(	(0 <= slotidx) && ((u32)slotidx < sfrm->getArgNum()) &&
			!sfrm->markArgDeleted(ctx, slotidx))
		{
			return false;
		}
		break;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	列挙 ⇒ args_enumerate
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsArgumentsClass::callEnumerate(EsContext* ctx, EsObject* obj)
{
	EsStackFrame* sfrm = (EsStackFrame*)obj->getPrivateData(ctx, EsSysClasses::getClass(EsSysClasses::CLASSTYPE_ARGUMENTS), 0L);
	if(sfrm == 0L)
	{
		return true;
	}
	ASSERT(sfrm->getArgsObject() != 0L);

	EsScopeProperty* sprop;
	EsObject* objParent;

	EsValue valLengthAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom());
	if(!obj->findPropertyBase(&sprop, &objParent, ctx, &valLengthAtom))
	{
		return false;
	}
	if(sprop != 0L)
	{
		objParent->dropPropertyOv(ctx, sprop);
	}

	EsValue valCalleeAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CALLEE)->getAtom());
	if(!obj->findPropertyBase(&sprop, &objParent, ctx, &valCalleeAtom))
	{
		return false;
	}
	if(sprop != 0L)
	{
		objParent->dropPropertyOv(ctx, sprop);
	}

	for(u32 slotidx = 0; slotidx < sfrm->getArgNum(); slotidx++)
	{
		EsValue valSlotIdx((s32)slotidx);
		if(!obj->findPropertyBase(&sprop, &objParent, ctx, &valSlotIdx))
		{
			return false;
		}
		if(sprop != 0L)
		{
			objParent->dropPropertyOv(ctx, sprop);
		}
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	解決 ⇒ args_resolve
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsArgumentsClass::callResolve(EsObject** objOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv)
{
	EsStackFrame* sfrm = (EsStackFrame*)obj->getPrivateData(ctx, EsSysClasses::getClass(EsSysClasses::CLASSTYPE_ARGUMENTS), 0L);
	if(sfrm == 0L)
	{
		return true;
	}
	ASSERT(sfrm->getArgsObject() != 0L);

	if(id->isInt())
	{
		s32 slotidx = id->getInt();
		if((0 <= slotidx) && ((u32)slotidx < sfrm->getArgNum()) && !sfrm->isArgDeleted(ctx, slotidx))
		{
			EsScopeProperty* sprop;
			EsValue valSlotIdx(slotidx);
			if(!obj->definePropertyBase(
				&sprop,
				ctx,
				&valSlotIdx,
				sfrm->getArg(slotidx),
				callGetProperty,
				callSetProperty,
				0	))
			{
				return false;
			}
		}
		*objOut = obj;
	}
	else if(id->isString())
	{
		const VcString* str = id->getString();
		const EsAtom* atom = EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom();
		s16 shortid = 0;
		EsValue val;
		if(atom->isSameString(str))
		{
			shortid = EsFunction::PROPSID_ARGS_LENGTH;
			val.setValue((s32)sfrm->getArgNum());
		}
		else
		{
			atom = EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CALLEE)->getAtom();
			if(atom->isSameString(str))
			{
				shortid = EsFunction::PROPSID_ARGS_CALLEE;
				val.setValue(sfrm->getCalleeObject());
			}
			else
			{
				atom = 0L;
			}
		}
		if((atom != 0L) && !TFW_IS_FLAG(sfrm->getOverrideFlags(), EsStackFrame::convPropsidToBits(shortid)))	// ⇒ if (atom && !TEST_OVERRIDE_BIT(fp, tinyid))
		{
			EsValue valAtom(atom);
			if(!obj->definePropertyNative(
				ctx,
				&valAtom,
				&val,
				callGetProperty,
				callSetProperty,
				0,
				EsScopeProperty::OPTF_HAS_SHORTID,
				shortid,
				false	))
			{
				return false;
			}
			*objOut = obj;
		}
	}

	return true;
}


//==========================================================================
// EsSysClasses::EsScriptClass メソッド

/*---------------------------------------------------------------------*//**
	宣言型環境オブジェクトクラスのコンストラクタ ⇒ js_ScriptClass
**//*---------------------------------------------------------------------*/
EsSysClasses::EsScriptClass::EsScriptClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_SCRIPT)->getAtom()
		, CLASSTYPE_SCRIPT
		, EsClass::F_HAS_PRIVATE
		, EsSysPrototypes::PTYPEKIND_SCRIPT
		, 1 )
{
}


//==========================================================================
// EsSysClasses::EsGlobalClass メソッド

/*---------------------------------------------------------------------*//**
	グローバルオブジェクトクラスのコンストラクタ ⇒ global_class
**//*---------------------------------------------------------------------*/
EsSysClasses::EsGlobalClass::EsGlobalClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_GLOBAL)->getAtom()
		, CLASSTYPE_GLOBAL
		, EsClass::F_RESOLVE_OUT_PARAM | EsClass::F_IS_GLOBAL | EsClass::F_HAS_PRIVATE
		, EsSysPrototypes::PTYPEKIND_NULL
		, 32 )
{
	_funcEnumerate = callEnumerate;
	_funcResolv = callResolve;
}

/*---------------------------------------------------------------------*//**
	eval メソッドの作成 ⇒ js_InitEval
**//*---------------------------------------------------------------------*/
EsObject* EsSysClasses::EsGlobalClass::initEvalMethod(EsContext* ctx, EsObject* obj)
{
	if(!obj->addFunction(
		ctx,
		EsKeywordSet::getStatic(EsKeywordSet::S_PROP_EVAL)->getAtom(),
		callEval,
		0,
		EsExtParam()	))
	{
		return 0L;
	}
	return obj;
}

/*---------------------------------------------------------------------*//**
	eval ⇒ obj_eval
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsGlobalClass::callEval(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	EsStackFrame* sfrmCaller = ctx->getScriptedCallerFrame();
	bool isIndirectCall = (sfrmCaller != 0L) && (sfrmCaller->getStackRegister() != 0L) && (sfrmCaller->getStackRegister()->pc()->get(0) != OP_EVAL);

	// ※※※ 実装不要 ※※※ ⇒ js_GetWrappedObject, JSCLASS_IS_EXTENDED

	EsObject* objParent = objThis->slots()->parentObject();
	if(isIndirectCall || (objParent != 0L))
	{
		ctx->addRunError(EsError::ID_BAD_INDIRECT_CALL);
		return false;
	}

	if(!vaArg[0].isString())
	{
		valRet->copy(&vaArg[0]);
		return true;
	}

	if((sfrmCaller != 0L) && (sfrmCaller->getVarObject() == 0L) && !sfrmCaller->makeCallObject(ctx))
	{
		return false;
	}

	EsObject* objScope = 0L;
	if(numArg >= 2)
	{
		if(!ctx->convValueToObject(&objScope, &vaArg[1]))
		{
			return false;
		}
		vaArg[1].setValue(objScope);
	}

	if(objScope == 0L)
	{
		if(sfrmCaller != 0L)
		{
			objScope = sfrmCaller->makeScopeChainObject(ctx);	// ⇒ parent = js_GetScopeChain(cx, caller);
			if(objScope == 0L)
			{
				return false;
			}
		}
	}
	else
	{
		// ※※※ 実装不要 ※※※ ⇒ js_GetWrappedObject, JSCLASS_IS_EXTENDED, OBJ_TO_INNER_OBJECT
		// ※※※ 実装保留 ※※※ ⇒ js_CheckPrincipalsAccess

		// グローバルオブジェクトを得る ⇒ JS_GetGlobalForObject
		EsObject* objGlobal = objScope;
		while(true)
		{
			EsObject* objWk = objGlobal->slots()->parentObject();
			if(objWk == 0L)	{	break;	}
			objGlobal = objWk;
		}

		// with オブジェクトを作成する
		objScope = ctx->newObject(EsWith::EsWithClass::getInstance(), objScope, objGlobal);
		if(objScope == 0L)	{	return false;	}

		vaArg[1].setValue(objScope);
	}

	// ※※※ 実装不要 ※※※ ⇒ js_CheckScopeChainValidity, JSCLASS_IS_EXTENDED
	// ※※※ 実装保留 ※※※ ⇒ JS_EvalFramePrincipals, js_ComputeFilename

	const VcString* strScript = vaArg[0].getString();

	// ※※※ 実装保留 ※※※ ⇒ EvalCacheHash

	// スクリプト解析
	Es es;
	if(!es.parse(ctx, strScript, sfrmCaller, EsBytecodeBuilder::BF_OPT_NAME_REF_ON_SCOPE))	// ⇒ JSCompiler::compileScript(cx, scopeobj, caller, principals, tcflags, JSSTRING_CHARS(str), JSSTRING_LENGTH(str), NULL, file, line, str);
	{
		return false;
	}

	if((numArg < 2) && (sfrmCaller != 0L))
	{
		objScope = sfrmCaller->getScopeChainObject();
	}

	// ※※※ 実装保留 ※※※ ⇒ js_CheckPrincipalsAccess

	// 実行開始
	if(!es.prepareRun(ctx, sfrmCaller, EsStackFrame::F_EVAL, valRet, ctx->getRunParameter()))	// ⇒ js_Execute(cx, scopeobj, script, caller, JSFRAME_EVAL, rval);
	{
		return false;
	}

	// 返り値を実行未終了値に設定する
	valRet->setUnfinished();

	return true;
}

/*---------------------------------------------------------------------*//**
	列挙 ⇒ global_enumerate, JS_EnumerateStandardClasses
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsGlobalClass::callEnumerate(EsContext* ctx, EsObject* obj)
{
	EsScopeProperty* sprop;

	// undefined の定義
	{
		const EsAtom* atom = ctx->getKeywordSet()->getLogicalType(LTYPE_VOID)->getAtom();
		EsValue id(atom);
		if(obj->getScope()->findScopeProperty(&id) == 0L) 	// ⇒ AlreadyHasOwnProperty(cx, obj, atom)
		{
			EsValue valVoid;
			if(!obj->definePropertyOv(&sprop, ctx, &id, &valVoid, g_funcDummyPropertyGetter, g_funcDummyPropertySetter, EsScopeProperty::ATRBF_PERMANENT))
			{
				return false;
			}
		}
	}

	// まだ解決されていない標準クラスの初期化
	for(int i = 0; i < EsSysClasses::getStandardClassNum(); i++)
	{
		const EsClass* cls = EsSysClasses::getStandardClass(i);
		if(cls == 0L)	{	continue;	}
		const EsAtom* atom = cls->getNameAtom();
		if(atom == 0L)	{	continue;	}
		EsValue id(atom);
		if(obj->getScope()->findScopeProperty(&id) == 0L)	// ⇒ AlreadyHasOwnProperty(cx, obj, atom)
		{
			const EsPrototype* ptype = EsSysPrototypes::getPrototype(cls->getPrototypeKind());
			EsObject* objPtype;
			if(ptype == 0L)
			{	
				return false;
			}
			if(!ptype->initClass(&objPtype, ctx, obj))
			{
				return false;
			}
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	解決 ⇒ global_resolve, JS_ResolveStandardClass
**//*---------------------------------------------------------------------*/
bool EsSysClasses::EsGlobalClass::callResolve(EsObject** objParentOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv)
{
	if(!TFW_IS_FLAG(flagsRslv, EsResolver::RSLVF_ASSIGNING))
	{
		// ⇒ JS_ResolveStandardClass
		if(!id->isString())	// ※※※ 未実装 ※※※ ⇒ rt->state, JSRTS_LANDING
		{
			return true;
		}
		const VcString* strId = id->getString();

		// undefined のチェック
		const EsAtom* atom = ctx->getKeywordSet()->getLogicalType(LTYPE_VOID)->getAtom();
		if((atom != 0L) && atom->isSameString(strId))
		{
			// undefined を定義して抜ける
			EsValue valAtom(atom);
			EsValue valVoid;
			if(!obj->definePropertyOv(0L, ctx, &valAtom, &valVoid, g_funcDummyPropertyGetter, g_funcDummyPropertySetter, EsScopeProperty::ATRBF_PERMANENT))
			{
				return false;
			}
			*objParentOut = obj;
			return true;
		}

		// 標準クラスから探してみる
		EsObject* objPtype;
		for(int i = 0; i < EsSysClasses::getStandardClassNum(); i++)
		{
			const EsClass* cls = EsSysClasses::getStandardClass(i);
			if(cls == 0L)	{	continue;	}
			atom = cls->getNameAtom();
			if((atom != 0L) && atom->isSameString(strId))
			{
				if(TFW_IS_FLAG(cls->getFlags(), EsClass::F_IS_ANONYMOUS) && TFW_IS_FLAG(obj->getClass()->getFlags(), EsClass::F_IS_GLOBAL))
				{
					return true;
				}
				const EsPrototype* ptype = EsSysPrototypes::getPrototype(cls->getPrototypeKind());
				if(ptype == 0L)
				{	
					return false;
				}
				if(!ptype->initClass(&objPtype, ctx, obj))
				{
					return false;
				}
				*objParentOut = obj;
				return true;
			}
		}

		// eval のチェック
		atom = EsKeywordSet::getStatic(EsKeywordSet::S_PROP_EVAL)->getAtom();
		if((atom != 0L) && atom->isSameString(strId))
		{
			// eval を定義して抜ける
			if(!initEvalMethod(ctx, obj))
			{
				return false;
			}
			*objParentOut = obj;
			return true;
		}

		// 数値グローバル関数のチェック
		const EsAtom* funcnameNums[] =	// ⇒ standard_class_names の数値クラス部
		{
			EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ISNAN)->getAtom(),
			EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ISFINITE)->getAtom(),
			EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PARSEFLOAT)->getAtom(),
			EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PARSEINT)->getAtom(),
		};
		for(int i = 0; i < sizeof(funcnameNums) / sizeof(const VcString*); i++)
		{
			if(funcnameNums[i]->isSameString(strId))
			{
				// 数値クラスを定義して抜ける
				if(!EsNumber::EsNumberClass::makeClassObject(ctx, obj))
				{
					return false;
				}
				*objParentOut = obj;
				return true;
			}
		}

		// 文字列グローバル関数のチェック
		// URI は当面取り扱わない… ※※※ 実装不要 ※※※ ⇒ string_functions

		// 例外グローバル関数のチェック
		// ※※※ 実装保留 ※※※

		// プロトタイプをチェック
		if(obj->getSlots()->getPrototypeObject() == 0L)
		{
			const EsAtom* funcnamePtypes[] =	// ⇒ object_prototype_names
			{
				EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOSTRING)->getAtom(),
				// ※※※ 実装保留 ※※※ ⇒ toLocaleString
				EsKeywordSet::getStatic(EsKeywordSet::S_PROP_VALUEOF)->getAtom(),
				EsKeywordSet::getStatic(EsKeywordSet::S_PROP_HASOWNPROPERTY)->getAtom(),
				EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ISPROTOTYPEOF)->getAtom(),
				EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PROPERTYISENUMERABLE)->getAtom(),
			};
			for(int i = 0; i < sizeof(funcnamePtypes) / sizeof(const VcString*); i++)
			{
				if(funcnamePtypes[i]->isSameString(strId))
				{
					// オブジェクトクラスを定義して抜ける
					if(!EsObject::EsObjectClass::makeClassObject(ctx, obj))
					{
						return false;
					}
					*objParentOut = obj;
					return true;
				}
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
