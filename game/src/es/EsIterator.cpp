/***********************************************************************//**
 *	EsIterator.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/07/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsIterator.h"

// Friends
#include "EsContext.h"
#include "EsSysClasses.h"
#include "EsKeywordSet.h"
#include "EsDefaultObjectOp.h"
#include "EsRunEnv.h"
#include "EsScopeProperty.h"
#include "EsStackFrame.h"
#include "EsValue.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsIterator メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	イテレータを次へ進める ⇒ js_CallIteratorNext
**//*---------------------------------------------------------------------*/
bool EsIterator::next(EsValue* valReturn, EsObject* objIter, EsContext* ctx)
{
	if(objIter->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ITERATOR)
	{
		EsIterator* iter = (EsIterator*)objIter;
		u8 flagsIter = (u32)iter->getSlots()->getValue(EsIteratorClass::getInstance()->getIteratorFlagsSlotIndex())->getInt();
		if(TFW_IS_FLAG(flagsIter, F_ENUMERATE))
		{
			return iter->nextEnumerator(valReturn, ctx, flagsIter);	// ⇒ CallEnumeratorNext(cx, iterobj, flags, rval);
		}
		return iter->nextNative(valReturn, ctx);	// ⇒ IteratorNextImpl(cx, iterobj, rval)
	}

	EsValue id(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_NEXT)->getAtom());
	if(!objIter->getMethodById(valReturn, ctx, &id))
	{
		return false;
	}
	if(!ctx->getRunEnv()->invokeInternal(valReturn, objIter, valReturn, 0L, 0, 0))	// ⇒ js_InternalCall(cx, iterobj, *rval, 0, NULL, rval)
	{
		// ※※※ 実装保留 ※※※ ⇒ cx->throwing, cx->exception, js_ValueIsStopIteration
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	イテレータを閉じる ⇒ js_CloseIterator
**//*---------------------------------------------------------------------*/
bool EsIterator::close(EsObject* objIter, EsContext* ctx)
{
	if(objIter->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ITERATOR)
	{
		EsIterator* iter = (EsIterator*)objIter;
		iter->closeNative(ctx);
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsIterator::EsIterator()
	: EsObject(EsIteratorClass::getInstance(), OBJT_ITER)
{
}

/*---------------------------------------------------------------------*//**
	作成 ⇒ InitNativeIterator
**//*---------------------------------------------------------------------*/
bool EsIterator::create(EsContext* ctx, EsObject* objParent, u8 flags)
{
	slots()->setParentObject(objParent);
	EsValue valNull(EsValue::VTYPE_NULL);
	slots()->setValue(EsIteratorClass::getInstance()->getIteratorStateSlotIndex(), &valNull);
	EsValue valFlags((s32)flags);
	slots()->setValue(EsIteratorClass::getInstance()->getIteratorFlagsSlotIndex(), &valFlags);

	// ※※※ 実装保留 ※※※ ⇒ js_RegisterCloseableIterator

	if(objParent == 0L)	{	return true;	}

	EsValue valState;
	if(!objParent->enumerateOv(&valState, 0L, ctx, ENUMOP_START))	{	return false;	}
	slots()->setValue(EsIteratorClass::getInstance()->getIteratorStateSlotIndex(), &valState);

	if(TFW_IS_FLAG(flags, F_ENUMERATE))
	{
		ASSERT(this != objParent);
		slots()->setPrototypeObject(objParent);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	列挙子を次に進める ⇒ CallEnumeratorNext
**//*---------------------------------------------------------------------*/
bool EsIterator::nextEnumerator(EsValue* valReturn, EsContext* ctx, u8 flagsIter)
{
	bool isForeach;
	
	ASSERT(TFW_IS_FLAG(flagsIter, F_ENUMERATE));
	ASSERT(getClass()->getClassType() == EsSysClasses::CLASSTYPE_ITERATOR);
	EsValue id;

	EsObject* objWk = slots()->parentObject();
	EsObject* objOrg = slots()->prototypeObject();
	EsValue valState(*getSlots()->getValue(EsIteratorClass::getInstance()->getIteratorStateSlotIndex()));
	if(valState.isNull())
	{
		goto STOP;
	}

	isForeach = TFW_IS_FLAG(flagsIter, F_FOREACH);
	do
	{
		if(!objWk->enumerateOv(&valState, &id, ctx, EsObject::ENUMOP_NEXT))
		{
			return false;
		}

		slots()->setValue(EsIteratorClass::getInstance()->getIteratorStateSlotIndex(), &valState);
		if(valState.isNull())
		{
			objWk = objWk->slots()->prototypeObject();
			if(objWk != 0L)
			{
				slots()->setParentObject(objWk);
				if(!objWk->enumerateOv(&valState, 0L, ctx, EsObject::ENUMOP_START))
				{
					return false;
				}
				slots()->setValue(EsIteratorClass::getInstance()->getIteratorStateSlotIndex(), &valState);
				if(!valState.isNull())
				{
					continue;
				}
			}
			goto STOP;
		}

		EsScopeProperty* sprop;
		EsObject* objOrgParent;
		if(!objOrg->findPropertyOv(&sprop, &objOrgParent, ctx, &id))
		{
			return false;
		}
		if(sprop == 0L)
		{
			continue;
		}
		objOrgParent->dropPropertyOv(ctx, sprop);

		if(objWk != objOrgParent)
		{
			// ※※※ 実装無し ※※※ ⇒ JSCLASS_IS_EXTENDED
			continue;
		}

		if(isForeach)
		{
			if(!objOrg->callGetPropertyOv(valReturn, ctx, &id))
			{
				return false;
			}
		}

		break;
	}
	while(true);

	if(isForeach)
	{
		if(TFW_IS_FLAG(flagsIter, F_KEYVALUE))
		{
			if(!makeKeyValuePair(valReturn, ctx, &id, valReturn))	// ⇒ NewKeyValuePair(cx, id, *rval, rval)
			{
				return false;
			}
		}
	}
	else
	{
		const VcString* cstr;
		if(!id.toString(&cstr, ctx) || (cstr == 0L))
		{
			return false;
		}
		valReturn->setValue(cstr);
	}

	return true;

STOP:
	ASSERT(getSlots()->getValue(EsIteratorClass::getInstance()->getIteratorStateSlotIndex())->isNull());
	valReturn->setHole();
	return true;
}

/*---------------------------------------------------------------------*//**
	ネイティブイテレータの次に進める処理 ⇒ IteratorNextImpl
**//*---------------------------------------------------------------------*/
bool EsIterator::nextNative(EsValue* valReturn, EsContext* ctx)
{
	bool isForeach;
	u32 flagsIter;
	
	ASSERT(getClass()->getClassType() == EsSysClasses::CLASSTYPE_ITERATOR);
	EsValue id;

	EsObject* objIterable = slots()->parentObject();
	ASSERT(objIterable != 0L);
	EsValue valState(*getSlots()->getValue(EsIteratorClass::getInstance()->getIteratorStateSlotIndex()));
	if(valState.isNull())
	{
		goto STOP;
	}

	flagsIter = getSlots()->getValue(EsIteratorClass::getInstance()->getIteratorFlagsSlotIndex())->getInt();
	ASSERT(!TFW_IS_FLAG(flagsIter, F_ENUMERATE));

	isForeach = TFW_IS_FLAG(flagsIter, F_FOREACH);

	if(!objIterable->enumerateOv(&valState, &id, ctx, EsObject::ENUMOP_NEXT))
	{
		return false;
	}
	slots()->setValue(EsIteratorClass::getInstance()->getIteratorStateSlotIndex(), &valState);
	if(valState.isNull())
	{
		goto STOP;
	}

	if(isForeach)
	{
		if(!makeKeyValuePair(valReturn, ctx, &id, valReturn))	// ⇒ NewKeyValuePair(cx, id, *rval, rval)
		{
			return false;
		}
	}
	else
	{
		valReturn->copy(&id);
	}

	return true;

STOP:
	ASSERT(getSlots()->getValue(EsIteratorClass::getInstance()->getIteratorStateSlotIndex())->isNull());
	valReturn->setHole();
	return true;
}

/*---------------------------------------------------------------------*//**
	ネイティブイテレータの閉じる処理 ⇒ js_CloseNativeIterator
**//*---------------------------------------------------------------------*/
void EsIterator::closeNative(EsContext* ctx)
{
	ASSERT(getClass()->getClassType() == EsSysClasses::CLASSTYPE_ITERATOR);

	EsValue valState(*getSlots()->getValue(EsIteratorClass::getInstance()->getIteratorStateSlotIndex()));
	if(valState.isNull())
	{
		return;
	}

	EsObject* objIterable = slots()->parentObject();
	if(objIterable != 0L)
	{
		objIterable->enumerateOv(&valState, 0L, ctx, EsObject::ENUMOP_END);
	}

	EsValue valNull(EsValue::VTYPE_NULL);
	objIterable->slots()->setValue(EsIteratorClass::getInstance()->getIteratorStateSlotIndex(), &valNull);
}

/*---------------------------------------------------------------------*//**
	Key - Value ペア値を作成する ⇒ NewKeyValuePair
**//*---------------------------------------------------------------------*/
bool EsIterator::makeKeyValuePair(EsValue* valReturn, EsContext* ctx, const EsValue* valKey, const EsValue* valValue)
{
	EsValue valarr[2];
	valarr[0].copy(valKey);
	valarr[1].copy(valValue);
	EsArray* arr = ctx->newArray(valarr, 2);
	valReturn->setValue((EsObject*)arr);
	return arr != 0L;
}


//==========================================================================
// EsSysClasses::EsIteratorPrototype メソッド

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool EsIterator::EsIteratorPrototype::initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const
{
	EsObject* objPtype = EsIterator::EsIteratorClass::makeClassObject(ctx, obj);
	*objPrototypeOut = objPtype;
	return objPtype != 0L;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsIterator::EsIteratorPrototype::EsIteratorPrototype()
	: EsPrototype(EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_ITERATOR)->getAtom(), EsSysPrototypes::PTYPEKIND_ITERATOR)
{
}


//==========================================================================
// EsSysClasses::EsIteratorClass メソッド

/*---------------------------------------------------------------------*//**
	イテレータスクリプトクラスの作成 ⇒ js_InitIteratorClasses
**//*---------------------------------------------------------------------*/
EsObject* EsIterator::EsIteratorClass::makeClassObject(EsContext* ctx, EsObject* objParent)
{
	// クラスオブジェクト作成
	EsObject* objPtype;
	EsObject* objCtor;
	objParent->makeClassObject(&objPtype, &objCtor, ctx, getInstance(), 0L, &callConstructor);	// ⇒ js_InitClass(cx, obj, NULL, &js_ObjectClass, js_Object, 1, object_props, object_methods, NULL, object_static_methods);
	if(objPtype == 0L)
	{
		return 0L;
	}
	// 非静的プロパティを定義する
	///objPtype->defineClassProperty();
	// 非静的関数を定義する ⇒ iterator_methods
	///objPtype->defineClassFunction();
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ITERATOR)->getAtom(), callSelf, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_NEXT)->getAtom(), callNext, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT);
	// 静的プロパティを定義する
	///objCtor->defineClassProperty();
	// 静的関数を定義する
	///objCtor->defineClassFunction();

	EsValue valNull(EsValue::VTYPE_NULL);
	objPtype->slots()->setValue(getInstance()->getIteratorStateSlotIndex(), &valNull);

	// ※※※ 実装保留 ※※※ ⇒ js_StopIterationClass

	return objPtype;
}

/*---------------------------------------------------------------------*//**
	イテレータオブジェクトクラスのコンストラクタ ⇒ js_IteratorClass
**//*---------------------------------------------------------------------*/
EsIterator::EsIteratorClass::EsIteratorClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_ITERATOR)->getAtom()
		, EsSysClasses::CLASSTYPE_ITERATOR
		, 0
		, EsSysPrototypes::PTYPEKIND_ITERATOR
		, 2 )
{
	_funcConstruct = callConstructor;
}

