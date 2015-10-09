/***********************************************************************//**
 *	EsObject.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsObject.h"

// Friends
#include "EsAtom.h"
#include "EsClass.h"
#include "EsContext.h"
#include "EsError.h"
#include "EsSysClasses.h"
#include "EsSysPrototypes.h"
#include "EsFunction.h"
#include "EsKeywordSet.h"
#include "EsObjectOp.h"
#include "EsOpcodeType.h"
#include "EsProgramCounter.h"
#include "EsResolver.h"
#include "EsRunEnv.h"
#include "EsScope.h"
#include "EsScopeProperty.h"
#include "EsStackFrame.h"
#include "EsStackRegister.h"
#include "EsValue.h"

// External
#include "../../tfw/collection/Array.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 静的関数

/*---------------------------------------------------------------------*//**
	コンストラクターの GET アクセスチェック ⇒ CheckCtorGetAccess
**//*---------------------------------------------------------------------*/
static bool checkCtorGetAccess(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	u8 spatrbf;
	EsValue valCtorAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CONSTRUCTOR)->getAtom());
	return obj->checkAccessOv(
		&spatrbf,
		val,
		ctx,
		&valCtorAtom,
		ACSF_READ	);
}

/*---------------------------------------------------------------------*//**
	コンストラクターの SET アクセスチェック ⇒ CheckCtorSetAccess
**//*---------------------------------------------------------------------*/
static bool checkCtorSetAccess(EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* val)
{
	u8 spatrbf;
	EsValue valWk(*val);
	EsValue valCtorAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CONSTRUCTOR)->getAtom());
	return obj->checkAccessOv(
		&spatrbf,
		&valWk,
		ctx,
		&valCtorAtom,
		ACSF_WRITE	);
}

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsObject メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	ネイティブオブジェクトであるかどうかを得る ⇒ OBJ_IS_NATIVE
**//*---------------------------------------------------------------------*/
bool EsObject::isNative() const
{
	return _oop->isNative();
}

/*---------------------------------------------------------------------*//**
	関数オブジェクトであるかどうかを得る ⇒ HAS_FUNCTION_CLASS
**//*---------------------------------------------------------------------*/
bool EsObject::isFunction() const
{
	return _class->getClassType() == EsSysClasses::CLASSTYPE_FUNC;
}

/*---------------------------------------------------------------------*//**
	呼び出しオブジェクトの関数を得る ⇒ GetCallObjectFunction
**//*---------------------------------------------------------------------*/
EsFunction* EsObject::getCallObjectFunction() const
{
	ASSERT(_class->getClassType() == EsSysClasses::CLASSTYPE_CALL);	// ⇒ JS_ASSERT(STOBJ_GET_CLASS(obj) == &js_CallClass);
	EsValue* v = _slots->value(EsSysClasses::EsCallClass::getInstance()->getCalleeSlotIndex());
	if(v->isVoid())	// ⇒ if (JSVAL_IS_VOID(v)) {
	{
		return 0L;
	}
	ASSERT(!v->isPrimitive());
	EsObject* obj = v->getObject();
	return (EsFunction*)obj;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	メソッドを得る ⇒ js_GetMethod
**//*---------------------------------------------------------------------*/
bool EsObject::getMethod(EsValue* val, EsContext* ctx, const EsValue* id)
{
	return callGetPropertyOv(val, ctx, id);
}

/*---------------------------------------------------------------------*//**
	コンストラクタを得る ⇒ JS_GetConstructor
**//*---------------------------------------------------------------------*/
bool EsObject::getConstructor(EsObject** objCtor, EsContext* ctx)
{
	EsValue cval;
	{
		EsResolver::AutoFlags raf(ctx->getResolver(), EsResolver::RSLVF_QUALIFIED);	// ⇒ JSAutoResolveFlags rf(cx, JSRESOLVE_QUALIFIED);

		EsValue valCtorAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CONSTRUCTOR)->getAtom());
		if(!callGetPropertyOv(&cval, ctx, &valCtorAtom))
		{
			*objCtor = 0L;
			return false;
		}
	}

	if(!cval.isFunctionObject())
	{
		ctx->addRunError(EsError::ID_NOT_CONSTRUCTOR);
		*objCtor = 0L;
		return false;
	}

	*objCtor = cval.getObject();
	return true;
}

/*---------------------------------------------------------------------*//**
	インスタンス判定をして、プライベートデータを取得する
	⇒ JS_GetInstancePrivate
**//*---------------------------------------------------------------------*/
void* EsObject::getPrivateData(EsContext* ctx, const EsClass* clsInstance, const EsValue* valInstance)
{
	if(clsInstance != 0L)
	{
		if(!ctx->instanceOf(this, clsInstance, valInstance))
		{
			return 0L;
		}
	}
	return _slots->privateData();
}

/*---------------------------------------------------------------------*//**
	メソッドコールに挑戦する ⇒ js_TryMethod
**//*---------------------------------------------------------------------*/
bool EsObject::tryCallMethod(EsValue* valReturn, EsContext* ctx, const EsValue* id, EsValue* vaStack, u32 numArg)
{
	// ※※※ 実装保留 ※※※ ⇒ JS_SetErrorReporter, JS_ClearPendingException
	EsValue valFunc;
	if(!getMethod(&valFunc, ctx, id))
	{
	}

	if(valFunc.isPrimitive())
	{
		return true;
	}
	return ctx->getRunEnv()->invokeInternal(valReturn, this, &valFunc, vaStack, numArg, 0);	// ⇒ js_InternalCall(cx, obj, fval, argc, argv, rval)
}

/*---------------------------------------------------------------------*//**
	valueOf メソッドコールに挑戦する ⇒ js_TryValueOf
**//*---------------------------------------------------------------------*/
bool EsObject::tryCallValueOf(EsValue* valReturn, EsContext* ctx, EsLogicalType ltype)
{
	EsValue valStack(ctx->getKeywordSet()->getLogicalType(LTYPE_VOID)->getAtom());
	EsValue valValueofAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_VALUEOF)->getAtom());
	return tryCallMethod(valReturn, ctx, &valValueofAtom, &valStack, 1);
}

