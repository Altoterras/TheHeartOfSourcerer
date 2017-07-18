/***********************************************************************//**
 *	EsFunction.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsFunction.h"

// Friends
#include "EsArray.h"
#include "EsAtom.h"
#include "EsAtomList.h"
#include "EsClass.h"
#include "EsContext.h"
#include "EsSysClasses.h"
#include "EsInterlang.h"
#include "EsKeywordSet.h"
#include "EsObject.h"
#include "EsDefaultObjectOp.h"
#include "EsParser.h"
#include "EsResolver.h"
#include "EsRunEnv.h"
#include "EsScopeProperty.h"
#include "EsStackFrame.h"
#include "EsValue.h"

// External
#include "../tfw/collection/List.h"
#include "../tfw/lib/TypeUtils.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	引数の数を得る ⇒ nargs ... 
**//*---------------------------------------------------------------------*/
u32 EsFunction::getLocalVarNum(LocalVarKind lvk) const
{
	if(_listLocalVar[lvk] == 0L)	{	return 0;	}
	return _listLocalVar[lvk]->getCount();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	局所変数を追加する ⇒ js_AddLocal
**//*---------------------------------------------------------------------*/
bool EsFunction::addLocalVar(const EsAtom* atomVar, LocalVarKind lvk)
{
	ASSERT(_listLocalVar[lvk] != 0L);
	_listLocalVar[lvk]->addTail(new LocalVar(atomVar));
	return true;
}

/*---------------------------------------------------------------------*//**
	局所変数を検索する ⇒ js_LookupLocal
**//*---------------------------------------------------------------------*/
bool EsFunction::findLocalVar(LocalVarKind* lvkOut, u32* indexOut, const EsAtom* atomVar)
{
	for(int i = 0; i < NUM_LVK; i++)
	{
		u32 index = 0;
		for(ListIterator<LocalVar*> it = _listLocalVar[i]->iterator(); it.has(); it.next())
		{
			LocalVar* lv = it.object();
			if(lv->_atom == atomVar)
			{
				if(lvkOut != 0L)	{	*lvkOut = (LocalVarKind)i;	}
				if(indexOut != 0L)	{	*indexOut = index;			}
				return true;
			}

			index++;
		}
	}
	if(lvkOut != 0L)	{	*lvkOut = LVK_NULL;			}
	if(indexOut != 0L)	{	*indexOut = 0xffffffff;		}
	return false;
}

/*---------------------------------------------------------------------*//**
	局所変数名を列挙する ⇒ js_GetLocalNameArray
**//*---------------------------------------------------------------------*/
bool EsFunction::enumLocalAtoms(EsAtomList* alist, LocalVarKind lvk)
{
	for(ListIterator<LocalVar*> it = _listLocalVar[lvk]->iterator(); it.has(); it.next())
	{
		LocalVar* lv = it.object();
		ASSERT(lv->_atom->getType() == EsAtom::ATYPE_STRING);
		alist->add(lv->_atom);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	this ポインタ値がプリミティブ型かを確認する ⇒ PRIMITIVE_THIS_TEST
**//*---------------------------------------------------------------------*/
bool EsFunction::testPrimitiveThisValue(const EsValue* valThis) const
{
	static const u32 flagsPrimitiveTest[EsValue::NUM_VTYPE] =
	{
		0,					// VTYPE_NULL
		0,					// VTYPE_VOID
		0,					// VTYPE_OBJ
		F_THISP_STRING,		// VTYPE_STRING
		F_THISP_NUMBER,		// VTYPE_INT
		F_THISP_NUMBER,		// VTYPE_INT64
		F_THISP_NUMBER,		// VTYPE_DOUBLE
		F_THISP_BOOLEAN,	// VTYPE_BOOLEAN
		0,					// VTYPE_ATOM
		0,					// VTYPE_PRIVATE
	};

	ASSERT(!valThis->isVoid());

	return TFW_IS_FLAG(_flags, flagsPrimitiveTest[valThis->getType()]);
}

/*---------------------------------------------------------------------*//**
	フラットなクロージャの作成 ⇒ js_NewFlatClosure
**//*---------------------------------------------------------------------*/
EsFunction* EsFunction::makeFlatClosure(EsContext* ctx, EsObject* objParent)
{
	ASSERT(!TFW_IS_FLAG(_flags, EsFunction::F_CLOSURE_FLAT));

	EsFunction* funcClosure = clone(ctx, objParent);
	if(funcClosure == 0) { return 0L; }
	if(funcClosure->_ilRef->isUpvarArrayNull()) { return funcClosure; }	// ⇒ fun->u.i.script->upvarsOffset == 0

	u16 numSlot = EsFunction::EsFunctionClass::getInstance()->getFreeSlotIndex();	// ⇒ uint32 nslots = JSSLOT_FREE(&js_FunctionClass);
	ASSERT(numSlot == EsStackFrame::NUM_INITIAL_SLOT);	// ⇒ JS_ASSERT(nslots == JS_INITIAL_NSLOTS);

	u16 numAddSlot = 0;
	EsFunction::EsFunctionClass::getInstance()->getAdditionalReservedSlotNum(&numAddSlot, funcClosure);
	numSlot += numAddSlot;

	if(!funcClosure->slots()->recreate(numSlot))
	{
		return 0L;
	}

	u32 depthNest = funcClosure->getInterlang()->getNestDepth();
	for(int i = 0; i < funcClosure->getInterlang()->getUpvarInfoNum(); i++)
	{
		funcClosure->slots()->dvalue(i)->copy(
			ctx->getRunEnv()->getUpvar(depthNest, funcClosure->getInterlang()->getUpvarInfo(i))	);
	}

	return funcClosure;
}

/*---------------------------------------------------------------------*//**
	エスケープクロージャの確認
**//*---------------------------------------------------------------------*/
bool EsFunction::checkEscapingClosure(EsContext* ctx, EsObject* obj, EsValue* val)
{
	ASSERT((obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_CALL) || (obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_DECLENV));

	EsValue valWk(*val);

	if(valWk.isFunctionObject())
	{
		EsObject* objFunc = valWk.getObject();
		EsFunction* func = (EsFunction*)objFunc->slots()->privateData();

		if(func->isNeedsWrapperClosure())
		{
			EsStackFrame* sfrm = (EsStackFrame*)obj->slots()->privateData();	// ⇒ fp = (JSStackFrame *) JS_GetPrivate(cx, obj);
			if(sfrm == 0L)
			{
				ctx->addRunError(EsError::ID_LEAK_OPTIMIZED_CLOSURE);
				return false;
			}

			EsObject* objWrapper = func->makeWrappedEscapingClosure(ctx, sfrm, objFunc);
			if(objWrapper == 0L)
			{
				return false;
			}
			val->setValue(objWrapper);
		}
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsFunction::EsFunction()
	: _flags(0)
	, _atomRef(0L)
	, _ilRef(0L)
	, _depthSkipMin(0)
	, _isWrapper(false)
	, _call(0L)
	, _classCtor(0L)
	, EsObject(EsFunctionClass::getInstance(), OBJT_FUNC)
{
	for(int i = 0; i < NUM_LVK; i++)
	{
		_listLocalVar[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsFunction::~EsFunction()
{
	for(int i = 0; i < NUM_LVK; i++)
	{
		delete _listLocalVar[i];
		_listLocalVar[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	内部関数として作成する
**//*---------------------------------------------------------------------*/
bool EsFunction::createAsInterpreted(EsContext* ctx, const EsAtom* atomRef, EsObject* objParent, bool isLambda)
{
	_flags |= F_KIND_INTERPRETED | (isLambda ? F_LAMBDA : 0);
	_atomRef = atomRef;

	// 親設定
	slots()->setParentObject(objParent);
	// プライベートデータ設定
	slots()->setPrivateData(this);

	// 必要な変数を用意する
	for(int i = 0; i < NUM_LVK; i++)
	{
		_listLocalVar[i] = new List<LocalVar*>(true);	// 中身は自動削除に
	}

	// 中間言語はここでは設定しない．EsBytecodeBuilder::buildSub_functionBody で設定される

	return true;
}

/*---------------------------------------------------------------------*//**
	外部関数として作成する
**//*---------------------------------------------------------------------*/
bool EsFunction::createAsNative(EsContext* ctx, const EsAtom* atomRef, EsObject* objParent, EsNativeCall call, EsExtParam paramDef)
{
	_flags |= F_KIND_NATIVE;
	_atomRef = atomRef;
	_call = call;
	_paramFuncDef = paramDef;

	// 親設定
	slots()->setParentObject(objParent);
	// プライベートデータ設定
	slots()->setPrivateData(this);

	// 関数の中間言語を作成 ⇒ fun->u.i.script = js_NewScript(cx, 1, 1, 0, 0, 0, 0, 0);
	EsInterlang* il = ctx->newInterlang();
	if(!il->create(0, true))
	{
		return 0L;
	}
	setInterlang(il);

	return true;
}

/*---------------------------------------------------------------------*//**
	複製 ⇒ js_CloneFunctionObject
**//*---------------------------------------------------------------------*/
EsFunction* EsFunction::clone(EsContext* ctx, EsObject* objParent)
{
	EsFunction* funcNew = ctx->newFunction(0L, 0L, objParent, false);
	funcNew->slots()->setPrivateData(this);
	return funcNew;
}

/*---------------------------------------------------------------------*//**
	ラップされたエスケープクロージャの作成 ⇒ WrapEscapingClosure
**//*---------------------------------------------------------------------*/
EsObject* EsFunction::makeWrappedEscapingClosure(EsContext* ctx, EsStackFrame* sfrm, EsObject* objFunc)
{
	ASSERT(objFunc->getSlots()->getPrivateData() == this);

	EsObject* objScope = sfrm->getScopeChainObject();
	if(objScope == 0L)
	{
		return 0L;
	}

	EsObject* objWrFunc = ctx->newObjectWithGivenPrototype(EsFunctionClass::getInstance(), objFunc, objScope);
	if(objWrFunc == 0L)
	{
		return 0L;
	}
	EsFunction* funcWr = (EsFunction*)objWrFunc;
	funcWr->slots()->setPrivateData(funcWr);
	TFW_SET_FLAG(funcWr->_flags, F_FUNC_NEED_CALLOBJ, true);
	funcWr->_depthSkipMin = this->_depthSkipMin;
	funcWr->_isWrapper = true;
	funcWr->_atomRef = this->_atomRef;

	s32 numLocalNames = getLocalVarNum(LVK_ARG) + getLocalVarNum(LVK_VAR) + getLocalVarNum(LVK_CONST) + getLocalVarNum(LVK_UPVAR);	// ⇒ countLocalNames()
	if(numLocalNames > 0)	// ⇒ hasLocalNames()
	{
		for(int lvk = LVK_ARG; lvk < NUM_LVK; lvk++)
		{
			EsAtomList alistTmp;
			if(!enumLocalAtoms(&alistTmp, (LocalVarKind)lvk))		{	return 0L;	}
			for(int i = 0; i < alistTmp.getCount(); i++)
			{
				if(!funcWr->addLocalVar(alistTmp.getAtom(i), (LocalVarKind)lvk))
				{
					return 0L;
				}
			}
		}

		// ※※※ 実装不要 ※※※ ⇒ js_FreezeLocalNames
	}

	EsInterlang* ilWr = ctx->newInterlang();
	if(!ilWr->create(this->_ilRef))
	{
		return 0L;
	}
	funcWr->setInterlang(ilWr);

	// ※※※ 実装保留 ※※※ OP_xxx → OP_xxx_DBG
	
	return objWrFunc;
}


//==========================================================================
// EsFunction::EsFunctionPrototype メソッド

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool EsFunction::EsFunctionPrototype::initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const
{
	return EsObject::EsObjectPrototype::initObjectAndFunctionClass(objPrototypeOut, ctx, obj);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsFunction::EsFunctionPrototype::EsFunctionPrototype()
	 : EsPrototype(EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_FUNCTION)->getAtom(), EsSysPrototypes::PTYPEKIND_FUNCTION)
{
}


//==========================================================================
// EsFunction::EsFunctionClass メソッド

/*---------------------------------------------------------------------*//**
	追加の予約スロット数を得る ⇒ fun_reserveSlots
**//*---------------------------------------------------------------------*/
bool EsFunction::EsFunctionClass::getAdditionalReservedSlotNum(u16* slotnumOut, EsObject* obj) const
{
	const EsFunction* func = (const EsFunction*)obj->getSlots()->getPrivateData();	// ⇒ fun = (JSFunction *) JS_GetPrivate(cx, obj);

	u16 numSlot = 0;
	if(	(func != 0L) && 
		func->isInterpreted() &&
		(func->getInterlang() != 0L)	)
	{
		numSlot += func->getInterlang()->getUpvarInfoNum();
		numSlot += func->getInterlang()->getRegexpNum();
	}

	*slotnumOut = numSlot;
	return true;
}

/*---------------------------------------------------------------------*//**
	関数スクリプトクラスの作成 ⇒ js_InitFunctionClass
**//*---------------------------------------------------------------------*/
EsObject* EsFunction::EsFunctionClass::makeClassObject(EsContext* ctx, EsObject* objParent)
{
	// クラスオブジェクト作成
	EsObject* objPtype;
	EsObject* objCtor;
	objParent->makeClassObject(&objPtype, &objCtor, ctx, getInstance(), 0L, callConstructor);	// ⇒ proto = JS_InitClass(cx, obj, NULL, &js_FunctionClass, Function, 1, function_props, function_methods, NULL, NULL);
	if(objPtype == 0L)
	{
		return 0L;
	}
	// 非静的プロパティを定義する ⇒ function_props
	objPtype->defineClassProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom(), callGetProperty, g_funcDummyPropertySetter, PROPSID_ARGS_LENGTH, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT | EsScopeProperty::ATRBF_SHARED);
	// 非静的関数を定義する ⇒ function_methods
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TOSTRING)->getAtom(), callToString, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_APPLY)->getAtom(), callApply, 0);
	objPtype->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CALL)->getAtom(), callCall, 0);
	// 静的プロパティを定義する
	///objCtor->defineClassProperty();
	// 静的関数を定義する
	///objCtor->defineClassFunction();

	EsFunction* func = ctx->newFunction(0L, objPtype, objParent, false);	// ⇒ fun = js_NewFunction(cx, proto, NULL, 0, JSFUN_INTERPRETED, obj, NULL);
	if(func == 0L)
	{
		return 0L;
	}

	EsInterlang* il = ctx->newInterlang();
	if(!il->create(0, true))	// ⇒ js_NewScript(cx, 1, 1, 0, 0, 0, 0, 0);
	{
		return 0L;
	}
	func->setInterlang(il);

	return objPtype;
}

/*---------------------------------------------------------------------*//**
	関数オブジェクトクラスのコンストラクタ
**//*---------------------------------------------------------------------*/
EsFunction::EsFunctionClass::EsFunctionClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_FUNCTION)->getAtom()
		, EsSysClasses::CLASSTYPE_FUNC
		, EsClass::F_RESOLVE_OUT_PARAM | EsClass::F_HAS_PRIVATE
		, EsSysPrototypes::PTYPEKIND_FUNCTION
		, 2 )
{
	_funcEnumerate = callEnumerate;
	_funcResolv = callResolve;
	_funcConvert = callConvert;
	_funcConstruct = callConstructor;
}

/*---------------------------------------------------------------------*//**
	アクセサ取得 ⇒ fun_getProperty
**//*---------------------------------------------------------------------*/
bool EsFunction::EsFunctionClass::callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id)
{
	if(!id->isInt())
	{
		return true;
	}
	s32 slotidx = id->getInt();

	EsFunction* func = 0L;
	while(obj != 0L)
	{
		func = (EsFunction*)obj->getPrivateData(ctx, EsSysClasses::getClass(EsSysClasses::CLASSTYPE_FUNC), 0L);	// 関数オブジェクトを得る ⇒ JS_GetInstancePrivate(cx, obj, &js_FunctionClass, NULL)

		if(slotidx != PROPSID_ARGS_LENGTH)
		{
			return true;
		}

		obj = obj->slots()->prototypeObject();
	}

	EsStackFrame* sfrm = ctx->getTopStackFrame();
	while((sfrm != 0L) && ((sfrm->getFunction() == func) || (TFW_IS_FLAG(sfrm->getFlags(), EsStackFrame::F_DEBUGGER | EsStackFrame::F_EVAL))))	// ⇒ fp && (fp->fun != fun || (fp->flags & JSFRAME_SPECIAL))
	{
		sfrm = sfrm->getDownStackFrame();
	}

	switch(slotidx)
	{
	case PROPSID_CALL_ARGUMENTS:
		// 非推奨の文法
		ctx->addRunError(EsError::ID_DEPRECATED_USAGE, true, true);

		if(sfrm != 0L)
		{
			if(!sfrm->getArgsValue(val, ctx))
			{
				return false;
			}
		}
		else
		{
			val->setNull();
		}
		break;

	case PROPSID_ARGS_LENGTH:
	case PROPSID_FUNC_ARITY:
		val->setValue((s32)func->getLocalVarNum(EsFunction::LVK_ARG));
		break;

	case PROPSID_FUNC_NAME:
		{
			const EsAtom* atom = func->getAtom();
			if(atom != 0L)
			{
				val->setValue(atom);
			}
			else
			{
				val->setValue(TypeUtils::getEmptyString());
			}
		}
		break;

	case PROPSID_FUNC_CALLER:
		if((sfrm != 0L) && (sfrm->getDownStackFrame() != 0L) && (sfrm->getDownStackFrame()->getFunction() != 0L))
		{
			EsFunction* funcCaller = sfrm->getDownStackFrame()->getFunction();
			if(funcCaller->isNeedsWrapperClosure())
			{
				EsObject* objWrapper = funcCaller->makeWrappedEscapingClosure(ctx, sfrm->getDownStackFrame(), funcCaller);
				if(objWrapper == 0L)
				{
					return false;
				}
				val->setValue(objWrapper);
				return true;
			}

			val->setValue(sfrm->getDownStackFrame()->getCalleeObject());
		}
		else
		{
			val->setNull();
		}
		/// ※※※ 未対応 ※※※ ⇒ JS_GetSecurityCallbacks, securityCallbacks
		break;

	default:
		if((sfrm != 0L) && (sfrm->getFunction() != 0L) && (slotidx >= 0) && ((u32)slotidx < sfrm->getFunction()->getLocalVarNum(LVK_ARG)))
		{
			val->setValue(sfrm->getFunction()->_listLocalVar[LVK_ARG]->getObject(slotidx)->_atom);	// ⇒ *vp = fp->argv[slot];
		}
		break;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	列挙 ⇒ fun_enumerate
**//*---------------------------------------------------------------------*/
bool EsFunction::EsFunctionClass::callEnumerate(EsContext* ctx, EsObject* obj)
{
	EsValue valId(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PROTOTYPE)->getAtom());	// ⇒ prototypeId = ATOM_TO_JSID(cx->runtime->atomState.classPrototypeAtom);
	EsScopeProperty* sprop;
	EsObject* objParent;
	if(!obj->findPropertyOv(&sprop, &objParent, ctx, &valId))
	{
		return false;
	}
	if(sprop != 0L)
	{
		objParent->dropPropertyOv(ctx, sprop);
	}
	return true;
}

/*---------------------------------------------------------------------*//**
	解決 ⇒ fun_resolve
**//*---------------------------------------------------------------------*/
bool EsFunction::EsFunctionClass::callResolve(EsObject** objOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv)
{
	if(!id->isString() || TFW_IS_FLAG(flagsRslv, EsResolver::RSLVF_ASSIGNING))
	{
		return true;
	}

	EsFunction* func = (EsFunction*)obj->slots()->privateData();	// ⇒ fun = GET_FUNCTION_PRIVATE(cx, obj);

	const EsAtom* atom = EsKeywordSet::getStatic(EsKeywordSet::S_PROP_PROTOTYPE)->getAtom();
	if(atom->isSameString(id->getString()))	// ⇒ id == ATOM_KEY(atom)
	{
		const EsClass* cldObject = EsSysClasses::getClass(EsSysClasses::CLASSTYPE_OBJECT);
		if(func->getAtom() == cldObject->getNameAtom())	// ⇒ if (fun->atom == CLASS_ATOM(cx, Object))
		{
			return true;
		}

		EsObject* objPtype = ctx->newObject(cldObject, 0L, obj->slots()->parentObject());
		if(objPtype == 0L)
		{
			return false;
		}

		if(!obj->setClassPrototype(ctx, objPtype, EsScopeProperty::ATRBF_ENUMERATE | EsScopeProperty::ATRBF_PERMANENT))
		{
			return false;
		}

		*objOut = obj;
		return true;
	}

	const EsAtom* const atomarrFuncProps[] =
	{
		EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ARGUMENTS)->getAtom(),
		EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ARITY)->getAtom(),
		EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CALLER)->getAtom(),
		EsKeywordSet::getStatic(EsKeywordSet::S_PROP_NAME)->getAtom(),
	};
	const short shoridarrFuncProps[] =
	{
		PROPSID_CALL_ARGUMENTS,
		PROPSID_FUNC_ARITY,
		PROPSID_FUNC_CALLER,
		PROPSID_FUNC_NAME,
	};
	for(int i = 0; i < sizeof(atomarrFuncProps) / sizeof(const EsAtom*); i++)
	{
		atom = atomarrFuncProps[i];
		if(atom->isSameString(id->getString()))
		{
			EsValue valAtom(atom);
			EsValue valVoid;
			if(!obj->definePropertyNative(
				ctx,
				&valAtom,
				&valVoid,
				callGetProperty,
				0L,
				EsScopeProperty::ATRBF_PERMANENT,
				EsScopeProperty::OPTF_HAS_SHORTID,
				shoridarrFuncProps[i],
				false	))
			{
				return false;
			}

			*objOut = obj;
			return true;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	変換 ⇒ fun_convert
**//*---------------------------------------------------------------------*/
bool EsFunction::EsFunctionClass::callConvert(EsValue* val, EsContext* ctx, EsObject* obj, EsLogicalType ltype)
{
	if(ltype == LTYPE_FUNC)
	{
		val->setValue(obj);
		return true;
	}

	return obj->tryCallValueOf(val, ctx, ltype);
}

/*---------------------------------------------------------------------*//**
	関数スクリプトクラスのコンストラクタ ⇒ Function
**//*---------------------------------------------------------------------*/
bool EsFunction::EsFunctionClass::callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(!TFW_IS_FLAG(ctx->getTopStackFrame()->getFlags(), EsStackFrame::F_CONSTRUCTING))	// ⇒ JS_IsConstructing(cx)
	{
		objThis = ctx->newObject(getInstance(), 0L, 0L);
		if(objThis == 0L)
		{
			return false;
		}
	}
	else
	{
		if(objThis->slots()->getPrivateData() != 0L)
		{
			return true;
		}
	}

	EsObject* objParent = valCallee->getObject()->slots()->parentObject();
	EsFunction* func = ctx->newFunction(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ANONYMOUS)->getAtom(), objThis, objParent, true);	// ⇒ fun = js_NewFunction(cx, obj, NULL, 0, JSFUN_LAMBDA | JSFUN_INTERPRETED, parent, cx->runtime->atomState.anonymousAtom);
	if(func == 0L)
	{
		return false;
	}

	#if 0	/// ※※※ 実装保留 ※※※ ⇒ JS_EvalFramePrincipals, js_ComputeFilename, js_CheckPrincipalsAccess
		EsStackFrame* sfrmCur = ctx->getTopStackFrame();
		ASSERT(sfrmCur->getInterlang() == 0L);
		ASSERT((sfrmCur->getFunction() != 0L) && (sfrmCur->getFunction()->getNativeCall() == EsFunctionClass::callConstructor));
		EsStackFrame* sfrmCaller = ctx->getScriptedCallerFrame();
		if(sfrmCaller != 0L)
		{

		}
		else
		{
		}
	#endif

	u32 nParams = (numArg > 0) ? numArg - 1 : 0;
	if(nParams > 0)
	{
		// 引数文字列を , で区切って一つにまとめる
		// ECMA 15.3.2.1 対応
		VcString strArgs;
		for(u32 i = 0; i < nParams; i++)
		{
			if(!vaArg[i].isString())
			{
				return false;
			}
			strArgs.add(vaArg[i].getString());

			// , で列挙
			if((i + 1) < nParams)	{	strArgs.add(',');	}
		}

		EsParser parser;
		if(!parser.parseFunctionParameters(ctx, func, &strArgs))
		{
			return false;
		}
	}

	const VcString* strBody = 0L;
	if(numArg > 0)
	{
		if(!vaArg[numArg - 1].toString(&strBody, ctx))
		{
			return false;
		}
		vaArg[numArg - 1].setValue(strBody);
	}

	EsParser parser;
	return parser.parseFunctionBody(ctx, func, strBody);
}

/*---------------------------------------------------------------------*//**
	文字列化 ⇒ fun_toString
**//*---------------------------------------------------------------------*/
bool EsFunction::EsFunctionClass::callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	EsObject* objFunc = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ fval = JS_THIS(cx, vp);
	EsValue valFunc(*valThis);
	if(valFunc.isNull())
	{
		return false;
	}

	if(!valFunc.isFunctionObject())
	{
		if(!valFunc.isPrimitive())
		{
			ASSERT(valFunc.isObject());
			objFunc = valFunc.getObject();
			ASSERT(objFunc->getClass()->getConvertFunc() != 0L);
			if(!objFunc->getClass()->getConvertFunc()(&valFunc, ctx, objFunc, LTYPE_FUNC))
			{
				return false;
			}
			valThis->copy(&valFunc);
		}
		if(!valFunc.isFunctionObject())
		{
			ctx->addRunError(EsError::ID_INCOMPATIBLE_PROTO);
			return false;
		}
	}

	u32 indent = 0;
	objFunc = valFunc.getObject();
	if(numArg != 0)
	{
		if(!vaArg->toUint32(&indent, ctx))
		{
			return false;
		}
	}

	EsFunction* func = (EsFunction*)objFunc->slots()->getPrivateData();
	if(func == 0L)
	{
		return true;
	}

	// 仮で関数名を出力する ※※※ 未実装 ※※※ ⇒ JS_DecompileFunction
	VcString strWk("function");
	const VcString* nameFunc = func->getAtom()->getString();
	if((nameFunc != 0L) && (nameFunc->getLength() > 0))
	{
		strWk.add(" ");
		strWk.add(nameFunc);
	}

	// 固定文字列を返答値に設定
	const VcString* cstr = ctx->newString(&strWk);
	valRet->setValue(cstr);
	return true;
}

