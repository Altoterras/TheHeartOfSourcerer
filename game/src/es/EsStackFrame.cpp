/***********************************************************************//**
 *	EsStackFrame.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsStackFrame.h"

// Friends
#include "EsBlock.h"
#include "EsClass.h"
#include "EsContext.h"
#include "EsKeywordSet.h"
#include "EsSysClasses.h"
#include "EsFunction.h"
#include "EsInterlang.h"
#include "EsObject.h"
#include "EsProgramCounter.h"
#include "EsScope.h"
#include "EsScopeProperty.h"
#include "EsStackRegister.h"
#include "EsValue.h"
#include "EsWith.h"

// External
#include "../tfw/lib/BitFlagArray.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsStackFrame メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	スロット取得
**//*---------------------------------------------------------------------*/
EsValue* EsStackFrame::getSlot(s32 slotidx) const
{
	return (0 <= slotidx) && (slotidx < _numSlot) ? &_varrSlots[slotidx] : 0L;
}

/*---------------------------------------------------------------------*//**
	関数の呼び出しオブジェクト値を得る
**//*---------------------------------------------------------------------*/
EsValue* EsStackFrame::getCalleeValue() const
{
	if(_valarrStack == 0L)	{	return 0L;	}
	return &_valarrStack[0];
}

/*---------------------------------------------------------------------*//**
	this オブジェクト値を得る
**//*---------------------------------------------------------------------*/
EsValue* EsStackFrame::getThisValue() const
{
	if(_valarrStack == 0L)	{	return 0L;	}
	return &_valarrStack[1];
}

/*---------------------------------------------------------------------*//**
	関数引数配列を得る
**//*---------------------------------------------------------------------*/
EsValue* EsStackFrame::getArgArray() const
{
	if(_valarrStack == 0L)	{	return 0L;	}
	return &_valarrStack[2];
}

/*---------------------------------------------------------------------*//**
	（遅延作成された）関数引数を得る ⇒ fp->argv
**//*---------------------------------------------------------------------*/
EsValue* EsStackFrame::getArg(s32 index) const
{
	ASSERT((0 <= index) && ((u32)index < _numArg));
	if((index < 0) || (_numArg <= (u32)index))	{	return 0L;	}
	return &_valarrStack[2 + index];
}

/*---------------------------------------------------------------------*//**
	スタック基点を得る ⇒ StackBase
**//*---------------------------------------------------------------------*/
EsValue* EsStackFrame::getStackTop() const
{
	return _varrSlots + _il->getFixedSlotNum();
}

/*---------------------------------------------------------------------*//**
	スロット設定
**//*---------------------------------------------------------------------*/
bool EsStackFrame::setSlot(s32 slotidx, const EsValue* val)
{
	if((slotidx < 0) && (_numSlot <= slotidx))	{	return false;	}
	_varrSlots[slotidx].copy(val);
	return true;
}

