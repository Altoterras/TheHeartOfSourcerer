/***********************************************************************//**
 *	EsMath.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/10/24.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_MATH_H_
#define _ES_ES_MATH_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsClass.h"
#include "EsPrototype.h"
#include "EsObject.h"
#include "EsObjectOp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	数値処理
 *
**//*---------------------------------------------------------------------*/
class EsMath
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	数値処理プロトタイプ クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsMathPrototype : public EsPrototype
	{
		//==================================================================
		// メソッド
	public:
		bool initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const;
		static const EsMathPrototype* getInstance() { static EsMathPrototype instance; return &instance; }

	private:
		EsMathPrototype();
	};

	/*-----------------------------------------------------------------*//**
	 *	数値処理スクリプト クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsMathClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		static const EsMathClass* getInstance() { static EsMathClass instance; return &instance; }
		static EsObject* makeClassObject(EsContext* ctx, EsObject* objParent);

	private:
		EsMathClass();
		static bool callAbs(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callAcos(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callAsin(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callAtan(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callAtan2(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callCeil(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callCos(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callExp(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callFloor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callLog(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callMax(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callMin(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callPow(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callRandom(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callRound(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callSin(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callSqrt(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callTan(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	};
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_MATH_H_
