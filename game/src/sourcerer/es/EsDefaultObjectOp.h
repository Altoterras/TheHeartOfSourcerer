/***********************************************************************//**
 *	EsDefaultObjectOp.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_OBJECT_DEFAULT_OP_H_
#define _ES_ES_OBJECT_DEFAULT_OP_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsObjectOp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	デフォルトのオブジェクト操作子
 *
**//*---------------------------------------------------------------------*/
class EsDefaultObjectOp : public EsObjectOp
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool defineProperty(EsScopeProperty** sprop, EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf);
	static bool deleteProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid);
	static void dropProperty(EsContext* ctx, EsObject* obj, EsScopeProperty* sprop);
	static bool findProperty(EsScopeProperty** sprop, EsObject** objScopeOwner, EsContext* ctx, EsObject* obj, const EsValue* spropid);
	static bool callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid);
	static bool callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* spropid, const EsValue* val);
	static bool getPropertyAttributeFlags(u8* spatrbf, EsContext* ctx, EsObject* obj, const EsValue* spropid, EsScopeProperty* sprop);
	static bool convDefaultValue(EsContext* ctx, EsObject* obj, EsValue* val, EsLogicalType ltype);
	static bool checkAccess(u8* spatrbf, EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* spropid, u8 acsflags);
	static bool enumerate(EsValue* valState, EsValue* id, EsContext* ctx, EsObject* obj, u8 enumop);

	static bool getRequiredSlot(EsValue* val, EsContext* ctx, EsObject* obj, s32 slotidx);
	static bool setRequiredSlot(EsContext* ctx, EsObject* obj, s32 slotidx, const EsValue* val);
	static bool call(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool construct(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	static bool hasInstance(EsValue* valRes, EsContext* ctx, EsObject* obj, const EsValue* valInst);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsDefaultObjectOp();
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_OBJECT_DEFAULT_OP_H_
