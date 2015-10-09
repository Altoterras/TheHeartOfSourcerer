/***********************************************************************//**
 *	EsWith.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/08/18.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsWith.h"

// Friends
#include "EsContext.h"
#include "EsError.h"
#include "EsSysClasses.h"
#include "EsKeywordSet.h"
#include "EsObject.h"
#include "EsObjectSlots.h"
#include "EsResolver.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsWith::EsWithClass メソッド

/*---------------------------------------------------------------------*//**
	with スクリプトクラスのオブジェクト演算子セットを得る
	⇒ js_WithObjectOps
**//*---------------------------------------------------------------------*/
EsObjectOp* EsWith::EsWithClass::getObjectOp() const
{
	return new EsWith::EsWithObjectOp();
}

/*---------------------------------------------------------------------*//**
	with オブジェクトクラスのコンストラクタ ⇒ js_WithClass
**//*---------------------------------------------------------------------*/
EsWith::EsWithClass::EsWithClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_WITH)->getAtom()
		, EsSysClasses::CLASSTYPE_WITH
		, EsClass::F_HAS_PRIVATE | EsClass::F_IS_ANONYMOUS
		, EsSysPrototypes::PTYPEKIND_NULL
		, 1 )
{
}


//==========================================================================
// EsWith::EsWithObjectOp メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	プロパティを定義
**//*---------------------------------------------------------------------*/
bool EsWith::EsWithObjectOp::defineProperty(EsScopeProperty** sprop, EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf)
{
	return obj->definePropertyBase(sprop, ctx, spropid, value, funcGetter, funcSetter, atrbf);
}

/*---------------------------------------------------------------------*//**
	プロパティを削除 ⇒ with_DeleteProperty
**//*---------------------------------------------------------------------*/
bool EsWith::EsWithObjectOp::deleteProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	EsObject* objPtype = obj->slots()->prototypeObject();
	if(objPtype != 0L)
	{
		return objPtype->deletePropertyOv(val, ctx, spropid);
	}
	return obj->deletePropertyBase(val, ctx, spropid);
}

/*---------------------------------------------------------------------*//**
	プロパティをドロップ
**//*---------------------------------------------------------------------*/
void EsWith::EsWithObjectOp::dropProperty(EsContext* ctx, EsObject* obj, EsScopeProperty* sprop)
{
	return obj->dropPropertyBase(ctx, sprop);
}

/*---------------------------------------------------------------------*//**
	プロパティを探す ⇒ with_LookupProperty
**//*---------------------------------------------------------------------*/
bool EsWith::EsWithObjectOp::findProperty(EsScopeProperty** sprop, EsObject** objScopeOwner, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	u8 flagsRslv = ctx->getResolver()->getResolvFlags();
	if(flagsRslv == EsResolver::RSLVF_INFER)
	{
		flagsRslv =	EsResolver::inferFlags(ctx, flagsRslv);
	}
	EsResolver::AutoFlags raf(ctx->getResolver(), flagsRslv);	// ⇒ JSAutoResolveFlags rf(cx, flags);

	EsObject* objPtype = obj->slots()->prototypeObject();
	if(objPtype != 0L)
	{
		return objPtype->findPropertyOv(sprop, objScopeOwner, ctx, spropid);
	}
	return obj->findPropertyBase(sprop, objScopeOwner, ctx, spropid);
}

/*---------------------------------------------------------------------*//**
	アクセサ取得 ⇒ with_GetProperty
**//*---------------------------------------------------------------------*/
bool EsWith::EsWithObjectOp::callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	EsObject* objPtype = obj->slots()->prototypeObject();
	if(objPtype != 0L)
	{
		return objPtype->callGetPropertyOv(val, ctx, spropid);
	}
	return obj->callGetPropertyBase(val, ctx, spropid);
}

/*---------------------------------------------------------------------*//**
	アクセサ設定 ⇒ with_SetProperty
**//*---------------------------------------------------------------------*/
bool EsWith::EsWithObjectOp::callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* val)
{
	EsObject* objPtype = obj->slots()->prototypeObject();
	if(objPtype != 0L)
	{
		return objPtype->callSetPropertyOv(ctx, spropid, val);
	}
	return obj->callSetPropertyBase(ctx, spropid, val);
}

/*---------------------------------------------------------------------*//**
	スコープ属性フラグを得る ⇒ with_GetAttributes
**//*---------------------------------------------------------------------*/
bool EsWith::EsWithObjectOp::getPropertyAttributeFlags(u8* spatrbf, EsContext* ctx, EsObject* obj, const EsValue* spropid, EsScopeProperty* sprop)
{
	EsObject* objPtype = obj->slots()->prototypeObject();
	if(objPtype != 0L)
	{
		return objPtype->getPropertyAttributeFlagsOv(spatrbf, ctx, spropid, sprop);
	}
	return obj->getPropertyAttributeFlagsBase(spatrbf, ctx, spropid, sprop);
}

/*---------------------------------------------------------------------*//**
	オブジェクトのデフォルト値を得る ⇒ with_DefaultValue
**//*---------------------------------------------------------------------*/
bool EsWith::EsWithObjectOp::convDefaultValue(EsContext* ctx, EsObject* obj, EsValue* val, EsLogicalType ltype)
{
	EsObject* objPtype = obj->slots()->prototypeObject();
	if(objPtype != 0L)
	{
		return objPtype->convDefaultValueOv(ctx, val, ltype);
	}
	return obj->convDefaultValueBase(ctx, val, ltype);
}

/*---------------------------------------------------------------------*//**
	アクセスチェック ⇒ with_CheckAccess
**//*---------------------------------------------------------------------*/
bool EsWith::EsWithObjectOp::checkAccess(u8* spatrbf, EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid, u8 acsflags)
{
	EsObject* objPtype = obj->slots()->prototypeObject();
	if(objPtype != 0L)
	{
		return objPtype->checkAccessOv(spatrbf, val, ctx, spropid, acsflags);
	}
	return obj->checkAccessBase(spatrbf, val, ctx, spropid, acsflags);
}

/*---------------------------------------------------------------------*//**
	列挙 ⇒ with_Enumerate
**//*---------------------------------------------------------------------*/
bool EsWith::EsWithObjectOp::enumerate(EsValue* valState, EsValue* id, EsContext* ctx, EsObject* obj, u8 enumop)
{
	EsObject* objPtype = obj->slots()->prototypeObject();
	if(objPtype != 0L)
	{
		return objPtype->enumerateOv(valState, id, ctx, enumop);
	}
	return obj->enumerateBase(valState, id, ctx, enumop);
}

/*---------------------------------------------------------------------*//**
	this オブジェクトを得る ⇒ with_ThisObject
**//*---------------------------------------------------------------------*/
EsObject* EsWith::EsWithObjectOp::getThisObject(EsContext* ctx, EsObject* obj)
{
	EsObject* objPtype = obj->slots()->prototypeObject();
	if(objPtype != 0L)
	{
		return objPtype->getThisObjectOv(ctx);
	}
	return obj;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsWith::EsWithObjectOp::EsWithObjectOp()
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
	_funcGetThisObject = getThisObject;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