/*---------------------------------------------------------------------*//**
	（遅延作成された）関数引数を設定する
**//*---------------------------------------------------------------------*/
bool EsStackFrame::setArg(s32 index, const EsValue* val)
{
	ASSERT((0 <= index) && ((u32)index < _numArg));
	if((index < 0) || (_numArg <= (u32)index))	{	return false;	}
	_valarrStack[2 + index].copy(val);
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	準備 ⇒ js_Execute の冒頭部分
**//*---------------------------------------------------------------------*/
bool EsStackFrame::prepareForRun(EsContext* ctx, EsInterlang* il, EsObject* objScope, EsStackFrame* sfrmDown, u16 flags)
{
	EsStackFrame* sfrmOld = ctx->getTopStackFrame();
	_il = il;
	if(sfrmDown != 0L)
	{
		_objCall = sfrmDown->_objCall;
		_objArgs = sfrmDown->_objArgs;
		_objVar = sfrmDown->_objVar;
		_objCallee = sfrmDown->_objCallee;
		_func = sfrmDown->_func;
		_objThis = sfrmDown->_objThis;
		if(TFW_IS_FLAG(sfrmDown->_flags, F_COMPUTED_THIS))
		{
			flags |= F_COMPUTED_THIS;
		}
		_numArg = sfrmDown->_numArg;
		_valarrStack = sfrmDown->_valarrStack;
	}
	else
	{
		_objCall = 0L;
		_objArgs = 0L;
		_objVar = objScope;
		_objCallee = 0L;
		_func = 0L;
		_objThis = objScope;
		_numArg = 0;
		_valarrStack = 0L;
	}
	_flags = flags;
	_sfrmDown = sfrmDown;
	_objScopeChain = objScope;
	_objBlockChain = 0L;

	// スロットの作成
	_numSlot = il->getSlotNum();		// ⇒ script->nslots
	if(_numSlot > 0)
	{
		_varrSlots = _varrSlotsAlloc = new EsValue[_numSlot];	// ⇒ frame.slots = js_AllocRawStack(cx, script->nslots, &mark);
		if(_varrSlots == 0L)	{	return false;	}
	}
	else
	{
		_varrSlots = _varrSlotsAlloc = 0L;
	}

	// レジスタの作成
	_pc = new EsProgramCounter(il->getBytecode());				// ⇒ regs.pc = script->code;
	_regs = new EsStackRegister(ctx, _pc, getStackTop(), _numSlot - il->getFixedSlotNum(), 0);

	// 関数返答値の作成
	_valFuncRes = new EsValue();

	return true;
}

/*---------------------------------------------------------------------*//**
	関数呼び出し用の準備 ⇒ inline_call:
**//*---------------------------------------------------------------------*/
bool EsStackFrame::prepareForCall(EsContext* ctx, EsFunction* func, EsInterlang* il, u16 flags, EsValue* vaStack, s32 numArg, EsObject* objCallee, EsObject* objThis, EsObject* objScopeChain, EsStackFrame* sfrmCaller)
{
	_pc = 0L;
	_regs = 0L;

	// プログラムカウンタを用意
	ASSERT(il != 0L);
	_pc = new EsProgramCounter(il->getBytecode());

	// スタックレジスタ、プログラムカウンタの作成
	vaStack = makeSlotArray(ctx, func->getLocalVarNum(EsFunction::LVK_ARG), func->getLocalVarNum(EsFunction::LVK_VAR), il->getSlotNum(), vaStack, numArg);
	if(vaStack == 0L)
	{
		return false;
	}

	// 呼び出し元のスタックレジスタの値を保存しておく
	ASSERT(sfrmCaller != 0L);
	_regsCallerRef = sfrmCaller->getStackRegister();	// ⇒ newifp->callerRegs = regs;

	_func = func;
	_il = il;
	_flags = flags;
	_objCallee = objCallee;
	_objThis = objThis;
	_objScopeChain = objScopeChain;
	_valarrStack = vaStack;
	_numArg = numArg;
	_sfrmDown = sfrmCaller;
	_objVar = 0L;

	// 外部で新たに確保したスタックメモリ（スタック破棄時に削除する）
	if(TFW_IS_FLAG(_flags, F_NEWSTACK))
	{
		_valarrStackAlloc = _valarrStackOrg;
	}

	// コールオブジェクトの解決
	if((func != 0L) && TFW_IS_FLAG(func->getFlags(), EsFunction::F_FUNC_NEED_CALLOBJ))	// ⇒ JSFUN_HEAVYWEIGHT_TEST(fun->flags)
	{
		if(!makeCallObject(ctx))
		{
			return false;
		}
	}

	// 関数返答値の作成
	_valFuncRes = new EsValue();	// 初期値は void で

	return true;
}

/*---------------------------------------------------------------------*//**
	関数呼び出し用の準備 ⇒ js_Invoke
**//*---------------------------------------------------------------------*/
bool EsStackFrame::prepareForInvoke(EsContext* ctx, EsFunction* func, EsInterlang* il, s32 numSlot, u16 flags, EsValue* vaStack, s32 numArg, EsObject* objCallee, EsObject* objThis, EsObject* objScopeChain, EsStackFrame* sfrmCaller)
{
	_pc = 0L;
	_regs = 0L;

	// スロットサイズの拡大、プログラムカウンタの作成
	if(il != 0L)
	{
		numSlot += il->getSlotNum();
		_pc = new EsProgramCounter(il->getBytecode());
	}

	// スタックレジスタの作成
	s32 numFuncArgs = 0;
	s32 numFuncVars = 0;
	if(func != 0L)
	{
		numFuncArgs = func->getLocalVarNum(EsFunction::LVK_ARG);
		numFuncVars = func->getLocalVarNum(EsFunction::LVK_VAR);
	}
	vaStack = makeSlotArray(ctx, numFuncArgs, numFuncVars, numSlot, vaStack, numArg);
	if(vaStack == 0L)
	{
		return false;
	}

	_func = func;
	_il = il;
	_flags = flags;
	_objCallee = objCallee;
	_objThis = objThis;
	_objScopeChain = objScopeChain;
	_valarrStack = vaStack;
	_numArg = numArg;
	_sfrmDown = sfrmCaller;
	_objVar = 0L;

	// 外部で新たに確保したスタックメモリ（スタック破棄時に削除する）
	if(TFW_IS_FLAG(_flags, F_NEWSTACK))
	{
		_valarrStackAlloc = _valarrStackOrg;
	}

	// コールオブジェクトの解決
	if((func != 0L) && TFW_IS_FLAG(func->getFlags(), EsFunction::F_FUNC_NEED_CALLOBJ))	// ⇒ JSFUN_HEAVYWEIGHT_TEST(fun->flags)
	{
		if(!makeCallObject(ctx))
		{
			return false;
		}
	}

	// 関数返答値の作成
	_valFuncRes = new EsValue();	// 初期値は void で

	return true;
}

/*---------------------------------------------------------------------*//**
	関数呼び出し用の後処理 ⇒ js_Invoke native / js_Interpret 後
**//*---------------------------------------------------------------------*/
bool EsStackFrame::postprocForInvoke(EsContext* ctx, bool isSucceededCall)
{
	// コールオブジェクトを持っている場合は、値を同期しポインタをクリアする
	if(_objCall != 0L)
	{
		if(!putCallObject(ctx))	{	isSucceededCall = false;	}
	}

	// 引数オブジェクトを持っている場合は、値を同期しポインタをクリアする
	if(_objArgs != 0L)
	{
		if(!putArgsObject(ctx))	{	isSucceededCall = false;	}
	}

	// 戻り値を設定
	if(!isSucceededCall)
	{
		_valFuncRes->setNull();	// ⇒ *vp = JSVAL_NULL;
	}

	// スタックの最初の要素に返答値をセットする ⇒ *vp = frame.rval;
	_valarrStack[0].copy(_valFuncRes);		// このスタックフレームで使用したスタックに反映
	if(_valarrStack != _valarrStackOrg)
	{
		_valarrStackOrg[0].copy(_valFuncRes);	// 呼び出し元が用意したスタックにも反映
	}

	return isSucceededCall;
}

/*---------------------------------------------------------------------*//**
	スコープチェインを作成する ⇒ js_GetScopeChain
**//*---------------------------------------------------------------------*/
EsObject* EsStackFrame::makeScopeChainObject(EsContext* ctx)
{
	EsObject* objBlockShared = _objBlockChain;

	if(objBlockShared == 0L)
	{
		ASSERT(	(_func == 0L) ||
				!TFW_IS_FLAG(_func->getFlags(), EsFunction::F_FUNC_NEED_CALLOBJ) ||
				(_objCall != 0L)	);
		ASSERT(_objScopeChain != 0L);
		return _objScopeChain;
	}

	EsObject* objLimitBlock = 0L;
	EsObject* objLimitClone = 0L;
	if((_func != 0L) && (_objCall == 0L))
	{
		ASSERT(	(_objScopeChain->getClass()->getClassType() == EsSysClasses::CLASSTYPE_BLOCK) ||
				(_objScopeChain->getSlots()->getPrivateData() != this)	);
		if(makeCallObject(ctx) == 0L)
		{
			return 0L;
		}
	}
	else
	{
		objLimitClone = _objScopeChain;
		while(objLimitClone->getClass()->getClassType() == EsSysClasses::CLASSTYPE_WITH)	// ⇒ while (OBJ_GET_CLASS(cx, limitClone) == &js_WithClass)
		{
			objLimitClone = objLimitClone->slots()->parentObject();
		}
		ASSERT(objLimitClone != 0L);

		objLimitBlock = objLimitClone->slots()->prototypeObject();

		if(objLimitBlock == objBlockShared)
		{
			return getScopeChainObject();
		}
	}

	ASSERT(objBlockShared->getClass()->getClassType() == EsSysClasses::CLASSTYPE_BLOCK);
	EsObject* objNewChildInnermost = ((EsBlock*)objBlockShared)->clone(ctx, _objScopeChain, this);	// ⇒ JSObject *innermostNewChild = js_CloneBlockObject(cx, sharedBlock, fp->scopeChain, fp);
	if(objNewChildInnermost == 0L)
	{
		return 0L;
	}

	EsObject* objNewChild = objNewChildInnermost;
	while(true)
	{
		ASSERT(objNewChild->getSlots()->getPrototypeObject() == objBlockShared);
		objBlockShared = objBlockShared->slots()->parentObject();

		if((objBlockShared == 0L) || (objBlockShared == objLimitBlock))
		{
			break;
		}

		EsObject* objClone = ((EsBlock*)objBlockShared)->clone(ctx, _objScopeChain, this);
		if(objClone == 0L)
		{
			return 0L;
		}

		objNewChild->slots()->setParentObject(objClone);
		objNewChild = objClone;
	}

	_objScopeChain = objNewChildInnermost;
	return _objScopeChain;
}

/*---------------------------------------------------------------------*//**
	呼び出しオブジェクトを作成する ⇒ js_GetCallObject
**//*---------------------------------------------------------------------*/
EsObject* EsStackFrame::makeCallObject(EsContext* ctx)
{
	EsObject* objCall = 0L;

	ASSERT(_func != 0L);
	objCall = getCallObject();
	if(objCall != 0L)	{	objCall;	}

	// ラムダ式の場合の処理
	const EsAtom* atomLambda = TFW_IS_FLAG(_func->getFlags(), EsFunction::F_LAMBDA) ? _func->getAtom() : 0L;
	if(atomLambda != 0L)
	{
		EsObject* objEnv = ctx->newObjectWithGivenPrototype(EsSysClasses::getClass(EsSysClasses::CLASSTYPE_DECLENV), 0L, _objScopeChain);
		if(objEnv == 0L)
		{
			return 0L;
		}
		objEnv->slots()->setPrivateData(this);

		_objScopeChain = objEnv;
		EsValue valLambdaAtom(atomLambda);
		EsValue valCalleeObj(_objCallee);
		if(!objEnv->definePropertyNative(
			ctx,
			&valLambdaAtom,
			&valCalleeObj,
			EsSysClasses::EsDeclEnvClass::callGetProperty,
			0L,
			EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT,
			0,
			0,
			false	))
		{
			return 0L;
		}
	}

	objCall = ctx->newObjectWithGivenPrototype(EsSysClasses::EsCallClass::getInstance(), 0L, _objScopeChain);
	if(objCall == 0L)
	{
		return 0L;
	}

	objCall->slots()->setPrivateData(this);
	ASSERT(_func == _objCallee->getSlots()->getPrivateData());	// ⇒ JS_ASSERT(fp->fun == GET_FUNCTION_PRIVATE(cx, fp->callee));
	EsValue valCalleeObj(_objCallee);
	objCall->slots()->setValue(EsFunction::EsFunctionClass::getInstance()->getCalleeSlotIndex(), &valCalleeObj);
	setCallObj(objCall);
	setScopeChainObject(objCall);	// スコープチェインのトップに設定する
	setVarObject(objCall);

	return objCall;
}

/*---------------------------------------------------------------------*//**
	引数オブジェクトを作成する ⇒ js_GetArgsObject
**//*---------------------------------------------------------------------*/
EsObject* EsStackFrame::makeArgsObject(EsContext* ctx)
{
	EsStackFrame* sfrmWk = this;
	if(TFW_IS_FLAG(sfrmWk->_flags, F_DEBUGGER) || TFW_IS_FLAG(sfrmWk->_flags, F_EVAL))
	{
		sfrmWk = sfrmWk->_sfrmDown;
	}

	EsObject* objArgs = sfrmWk->_objArgs;
	if(objArgs != 0L)
	{
		return objArgs;
	}

	objArgs = ctx->newObject(EsSysClasses::getClass(EsSysClasses::CLASSTYPE_ARGUMENTS), 0L, 0L);
	if(objArgs == 0L)
	{
		return 0L;
	}
	objArgs->slots()->setPrivateData(sfrmWk);

	EsObject* objGlobal = sfrmWk->_objScopeChain;
	EsObject* objParent = objGlobal->slots()->parentObject();
	while(objParent != 0L)
	{
		objGlobal = objParent;
		objParent = objGlobal->slots()->parentObject();
	}
	objArgs->slots()->setParentObject(objGlobal);

	sfrmWk->_objArgs = objArgs;
	return objArgs;
}

/*---------------------------------------------------------------------*//**
	呼び出しオブジェクトをクリア ⇒ js_PutCallObject
**//*---------------------------------------------------------------------*/
bool EsStackFrame::putCallObject(EsContext* ctx)
{
	if(_objCall == 0L)
	{
		return true;
	}

	bool isOk = true;

	if(_objArgs != 0L)
	{
		if(!TFW_IS_FLAG(_bitsOvdPropsid, convPropsidToBits(EsFunction::PROPSID_CALL_ARGUMENTS)))	// ⇒ if (!TEST_OVERRIDE_BIT(fp, CALL_ARGUMENTS))
		{
			EsValue valArgsObj(_objArgs);
			_objCall->slots()->setValue(EsFunction::EsFunctionClass::getInstance()->getCallArgumentsSlotIndex(), &valArgsObj);	// ⇒ STOBJ_SET_SLOT(callobj, JSSLOT_CALL_ARGUMENTS, OBJECT_TO_JSVAL(fp->argsobj));
		}
		if(!putArgsObject(ctx))	{	isOk = false;	}
	}

	ASSERT(_func == _objCall->getCallObjectFunction());
	{
		s32 numArg = _func->getLocalVarNum(EsFunction::LVK_ARG);
		s32 numVar = _func->getLocalVarNum(EsFunction::LVK_VAR);
		s32 numArgVar = numArg + numVar;	// ⇒ fun->countArgsAndVars();
		if(numArgVar > 0)
		{
			s32 numSlot = numArgVar + NUM_INITIAL_SLOT;
			if(numSlot > _objCall->getSlots()->getNum())
			{
				if(!_objCall->slots()->recreate(numSlot))	{	isOk = false;	}
			}
			EsScope* scope = _objCall->getScope();
			if(isOk)
			{
				for(int i = 0; i < numArg; i++)
				{
					_objCall->slots()->setDvalue(i, &_valarrStack[2 + i]);
				}
				for(int i = 0; i < numVar; i++)
				{
					_objCall->slots()->setDvalue(i + numArg, &_varrSlots[i]);
				}

				if((scope->getOwnerObject() == _objCall) && (numSlot > scope->getFreeSlotIndex()))
				{
					scope->setFreeSlotIndex(numSlot);
				}
			}
		}
	}

	if(TFW_IS_FLAG(_func->getFlags(), EsFunction::F_LAMBDA) && (_func->getAtom() != 0L))
	{
		EsObject* objEnv = _objCall->slots()->parentObject();
		ASSERT(objEnv->getClass()->getClassType() == EsSysClasses::CLASSTYPE_DECLENV);
		ASSERT(objEnv->getSlots()->getPrivateData() == this);
		objEnv->slots()->setPrivateData(0L);
	}

	_objCall->slots()->setPrivateData(0L);
	_objCall = 0L;

	return isOk;
}

/*---------------------------------------------------------------------*//**
	引数プロパティを取得する ⇒ js_GetArgsProperty
**//*---------------------------------------------------------------------*/
bool EsStackFrame::getArgsProperty(EsValue* val, EsContext* ctx, const EsValue* spropid)
{
	if(TFW_IS_FLAG(_bitsOvdPropsid, convPropsidToBits(EsFunction::PROPSID_CALL_ARGUMENTS)))	// ⇒ if (TEST_OVERRIDE_BIT(fp, CALL_ARGUMENTS))
	{
		EsValue valWk;
		ASSERT(_objCall != 0L);
		EsValue valArgumentsAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ARGUMENTS)->getAtom());
		if(!_objCall->callGetPropertyOv(&valWk, ctx, &valArgumentsAtom))
		{
			return false;
		}
		EsObject* objWk;
		if(valWk.isPrimitive())
		{
			if(!ctx->convValueToObject(&objWk, &valWk))
			{
				return false;
			}
		}
		else
		{
			ASSERT(valWk.isObject());
			objWk = valWk.getObject();
		}
		return objWk->callGetPropertyOv(val, ctx, spropid);
	}

	val->setVoid();
	if(spropid->isInt())
	{
		s32 slotidx = spropid->getInt();
		if((0 <= slotidx) && ((u32)slotidx < _numArg))
		{
			if((_objArgs != 0L) && isArgDeleted(ctx, (u16)slotidx))
			{
				return _objArgs->callGetPropertyOv(val, ctx, spropid);
			}
			val->copy(&_valarrStack[2 + slotidx]);
		}
		else
		{
			if(_objArgs != 0L)
			{
				return _objArgs->callGetPropertyOv(val, ctx, spropid);
			}
		}
	}
	else
	{
		if(	spropid->isAtom() &&
			(spropid->getAtom() == EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom())	)
		{
			if((_objArgs != 0L) && TFW_IS_FLAG(_bitsOvdPropsid, convPropsidToBits(EsFunction::PROPSID_ARGS_LENGTH)))
			{
				return _objArgs->callGetPropertyOv(val, ctx, spropid);
			}
			val->setValue((s32)_numArg);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	引数値を取得する ⇒ js_GetArgsValue
**//*---------------------------------------------------------------------*/
bool EsStackFrame::getArgsValue(EsValue* val, EsContext* ctx)
{
	if(TFW_IS_FLAG(_bitsOvdPropsid, convPropsidToBits(EsFunction::PROPSID_CALL_ARGUMENTS)))
	{
		ASSERT(_objCall != 0L);
		EsValue valArgumentsAtom(EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ARGUMENTS)->getAtom());
		return _objCall->callGetPropertyOv(val, ctx, &valArgumentsAtom);
	}

	EsObject* objArgs = makeArgsObject(ctx);
	if(objArgs == 0L)
	{
		return false;
	}
	val->setValue(objArgs);
	return true;
}

/*---------------------------------------------------------------------*//**
	引数オブジェクトをクリア ⇒ js_PutArgsObject
**//*---------------------------------------------------------------------*/
bool EsStackFrame::putArgsObject(EsContext* ctx)
{
	bool isOk;

	isOk = EsSysClasses::EsArgumentsClass::enumerate(ctx, _objArgs);	// ⇒ ok = args_enumerate(cx, argsobj);

	EsValue valBits;
	_objArgs->getSlots()->getReservedSlotValue(&valBits, ctx, 0);	// ⇒ JS_GetReservedSlot(cx, argsobj, 0, &bmapval);
	if(!valBits.isVoid())
	{
		EsValue valVoid;
		_objArgs->slots()->setReservedSlotValue(ctx, 0, &valVoid);	// ⇒ JS_SetReservedSlot(cx, argsobj, 0, JSVAL_VOID);
		if(_numArg > 0x3f)	// ⇒ if (fp->argc > JSVAL_INT_BITS)
		{
			ASSERT(valBits.isPrivateData());
			ArgBitFlags* abf = (ArgBitFlags*)valBits.getPrivateData();
			ctx->deleteRegisteredCustomUnit(abf);
		}
	}

	// _objArgs が消える前にプロトタイプのプロパティを取得する
	EsValue valRes;
	EsValue valSpopId;
	if(!_objArgs->callGetPropertyBase(&valRes, ctx, EsValue::setout(&valSpopId, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CALLEE)->getAtom())))	{	isOk = false;	}
	if(!_objArgs->callSetPropertyBase(ctx, EsValue::setout(&valSpopId, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CALLEE)->getAtom()), &valRes))	{	isOk = false;	}
	if(!_objArgs->callGetPropertyBase(&valRes, ctx, EsValue::setout(&valSpopId, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom())))	{	isOk = false;	}
	if(!_objArgs->callSetPropertyBase(ctx, EsValue::setout(&valSpopId, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LENGTH)->getAtom()), &valRes))	{	isOk = false;	}

	// ポインタのクリア
	_objArgs->slots()->setPrivateData(0L);
	_objArgs = 0L;

	return isOk;
}

