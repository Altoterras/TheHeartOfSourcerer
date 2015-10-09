/***********************************************************************//**
 *	EsBoolean.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/08/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_BOOLEAN_H_
#define _ES_ES_BOOLEAN_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsClass.h"
#include "EsPrototype.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スクリプト ブーリアン
 *
**//*---------------------------------------------------------------------*/
class EsBoolean
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	真偽値プロトタイプ クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsBooleanPrototype : public EsPrototype
	{
		//==================================================================
		// メソッド
	public:
		bool initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const;
		static const EsBooleanPrototype* getInstance() { static EsBooleanPrototype instance; return &instance; }

	private:
		EsBooleanPrototype();
	};

	/*-----------------------------------------------------------------*//**
	 *	ブーリアン スクリプト クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsBooleanClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		static const EsBooleanClass* getInstance() { static EsBooleanClass instance; return &instance; }
		static EsObject* makeClassObject(EsContext* ctx, EsObject* objParent); 

	private:
		EsBooleanClass();
		static bool callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callValueOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	};
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_BOOLEAN_H_
