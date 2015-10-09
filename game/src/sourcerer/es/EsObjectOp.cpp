/***********************************************************************//**
 *	EsObjectOp.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/09/15.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsObjectOp.h"

// Friends

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
bool EsObjectOp::defineProperty(EsScopeProperty** sprop, EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf)
{
	return (_funcDefineProperty != 0L) ? _funcDefineProperty(sprop, ctx, obj, spropid, value, funcGetter, funcSetter, atrbf) : false;
}

/*---------------------------------------------------------------------*//**
	プロパティを削除
**//*---------------------------------------------------------------------*/
bool EsObjectOp::deleteProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	return (_funcDeleteProperty != 0L) ? _funcDeleteProperty(val, ctx, obj, spropid) : false;
}

/*---------------------------------------------------------------------*//**
	プロパティをドロップ
**//*---------------------------------------------------------------------*/
void EsObjectOp::dropProperty(EsContext* ctx, EsObject* obj, EsScopeProperty* sprop)
{
	if(_funcDropProperty != 0L) { _funcDropProperty(ctx, obj, sprop); }
}

/*---------------------------------------------------------------------*//**
	プロパティを探す
**//*---------------------------------------------------------------------*/
bool EsObjectOp::findProperty(EsScopeProperty** sprop, EsObject** objScopeOwner, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	return (_funcFindProperty != 0L) ? _funcFindProperty(sprop, objScopeOwner, ctx, obj, spropid) : false;
}

/*---------------------------------------------------------------------*//**
	アクセサ取得
**//*---------------------------------------------------------------------*/
bool EsObjectOp::callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid)
{
	return (_funcGetProperty != 0L) ? _funcGetProperty(val, ctx, obj, spropid) : false;
}

/*---------------------------------------------------------------------*//**
	アクセサ設定
**//*---------------------------------------------------------------------*/
bool EsObjectOp::callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* val)
{
	return (_funcSetProperty != 0L) ? _funcSetProperty(ctx, obj, spropid, val) : false;
}

/*---------------------------------------------------------------------*//**
	スコープ属性フラグを得る
**//*---------------------------------------------------------------------*/
bool EsObjectOp::getPropertyAttributeFlags(u8* spatrbf, EsContext* ctx, EsObject* obj, const EsValue* spropid, EsScopeProperty* sprop)
{
	return (_funcGetPropertyAttributeFlags != 0L) ? _funcGetPropertyAttributeFlags(spatrbf, ctx, obj, spropid, sprop) : false;
}

/*---------------------------------------------------------------------*//**
	オブジェクトのデフォルト値を得る
**//*---------------------------------------------------------------------*/
bool EsObjectOp::convDefaultValue(EsContext* ctx, EsObject* obj, EsValue* val, EsLogicalType ltype)
{
	return (_funcConvDefaultValue != 0L) ? _funcConvDefaultValue(ctx, obj, val, ltype) : false;
}

/*---------------------------------------------------------------------*//**
	アクセスチェック
**//*---------------------------------------------------------------------*/
bool EsObjectOp::checkAccess(u8* spatrbf, EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid, u8 acsflags)
{
	return (_funcCheckAccess != 0L) ? _funcCheckAccess(spatrbf, val, ctx, obj, spropid, acsflags) : false;
}

/*---------------------------------------------------------------------*//**
	列挙
**//*---------------------------------------------------------------------*/
bool EsObjectOp::enumerate(EsValue* valState, EsValue* id, EsContext* ctx, EsObject* obj, u8 enumop)
{
	return (_funcEnumerate != 0L) ? _funcEnumerate(valState, id, ctx, obj, enumop) : false;
}

/*---------------------------------------------------------------------*//**
	this ポインタを得る
**//*---------------------------------------------------------------------*/
EsObject* EsObjectOp::getThisObject(EsContext* ctx, EsObject* obj)
{
	return (_funcGetThisObject != 0L) ? _funcGetThisObject(ctx, obj) : obj;
}

/*---------------------------------------------------------------------*//**
	必要スロットの値を得る
**//*---------------------------------------------------------------------*/
bool EsObjectOp::getRequiredSlot(EsValue* val, EsContext* ctx, EsObject* obj, s32 slotidx)
{
	return (_funcGetRequiredSlot != 0L) ? _funcGetRequiredSlot(val, ctx, obj, slotidx) : false;
}

/*---------------------------------------------------------------------*//**
	必要スロットの値を設定する
**//*---------------------------------------------------------------------*/
bool EsObjectOp::setRequiredSlot(EsContext* ctx, EsObject* obj, s32 slotidx, const EsValue* val)
{
	return (_funcSetRequiredSlot != 0L) ? _funcSetRequiredSlot(ctx, obj, slotidx, val) : false;
}

/*---------------------------------------------------------------------*//**
	コール
**//*---------------------------------------------------------------------*/
bool EsObjectOp::call(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	return (_funcCall != 0L) ? _funcCall(ctx, objThis, valCallee, valThis, vaArg, numArg, valRet, exprm) : false;
}

/*---------------------------------------------------------------------*//**
	コンストラクト
**//*---------------------------------------------------------------------*/
bool EsObjectOp::construct(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	return (_funcConstruct != 0L) ? _funcConstruct(ctx, objThis, valCallee, valThis, vaArg, numArg, valRet, exprm) : false;
}

/*---------------------------------------------------------------------*//**
	インスタンス型判定
**//*---------------------------------------------------------------------*/
bool EsObjectOp::hasInstance(EsValue* valRes, EsContext* ctx, EsObject* obj, const EsValue* valInst)
{
	return (_funcHasInstance != 0L) ? _funcHasInstance(valRes, ctx, obj, valInst) : false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsObjectOp::EsObjectOp()
	: _funcDefineProperty(0L)
	, _funcDeleteProperty(0L)
	, _funcDropProperty(0L)
	, _funcFindProperty(0L)
	, _funcGetProperty(0L)
	, _funcSetProperty(0L)
	, _funcGetPropertyAttributeFlags(0L)
	, _funcConvDefaultValue(0L)
	, _funcCheckAccess(0L)
	, _funcEnumerate(0L)
	, _funcGetThisObject(0L)
	, _funcGetRequiredSlot(0L)
	, _funcSetRequiredSlot(0L)
	, _funcCall(0L)
	, _funcConstruct(0L)
	, _funcHasInstance(0L)
{
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