/*---------------------------------------------------------------------*//**
	apply ⇒ js_fun_apply
**//*---------------------------------------------------------------------*/
bool EsFunction::EsFunctionClass::callApply(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg == 0)
	{
		return callCall(ctx, objThis, valCallee, valThis, vaArg, numArg, valRet, exprm);
	}

	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ obj = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}
	if(!objThis->convDefaultValueOv(ctx, valThis, LTYPE_FUNC))	{	return false;	}

	EsValue valFunc(*valThis);
	if(!valFunc.isFunctionObject())
	{
		ctx->addRunError(EsError::ID_INCOMPATIBLE_PROTO);
		return false;
	}

	EsObject* objArray = 0L;
	s32 lenArray = 0;
	s32 numInvokeArg = numArg;
	if(numInvokeArg >= 2)
	{
		// ２番目の引数が NULL もしくは Void の場合は引数なし
		if(vaArg[1].isNull() || vaArg[1].isVoid())
		{
			numInvokeArg = 0;
		}
		else
		{
			//  ２番目の引数は引数オブジェクトか配列オブジェクトのはずである
			if(!vaArg[1].isPrimitive())
			{
				do
				{
					ASSERT(vaArg[1].isObject());
					if(!vaArg[1].isObject())	{	break;	}
					EsObject* objWk = vaArg[1].getObject();
					if(objWk == 0L)					{	break;	}
					// 配列かどうか確認する ⇒ js_IsArrayLike
					u32 clstype = objWk->getClass()->getClassType();
					if((clstype != EsSysClasses::CLASSTYPE_ARGUMENTS) && (clstype != EsSysClasses::CLASSTYPE_ARRAY))	{	break;	}
					// 配列の長さを得る ⇒ js_GetLengthProperty
					if(!EsArray::getArrayLengthProperty(&lenArray, ctx, objWk))	{	break;	}
					objArray = objWk;
				}
				while(false);
				if(objArray == 0L)
				{
					ctx->addRunError(EsError::ID_BAD_APPLY_ARGS);
					return false;
				}
			}
		}
	}

	// １番目のオブジェクトは this オブジェクト
	if(!vaArg[0].isPrimitive())
	{
		ASSERT(vaArg[0].isObject());
		objThis = vaArg[0].getObject();
	}
	else if(!ctx->convValueToObject(&objThis, &vaArg[0]))
	{
		return false;
	}

	// スタック配列の作成
	EsValue* vaInvokeStack = new EsValue[numArg + 2];
	if(vaInvokeStack == 0L)
	{
		return false;
	}
	vaInvokeStack[0].copy(&valFunc);
	vaInvokeStack[1].setValue(objThis);
	for(int i = 0; i < numInvokeArg; i++)
	{
		if(!objArray->getElement(&vaInvokeStack[2 + i], ctx, i))
		{
			delete[] vaInvokeStack;
			return false;
		}
	}

	// 関数呼び出し実行
	EsStackFrame* sfrmInvoke;
	bool isOk = ctx->getRunEnv()->invoke(&sfrmInvoke, vaInvokeStack, numInvokeArg, EsStackFrame::F_NEWSTACK);
	ASSERT(!sfrmInvoke->getFunctionResultValue()->isUnfinished());	// 想定外
	valRet->copy(sfrmInvoke->getFunctionResultValue());

	return isOk;
}