/*---------------------------------------------------------------------*//**
	with ブロックに入る ⇒ js_EnterWith
**//*---------------------------------------------------------------------*/
bool EsStackFrame::enterWithBlock(EsContext* ctx, s32 indexStack)
{
	EsValue* sp = _regs->sp(0);
	ASSERT(indexStack < 0);
	ASSERT(getStackTop() <= (sp + indexStack));

	EsObject* obj;
	if(!sp[-1].isPrimitive())
	{
		ASSERT(sp[-1].isObject());
		obj = sp[-1].getObject();
	}
	else
	{
		if(!ctx->convValueToObject(&obj, &sp[-1]))
		{
			return false;
		}
		sp[-1].setValue(obj);
	}

	EsObject* objParent = makeScopeChainObject(ctx);
	if(objParent == 0L)
	{
		return false;
	}

	// ※※※ 実装不要 ※※※ ⇒ OBJ_TO_INNER_OBJECT, JSExtendedClass, JSExtendedClass

	// with オブジェクトを作成する ⇒ js_NewWithObject(cx, obj, parent, sp + stackIndex - StackBase(fp));
	EsObject* objWith = ctx->newObject(EsWith::EsWithClass::getInstance(), obj, objParent);
	if(objWith == 0L)	{	return false;	}
	objWith->slots()->setPrivateData(this);
	s32 depth = (s32)(sp + indexStack - getStackTop());
	objWith->slots()->setBlockDepth(depth);	// ⇒ OBJ_SET_BLOCK_DEPTH(cx, obj, depth);

	setScopeChainObject(objWith);

	return true;
}