/*---------------------------------------------------------------------*//**
	イテレータスクリプトクラスのコンストラクタ ⇒ Iterator
**//*---------------------------------------------------------------------*/
bool EsIterator::EsIteratorClass::callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	bool isKeyOnly;
	vaArg[1].toBoolean(&isKeyOnly, ctx);
	u8 flagsIter = isKeyOnly ? 0 : EsIterator::F_FOREACH;

	if(TFW_IS_FLAG(ctx->getTopStackFrame()->getFlags(), EsStackFrame::F_CONSTRUCTING))	// ⇒ JS_IsConstructing(cx)
	{
		EsObject* objParent;
		if(!vaArg[0].isPrimitive())
		{
			objParent = vaArg[0].getObject();
		}
		else
		{
			if(!ctx->convValueToObject(&objParent, &vaArg[0]))	// ⇒ obj = js_ValueToNonNullObject(cx, argv[0]);
			{
				return false;
			}
			vaArg[0].setValue(objParent);
		}

		ASSERT(objThis->getClass()->getClassType() == EsSysClasses::CLASSTYPE_ITERATOR);
		EsIterator* iter = (EsIterator*)objThis;
		return iter->create(ctx, objParent, flagsIter);
	}

	valRet->copy(&vaArg[0]);
	EsIterator* iter;
	return ctx->convValueToIterator(&iter, valRet, flagsIter);
}

/*---------------------------------------------------------------------*//**
	イテレータ自身を返す ⇒ iterator_self
**//*---------------------------------------------------------------------*/
bool EsIterator::EsIteratorClass::callSelf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	valRet->setValue(ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg));
	return !valRet->isNull();
}

/*---------------------------------------------------------------------*//**
	イテレータスクリプトクラスのコンストラクタ ⇒ iterator_next
**//*---------------------------------------------------------------------*/
bool EsIterator::EsIteratorClass::callNext(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ obj = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}
	if(!ctx->instanceOf(objThis, getInstance(), &vaArg[0]))	{	return false;	}

	EsIterator* iter = (EsIterator*)objThis;
	if(!iter->nextNative(valRet, ctx))
	{
		return false;
	}

	if(valRet->isHole())
	{
		valRet->setNull();
		// ※※※ 実装保留 ※※※ ⇒ js_ThrowStopIteration(cx);
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