/*---------------------------------------------------------------------*//**
	call ⇒ js_fun_call
**//*---------------------------------------------------------------------*/
bool EsFunction::EsFunctionClass::callCall(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	objThis = ctx->getRunEnv()->getThisObject(false, valCallee, valThis, vaArg);	// ⇒ obj = JS_THIS_OBJECT(cx, vp);
	if(objThis == 0L)	{	return false;	}
	if(!objThis->convDefaultValueOv(ctx, valThis, LTYPE_FUNC))	{	return false;	}

	EsValue valFunc(*valThis);
	if(!valFunc.isFunctionObject())
	{
		ctx->addRunError(EsError::ID_INCOMPATIBLE_PROTO);
		return false;
	}

	EsValue* vaArgWk = vaArg;
	s32 numInvokeArg = numArg;
	if(numInvokeArg == 0)
	{
		objThis = 0L;
	}
	else
	{
		if(!vaArgWk[0].isPrimitive())
		{
			ASSERT(vaArgWk[0].isObject());
			objThis = vaArgWk[0].getObject();
		}
		else if(!ctx->convValueToObject(&objThis, &vaArgWk[0]))
		{
			return false;
		}
		numInvokeArg--;
		vaArgWk++;
	}

	// スタック配列の作成
	EsValue* vaInvokeStack = new EsValue[numArg + 2];
	if(vaInvokeStack == 0L)
	{
		return false;
	}
	vaInvokeStack[0].copy(&valFunc);
	vaInvokeStack[1].setValue(objThis);
	TFW_COPY_MEMORY(&vaInvokeStack[2], vaArgWk, sizeof(EsValue) * numInvokeArg);

	// 関数呼び出し実行
	EsStackFrame* sfrmInvoke;
	bool isOk = ctx->getRunEnv()->invoke(&sfrmInvoke, vaInvokeStack, numInvokeArg, EsStackFrame::F_NEWSTACK);
	ASSERT(!sfrmInvoke->getFunctionResultValue()->isUnfinished());	// 想定外
	valRet->copy(sfrmInvoke->getFunctionResultValue());

	return isOk;
}


////////////////////////////////////////////////////////////////////////////

ES_END_NS