/*---------------------------------------------------------------------*//**
	with ブロックから抜ける ⇒ js_LeaveWith
**//*---------------------------------------------------------------------*/
void EsStackFrame::leaveWithBlock(EsContext* ctx)
{
	EsStackFrame* sfrm = ctx->getTopStackFrame();

	EsObject* objWith = sfrm->getScopeChainObject();
	ASSERT(objWith->getClass()->getClassType() == EsSysClasses::CLASSTYPE_WITH);
	ASSERT(objWith->getSlots()->getPrivateData() == sfrm);
	ASSERT(objWith->getSlots()->getBlockDepth() >= 0);
	
	sfrm->setScopeChainObject(objWith->slots()->parentObject());

	objWith->slots()->setPrivateData(0L);
	objWith->becomeGarbage();	// 削除可能
}

/*---------------------------------------------------------------------*//**
	アクティブなブロックのクラスを得る ⇒ js_IsActiveWithOrBlock
**//*---------------------------------------------------------------------*/
const EsClass* EsStackFrame::getActiveBlockClass(EsContext* ctx, const EsObject* obj, s32 depthStack)
{
	const EsClass* cls = obj->getClass();
	switch(cls->getClassType())
	{
	case EsSysClasses::CLASSTYPE_BLOCK:
	case EsSysClasses::CLASSTYPE_WITH:
		if(	(obj->getSlots()->getPrivateData() == ctx->getTopStackFrame()) &&
			(obj->getSlots()->getBlockDepth() >= depthStack)	)
		{
			return cls;
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	スコープを遡る ⇒ js_UnwindScope
**//*---------------------------------------------------------------------*/
bool EsStackFrame::unwindScope(EsContext* ctx, s32 depthStack, bool isNormalUnwind)
{
	ASSERT(depthStack >= 0);
	ASSERT((getStackTop() + depthStack) <= _regs->sp(0));

	EsObject* obj = _objBlockChain;
	while(obj != 0L)
	{
		ASSERT(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_BLOCK);
		if(obj->getSlots()->getBlockDepth() < depthStack)
		{
			break;
		}
		obj = obj->slots()->parentObject();
	}
	_objBlockChain = obj;

	while(true)
	{
		obj = _objScopeChain;
		const EsClass* cls = getActiveBlockClass(ctx, obj, depthStack);
		if(cls == 0L)	{	break;	}
		if(cls->getClassType() == EsSysClasses::CLASSTYPE_BLOCK)
		{
			if(!putBlockObject(ctx, isNormalUnwind))	{	isNormalUnwind = false;	}
		}
		else
		{
			leaveWithBlock(ctx);
		}
	}

	_regs->setCurSpIndex(depthStack);	// ⇒ fp->regs->sp = StackBase(fp) + stackDepth;
	return isNormalUnwind;
}

/*---------------------------------------------------------------------*//**
	ブロックオブジェクトを積む ⇒ js_PutBlockObject
**//*---------------------------------------------------------------------*/
bool EsStackFrame::putBlockObject(EsContext* ctx, bool isNormalUnwind)
{
	EsObject* obj = _objScopeChain;

	ASSERT(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_BLOCK);
	ASSERT(obj->getSlots()->getPrivateData() == this);
	ASSERT(obj->getObjType() == OBJT_BLOCK);
	ASSERT(((EsBlock*)obj)->isCloned());		// ⇒ JS_ASSERT(OBJ_IS_CLONED_BLOCK(obj));

	s32 depth = obj->getSlots()->getBlockDepth();
	s32 count = obj->getScope()->getScopePropertyNum();	// ⇒ count = OBJ_BLOCK_COUNT(cx, obj);
	ASSERT(depth <= (_regs->sp() - getStackTop()));
	ASSERT(count <= (_regs->sp() - getStackTop() - depth));
	ASSERT(count >= 1);

	depth += _il->getFixedSlotNum();
	obj->slots()->setValue(obj->getSlots()->getBlockDepthIndex() + 1, obj->getSlots()->getValue(depth));	// ⇒ obj->fslots[JSSLOT_BLOCK_DEPTH + 1] = fp->slots[depth];
	if(isNormalUnwind && (count > 1))
	{
		count--;
		if(obj->slots()->recreate(NUM_INITIAL_SLOT + count))	// ⇒ js_ReallocSlots(cx, obj, JS_INITIAL_NSLOTS + count, JS_TRUE)
		{
			TFW_COPY_MEMORY(obj->slots()->dvalue(0), &_varrSlots[depth + 1], sizeof(EsValue) * count);	// ⇒ memcpy(obj->dslots, fp->slots + depth + 1, count * sizeof(jsval));
		}
		else
		{
			isNormalUnwind = false;
		}
	}

	obj->slots()->setPrivateData(0L);
	_objScopeChain = obj->slots()->parentObject();

	return isNormalUnwind;
}

/*---------------------------------------------------------------------*//**
	引数が削除されたか判定する ⇒ ArgWasDeleted
**//*---------------------------------------------------------------------*/
bool EsStackFrame::isArgDeleted(EsContext* ctx, u16 slotidx)
{
	EsValue valBits;
	_objArgs->getSlots()->getReservedSlotValue(&valBits, ctx, 0);	// ⇒ JS_GetReservedSlot(cx, argsobj, 0, &bmapval);
	if(valBits.isVoid())
	{
		return false;
	}

	if(_numArg <= 0x3f)	// ⇒ if (fp->argc <= JSVAL_INT_BITS)
	{
		ASSERT(valBits.isInt());
		s32 ibits = valBits.getInt();
		return (ibits & (1 << (slotidx & 0x3f))) != 0;
	}
	else
	{
		ASSERT(valBits.isPrivateData());
		ArgBitFlags* abf = (ArgBitFlags*)valBits.getPrivateData();
		return abf->_bfarr->getFlag(slotidx);
	}
}

/*---------------------------------------------------------------------*//**
	引数が削除されたか判定する ⇒ MarkArgDeleted
**//*---------------------------------------------------------------------*/
bool EsStackFrame::markArgDeleted(EsContext* ctx, u16 slotidx)
{
	EsValue valBits;
	_objArgs->getSlots()->getReservedSlotValue(&valBits, ctx, 0);	// ⇒ JS_GetReservedSlot(cx, argsobj, 0, &bmapval);
	if(_numArg <= 0x3f)	// ⇒ if (nbits <= JSVAL_INT_BITS)
	{
		s32 ibits = 0;
		if(!valBits.isVoid())
		{
			ASSERT(valBits.isInt());
			ibits = valBits.getInt();
		}
		ibits |= (1 << (slotidx & 0x3f));
		valBits.setValue(ibits);
	}
	else
	{
		ArgBitFlags* abf = 0L;
		if(valBits.isVoid())
		{
			abf = new ArgBitFlags();
			if(abf == 0L)	{	return false;	}
			ctx->registerCustomUnit(abf);
			valBits.setPrivateData(abf);
		}
		else
		{
			ASSERT(valBits.isPrivateData());
			abf = (ArgBitFlags*)valBits.getPrivateData();
		}
		if(abf == 0L)	{	return false;	}
		abf->_bfarr->setFlag(slotidx, true);
	}
	_objArgs->slots()->setReservedSlotValue(ctx, 0, &valBits);	// ⇒ JS_SetReservedSlot(cx, argsobj, 0, bmapval);
	return true;
}

/*---------------------------------------------------------------------*//**
	関数のプロパティショートIDをフラグビットに変換する
**//*---------------------------------------------------------------------*/
u16 EsStackFrame::convPropsidToBits(s16 propsid)
{
	return 1 << (- propsid - 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsStackFrame::EsStackFrame()
	: _il(0L)
	, _pc(0L)
	, _regs(0L)
	, _flags(0)
	, _bitsOvdPropsid(0)
	, _numSlot(0)
	, _varrSlots(0L)
	, _varrSlotsAlloc(0L)
	, _objScopeChain(0L)
	, _objBlockChain(0L)
	, _objVar(0L)
	, _objCallee(0L)
	, _objThis(0L)
	, _func(0L)
	, _objCall(0L)
	, _valarrStack(0L)
	, _valarrStackOrg(0L)
	, _valarrStackAlloc(0L)
	, _numArg(0)
	, _objArgs(0L)
	, _valFuncRes(0L)
	, _sfrmDown(0L)
	, _lenLastOpcode(0)
	, _depthInit(0)
	, _objSharpVar(0L)
	, _sfrmPrevDisp(0L)
	, _regsCallerRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsStackFrame::~EsStackFrame()
{
	delete _valFuncRes;
	delete _regs;
	delete _pc;
	delete[] _varrSlotsAlloc;
	delete[] _valarrStackAlloc;
}

/*---------------------------------------------------------------------*//**
	スロット配列の作成
**//*---------------------------------------------------------------------*/
EsValue* EsStackFrame::makeSlotArray(EsContext* ctx, s32 numFuncArgs, s32 numFuncVars, s32 numSlot, EsValue* vaStack, s32 numActualArg)
{
	_varrSlots = 0L;
	_varrSlotsAlloc = 0L;
	_valarrStackOrg = vaStack;	// 与えられたオリジナルのスタック配列ポインタを保存する
	_numSlot = (u16)numSlot;	// スロットサイズを確定

	// 必要な配列を求める
	s32 numSlotNeeds = 2 + numFuncArgs + numSlot;
	EsValue* varrSlotsNew;
	if(numSlotNeeds <= (2 + numActualArg))
	{
		varrSlotsNew = _valarrStackOrg;
	}
	else
	{
		varrSlotsNew = _varrSlotsAlloc = new EsValue[numSlotNeeds];	// 新たに確保（スタック破棄時に削除する）
		if(varrSlotsNew == 0L)	{	return 0L;	}
	}

	// 配列の初期化
	if(varrSlotsNew != _valarrStackOrg)	// 新たに確保された場合
	{
		s32 numArgsCopy = TFW_MIN(numActualArg, numFuncArgs) + 2;
		for(int i = 0; i < numArgsCopy; i++)
		{
			varrSlotsNew[i].copy(&_valarrStackOrg[i]);
		}
	}
	else						// 与えられた配列を使用する場合
	{
		for(int i = 2 + numFuncArgs; i < numSlotNeeds; i++)
		{
			varrSlotsNew[i].setVoid();
		}
	}

	// スタック配列の位置を決める
	vaStack = &varrSlotsNew[0];

	// スロット配列をセット
	_varrSlots = &varrSlotsNew[2 + numFuncArgs];

	// スタックレジスタを用意
	EsValue* valarrSpNew = &varrSlotsNew[2 + numFuncArgs + numFuncVars];
	s32 numSp = numSlotNeeds - (2 + numFuncArgs + numFuncVars);
	_regs = new EsStackRegister(ctx, _pc, valarrSpNew, numSp, 0);

	return vaStack;
}


//==========================================================================
// EsStackFrame::ArgBitFlags メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsStackFrame::ArgBitFlags::ArgBitFlags()
	: _bfarr(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsStackFrame::ArgBitFlags::~ArgBitFlags()
{
	delete _bfarr;
}


////////////////////////////////////////////////////////////////////////////

ES_END_NS
