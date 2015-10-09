/***********************************************************************//**
 *	EsIterator.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/07/19.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_ITERATOR_H_
#define _ES_ES_ITERATOR_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsClass.h"
#include "EsPrototype.h"
#include "EsObject.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	イテレータ
 *
**//*---------------------------------------------------------------------*/
class EsIterator : public EsObject
{
	//======================================================================
	// 定数
public:
	// イテレータ制御フラグ
	static const u8 F_ENUMERATE	= 0x1;	// for in の隠された列挙子 ⇒ JSITER_ENUMERATE
	static const u8 F_FOREACH	= 0x2;	// for each のキー・値ペアを返す ⇒ JSITER_FOREACH
	static const u8	F_KEYVALUE	= 0x4;	// キー・値ペア ⇒ JSITER_KEYVALUE

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	イテレータプロトタイプ クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsIteratorPrototype : public EsPrototype
	{
		//==================================================================
		// メソッド
	public:
		bool initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const;
		static const EsIteratorPrototype* getInstance() { static EsIteratorPrototype instance; return &instance; }

	private:
		EsIteratorPrototype();
	};

	/*-----------------------------------------------------------------*//**
	 *	イテレータ スクリプト クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsIteratorClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		// イテレータステイトスロットのインデックスを得る ⇒ JSSLOT_ITER_STATE
		inline u16 getIteratorStateSlotIndex() const { return getPrivateSlotIndex(); }
		// イテレータフラグスロットのインデックスを得る ⇒ JSSLOT_ITER_FLAGS
		inline u16 getIteratorFlagsSlotIndex() const { return getPrivateSlotIndex() + 1; }
		// ダイナミックスロットの開始インデックスを得る
		virtual u16 getStartDynamicSlotIndex() const { return getPrivateSlotIndex() + 2; }

		static const EsIteratorClass* getInstance() { static EsIteratorClass instance; return &instance; }
		static EsObject* makeClassObject(EsContext* ctx, EsObject* objParent); 

	private:
		EsIteratorClass();
		static bool callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callSelf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callNext(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static bool next(EsValue* valReturn, EsObject* objIter, EsContext* ctx);
	static bool close(EsObject* objIter, EsContext* ctx);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsIterator();
	bool create(EsContext* ctx, EsObject* objParent, u8 flags);

private:
	bool nextEnumerator(EsValue* valReturn, EsContext* ctx, u8 flagsIter);
	bool nextNative(EsValue* valReturn, EsContext* ctx);
	void closeNative(EsContext* ctx);

	static bool makeKeyValuePair(EsValue* valReturn, EsContext* ctx, const EsValue* valKey, const EsValue* valValue);

	//======================================================================
	// 変数
private:
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_ITERATOR_H_
