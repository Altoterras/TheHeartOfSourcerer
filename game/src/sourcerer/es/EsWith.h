/***********************************************************************//**
 *	EsWith.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/08/18.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_WITH_H_
#define _ES_ES_WITH_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsClass.h"
#include "EsObject.h"
#include "EsObjectOp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	with
 *
**//*---------------------------------------------------------------------*/
class EsWith
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	with スクリプト クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsWithClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		virtual EsObjectOp* getObjectOp() const;

		static const EsWithClass* getInstance() { static EsWithClass instance; return &instance; }

	private:
		EsWithClass();
	};

	/*-----------------------------------------------------------------*//**
	 *	with オブジェクト操作子 ⇒ js_WithObjectOps
	 *
	**//*-----------------------------------------------------------------*/
	class EsWithObjectOp : public EsObjectOp
	{
		//==================================================================
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
		static EsObject* getThisObject(EsContext* ctx, EsObject* obj);

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		EsWithObjectOp();
	};
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_WITH_H_
