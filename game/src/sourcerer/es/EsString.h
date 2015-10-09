/***********************************************************************//**
 *	EsString.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/08/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_STRING_H_
#define _ES_ES_STRING_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsClass.h"
#include "EsPrototype.h"
#include "../../tfw/hash/HashOp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スクリプト文字列
 *
**//*---------------------------------------------------------------------*/
class EsString
{
	//======================================================================
	// 定数
public:

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	文字列アトムハッシュ演算
	 *		※ アトムは内部で new / delete する
	 *
	**//*-----------------------------------------------------------------*/
	class EsStringAtomHashOp : public HashOp
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
	 *	外部文字列アトムハッシュ演算
	 *		※ アトムは外部のものを参照するため、new / delete しない
	 *
	**//*-----------------------------------------------------------------*/
	class EsStringRefAtomHashOp : public EsStringAtomHashOp
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 外部サービス
	public:
		virtual bool	initEntry(HashEntry* entry, const void* key);
		virtual void	cleanupEntry(HashEntry* entry);
	};

	/*-----------------------------------------------------------------*//**
	 *	文字列プロトタイプ クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsStringPrototype : public EsPrototype
	{
		//==================================================================
		// メソッド
	public:
		bool initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const;
		static const EsStringPrototype* getInstance() { static EsStringPrototype instance; return &instance; }

	private:
		EsStringPrototype();
	};

	/*-----------------------------------------------------------------*//**
	 *	文字列スクリプト クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsStringClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		static const EsStringClass* getInstance() { static EsStringClass instance; return &instance; }
		static EsObject* makeClassObject(EsContext* ctx, EsObject* objParent); 

	private:
		EsStringClass();
		static bool callEnumerate(EsContext* ctx, EsObject* obj);
		static bool callResolve(EsObject** objOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv);
		static bool callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callSubstring(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callToLowerCase(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callToUpperCase(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callCharAt(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callCharCodeAt(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callIndexOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callLastIndexOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callTrim(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callTrimLeft(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callTrimRight(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callMatch(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callSearch(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callReplace(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callSplit(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callConcat(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callSlice(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callFromCharCode(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);

	private:
		static const VcString* convThisString(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg);
		static const VcString* convArgString(EsContext* ctx, EsValue* vaArg, u32 numArg, u32 index);
	};
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_STRING_H_