/*---------------------------------------------------------------------*//**
	プロトタイプ チェインを解く ⇒ PurgeProtoChain
**//*---------------------------------------------------------------------*/
bool EsObject::purgePrototypeChain(EsContext* ctx, const EsValue* spropid)
{
	EsObject* objWk = this;
	while(objWk != 0L)
	{
		if(!objWk->isNative())
		{
			objWk = objWk->_slots->prototypeObject();	// ⇒ obj = OBJ_GET_PROTO(cx, obj);
			continue;
		}

		EsScope* scope = objWk->getScope();
		EsScopeProperty* sprop = scope->findScopeProperty(spropid);

		if(sprop != 0L)
		{
			scope->makeUniqueShape(ctx);	// ⇒ js_MakeScopeShapeUnique
			return true;
		}

		objWk = objWk->_slots->prototypeObject();	// ⇒ obj = LOCKED_OBJ_GET_PROTO(scope->object);
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	スコープ チェインを解く ⇒ js_PurgeScopeChain
**//*---------------------------------------------------------------------*/
bool EsObject::purgeScopeChain(EsContext* ctx, const EsValue* spropid)
{
	if(!isDelegateObject())	{	return true;	}

	_slots->prototypeObject()->purgePrototypeChain(ctx, spropid);

	if(_class->getClassType() == EsSysClasses::CLASSTYPE_CALL)
	{
		EsObject* objWk = this->_slots->parentObject();
		while(objWk != 0L)
		{
			if(objWk->purgePrototypeChain(ctx, spropid))
			{
				break;
			}
			objWk = objWk->_slots->parentObject();
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	変更可能なスコープを得る ⇒ js_GetMutableScope
**//*---------------------------------------------------------------------*/
EsScope* EsObject::getMutableScope()
{
	if(_scope->getOwnerObject() == this)
	{
		return _scope;
	}

	EsScope* scopeNew = new EsScope(this, _class->getFreeSlotIndex());	// ⇒ newscope = js_NewScope(cx, scope->map.ops, LOCKED_OBJ_GET_CLASS(obj), obj);

	u16 numReservedSlot;
	if(_class->getAdditionalReservedSlotNum(&numReservedSlot, this))
	{
		s32 slotidxFree = _class->getFreeSlotIndex() + numReservedSlot;	// ⇒ freeslot = JSSLOT_FREE(clasp) + clasp->reserveSlots(cx, obj);
		if(slotidxFree > _slots->getNum())
		{
			slotidxFree = _slots->getNum();
		}
		scopeNew->setFreeSlotIndex(slotidxFree);
	}

	scopeNew->releaseRef(this);	// ⇒ js_DropScope(cx, scope, obj);

	return scopeNew;
}

/*---------------------------------------------------------------------*//**
	ロック済みオブジェクトのスロットに値を書き込む
**//*---------------------------------------------------------------------*/
inline static void writeGcBarrier(EsScope* scope, const EsValue* valOld, const EsValue* valNew)
{
	// ※※※ 実装保留 ※※※ ⇒ #define GC_WRITE_BARRIER(cx,scope,oldval,newval)
}
void EsObject::writeSlotValueLockedBarrier(u32 slotidx, const EsValue* val)
{
	// ⇒ #define LOCKED_OBJ_WRITE_BARRIER(cx,obj,slot,newval)
	ASSERT(_scope != 0L);
	ASSERT(_scope->getOwnerObject() == this);
	writeGcBarrier(_scope, this->_slots->getValue(slotidx), val);
	this->_slots->setValue(slotidx, val);
}

/*---------------------------------------------------------------------*//**
	スロットをフリー化 ⇒ js_FreeSlot
**//*---------------------------------------------------------------------*/
void EsObject::freeSlot(u16 slotidx)
{
	EsValue valVoid;
	_slots->setValue(slotidx, &valVoid);	// ⇒ LOCKED_OBJ_SET_SLOT(obj, slot, JSVAL_VOID);

	if(_scope->getFreeSlotIndex() == (slotidx + 1))
	{
		_scope->setFreeSlotIndex(slotidx);

		// スロット縮小
		_slots->recreate(slotidx);	// ⇒ js_ReallocSlots(cx, obj, slot, JS_FALSE);
	}
}

/*---------------------------------------------------------------------*//**
	指定されたオブジェクトのプロトタイプチェーン内にあるかどうかを返す
	⇒ js_IsDelegate
**//*---------------------------------------------------------------------*/
bool EsObject::isPrototypeOf(const EsValue* valObj)
{
	if(valObj->isPrimitive())
	{
		return false;
	}
	const EsObject* objPtype = valObj->getObject();
	while(true)
	{
		objPtype = objPtype->getSlots()->getPrototypeObject();
		if(objPtype == 0L)	{	break;	}

		if(objPtype == this)
		{
			return true;
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	指定した ID のプロパティは列挙可能であるか調べる
	⇒ js_PropertyIsEnumerable
**//*---------------------------------------------------------------------*/
bool EsObject::checkPropertyIsEnumerable(EsValue* valOut, EsContext* ctx, const EsValue* id)
{
	EsScopeProperty* sprop;
	EsObject* objScope;
	if(!findPropertyOv(&sprop, &objScope, ctx, id))
	{
		return false;
	}
	if(sprop == 0L)
	{
		valOut->setValue(false);
		return true;
	}

	if(	(this != objScope) &&
		!objScope->isNative() &&
		sprop->isSharedPermanent()	)
	{
		objScope->dropPropertyOv(ctx, sprop);
		valOut->setValue(false);
		return true;
	}

	u8 spatrbf;
	bool isOk = objScope->getPropertyAttributeFlagsOv(&spatrbf, ctx, id, sprop);
	objScope->dropPropertyOv(ctx, sprop);
	if(isOk)
	{
		valOut->setValue(TFW_IS_FLAG(spatrbf, EsScopeProperty::ATRBF_ENUMERATE));
	}
	return isOk;
}

/*---------------------------------------------------------------------*//**
	アクセサ追加
	⇒ DefineProperty, DefinePropertyById,　JS_DefineProperty
**//*---------------------------------------------------------------------*/
bool EsObject::addProperty(EsContext* ctx, const EsAtom* atom, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, const EsValue* value, s16 shortid, u8 atrbf, u8 optf)
{
	EsValue spropid;

	if(TFW_IS_FLAG(atrbf, EsScopeProperty::ATRBF_INDEX))
	{
		TFW_SET_FLAG(atrbf, EsScopeProperty::ATRBF_INDEX, false);
	}
	else
	{
		spropid.setValue(atom);
	}

	if((atrbf != 0) && isNative())
	{
		if(!definePropertyNative(
			ctx,
			&spropid,
			value,
			funcGetter,
			funcSetter, 
			atrbf,
			optf,
			shortid,
			false))
		{
			return false;
		}
	}
	else
	{
		EsScopeProperty* sprop;
		if(!definePropertyOv(
			&sprop,
			ctx,
			&spropid,
			value,
			funcGetter,
			funcSetter, 
			atrbf))
		{
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	アクセサ追加（名前文字列指定）
**//*---------------------------------------------------------------------*/
bool EsObject::addProperty(EsContext* ctx, const VcString* name, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, const EsValue* value, s16 shortid, u8 atrbf, u8 optf)
{
	const EsAtom* atom = ctx->makeStringAtom(name);
	if(atom == 0L)	{	return false;	}
	return addProperty(ctx, atom, funcGetter, funcSetter, value, shortid, atrbf, optf);
}

/*---------------------------------------------------------------------*//**
	関数追加 ⇒ JS_DefineFunction, js_DefineFunction
**//*---------------------------------------------------------------------*/
EsFunction* EsObject::addFunction(EsContext* ctx, const EsAtom* atom, EsNativeCall call, u8 atrbf, EsExtParam paramDef)
{
	EsFunction* func = ctx->newFunction(atom, call, this, paramDef);
	if(func == 0L)	{	return 0L;	}

	ASSERT(_scope != 0L);
	EsValue valAtom(atom);
	EsValue valFunc((EsObject*)func);
	if(!definePropertyOv(0L, ctx, &valAtom, &valFunc, 0L, 0L, atrbf))
	{
		return 0L;
	}

	return func;
}

/*---------------------------------------------------------------------*//**
	関数追加（名前文字列指定）
**//*---------------------------------------------------------------------*/
EsFunction* EsObject::addFunction(EsContext* ctx, const VcString* name, EsNativeCall call, u8 atrbf, EsExtParam paramDef)
{
	const EsAtom* atom = ctx->makeStringAtom(name);
	if(atom == 0L)	{	return 0L;	}
	return addFunction(ctx, atom, call, atrbf, paramDef);
}

/*---------------------------------------------------------------------*//**
	クラスオブジェクトを作成する ⇒ JS_InitClass, js_InitClass
**//*---------------------------------------------------------------------*/
bool EsObject::makeClassObject(EsObject** objPtypeOut, EsObject** objCtorOut, EsContext* ctx, const EsClass* cls, EsObject* objParentPtype, EsNativeCall constructor)
{
	bool isNamed;
	EsObject* objCtor;

	const EsAtom* atom = ctx->makeStringAtom(cls->getName());	// ⇒ atom = js_Atomize(cx, clasp->name, strlen(clasp->name), 0);
	if(atom == 0L)
	{
		return false;
	}

	u16 ptypeKey = cls->getPrototypeKind();	// ⇒ key = JSCLASS_CACHED_PROTO_KEY(clasp);
	if((ptypeKey != EsSysPrototypes::PTYPEKIND_NULL) && (objParentPtype != 0L))
	{
		EsValue valPtypeKindObj(EsSysPrototypes::PTYPEKIND_OBJECT);
		if(!this->getClassPrototype(&objParentPtype, ctx, &valPtypeKindObj))
		{
			return false;
		}
	}

	EsObject* objPtype = ctx->newObject(cls, objParentPtype, this);
	if(objPtype == 0L)
	{
		return false;
	}

	// ※※※ 実装保留 ※※※ ⇒ JS_PUSH_TEMP_ROOT_OBJECT

	if(constructor == 0L)
	{
		if(	TFW_IS_FLAG(cls->getFlags(), EsClass::F_IS_ANONYMOUS) &&
			TFW_IS_FLAG(_class->getFlags(), EsClass::F_IS_GLOBAL) &&
			(ptypeKey != EsSysPrototypes::PTYPEKIND_NULL)	)
		{
			isNamed = false;
		}
		else
		{
			EsScopeProperty* sprop = 0L;
			EsValue valAtom(atom);
			EsValue valPtypeObj(objPtype);
			isNamed = definePropertyOv(
				&sprop,
				ctx,
				&valAtom,
				&valPtypeObj, 
				g_funcDummyPropertyGetter,
				g_funcDummyPropertySetter,
				TFW_IS_FLAG(cls->getFlags(), EsClass::F_IS_ANONYMOUS) ? EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT : 0	);
			if(!isNamed)
			{
				goto FAILED;
			}
		}

		objCtor = objPtype;
	}
	else
	{
		EsFunction* func = addFunction(ctx, atom, constructor, 0, EsExtParam());	// ⇒ js_DefineFunction(cx, obj, atom, constructor, nargs, JSFUN_STUB_GSOPS);
		isNamed = (func != 0L);
		if(!isNamed)
		{
			goto FAILED;
		}
		func->setConstructClass(cls);	// コンストラクタ関数にクラスを記憶させる ⇒ FUN_CLASP(fun) = clasp;
		objCtor = func;					// ⇒ FUN_OBJECT(fun)

		// ※※※ 実装なし ※※※ XML 用実装のため対応なし ⇒ if (clasp->flags & JSCLASS_CONSTRUCT_PROTOTYPE)

		// コンストラクタをプロトタイプを接続する
		if(!setClassPrototype(ctx, objCtor, objPtype, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT))	// ⇒ js_SetClassPrototype(cx, ctor, proto, JSPROP_READONLY | JSPROP_PERMANENT)
		{
			goto FAILED;
		}

		// (関数).prototype の起動を可能に
		if(objCtor->_class == cls)	// ⇒ if (OBJ_GET_CLASS(cx, ctor) == clasp)
		{
			objCtor->slots()->setPrototypeObject(objPtype);	// ⇒ OBJ_SET_PROTO(cx, ctor, proto);
		}
	}

	// 標準的なクラスの場合、プロトタイプをキャッシュする
	if(ptypeKey != EsSysPrototypes::PTYPEKIND_NULL)
	{
		if(!setClassObject(ctx, ptypeKey, objCtor))
		{
			goto FAILED;
		}
	}

	*objPtypeOut = objPtype;
	*objCtorOut = objCtor;
	return objPtype != 0L;

FAILED:
	if(isNamed)
	{
		EsValue valRes;
		EsValue valAtom(atom);
		deletePropertyOv(&valRes, ctx, &valAtom);
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	クラス作成時のアクセサ定義 ⇒ JS_DefineProperties
**//*---------------------------------------------------------------------*/
bool EsObject::defineClassProperty(EsContext* ctx, const EsAtom* atom, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, s16 shortid, u8 atrbf)
{
	EsValue valVoid;
	return addProperty(ctx, atom, funcGetter, funcSetter, &valVoid, shortid, atrbf, EsScopeProperty::OPTF_HAS_SHORTID);
}

/*---------------------------------------------------------------------*//**
	クラス作成時の関数定義 ⇒ JS_DefineFunctions
**//*---------------------------------------------------------------------*/
bool EsObject::defineClassFunction(EsContext* ctx, const EsAtom* atom, EsNativeCall call, u8 atrbf)
{
	EsObject* objCtor = 0L;
	if(TFW_IS_FLAG(atrbf, EsFunction::F_LAMBDA))
	{
		if(!getConstructor(&objCtor, ctx))
		{
			return false;
		}

		TFW_SET_FLAG(atrbf, EsFunction::F_LAMBDA, false);

		EsFunction* funcCtor = objCtor->addFunction(
			ctx,
			atom,
			callNativeMethodDispatcher,
			atrbf,
			EsExtParam());
		if(funcCtor == 0L)
		{
			return false;
		}
		// ※※※ 実装保留 ※※※ ⇒ fun->u.n.extra = (uint16)fs->extra;

		EsValue valCall(reinterpret_cast<u64>(call));
		funcCtor->slots()->setReservedSlotValue(ctx, 0, &valCall);	// ⇒ JS_SetReservedSlot(cx, FUN_OBJECT(fun), 0, PRIVATE_TO_JSVAL(fs))
	}

	EsFunction* func = addFunction(
		ctx,
		atom,
		call,
		atrbf,
		EsExtParam());
	if(func == 0L)
	{
		return false;
	}
	// ※※※ 実装保留 ※※※ ⇒ fun->u.n.extra = (uint16)fs->extra;

	return true;
}

/*---------------------------------------------------------------------*//**
	クラスのプロトタイプを得る ⇒ js_GetClassPrototype
**//*---------------------------------------------------------------------*/
bool EsObject::getClassPrototype(EsObject** objPtypeOut, EsContext* ctx, EsObject* objScope, const EsValue* id)
{
	EsValue val;

	if(!findClassObject(&val, ctx, objScope, id))
	{
		return false;
	}
	if(val.isFunctionObject())
	{
		EsObject* objCtor = val.getObject();
		EsValue valPtypeAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PROTOTYPE)->getAtom());
		if(!objCtor->callGetPropertyOv(&val, ctx, &valPtypeAtom))
		{
			return false;
		}

		// ※※※ 実装保留 ※※※ ⇒ cx->weakRoots.newborn[GCX_OBJECT] = JSVAL_TO_GCTHING(v);
	}

	*objPtypeOut = val.isObject() ? val.getObject() : 0L;
	return true;
}

/*---------------------------------------------------------------------*//**
	クラスのプロトタイプを設定する ⇒ js_SetClassPrototype
**//*---------------------------------------------------------------------*/
bool EsObject::setClassPrototype(EsContext* ctx, EsObject* objCtor, EsObject* objPtype, u8 atrbf)
{
	EsScopeProperty* sprop;

	EsValue valPtypeAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PROTOTYPE)->getAtom());
	EsValue valPtypeObj(objPtype);
	if(!objCtor->definePropertyOv(
		&sprop,
		ctx,
		&valPtypeAtom,
		&valPtypeObj,
		g_funcDummyPropertyGetter,
		g_funcDummyPropertySetter,
		atrbf))
	{
		return false;
	}

	EsValue valCtorAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CONSTRUCTOR)->getAtom());
	EsValue valCtorObj(objCtor);
	if(!objPtype->definePropertyOv(
		&sprop,
		ctx,
		&valCtorAtom,
		&valCtorObj,
		checkCtorGetAccess,
		checkCtorSetAccess,
		atrbf))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	クラスオブジェクトを探す ⇒ js_FindClassObject
**//*---------------------------------------------------------------------*/
bool EsObject::findClassObject(EsValue* valOut, EsContext* ctx, EsObject* objStart, const EsValue* id)
{
	// 開始オブジェクトが NULL のときは、グローバルのオブジェクトを利用する
	EsStackFrame* sfrm = 0L;
	if(objStart == 0L)
	{
		sfrm = ctx->getTopStackFrame();
		if(sfrm != 0L)
		{
			objStart = sfrm->getScopeChainObject();
		}
	}

	EsObject* objWk;
	if(objStart != 0L)
	{
		// スコープチェインの最上位オブジェクトを得る
		do
		{
			objWk = objStart;
			objStart = objWk->_slots->parentObject();
		}
		while(objStart != 0L);
	}
	else
	{
		objWk = ctx->getGlobalObject();
		if(objWk == 0L)
		{
			// オブジェクトは見つからなかった
			valOut->setVoid();
			return true;
		}
	}

	// ※※※ 実装不要 ※※※ ⇒ OBJ_TO_INNER_OBJECT, JSExtendedClass, JSExtendedClass

	EsValue idWk(*id);
	if(idWk.isInt())
	{
		u16 pk = (u16)idWk.getInt();
		EsObject* objClass;
		if(!objWk->getClassObject(&objClass, ctx, (u8)pk))
		{
			return false;
		}
		if(objClass != 0L)
		{
			valOut->setValue(objClass);
			return true;
		}
		idWk.setValue(EsSysPrototypes::getPrototype(pk)->getNameAtom());	// ⇒ id = ATOM_TO_JSID(cx->runtime->atomState.classAtoms[key]);
	}

	ASSERT(objWk->isNative());
	EsScopeProperty* sprop;
	EsObject* objParent;
	if(objWk->findPropertyNative(&sprop, &objParent, ctx, &idWk, EsResolver::RSLVF_CLASSNAME) < 0)	// ⇒ if (js_LookupPropertyWithFlags(cx, obj, id, JSRESOLVE_CLASSNAME, &pobj, &prop) < 0)
	{
		return false;
	}

	EsValue valWk;
	if(sprop != 0L)
	{
		if(objParent->isNative())
		{
			if(sprop->getSlotIndex() < objParent->getScope()->getFreeSlotIndex())			// ⇒ if (SPROP_HAS_VALID_SLOT(sprop, OBJ_SCOPE(pobj)))
			{
				valWk.copy(objParent->getSlots()->getValue(sprop->getSlotIndex()));	// ⇒ v = LOCKED_OBJ_GET_SLOT(pobj, sprop->slot);
				if(valWk.isPrimitive())
				{
					valWk.setVoid();
				}
			}
		}
		objParent->dropPropertyOv(ctx, sprop);	// ⇒ OBJ_DROP_PROPERTY(cx, pobj, prop);
	}
	valOut->copy(&valWk);
	return true;
}

/*---------------------------------------------------------------------*//**
	クラスオブジェクトを取得する ⇒ js_GetClassObject
**//*---------------------------------------------------------------------*/
bool EsObject::getClassObject(EsObject** objClassOut, EsContext* ctx, EsObject* obj, u16 ptypekind)
{
	*objClassOut = 0L;

	while(true)
	{
		EsObject* objWk = obj->_slots->parentObject();
		if(objWk == 0L)	{	break;	}
		obj = objWk;
	}
	if(!TFW_IS_FLAG(obj->getClass()->getFlags(), EsClass::F_IS_GLOBAL))
	{
		objClassOut = 0L;
		return true;
	}

	EsValue val;
	if(!obj->_slots->getReservedSlotValue(&val, ctx, (u16)ptypekind))
	{
		return false;
	}
	if(!val.isPrimitive())
	{
		ASSERT(val.isObject());
		*objClassOut = val.getObject();
		return true;
	}

	const EsPrototype* ptype = EsSysPrototypes::getPrototype(ptypekind);
	if(ptype == 0L)
	{
		return false;
	}
	EsResolver* reslv = ctx->getResolver();
	EsResolver::EsResolverHashKey key;
	key._obj = obj;
	key._id = EsValue(ptype->getNameAtom());		// ⇒ rkey.id = ATOM_TO_JSID(cx->runtime->atomState.classAtoms[key]);
	EsResolver::EsResolverHashEntry* entry = 0L;
	if(!reslv->startResolving(&entry, &key, EsResolver::OPF_SEARCH))
	{
		return false;
	}
	if(entry == 0L)
	{
		*objClassOut = 0L;
		return true;
	}
	u32 gencnt = reslv->getGenerationCount();

	EsObject* objPrototype = 0L;
	if(!ptype->initClass(&objPrototype, ctx, obj))
	{
		return false;
	}
	if(objPrototype == 0L)
	{
		return false;
	}
	if(!obj->_slots->getReservedSlotValue(&val, ctx, (u16)ptypekind))
	{
		return false;
	}
	if(!val.isPrimitive())
	{
		ASSERT(val.isObject());
		*objClassOut = val.getObject();
	}

	reslv->stopResolving(&key, EsResolver::OPF_SEARCH, entry, gencnt);
	return *objClassOut != 0L;
}

/*---------------------------------------------------------------------*//**
	クラスオブジェクトを設定する ⇒ js_SetClassObject
**//*---------------------------------------------------------------------*/
bool EsObject::setClassObject(EsContext* ctx, EsObject* obj, u16 ptypekind, EsObject* objClass)
{
	ASSERT(obj->_slots->getParentObject() == 0L);

	if(!TFW_IS_FLAG(obj->_class->getFlags(), EsClass::F_IS_GLOBAL))	// ⇒ if (!(OBJ_GET_CLASS(cx, obj)->flags & JSCLASS_IS_GLOBAL))
	{
		// グローバルクラス以外は不要
		return true;
	}

	EsValue valClassObj(objClass);
	return obj->_slots->setReservedSlotValue(ctx, ptypekind, &valClassObj);
}

/*---------------------------------------------------------------------*//**
	汎用のネイティブメソッド仲介呼び出し関数
	⇒ js_generic_native_method_dispatcher
**//*---------------------------------------------------------------------*/
bool EsObject::callNativeMethodDispatcher(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	ASSERT(valCallee->isObject());
	EsValue valCall;
	valCallee->getObject()->getSlots()->getReservedSlotValue(&valCall, ctx, 0);
	ASSERT(valCall.isInt());
	EsNativeCall call = reinterpret_cast<EsNativeCall>(valCall.getInt64());

	if(vaArg[0].isPrimitive())
	{
		EsObject* objTmp;
		if(!ctx->convValueToObject(&objTmp, &vaArg[0]))
		{
			return false;
		}
		vaArg[0].setValue(objTmp);
	}

	// ⇒ memmove(argv - 1, argv, argc * sizeof(jsval));
	if(numArg > 0)
	{
		valThis->copy(&vaArg[0]);
		for(u32 i = 1; i < numArg; i++)
		{
			vaArg[i - 1].copy(&vaArg[i]);
		}
	}

	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);
	if(objThis == 0L)
	{
		return false;
	}
	EsStackFrame* sfrm = ctx->getTopStackFrame();
	sfrm->setThisObject(objThis);

	if(numArg != 0L)
	{
		numArg--;
		vaArg[numArg].setVoid();
	}

	return call(ctx, objThis, valCallee, valThis, vaArg, numArg, valRet, exprm);
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// 自身のオブジェクト基本操作メソッド

/*---------------------------------------------------------------------*//**
	基底のアクセサ定義 ⇒ js_DefineNativeProperty
**//*---------------------------------------------------------------------*/
EsScopeProperty* EsObject::definePropertyNative(EsContext* ctx, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf, u8 optf, s16 shortid, bool isDontPurge)
{
	if(!isDontPurge)
	{
		purgeScopeChain(ctx, spropid);
	}

	if(isDelegateObject() && TFW_IS_FLAG(atrbf, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_OBJ_SETTER))
	{
		// ※※※ 実装保留  ※※※ ⇒ cx->runtime->protoHazardShape = js_GenerateShape(cx, false);
	}
	
	const EsClass* cls = getClass();
	if(funcGetter == 0L)
	{
		funcGetter = cls->getGetPropertyFunc();
	}
	if(funcSetter == 0L)
	{
		funcSetter = cls->getSetPropertyFunc();
	}

	EsScope* scope = getMutableScope();
	if(scope == 0L)
	{
		return 0L;
	}

	if(TFW_IS_FLAG(cls->getFlags(), EsClass::F_SHARE_ALL_PROPS))
	{
		TFW_SET_FLAG(atrbf, EsScopeProperty::ATRBF_SHARED, true);
	}
	EsScopeProperty* sprop = _scope->addScopeProperty(ctx, spropid, funcGetter, funcSetter, EsScopeProperty::SLOTIDX_INVALUD, atrbf, optf, shortid);		// ⇒ js_AddScopeProperty
	if(sprop == 0L)
	{
		return 0L;
	}

	if(sprop->getSlotIndex() < scope->getFreeSlotIndex())			// ⇒ if (SPROP_HAS_VALID_SLOT(sprop, scope))
	{
		_slots->setValue(sprop->getSlotIndex(), value);	// ⇒ LOCKED_OBJ_WRITE_BARRIER(cx, obj, sprop->slot, value);
	}

	// クラスのアクセサ追加実行 ⇒ ADD_PROPERTY_HELPER
	EsValue valWk(*value);
	EsPropertySetterFunc funcAddProp = cls->getAddPropertyFunc();
	if(funcAddProp != g_funcDummyPropertySetter)
	{
		EsValue valUserId(sprop->getUserId());
		if(!funcAddProp(ctx, this, &valUserId, &valWk))
		{
			return 0L;
		}
	}
	if(!value->equals(&valWk))
	{
		if(sprop->getSlotIndex() < scope->getFreeSlotIndex())		// ⇒ if (SPROP_HAS_VALID_SLOT(sprop, scope))
		{
			_slots->setValue(sprop->getSlotIndex(), &valWk);		// ⇒ LOCKED_OBJ_WRITE_BARRIER(cx, obj, (sprop)->slot, *(vp));
		}
	}

	return sprop;
}

/*---------------------------------------------------------------------*//**
	基底のアクセサ取得 ⇒ js_NativeGet
**//*---------------------------------------------------------------------*/
bool EsObject::callGetPropertyNative(EsValue* val, EsContext* ctx, EsObject* objScopeOwn, EsScopeProperty* sprop)
{
	ASSERT(objScopeOwn != 0L);
	EsScope* scope = objScopeOwn->getScope();
	ASSERT(scope != 0L);
	ASSERT(scope->getOwnerObject() == objScopeOwn);
	ASSERT(objScopeOwn->isNative());

	s32 slotidx = sprop->getSlotIndex();
	if(slotidx != EsScopeProperty::SLOTIDX_INVALUD)	// ⇒ slot != SPROP_INVALID_SLOT
	{
		val->copy(objScopeOwn->_slots->getValue(slotidx));	// ⇒ LOCKED_OBJ_GET_SLOT(pobj, slot)
	}
	else
	{
		val->setVoid();
	}
	if(sprop->getGetterFunc() == 0L)	{	return true;	}	// ⇒ if (SPROP_HAS_STUB_GETTER(sprop))

	// ※※※ 実装保留 ※※※ ⇒ JS_PUSH_TEMP_ROOT_SPROP, JS_PUSH_TEMP_ROOT_OBJECT
	bool isCallGetSucceeded = sprop->callGetProperty(val, ctx, this);
	// ※※※ 実装保留 ※※※ ⇒ JS_POP_TEMP_ROOT
	if(!isCallGetSucceeded)	{	return false;	}

	if((0 <= slotidx) && (slotidx < scope->getFreeSlotIndex()))	// ⇒ SLOT_IN_SCOPE(slot, scope)
	{
		if(scope->findScopeProperty(sprop->getId()) == sprop)	// ⇒ SCOPE_GET_PROPERTY(scope, sprop->id) == sprop
		{
			objScopeOwn->_slots->setValue(slotidx, val);	// ⇒ LOCKED_OBJ_SET_SLOT(pobj, slot, *vp);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	基底のアクセサ設定 ⇒ js_NativeSet
**//*---------------------------------------------------------------------*/
bool EsObject::callSetPropertyNative(EsContext* ctx, EsObject* objScopeOwn, EsScopeProperty* sprop, const EsValue* val)
{
	ASSERT(isNative());
	ASSERT((_scope->getOwnerObject() == this) || TFW_IS_FLAG(sprop->getAtrbFlags(), EsScopeProperty::ATRBF_SHARED));

	s32 slotidx = sprop->getSlotIndex();
	if(slotidx != EsScopeProperty::SLOTIDX_INVALUD)
	{
		ASSERT(!isNative() || (slotidx < _scope->getFreeSlotIndex()));
		if(sprop->getSetterFunc() == 0L)	// ⇒ if (SPROP_HAS_STUB_SETTER(sprop))
		{
			writeSlotValueLockedBarrier(slotidx, val);
			return true;
		}
	}
	else
	{
		if(!TFW_IS_FLAG(sprop->getAtrbFlags(), EsScopeProperty::ATRBF_OBJ_GETTER) && (sprop->getSetterFunc() == 0L))	// ⇒ if (!(sprop->attrs & JSPROP_GETTER) && SPROP_HAS_STUB_SETTER(sprop))
		{
			ASSERT(!TFW_IS_FLAG(sprop->getAtrbFlags(), EsScopeProperty::ATRBF_OBJ_SETTER));
			return true;
		}
	}

	// ※※※ 実装保留 ※※※ ⇒ propertyRemovals
	// ※※※ 実装保留 ※※※ ⇒ JS_PUSH_TEMP_ROOT_SPROP, JS_PUSH_TEMP_ROOT_OBJECT
	bool isCallSetSucceeded = sprop->callSetProperty(ctx, this, val);
	// ※※※ 実装保留 ※※※ ⇒ JS_POP_TEMP_ROOT
	if(!isCallSetSucceeded)	{	return false;	}
	
	EsScope* scope = objScopeOwn->getScope();
	if((0 <= slotidx) && (slotidx < scope->getFreeSlotIndex()))	// ⇒ SLOT_IN_SCOPE(slot, scope)
	{
		if(scope->findScopeProperty(sprop->getId()) == sprop)	// ⇒ SCOPE_GET_PROPERTY(scope, sprop->id) == sprop
		{
			writeSlotValueLockedBarrier(slotidx, val);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	プロパティを探す ⇒ js_LookupPropertyWithFlags
**//*---------------------------------------------------------------------*/
s32 EsObject::findPropertyNative(EsScopeProperty** spropOut, EsObject** objScopeOwnerOut, EsContext* ctx, const EsValue* spropid, u8 flagsResolv)
{
	EsObject* objWk = this;
	EsScope* scopeWk = 0L;
	EsScopeProperty* spropWk = 0L;

	*spropOut = 0L;
	*objScopeOwnerOut = 0L;

	// プロトタイプリンクを走査して探す
	s32 ptypeidx = 0;
	while(true)
	{
		scopeWk = objWk->getScope();
		if(scopeWk != 0L)
		{
			if(scopeWk->getOwnerObject() == objWk)	{	spropWk = scopeWk->findScopeProperty(spropid);	}
			else									{	spropWk = 0L;									}

			// オブジェクトのスコープから見つからなかった場合は、オブジェクトのクラスリゾルバから探してみる
			if(spropWk == 0L)
			{
				const EsClass* cls = objWk->getClass();

				bool isOk = false;
				EsResolver* reslv = ctx->getResolver();
				EsResolver::EsResolverHashKey key;
				key._obj = objWk;
				key._id.copy(spropid);
				EsResolver::EsResolverHashEntry* entry = 0L;
				if(!reslv->startResolving(&entry, &key, EsResolver::OPF_SEARCH))	// ⇒ if (!js_StartResolving(cx, &key, JSRESFLAG_LOOKUP, &entry))
				{
					return -1;
				}
				if(entry == 0L)
				{
					return ptypeidx;
				}
				u32 gencnt = reslv->getGenerationCount();

				*spropOut = 0L;

				if(TFW_IS_FLAG(cls->getFlags(), EsClass::F_RESOLVE_OUT_PARAM))
				{
					if(flagsResolv == EsResolver::RSLVF_INFER)
					{
						flagsResolv = EsResolver::inferFlags(ctx, flagsResolv);
					}

					EsObject* objFound = 0L;	// NULL 固定 ⇒ JSCLASS_NEW_RESOLVE_GETS_START は使用されていない
					ASSERT(cls->getResolvFunc() != 0L);
					isOk = cls->getResolvFunc()(&objFound, ctx, objWk, spropid, flagsResolv);
					if(!isOk)
					{
						goto END_RESOLV;
					}

					if(objFound != 0L)
					{
						ptypeidx = 0;
						for(EsObject* objPtype = this; (objPtype != 0L) && (objPtype != objFound); objPtype = objPtype->slots()->prototypeObject())
						{
							ptypeidx++;
						}
						if(!objFound->isNative())
						{
							isOk = objFound->findPropertyOv(spropOut, objScopeOwnerOut, ctx, spropid);
							if(!isOk || (*spropOut != 0L))
							{
								goto END_RESOLV;
							}
						}
						else
						{
							scopeWk = objFound->getScope();
							if(scopeWk->getOwnerObject() == objFound)
							{
								spropWk = scopeWk->findScopeProperty(spropid);	// ⇒ sprop = SCOPE_GET_PROPERTY(scope, id);
							}
						}
						if(spropWk != 0L)
						{
							objWk = objFound;
						}
					}
				}
				else
				{
					EsObject* objWkDummy = 0L;
					ASSERT(cls->getResolvFunc() != 0L);
					isOk = cls->getResolvFunc()(&objWkDummy, ctx, objWk, spropid, 0);
					if(!isOk)
					{
						goto END_RESOLV;
					}
					ASSERT(objWk->isNative());
					scopeWk = objWk->getScope();
					if(scopeWk->getOwnerObject() == objWk)
					{
						spropWk = scopeWk->findScopeProperty(spropid);	// ⇒ sprop = SCOPE_GET_PROPERTY(scope, id);
					}
				}

			END_RESOLV:
				reslv->stopResolving(&key, EsResolver::OPF_SEARCH, entry, gencnt);	// ⇒ js_StopResolving(cx, &key, JSRESFLAG_LOOKUP, entry, generation);
				if(!isOk)
				{
					return -1;
				}
				if(*spropOut != 0L)
				{
					return ptypeidx;
				}
			}

			// プロパティが見つかった
			if(spropWk != 0L)
			{
				*objScopeOwnerOut = scopeWk->getOwnerObject();
				*spropOut = spropWk;
				return ptypeidx;
			}

			// プロトタイプオブジェクトを探す
			EsObject* objPtype = objWk->slots()->prototypeObject();
			if(objPtype == 0L)	{	break;	}
			if(!objPtype->isNative())
			{
				if(!objPtype->findPropertyOv(spropOut, objScopeOwnerOut, ctx, spropid))
				{
					return -1;
				}
				return ptypeidx + 1;
			}
			objWk = objPtype;
		}
		ptypeidx++;
	}

	return true;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// ベース（非オーバーライド）のオブジェクト操作メソッド

/*---------------------------------------------------------------------*//**
	プロパティを定義する（ベース） ⇒ js_DefineProperty
**//*---------------------------------------------------------------------*/
bool EsObject::definePropertyBase(EsScopeProperty** sprop, EsContext* ctx, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf)
{
	EsScopeProperty* spropTmp = definePropertyNative(ctx, spropid, value, funcGetter, funcSetter, atrbf, 0, 0, false);
	if(spropTmp == 0L)	{	return false;	}
	if(sprop != 0L)	{	*sprop = spropTmp;	}
	return true;
}

/*---------------------------------------------------------------------*//**
	プロパティを削除する（ベース） ⇒ js_DeleteProperty
**//*---------------------------------------------------------------------*/
bool EsObject::deletePropertyBase(EsValue* val, EsContext* ctx, const EsValue* spropid)
{
	EsScopeProperty* sprop;
	EsObject* objPtype;
	if(!findPropertyBase(&sprop, &objPtype, ctx, spropid))
	{
		return false;
	}

	if((sprop == 0L) || (objPtype != this))
	{
		if(sprop != 0L)
		{
			if(objPtype->isNative())
			{
				if(sprop->isSharedPermanent())
				{
					val->setValue(false);
				}
				objPtype->dropPropertyOv(ctx, sprop);
				if(val->isBoolean() && (val->getBoolean() == false))
				{
					return true;
				}
			}
		}
		return _class->getDeletePropertyFunc()(ctx, this, spropid, val);
	}

	if(TFW_IS_FLAG(sprop->getAtrbFlags(), EsScopeProperty::ATRBF_PERMANENT))
	{
		dropPropertyOv(ctx, sprop);
		val->setValue(false);
		return true;
	}

	EsValue valUserId(sprop->getUserId());
	if(_class->getDeletePropertyFunc()(ctx, this, &valUserId, val))
	{
		dropPropertyOv(ctx, sprop);
		return true;
	}

	if(sprop->getSlotIndex() < _scope->getFreeSlotIndex())			// ⇒ if (SPROP_HAS_VALID_SLOT(sprop, scope))
	{
		// ※※※ 実装保留 ※※※ ⇒ GC_POKE(cx, LOCKED_OBJ_GET_SLOT(obj, sprop->slot));
	}

	bool isSuccess = _scope->deleteScopeProperty(ctx, spropid);	// ⇒ js_RemoveScopeProperty

	dropPropertyOv(ctx, sprop);

	return isSuccess;
}

/*---------------------------------------------------------------------*//**
	プロパティをドロップする（ベース） ⇒ js_DropProperty
**//*---------------------------------------------------------------------*/
void EsObject::dropPropertyBase(EsContext* ctx, EsScopeProperty* sprop)
{
	// 何もしない
}

/*---------------------------------------------------------------------*//**
	プロパティを探す（ベース） ⇒ js_LookupProperty
**//*---------------------------------------------------------------------*/
bool EsObject::findPropertyBase(EsScopeProperty** sprop, EsObject** objScopeOwner, EsContext* ctx, const EsValue* spropid)
{
	return findPropertyNative(sprop, objScopeOwner, ctx, spropid, ctx->getResolver()->getResolvFlags()) >= 0;
}

/*---------------------------------------------------------------------*//**
	取得プロパティのコール（ベース） ⇒ js_GetProperty, js_GetPropertyHelper
**//*---------------------------------------------------------------------*/
bool EsObject::callGetPropertyBase(EsValue* val, EsContext* ctx, const EsValue* spropid)
{
	EsScopeProperty* sprop;
	EsObject* objScopeOwn;
	if(!findPropertyBase(&sprop, &objScopeOwn, ctx, spropid))
	{
		return false;
	}

	if(sprop == 0L)
	{
		val->setVoid();

		EsPropertyGetterFunc funcGetProp = _class->getGetPropertyFunc();
		if(funcGetProp != 0L)
		{
			if(!funcGetProp(val, ctx, this, spropid))
			{
				return false;
			}
		}

		if(val->isVoid())
		{
			// 現在のプログラムカウンターを得る ⇒ js_GetCurrentBytecodePC
			const EsProgramCounter* pcCur = 0L;
			EsStackFrame* sfrmCur = ctx->getTopStackFrame();
			if(sfrmCur != 0L)
			{
				EsStackRegister* sregCur = sfrmCur->getStackRegister();
				if(sregCur != 0L)
				{
					pcCur = sregCur->pc();
				}
			}

			if(pcCur != 0L)
			{
				EsOpcodeType ot = static_cast<EsOpcodeType>(pcCur->get(0));
				if(ot == OP_TRAP)
				{
					// ※※※ 実装保留 ※※※
				}
				if(ot != OP_GETXPROP)
				{
					if((ot != OP_GETPROP) && (ot != OP_GETELEM))
					{
						return true;
					}

					// __iterator__ チェック
					if(	(spropid != 0L) &&
						spropid->isAtom() &&
						(spropid->getAtom() == EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ITERATOR)->getAtom())	)
					{
						return true;
					}

					u8 flagsRslv = ctx->getResolver()->getResolvFlags();
					if(flagsRslv == EsResolver::RSLVF_INFER)
					{
						if(pcCur->checkDetecting(ctx, EsOpcode::getSpec(ot)->_length))
						{
							return true;
						}
					}
					else if(TFW_IS_FLAG(flagsRslv, EsResolver::RSLVF_DETECTING))
					{
						return true;
					}
				}

				ctx->getError()->addCompileError(EsError::ID_UNDEFINED_PROP, EsTokenPos(), 0L);
				return false;
			}

		}

		return true;
	}

	if(!objScopeOwn->isNative())
	{
		return objScopeOwn->_oop->callGetProperty(val, ctx, objScopeOwn, spropid);
	}

	if(!callGetPropertyNative(val, ctx, objScopeOwn, sprop))
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	設定プロパティのコール（ベース） ⇒ js_SetProperty, js_SetPropertyHelper
**//*---------------------------------------------------------------------*/
bool EsObject::callSetPropertyBase(EsContext* ctx, const EsValue* spropid, const EsValue* val)
{
	if(TFW_IS_FLAG(_scope->getScopeFlags(), EsScope::SCOPEF_SEALED) && (_scope->getOwnerObject() == this))
	{
		ctx->addRunError(EsError::ID_ERROR_READ_ONLY);
		return false;
	}

	EsScopeProperty* sprop;
	EsObject* objScopeOwn;
	if(!findPropertyBase(&sprop, &objScopeOwn, ctx, spropid))
	{
		return false;
	}

	if(sprop != 0L)
	{
		if(!objScopeOwn->isNative())
		{
			sprop = 0L;
		}
	}
	else
	{
		ASSERT(_class->getClassType() != EsSysClasses::CLASSTYPE_BLOCK);

		if((_slots->getParentObject() == 0L) && !true)	// ※※※ 実装保留 ※※※ ⇒ js_CheckUndeclaredVarAssignment で必ず !JS_HAS_STRICT_OPTION(cx) -> return JS_TRUE; とする
		{
			return false;
		}
	}

	u8 spatrbf = EsScopeProperty::ATRBF_ENUMERATE;
	u8 spoptf = 0;
	s16 shortid = 0;
	EsPropertyGetterFunc funcGetter = 0L;
	EsPropertySetterFunc funcSetter = 0L;

	if(sprop != 0L)
	{
		EsScope* scope = objScopeOwn->getScope();

		spatrbf = sprop->getAtrbFlags();
		if(	TFW_IS_FLAG(spatrbf, EsScopeProperty::ATRBF_READONLY) ||
			(TFW_IS_FLAG(scope->getScopeFlags(), EsScope::SCOPEF_SEALED) && TFW_IS_FLAG(spatrbf, EsScopeProperty::ATRBF_SHARED)) )
		{
			if(TFW_IS_FLAG(spatrbf, EsScopeProperty::ATRBF_READONLY))
			{
				if(true)	// ⇒ if (!JS_HAS_STRICT_OPTION(cx))
				{
					// ※※※ 実装しない ※※※ 結果キャッシュ ⇒ chcheResult

					return true;
				}
			}

			ctx->addRunError(EsError::ID_ERROR_READ_ONLY);
			return false;
		}

		if(objScopeOwn != this)
		{
			if(TFW_IS_FLAG(spatrbf, EsScopeProperty::ATRBF_SHARED))
			{
				// ※※※ 実装しない ※※※ 結果キャッシュ ⇒ chcheResult

				if(	(sprop->getSetterFunc() == 0L) &&	// ⇒ SPROP_HAS_STUB_SETTER(sprop)
					!TFW_IS_FLAG(sprop->getAtrbFlags(), EsScopeProperty::ATRBF_OBJ_GETTER) )
				{
					return true;
				}

				return sprop->callSetProperty(ctx, this, val);
			}

			spatrbf = EsScopeProperty::ATRBF_ENUMERATE;

			if(TFW_IS_FLAG(sprop->getOptionFlags(), EsScopeProperty::OPTF_HAS_SHORTID))
			{
				shortid = sprop->getShortId();
				funcGetter = sprop->getGetterFunc();
				funcSetter = sprop->getSetterFunc();
			}

			sprop = 0L;
		}
	}

	if(sprop == 0L)
	{
		purgeScopeChain(ctx, spropid);

		EsScope* scope = getMutableScope();
		if(scope == 0L)
		{
			return false;
		}

		if(TFW_IS_FLAG(_class->getFlags(), EsClass::F_SHARE_ALL_PROPS))
		{
			TFW_SET_FLAG(spatrbf, EsScopeProperty::ATRBF_SHARED, true);
		}

		sprop = scope->addScopeProperty(ctx, spropid, funcGetter, funcSetter, EsScopeProperty::SLOTIDX_INVALUD, spatrbf, spoptf, shortid);		// ⇒ js_AddScopeProperty
		if(sprop == 0L)
		{
			return false;
		}

		if(sprop->getSlotIndex() < scope->getFreeSlotIndex())			// ⇒ if (SPROP_HAS_VALID_SLOT(sprop, scope))
		{
			EsValue valVoid;
			_slots->setValue(sprop->getSlotIndex(), &valVoid);	// ⇒ LOCKED_OBJ_SET_SLOT(obj, sprop->slot, JSVAL_VOID);
		}

		// クラスのアクセサ追加実行 ⇒ ADD_PROPERTY_HELPER
		EsValue valBk(*val);
		EsPropertySetterFunc funcAddProp = _class->getAddPropertyFunc();
		if(funcAddProp != 0L)
		{
			if(!funcAddProp(ctx, this, spropid, val))
			{
				return false;
			}
		}
		if(!val->equals(&valBk))
		{
			if(sprop->getSlotIndex() < scope->getFreeSlotIndex())		// ⇒ if (SPROP_HAS_VALID_SLOT(sprop, scope))
			{
				_slots->setValue(sprop->getSlotIndex(), val);		// ⇒ LOCKED_OBJ_WRITE_BARRIER(cx, obj, (sprop)->slot, *(vp));
			}
		}
	}

	if(!callSetPropertyNative(ctx, objScopeOwn, sprop, val))
	{
		return false;
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	スコープ属性フラグを得る（ベース） ⇒ js_GetAttributes
**//*---------------------------------------------------------------------*/
bool EsObject::getPropertyAttributeFlagsBase(u8* spatrbf, EsContext* ctx, const EsValue* spropid, EsScopeProperty* sprop)
{
	if(sprop != 0L)
	{
		*spatrbf = sprop->getAtrbFlags();
		return true;
	}

	if(_scope != 0L)
	{
		sprop = _scope->findScopeProperty(spropid);
		if(sprop != 0L)
		{
			*spatrbf = sprop->getAtrbFlags();
			return true;
		}
	}

	return false;
}

/*---------------------------------------------------------------------*//**
	オブジェクトのデフォルト値を得る（ベース） ⇒ js_DefaultValue
**//*---------------------------------------------------------------------*/
bool EsObject::convDefaultValueBase(EsContext* ctx, EsValue* val, EsLogicalType ltypeHint)
{
	EsValue valWk(this);

	switch(ltypeHint)
	{
	default:
		ASSERT(_class->getConvertFunc() != 0L);
		if(!_class->getConvertFunc()(&valWk, ctx, this, ltypeHint))
		{
			return false;
		}
		if(!valWk.isPrimitive())
		{
			EsLogicalType ltype = valWk.getLogicalType();
			if(	(ltype == ltypeHint) ||
				((ltype == LTYPE_FUNC) && (ltypeHint == LTYPE_OBJ))	)
			{
				val->copy(&valWk);
				return true;
			}
			EsValue valTostringAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOSTRING)->getAtom());
			if(!tryCallMethod(
				&valWk,
				ctx,
				&valTostringAtom,
				0L,
				0)	)
			{
				return false;
			}
		}
		break;

	case LTYPE_STRING:
		{
			EsValue valTostringAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOSTRING)->getAtom());
			if(!tryCallMethod(
				&valWk,
				ctx,
				&valTostringAtom,
				0L,
				0)	)
			{
				return false;
			}
		}
		if(!valWk.isPrimitive())
		{
			ASSERT(_class->getConvertFunc() != 0L);
			if(!_class->getConvertFunc()(&valWk, ctx, this, LTYPE_STRING))
			{
				return false;
			}
		}
		break;
	}

	if(!valWk.isPrimitive())
	{
		val->setValue(this);
		ctx->addRunError(EsError::ID_CANT_CONVERT_TO);
		return false;
	}

	val->copy(&valWk);
	return true;
}

/*---------------------------------------------------------------------*//**
	アクセスチェック（ベース） ⇒ js_CheckAccess
**//*---------------------------------------------------------------------*/
bool EsObject::checkAccessBase(u8* spatrbf, EsValue* val, EsContext* ctx, const EsValue* spropid, u8 acsflags)
{
	bool isWriting = TFW_IS_FLAG(acsflags, ACSF_WRITE);
	EsObject* objTrg = 0L;

	switch(acsflags & ACSF_TYPEMASK)
	{
	case ACSF_PTYPE:
		objTrg = this;
		if(!isWriting)
		{
			val->setValue(this->_slots->prototypeObject());
		}
		*spatrbf = EsScopeProperty::ATRBF_PERMANENT;
		break;

	case ACSF_PARENT:
		objTrg = this;
		ASSERT(!isWriting);
		if(!isWriting)
		{
			val->setValue(this->_slots->parentObject());
		}
		*spatrbf = EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT;
		break;

	default:
		{
			EsScopeProperty* sprop;
			if(!this->findPropertyOv(&sprop, &objTrg, ctx, spropid))
			{
				return false;
			}
			if(sprop == 0L)
			{
				if(!isWriting)
				{
					val->setVoid();
				}
				*spatrbf = 0;
				objTrg = this;
				break;
			}

			if(!objTrg->isNative())
			{
				objTrg->dropPropertyOv(ctx, sprop);
				return objTrg->checkAccessOv(spatrbf, val, ctx, spropid, acsflags);
			}

			*spatrbf = sprop->getAtrbFlags();
			if(!isWriting)
			{
				s32 slotidx = sprop->getSlotIndex();
				if(slotidx < objTrg->getScope()->getFreeSlotIndex())	// ⇒ SPROP_HAS_VALID_SLOT(sprop, OBJ_SCOPE(pobj))
				{
					val->copy(objTrg->slots()->getValue(slotidx));	// ⇒ LOCKED_OBJ_GET_SLOT(pobj, sprop->slot)
				}
				else
				{
					val->setVoid();
				}
			}

			objTrg->dropPropertyOv(ctx, sprop);
		}
		break;
	}

	const EsClass* cls = objTrg->getClass();
	// ※※※ 実装なし ※※※ ⇒ JS_GetSecurityCallbacks
	ASSERT(cls->getCheckAccessFunc() != 0L);
	return cls->getCheckAccessFunc()(spatrbf, val, ctx, objTrg, spropid, acsflags);
}

/*---------------------------------------------------------------------*//**
	列挙する（ベース） ⇒ js_Enumerate
**//*---------------------------------------------------------------------*/
bool EsObject::enumerateBase(EsValue* valState, EsValue* id, EsContext* ctx, u8 enumop)
{
	if(TFW_IS_FLAG(_class->getFlags(), EsClass::F_ENUMERATE_OUT_PARAM))
	{
		ASSERT(_class->getEnumrateOutParamFunc() != 0L);
		return _class->getEnumrateOutParamFunc()(valState, id, ctx, this, enumop);
	}

	switch(enumop)
	{
	case ENUMOP_START:
		// 開始処理
		{
			ASSERT(_class->getEnumrateFunc() != 0L);
			if(!_class->getEnumrateFunc()(ctx, this))
			{
				return false;
			}

			EsEnumerator* enmtr = 0L;
			s32 length = 0;
			do
			{
				if(_scope->getOwnerObject() != this)
				{
					break;
				}

				// アクセサ配列を作成する
				Array<const EsValue*>* arrPropId = 0L;
				EsScopeProperty* sprop = _scope->getLastScopeProperty();
				while(sprop != 0L)
				{
					if(	TFW_IS_FLAG(sprop->getAtrbFlags(), EsScopeProperty::ATRBF_ENUMERATE) &&
						!TFW_IS_FLAG(sprop->getOptionFlags(), EsScopeProperty::OPTF_IS_ALIAS) &&
						(_scope->findScopeProperty(sprop->getId()) == sprop)	)	// ⇒ SCOPE_HAS_PROPERTY(scope, sprop)
					{
						if(arrPropId == 0L)	{	arrPropId = new Array<const EsValue*>(true, false);	}
						arrPropId->add(sprop->getId());
						length++;
					}
					sprop = sprop->getParent();
				}

				// 列挙子を作成する
				if(arrPropId != 0L)
				{
					enmtr = new EsEnumerator(arrPropId);
					ctx->registerCustomUnit(enmtr);
					enmtr->_cursor = length;	// 初期位置は末尾から
				}
			}
			while(false);

			if(enmtr == 0L)
			{
				valState->setValue((s32)0);
			}
			else
			{
				valState->setPrivateData(enmtr);
			}

			if(id != 0L)	{	id->setValue(length);	}
		}
		break;
	case ENUMOP_NEXT:
	case ENUMOP_END:
		// 送り・終了処理
		if(valState->isInt() && (valState->getInt() == 0))
		{
			valState->setNull();
			break;
		}

		ASSERT(valState->isPrivateData());
		EsEnumerator* enmtr = (EsEnumerator*)valState->getPrivateData();
		ASSERT(enmtr != 0L);
		ASSERT(enmtr->_cursor >= 1);
		ASSERT(enmtr->_arr->getCount() >= 1);

		switch(enumop)
		{
		case ENUMOP_NEXT:
			// 次へ
			enmtr->_cursor--;
			id->copy(enmtr->_arr->getAt(enmtr->_cursor));
			if(enmtr->_cursor <= 0)
			{
				ctx->deleteRegisteredCustomUnit(enmtr);
				valState->setValue((s32)0);
			}
			break;
		case ENUMOP_END:
			// 終了
			enmtr->_cursor = 0;
			break;
		}
		break;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	必要スロットの値を得る（ベース） ⇒ js_GetRequiredSlot
**//*---------------------------------------------------------------------*/
bool EsObject::getRequiredSlotBase(EsValue* val, EsContext* ctx, s32 slotidx)
{
	if((0 <= slotidx) && (slotidx < _slots->getNum()))
	{
		val->copy(_slots->getValue(slotidx));
	}
	else
	{
		val->setVoid();
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	必要スロットの値を設定する（ベース） ⇒ js_SetRequiredSlot
**//*---------------------------------------------------------------------*/
bool EsObject::setRequiredSlotBase(EsContext* ctx, s32 slotidx, const EsValue* val)
{
	if(slotidx >= _slots->getNum())
	{
		s32 numSlot = _class->getFreeSlotIndex();
		u16 numReservedSlot;
		if(_class->getAdditionalReservedSlotNum(&numReservedSlot, this))
		{
			numSlot += numReservedSlot;
		}
		ASSERT(slotidx < numSlot);
		if(!_slots->recreate(numSlot))
		{
			return false;
		}
	}

	if((_scope->getOwnerObject() == this) && (slotidx >= _scope->getFreeSlotIndex()))
	{
		_scope->setFreeSlotIndex(slotidx + 1);
	}

	_slots->setValue(slotidx, val);

	return true;
}

/*---------------------------------------------------------------------*//**
	コール（ベース） ⇒ js_Call
**//*---------------------------------------------------------------------*/
bool EsObject::callBase(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(_class->getCallFunc() == 0L)
	{
		return false;
	}
	return _class->getCallFunc()(ctx, this, valCallee, valThis, vaArg, numArg, valRet, exprm);
}

/*---------------------------------------------------------------------*//**
	コンストラクト（ベース） ⇒ js_Construct
**//*---------------------------------------------------------------------*/
bool EsObject::constructBase(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(_class->getConstructFunc() == 0L)
	{
		return false;
	}
	return _class->getConstructFunc()(ctx, this, valCallee, valThis, vaArg, numArg, valRet, exprm);
}

/*---------------------------------------------------------------------*//**
	インスタンス型判定（ベース） ⇒ js_HasInstance
**//*---------------------------------------------------------------------*/
bool EsObject::hasInstanceBase(EsValue* valRes, EsContext* ctx, EsObject* obj, const EsValue* valInst)
{
	EsHasInstanceFunc func = _class->getHasInstanceFunc();
	if(func == 0L)
	{
		ctx->addRunError(EsError::ID_BAD_INSTANCEOF_RHS);
	}
	return func(valRes, ctx, obj, valInst);
}

/*---------------------------------------------------------------------*//**
	this オブジェクトを得る（ベース） 
**//*---------------------------------------------------------------------*/
EsObject* EsObject::getThisObjectBase(EsContext* ctx)
{
	return this;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// オーバーライドされたオブジェクト操作メソッド

/*---------------------------------------------------------------------*//**
	プロパティを定義する（オーバーライド）
	⇒ OBJ_DEFINE_PROPERTY
**//*---------------------------------------------------------------------*/
bool EsObject::definePropertyOv(EsScopeProperty** sprop, EsContext* ctx, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf)
{
	return _oop->defineProperty(sprop, ctx, this, spropid, value, funcGetter, funcSetter, atrbf);
}

/*---------------------------------------------------------------------*//**
	プロパティを削除する（オーバーライド）
	⇒ OBJ_DELETE_PROPERTY
**//*---------------------------------------------------------------------*/
bool EsObject::deletePropertyOv(EsValue* val, EsContext* ctx, const EsValue* spropid)
{
	return _oop->deleteProperty(val, ctx, this, spropid);
}

/*---------------------------------------------------------------------*//**
	プロパティをドロップする（オーバーライド）
	⇒ OBJ_DROP_PROPERTY
**//*---------------------------------------------------------------------*/
void EsObject::dropPropertyOv(EsContext* ctx, EsScopeProperty* sprop)
{
	return _oop->dropProperty(ctx, this, sprop);
}

/*---------------------------------------------------------------------*//**
	プロパティを探す（オーバーライド）
	⇒ OBJ_LOOKUP_PROPERTY
**//*---------------------------------------------------------------------*/
bool EsObject::findPropertyOv(EsScopeProperty** sprop, EsObject** objScopeOwner, EsContext* ctx, const EsValue* spropid)
{
	return _oop->findProperty(sprop, objScopeOwner, ctx, this, spropid);
}

/*---------------------------------------------------------------------*//**
	アクセサ取得（オーバーライド）
	⇒ OBJ_GET_PROPERTY
**//*---------------------------------------------------------------------*/
bool EsObject::callGetPropertyOv(EsValue* val, EsContext* ctx, const EsValue* spropid)
{
	return _oop->callGetProperty(val, ctx, this, spropid);
}

/*---------------------------------------------------------------------*//**
	アクセサ設定（オーバーライド）
	⇒ OBJ_SET_PROPERTY
**//*---------------------------------------------------------------------*/
bool EsObject::callSetPropertyOv(EsContext* ctx, const EsValue* spropid, const EsValue* val)
{
	return _oop->callSetProperty(ctx, this, spropid, val);
}

/*---------------------------------------------------------------------*//**
	スコープ属性フラグを得る（オーバーライド）
	⇒ OBJ_GET_ATTRIBUTES
**//*---------------------------------------------------------------------*/
bool EsObject::getPropertyAttributeFlagsOv(u8* spatrbf, EsContext* ctx, const EsValue* spropid, EsScopeProperty* sprop)
{
	return _oop->getPropertyAttributeFlags(spatrbf, ctx, this, spropid, sprop);
}

/*---------------------------------------------------------------------*//**
	オブジェクトのデフォルト値を得る（オーバーライド） 
	⇒ OBJ_DEFAULT_VALUE
**//*---------------------------------------------------------------------*/
bool EsObject::convDefaultValueOv(EsContext* ctx, EsValue* val, EsLogicalType ltype)
{
	return _oop->convDefaultValue(ctx, this, val, ltype);
}

/*---------------------------------------------------------------------*//**
	アクセスチェック（オーバーライド） 
	⇒ OBJ_CHECK_ACCESS
**//*---------------------------------------------------------------------*/
bool EsObject::checkAccessOv(u8* spatrbf, EsValue* val, EsContext* ctx, const EsValue* spropid, u8 acsflags)
{
	return _oop->checkAccess(spatrbf, val, ctx, this, spropid, acsflags);
}

/*---------------------------------------------------------------------*//**
	列挙（オーバーライド） 
	⇒ OBJ_ENUMERATE
**//*---------------------------------------------------------------------*/
bool EsObject::enumerateOv(EsValue* valState, EsValue* id, EsContext* ctx, u8 enumop)
{
	return _oop->enumerate(valState, id, ctx, this, enumop);
}

/*---------------------------------------------------------------------*//**
	必要スロットの値を得る（オーバーライド）
	⇒ OBJ_GET_REQUIRED_SLOT
**//*---------------------------------------------------------------------*/
bool EsObject::getRequiredSlotOv(EsValue* val, EsContext* ctx, s32 slotidx)
{
	return _oop->getRequiredSlot(val, ctx, this, slotidx);
}

/*---------------------------------------------------------------------*//**
	必要スロットの値を設定する（オーバーライド）
	⇒ OBJ_SET_REQUIRED_SLOT
**//*---------------------------------------------------------------------*/
bool EsObject::setRequiredSlotOv(EsContext* ctx, s32 slotidx, const EsValue* val)
{
	return _oop->setRequiredSlot(ctx, this, slotidx, val);
}

/*---------------------------------------------------------------------*//**
	コール（オーバーライド）
**//*---------------------------------------------------------------------*/
bool EsObject::callOv(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	return _oop->call(ctx, this, valCallee, valThis, vaArg, numArg, valRet, exprm);
}

/*---------------------------------------------------------------------*//**
	コンストラクト（オーバーライド）
**//*---------------------------------------------------------------------*/
bool EsObject::constructOv(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	return _oop->construct(ctx, this, valCallee, valThis, vaArg, numArg, valRet, exprm);
}

/*---------------------------------------------------------------------*//**
	インスタンス型判定（オーバーライド）
**//*---------------------------------------------------------------------*/
bool EsObject::hasInstanceOv(EsValue* valRes, EsContext* ctx, EsObject* obj, const EsValue* valInst)
{
	return _oop->hasInstance(valRes, ctx, this, valInst);
}

/*---------------------------------------------------------------------*//**
	this オブジェクトを得る（オーバーライド）
	⇒ OBJ_THIS_OBJECT
**//*---------------------------------------------------------------------*/
EsObject* EsObject::getThisObjectOv(EsContext* ctx)
{
	return (_oop->getGetThisObjectFunc() != 0L) ? _oop->getThisObject(ctx, this) : this;
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// API

/*---------------------------------------------------------------------*//**
	ID からメソッドを得る ⇒ JS_GetMethodById
**//*---------------------------------------------------------------------*/
bool EsObject::getMethodById(EsValue* val, EsContext* ctx, const EsValue* id)
{
	EsResolver::AutoFlags raf(ctx->getResolver(), EsResolver::RSLVF_QUALIFIED);	// ⇒ JSAutoResolveFlags rf(cx, JSRESOLVE_QUALIFIED);

	return getMethod(val, ctx, id);
}

/*---------------------------------------------------------------------*//**
	要素の値を得る ⇒ JS_GetElement
**//*---------------------------------------------------------------------*/
bool EsObject::getElement(EsValue* valOut, EsContext* ctx, s32 index)
{
	EsResolver::AutoFlags raf(ctx->getResolver(), EsResolver::RSLVF_QUALIFIED);	// ⇒ JSAutoResolveFlags rf(cx, JSRESOLVE_QUALIFIED);

	EsValue valIndex(index);
	return callGetPropertyOv(valOut, ctx, &valIndex);
}

/*---------------------------------------------------------------------*//**
	要素の値を設定する ⇒ JS_SetElement
**//*---------------------------------------------------------------------*/
bool EsObject::setElement(EsContext* ctx, s32 index, const EsValue* val)
{
	EsResolver::AutoFlags raf(ctx->getResolver(), EsResolver::RSLVF_QUALIFIED | EsResolver::RSLVF_ASSIGNING);	// ⇒ JSAutoResolveFlags rf(cx, JSRESOLVE_QUALIFIED | JSRESOLVE_ASSIGNING);

	EsValue valIndex(index);
	return callSetPropertyOv(ctx, &valIndex, val);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsObject::EsObject(const EsClass* cls, u8 objct)
	: _class(cls)
	, _scope(0L)
	, _oop(0L)
	, _slots(0L)
	, _objflags(0)
	, _objt(objct)
{
	_oop = cls->getObjectOp();
	_slots = new EsObjectSlots(this);

	createScope(_oop);	// ⇒ InitScopeForObject
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsObject::~EsObject()
{
	delete _slots;
	delete _oop;
	delete _scope;
}

/*---------------------------------------------------------------------*//**
	スコープ作成 ⇒ InitScopeForObject
**//*---------------------------------------------------------------------*/
bool EsObject::createScope(EsObjectOp* oop)
{
	_scope = new EsScope(this, _class->getFreeSlotIndex());	// ⇒ JSScope *scope = js_NewScope(cx, ops, clasp, obj);

	if(_scope->getFreeSlotIndex() > _slots->getNum())
	{
		// オブジェクトスロットが足りない場合は拡張する ⇒ js_ReallocSlots
		if(!_slots->recreate(_scope->getFreeSlotIndex()))
		{
			delete _scope;
			_scope = 0L;
			return false;
		}
	}

	return _scope != 0L;
}


//==========================================================================
// EsObject::EsObjectPrototype メソッド

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool EsObject::EsObjectPrototype::initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const
{
	return initObjectAndFunctionClass(objPrototypeOut, ctx, obj);
}

/*---------------------------------------------------------------------*//**
	オブジェクトと関数プロトタイプの初期化
	⇒ js_InitFunctionAndObjectClasses
**//*---------------------------------------------------------------------*/
bool EsObject::EsObjectPrototype::initObjectAndFunctionClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj)
{
	EsObject* objFuncPtype = 0L;
	EsObject* objObjPtype = 0L;

	// コンテキストにグローバルオブジェクトが設定されていない場合、このオブジェクトを設定する
	if(ctx->getGlobalObject() == 0L)
	{
		ctx->setGlobalObject(obj);
	}

	// リゾルバを得る
	EsResolver* reslv = ctx->getResolver();
	bool isResolving = reslv->getEntryCount() > 0;

	// リゾルバキーを作成
	EsResolver::EsResolverHashKey key;
	key._obj = obj;

	EsResolver::EsResolverHashEntry* entry = 0L;
	if(isResolving)	// リゾルバ開始済み
	{
		key._id = EsValue(EsSysPrototypes::getPrototype(EsSysPrototypes::PTYPEKIND_FUNCTION)->getNameAtom());	// ⇒ key.id = ATOM_TO_JSID(rt->atomState.classAtoms[JSProto_Function]);
		entry = reslv->add(&key);
		if(entry != 0L)
		{
			if((entry->getKey()->_obj != 0L) && TFW_IS_FLAG(entry->_flags, EsResolver::OPF_SEARCH))
			{
				ASSERT(entry->getKey()->_obj == obj);
				key._id = EsValue(EsSysPrototypes::getPrototype(EsSysPrototypes::PTYPEKIND_OBJECT)->getNameAtom());	// ⇒ key.id = ATOM_TO_JSID(rt->atomState.classAtoms[JSProto_Object]);
				entry = reslv->add(&key);
			}
		}
		if(entry == 0L)
		{
			ASSERT(false);
			return false;
		}
		ASSERT((entry->getKey()->_obj == 0L) && (entry->_flags == 0));
		entry->key()->copy(&key);					// ⇒ entry->key = key;
		entry->_flags = EsResolver::OPF_SEARCH;
	}
	else
	{
		key._id = EsValue(EsSysPrototypes::getPrototype(EsSysPrototypes::PTYPEKIND_OBJECT)->getNameAtom());		// ⇒ key.id = ATOM_TO_JSID(rt->atomState.classAtoms[JSProto_Object]);
		if(!reslv->startResolving(&entry, &key, EsResolver::OPF_SEARCH))	// ⇒ if (!js_StartResolving(cx, &key, JSRESFLAG_LOOKUP, &entry))
		{
			return false;
		}

		key._id = EsValue(EsSysPrototypes::getPrototype(EsSysPrototypes::PTYPEKIND_FUNCTION)->getNameAtom());	// ⇒ key.id = ATOM_TO_JSID(rt->atomState.classAtoms[JSProto_Function]);
		if(!reslv->startResolving(&entry, &key, EsResolver::OPF_SEARCH))	// ⇒ if (!js_StartResolving(cx, &key, JSRESFLAG_LOOKUP, &entry))
		{
			key._id = EsValue(EsSysPrototypes::getPrototype(EsSysPrototypes::PTYPEKIND_OBJECT)->getNameAtom());		// ⇒ key.id = ATOM_TO_JSID(rt->atomState.classAtoms[JSProto_Object]);
			reslv->remove(&key);
			return false;
		}
	}

	// 関数クラスの初期化
	EsValue valPtypeKindFunc(EsSysPrototypes::PTYPEKIND_FUNCTION);
	if(!obj->getClassPrototype(&objFuncPtype, ctx, &valPtypeKindFunc))	// ⇒ if (!js_GetClassPrototype(cx, obj, INT_TO_JSID(JSProto_Function), &fun_proto))
	{
		goto END;
	}
	if(objFuncPtype == 0L)
	{
		objFuncPtype = EsFunction::EsFunctionClass::makeClassObject(ctx, obj);	// ⇒ js_InitFunctionClass
		if(objFuncPtype == 0L)
		{
			goto END;
		}
	}
	else
	{
		EsObject* objCtor;
		objFuncPtype->getConstructor(&objCtor, ctx);
		if(objCtor == 0L)
		{
			objFuncPtype = 0L;
			goto END;
		}

		EsScopeProperty* sprop;
		EsValue valNameAtom(EsFunction::EsFunctionClass::getInstance()->getNameAtom());
		EsValue valCtorObj(objCtor);
		obj->definePropertyOv(
			&sprop,
			ctx, 
			&valNameAtom,
			&valCtorObj,
			0L,
			0L,
			0	);
	}

	// オブジェクトクラスの初期化
	{
		EsValue valPtypeKindObj(EsSysPrototypes::PTYPEKIND_OBJECT);
		if(!obj->getClassPrototype(&objObjPtype, ctx, &valPtypeKindObj))	// ⇒ js_GetClassPrototype(cx, obj, INT_TO_JSID(JSProto_Object), &obj_proto)
		{
			objFuncPtype = 0L;
			goto END;
		}
	}
	if(objObjPtype == 0L)
	{
		objObjPtype = EsObject::EsObjectClass::makeClassObject(ctx, obj);
	}
	if(objObjPtype == 0L)
	{
		objFuncPtype = 0L;
		goto END;
	}

	// 関数プロトタイプとグローバルオブジェクトはオブジェクトプロトタイプに委譲する
	objFuncPtype->slots()->setPrototypeObject(objObjPtype);	// ⇒ OBJ_SET_PROTO(cx, fun_proto, obj_proto);
	if(obj->getSlots()->getPrototypeObject() == 0L)	// ⇒ if (!OBJ_GET_PROTO(cx, obj))
	{
		obj->slots()->setPrototypeObject(objObjPtype);	// ⇒ OBJ_SET_PROTO(cx, obj, obj_proto);
	}

END:
	reslv->remove(&key);
	if(!isResolving)
	{
		EsValue valFunctionAtom(EsSysPrototypes::getPrototype(EsSysPrototypes::PTYPEKIND_FUNCTION)->getNameAtom());
		ASSERT(key._id.equals(&valFunctionAtom));	// ⇒ JS_ASSERT(key.id == ATOM_TO_JSID(rt->atomState.classAtoms[JSProto_Function]));
		key._id = EsValue(EsSysPrototypes::getPrototype(EsSysPrototypes::PTYPEKIND_OBJECT)->getNameAtom());	// ⇒ key.id = ATOM_TO_JSID(rt->atomState.classAtoms[JSProto_Object]);
		reslv->remove(&key);
	}

	*objPrototypeOut = objFuncPtype;
	return objFuncPtype != 0L;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsObject::EsObjectPrototype::EsObjectPrototype()
	: EsPrototype(EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_OBJECT)->getAtom(), EsSysPrototypes::PTYPEKIND_OBJECT)
{
}


//==========================================================================
// EsObject::EsObjectClass メソッド

/*---------------------------------------------------------------------*//**
	オブジェクトスクリプトクラスの作成 ⇒ js_InitObjectClass
**//*---------------------------------------------------------------------*/
EsObject* EsObject::EsObjectClass::makeClassObject(EsContext* ctx, EsObject* objParent)
{
	// クラスオブジェクト作成
	EsObject* objPtype;
	EsObject* objCtor;
	objParent->makeClassObject(&objPtype, &objCtor, ctx, getInstance(), 0L, &callConstructor);	// ⇒ js_InitClass(cx, obj, NULL, &js_ObjectClass, js_Object, 1, object_props, object_methods, NULL, object_static_methods);
	if(objPtype == 0L)
	{
		return 0L;
	}
	// 非静的プロパティを定義する ⇒ object_props
	///objPtype->defineClassProperty();
	// 非静的関数を定義する ⇒ object_methods
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOSTRING)->getAtom(), callToString, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_VALUEOF)->getAtom(), callValueOf, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_HASOWNPROPERTY)->getAtom(), callHasOwnProperty, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ISPROTOTYPEOF)->getAtom(), callIsPrototypeOf, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PROPERTYISENUMERABLE)->getAtom(), callPropertyIsEnumerable, 0);
	// 静的プロパティを定義する
	///objCtor->defineClassProperty();
	// 静的関数を定義する ⇒ object_static_methods
	objCtor->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_GETPROTOTYPEOF)->getAtom(), callGetPrototypeOf, 0);

	return objPtype;
}

/*---------------------------------------------------------------------*//**
	オブジェクトオブジェクトクラスのコンストラクタ
**//*---------------------------------------------------------------------*/
EsObject::EsObjectClass::EsObjectClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_OBJECT)->getAtom()
		, EsSysClasses::CLASSTYPE_OBJECT
		, 0
		, EsSysPrototypes::PTYPEKIND_OBJECT
		, 0 )
{
	_funcConstruct = callConstructor;
}

/*---------------------------------------------------------------------*//**
	オブジェクトスクリプトクラスのコンストラクタ ⇒ js_Object
**//*---------------------------------------------------------------------*/
bool EsObject::EsObjectClass::callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg == 0)
	{
		objThis = 0L;
	}
	else
	{
		if(!ctx->convValueToObject(&objThis, &vaArg[0]))
		{
			return false;
		}
	}
	if(objThis == 0L)
	{
		if(TFW_IS_FLAG(ctx->getTopStackFrame()->getFlags(), EsStackFrame::F_CONSTRUCTING))	// ⇒ JS_IsConstructing(cx)
		{
			return true;
		}
		objThis = ctx->newObject(getInstance(), 0L, 0L);
		if(objThis == 0L)
		{
			return false;
		}
	}

	valRet->setValue(objThis);	// ⇒ *rval = OBJECT_TO_JSVAL(obj);

	return true;
}

/*---------------------------------------------------------------------*//**
	文字列化 ⇒ obj_toString
**//*---------------------------------------------------------------------*/
bool EsObject::EsObjectClass::callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ obj = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}

	VcString strWk;
	VcString::format(&strWk, "[object %s]", objThis->getClass()->getName()->getRaw());
	const VcString* cstr = ctx->newString(&strWk);
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	オブジェクトの基本データ型の値を返す ⇒ obj_valueOf
**//*---------------------------------------------------------------------*/
bool EsObject::EsObjectClass::callValueOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	valRet->setValue(ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg));	// ⇒ *vp = JS_THIS(cx, vp);
	return !valRet->isNull();
}

/*---------------------------------------------------------------------*//**
	指定した名前のプロパティがオブジェクトにあるかどうかを返す
	⇒ obj_hasOwnProperty
**//*---------------------------------------------------------------------*/
bool EsObject::EsObjectClass::callHasOwnProperty(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);
	if(objThis == 0L)	{	return false;	}

	// アクセサ ID を作成する
	EsValue id;
	EsValue valVoid;
	if(!ctx->convValueToId(&id, (numArg != 0) ? &vaArg[0] : &valVoid))	// ⇒ if (!JS_ValueToId(cx, argc != 0 ? vp[2] : JSVAL_VOID, &id))
	{
		return false;
	}

	// プロパティについて判定 ⇒ js_HasOwnProperty
	EsScopeProperty* sprop;
	EsObject* objScope;
	if(!objThis->findPropertyOv(&sprop, &objScope, ctx, &id))
	{
		return false;
	}
	if(sprop == 0L)
	{
		valRet->setValue(false);
	}
	else if(objScope == objThis)
	{
		valRet->setValue(true);
	}
	else
	{
		// ※※※ 実装無し ※※※ ⇒ JSCLASS_IS_EXTENDED

		if((objScope != 0L) && (objThis->getClass() == objScope->getClass()))
		{
			valRet->setValue(sprop->isSharedPermanent());
		}
		else
		{
			valRet->setValue(true);
		}
	}

	if(sprop != 0L)
	{
		objScope->dropPropertyOv(ctx, sprop);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	指定されたオブジェクトのプロトタイプチェーン内にあるかどうかを返す
	⇒ obj_isPrototypeOf
**//*---------------------------------------------------------------------*/
bool EsObject::EsObjectClass::callIsPrototypeOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);
	if(objThis == 0L)
	{
		valRet->setValue(false);
	}
	else
	{
		EsValue valVoid;
		valRet->setValue(objThis->isPrototypeOf((numArg != 0) ? &vaArg[0] : &valVoid));	// ⇒ js_IsDelegate(cx, JS_THIS_OBJECT(cx, vp), argc != 0 ? vp[2] : JSVAL_VOID, &b)
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	指定したプロパティが列挙可能かを返す
	⇒ obj_propertyIsEnumerable
**//*---------------------------------------------------------------------*/
bool EsObject::EsObjectClass::callPropertyIsEnumerable(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);
	if(objThis == 0L)	{	return false;	}

	// アクセサ ID を作成する
	EsValue id;
	EsValue valVoid;
	if(!ctx->convValueToId(&id, (numArg != 0) ? &vaArg[0] : &valVoid))	// ⇒ if (!JS_ValueToId(cx, argc != 0 ? vp[2] : JSVAL_VOID, &id))
	{
		return false;
	}

	return objThis->checkPropertyIsEnumerable(valRet, ctx, &id);
}

/*---------------------------------------------------------------------*//**
	プロトタイプチェーン（__proto__）のアクセスチェック
	⇒ obj_getPrototypeOf
**//*---------------------------------------------------------------------*/
bool EsObject::EsObjectClass::callGetPrototypeOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg == 0L)
	{
		ctx->addRunError(EsError::ID_MISSING_FUN_ARG);
		return false;
	}

	if(!ctx->convValueToObject(&objThis, &vaArg[0]) || (objThis == 0L))
	{
		return false;
	}
	vaArg[0].setValue(objThis);

	u8 spatrbf;
	EsValue valProtoAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PROTO)->getAtom());
	return objThis->checkAccessOv(&spatrbf, valRet, ctx, &valProtoAtom, ACSF_PTYPE);
}

//==========================================================================
// EsObject::EsEnumerator メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsObject::EsEnumerator::EsEnumerator(Array<const EsValue*>* arrEntr)
	: _arr(arrEntr)
	, _cursor(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsObject::EsEnumerator::~EsEnumerator()
{
	delete _arr;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
