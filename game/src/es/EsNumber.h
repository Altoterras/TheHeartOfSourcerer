/***********************************************************************//**
 *	EsNumber.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/08/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_NUMBER_H_
#define _ES_ES_NUMBER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsClass.h"
#include "EsPrototype.h"
#include "../tfw/hash/HashOp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スクリプト 数値
 *
**//*---------------------------------------------------------------------*/
class EsNumber
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	ダブル値アトムハッシュ演算
	 *
	**//*-----------------------------------------------------------------*/
	class EsF64AtomHashOp : public HashOp
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 外部サービス
	public:
		virtual u32		getEntrySize();
		virtual void*	newEntryBuffer(u32 numEntry);
		virtual void	deleteEntryBuffer(void* entrybuf);
		virtual u32		hashCode(const void* key);
		virtual bool	matchEntry(HashEntry* entry, const void* key);
		virtual bool	initEntry(HashEntry* entry, const void* key);
		virtual void	cleanupEntry(HashEntry* entry);
	};

	/*-----------------------------------------------------------------*//**
	 *	数値プロトタイプ クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsNumberPrototype : public EsPrototype
	{
		//==================================================================
		// メソッド
	public:
		bool initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const;
		static const EsNumberPrototype* getInstance() { static EsNumberPrototype instance; return &instance; }

	private:
		EsNumberPrototype();
	};

	/*-----------------------------------------------------------------*//**
	 *	数値スクリプト クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsNumberClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		static const EsNumberClass* getInstance() { static EsNumberClass instance; return &instance; }
		static EsObject* makeClassObject(EsContext* ctx, EsObject* objParent); 

	private:
		EsNumberClass();
		static bool callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callIsNan(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callIsFinite(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callParseFloat(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callParseInt(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callValueOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callToFixed(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callToExponential(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callToPrecision(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);

	private:
		static bool convertTo(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, s32 modeArgZero, s32 modeArgOne, s32 precisionMin, s32 precisionMax, s32 precisionOffset);
	};
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_NUMBER_H_
