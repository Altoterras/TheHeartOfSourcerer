/***********************************************************************//**
 *	EsObjectOp.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_OBJECT_OP_H_
#define _ES_ES_OBJECT_OP_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsAtom;
class EsContext;
class EsFunction;
class EsObject;
class EsScopeProperty;

/*---------------------------------------------------------------------*//**
 *	オブジェクト演算子セット
 *		手動実装関数テーブル
 *		EsObject でこちらのオーバーライドされた関数を呼ぶときは、
 *		オリジナルのメソッドではなく、～Ov を呼ぶようにする
 *
**//*---------------------------------------------------------------------*/
class EsObjectOp
{
	//======================================================================
	// 型定義
public:
	typedef bool (* EsObjectOpDefinePropertyFunc)(EsScopeProperty** sprop, EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf);
	typedef bool (* EsObjectOpDeletePropertyFunc)(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid);
	typedef void (* EsObjectOpDropPropertyFunc)(EsContext* ctx, EsObject* obj, EsScopeProperty* sprop);
	typedef bool (* EsObjectOpFindPropertyFunc)(EsScopeProperty** sprop, EsObject** objScopeOwner, EsContext* ctx, EsObject* obj, const EsValue* spropid);
	typedef bool (* EsObjectOpGetPropertyAttributeFlagsFunc)(u8* spatrbf, EsContext* ctx, EsObject* obj, const EsValue* spropid, EsScopeProperty* sprop);
	typedef bool (* EsObjectOpConvDefaultValueFunc)(EsContext* ctx, EsObject* obj, EsValue* val, EsLogicalType ltype);
	typedef EsObject* (* EsObjectOpGetThisObjectFunc)(EsContext* ctx, EsObject* obj);
	typedef bool (* EsObjectOpGetRequiredSlotFunc)(EsValue* val, EsContext* ctx, EsObject* obj, s32 slotidx);
	typedef bool (* EsObjectOpSetRequiredSlotFunc)(EsContext* ctx, EsObject* obj, s32 slotidx, const EsValue* val);

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 各スクリプトメソッドを得る
	inline EsObjectOpDefinePropertyFunc getDefinePropertyFunc() const { return _funcDefineProperty; }
	inline EsObjectOpDeletePropertyFunc getDeletePropertyFunc() const { return _funcDeleteProperty; }
	inline EsObjectOpDropPropertyFunc getDropPropertyFunc() const { return _funcDropProperty; }
	inline EsObjectOpFindPropertyFunc getFindPropertyFunc() const { return _funcFindProperty; }
	inline EsPropertyGetterFunc getGetPropertyFunc() const { return _funcGetProperty; }
	inline EsPropertySetterFunc getSetPropertyFunc() const { return _funcSetProperty; }
	inline EsObjectOpGetPropertyAttributeFlagsFunc getGetPropertyAttributeFlagsFunc() const { return _funcGetPropertyAttributeFlags; }
	inline EsObjectOpConvDefaultValueFunc getConvDefaultValueFunc() const { return _funcConvDefaultValue; }
	inline EsCheckAccessFunc getCheckAccessFunc() const { return _funcCheckAccess; }
	inline EsEnumerateOutFunc getEnumerateFunc() const { return _funcEnumerate; }
	inline EsObjectOpGetThisObjectFunc getGetThisObjectFunc() const { return _funcGetThisObject; }
	inline EsObjectOpGetRequiredSlotFunc getGetRequiredSlotFunc() const { return _funcGetRequiredSlot; }
	inline EsObjectOpSetRequiredSlotFunc getSetRequiredSlotFunc() const { return _funcSetRequiredSlot; }
	inline EsNativeCall getCallFunc() const { return _funcCall; }
	inline EsNativeCall getConstructFunc() const { return _funcConstruct; }
	inline EsHasInstanceFunc getHasInstanceFunc() const { return _funcHasInstance; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool defineProperty(EsScopeProperty** sprop, EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf);
	bool deleteProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid);
	void dropProperty(EsContext* ctx, EsObject* obj, EsScopeProperty* sprop);
	bool findProperty(EsScopeProperty** sprop, EsObject** objScopeOwner, EsContext* ctx, EsObject* obj, const EsValue* spropid);
	bool callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid);
	bool callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* val);
	bool getPropertyAttributeFlags(u8* spatrbf, EsContext* ctx, EsObject* obj, const EsValue* spropid, EsScopeProperty* sprop);
	bool convDefaultValue(EsContext* ctx, EsObject* obj, EsValue* val, EsLogicalType ltype);
	bool checkAccess(u8* spatrbf, EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid, u8 acsflags);
	bool enumerate(EsValue* valState, EsValue* id, EsContext* ctx, EsObject* obj, u8 enumop);
	EsObject* getThisObject(EsContext* ctx, EsObject* obj);
	bool getRequiredSlot(EsValue* val, EsContext* ctx, EsObject* obj, s32 slotidx);
	bool setRequiredSlot(EsContext* ctx, EsObject* obj, s32 slotidx, const EsValue* val);
	bool call(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	bool construct(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	bool hasInstance(EsValue* valRes, EsContext* ctx, EsObject* obj, const EsValue* valInst);
	
	virtual bool isNative() const { return true; }			// ネイティブかどうか ⇒ OPS_IS_NATIVE

	EsObjectOp();

	//======================================================================
	// 変数
protected:
	EsObjectOpDefinePropertyFunc _funcDefineProperty;
	EsObjectOpDeletePropertyFunc _funcDeleteProperty;
	EsObjectOpDropPropertyFunc _funcDropProperty;
	EsObjectOpFindPropertyFunc _funcFindProperty;
	EsPropertyGetterFunc _funcGetProperty;
	EsPropertySetterFunc _funcSetProperty;
	EsObjectOpGetPropertyAttributeFlagsFunc _funcGetPropertyAttributeFlags;
	EsObjectOpConvDefaultValueFunc _funcConvDefaultValue;
	EsCheckAccessFunc _funcCheckAccess;
	EsEnumerateOutFunc _funcEnumerate;
	EsObjectOpGetThisObjectFunc _funcGetThisObject;
	EsObjectOpGetRequiredSlotFunc _funcGetRequiredSlot;
	EsObjectOpSetRequiredSlotFunc _funcSetRequiredSlot;
	EsNativeCall _funcCall;
	EsNativeCall _funcConstruct;
	EsHasInstanceFunc _funcHasInstance;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_OBJECT_OP_H_
