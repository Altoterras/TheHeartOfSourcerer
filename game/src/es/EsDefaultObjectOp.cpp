/***********************************************************************//**
 *	EsDefaultObjectOp.cpp
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
#include "EsDefaultObjectOp.h"

// Friends
#include "EsError.h"
#include "EsContext.h"
#include "EsSysClasses.h"
#include "EsFunction.h"
#include "EsObject.h"
#include "EsOpcodeType.h"
#include "EsProgramCounter.h"
#include "EsScope.h"
#include "EsScopeProperty.h"
#include "EsStackFrame.h"
#include "EsStackRegister.h"
#include "EsValue.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	プロパティを定義
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::defineProperty(EsScopeProperty** sprop, EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf)
{
	return obj->definePropertyBase(sprop, ctx, spropid, value, funcGetter, funcSetter, atrbf);
}

/*---------------------------------------------------------------------*//**
	プロパティを削除
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::deleteProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	return obj->deletePropertyBase(val, ctx, spropid);
}

/*---------------------------------------------------------------------*//**
	プロパティをドロップ
**//*---------------------------------------------------------------------*/
void EsDefaultObjectOp::dropProperty(EsContext* ctx, EsObject* obj, EsScopeProperty* sprop)
{
	return obj->dropPropertyBase(ctx, sprop);
}

/*---------------------------------------------------------------------*//**
	プロパティを探す
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::findProperty(EsScopeProperty** sprop, EsObject** objScopeOwner, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	return obj->findPropertyBase(sprop, objScopeOwner, ctx, spropid);
}

/*---------------------------------------------------------------------*//**
	取得プロパティのコール
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	return obj->callGetPropertyBase(val, ctx, spropid);
}

/*---------------------------------------------------------------------*//**
	設定プロパティのコール
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* val)
{
	return obj->callSetPropertyBase(ctx, spropid, val);
}

/*---------------------------------------------------------------------*//**
	スコープ属性フラグを得る
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::getPropertyAttributeFlags(u8* spatrbf, EsContext* ctx, EsObject* obj, const EsValue* spropid, EsScopeProperty* sprop)
{
	return obj->getPropertyAttributeFlagsBase(spatrbf, ctx, spropid, sprop);
}

/*---------------------------------------------------------------------*//**
	オブジェクトのデフォルト値を得る
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::convDefaultValue(EsContext* ctx, EsObject* obj, EsValue* val, EsLogicalType ltype)
{
	return obj->convDefaultValueBase(ctx, val, ltype);
}

/*---------------------------------------------------------------------*//**
	アクセスチェック
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::checkAccess(u8* spatrbf, EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid, u8 acsflags)
{
	return obj->checkAccessBase(spatrbf, val, ctx, spropid, acsflags);
}

/*---------------------------------------------------------------------*//**
	列挙
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::enumerate(EsValue* valState, EsValue* id, EsContext* ctx, EsObject* obj, u8 enumop)
{
	return obj->enumerateBase(valState, id, ctx, enumop);
}

/*---------------------------------------------------------------------*//**
	必要スロットの値を得る
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::getRequiredSlot(EsValue* val, EsContext* ctx, EsObject* obj, s32 slotidx)
{
	return obj->getRequiredSlotBase(val, ctx, slotidx);
}

/*---------------------------------------------------------------------*//**
	必要スロットの値を設定する
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::setRequiredSlot(EsContext* ctx, EsObject* obj, s32 slotidx, const EsValue* val)
{
	return obj->setRequiredSlotBase(ctx, slotidx, val);
}

/*---------------------------------------------------------------------*//**
	コール
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::call(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	return objThis->callBase(ctx, valCallee, valThis, vaArg, numArg, valRet, exprm);
}

/*---------------------------------------------------------------------*//**
	コンストラクト
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::construct(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	return objThis->constructBase(ctx, valCallee, valThis, vaArg, numArg, valRet, exprm);
}

/*---------------------------------------------------------------------*//**
	インスタンス型判定
**//*---------------------------------------------------------------------*/
bool EsDefaultObjectOp::hasInstance(EsValue* valRes, EsContext* ctx, EsObject* obj, const EsValue* valInst)
{
	return obj->hasInstanceBase(valRes, ctx, obj, valInst);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsDefaultObjectOp::EsDefaultObjectOp()
{
	_funcDefineProperty = defineProperty;
	_funcDeleteProperty = deleteProperty;
	_funcDropProperty = dropProperty;
	_funcFindProperty = findProperty;
	_funcGetProperty = callGetProperty;
	_funcSetProperty = callSetProperty;
	_funcGetPropertyAttributeFlags = getPropertyAttributeFlags;
	_funcConvDefaultValue = convDefaultValue;
	_funcCheckAccess = checkAccess;
	_funcEnumerate = enumerate;
	_funcGetRequiredSlot = getRequiredSlot;
	_funcSetRequiredSlot = setRequiredSlot;
	_funcCall = call;
	_funcConstruct = construct;
	_funcHasInstance = hasInstance;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
