/***********************************************************************//**
 *	EsSysClasses.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2010/06/01.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_SYS_CLASSES_H_
#define _ES_ES_SYS_CLASSES_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsClass.h"
#include "EsSysPrototypes.h"

#include "../tfw/collection/Array.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	固定クラス定義
 *
**//*---------------------------------------------------------------------*/
class EsSysClasses
{
	//======================================================================
	// 定数
public:
	// 固定クラスのタイプ
	enum FixedClassType
	{
		CLASSTYPE_NULL		= 0,
		CLASSTYPE_OBJECT,
		CLASSTYPE_FUNC,
		CLASSTYPE_CALL,
		CLASSTYPE_BLOCK,
		CLASSTYPE_WITH,
		CLASSTYPE_DECLENV,
		CLASSTYPE_NUMBER,
		CLASSTYPE_BOOLEAN,
		CLASSTYPE_STRING,
		CLASSTYPE_ARGUMENTS,
		CLASSTYPE_ARRAY,
		CLASSTYPE_ITERATOR,
		CLASSTYPE_SCRIPT,
		CLASSTYPE_GLOBAL,
		CLASSTYPE_MATH,
		CLASSTYPE__CUSTOM__,
	};

private:
	static const s32 NUM_FIXED_STANDARD_CLASSES = 7;
	static const FixedClassType FIXED_STANDARD_CLASSES[NUM_FIXED_STANDARD_CLASSES];

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	コール スクリプト クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsCallClass : public EsClass
	{
		//==================================================================
		// 定数
	private:
		static const u16 NUM_DEFAULT_RESERVED_SLOTS = 2;	// ⇒ CALL_CLASS_FIXED_RESERVED_SLOTS

		//==================================================================
		// メソッド
	public:
		// 呼び出し関数スロットのインデックスを得る ⇒ JSSLOT_CALLEE
		inline u16 getCalleeSlotIndex() const { return getPrivateSlotIndex() + 1; }

		virtual bool getAdditionalReservedSlotNum(u16* slotnumOut, EsObject* obj) const;

		static const EsCallClass* getInstance() { static EsCallClass instance; return &instance; }

	private:
		EsCallClass();
		static bool callEnumerate(EsContext* ctx, EsObject* obj);
		static bool funcGetArguments(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id);
		static bool funcSetArguments(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val);
		static bool funcGetArg(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id);
		static bool funcSetArg(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val);
		static bool funcGetVar(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id);
		static bool funcGetVarChecked(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id);
		static bool funcSetVar(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val);
		static bool callResolve(EsObject** objOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv);
		static bool callConvert(EsValue* val, EsContext* ctx, EsObject* obj, EsLogicalType ltype);
	};

	/*-----------------------------------------------------------------*//**
	 *	宣言型環境オブジェクト クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsDeclEnvClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		static const EsDeclEnvClass* getInstance() { static EsDeclEnvClass instance; return &instance; }

	private:
		EsDeclEnvClass();

	public:
		static bool callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id);
	};

	/*-----------------------------------------------------------------*//**
	 *	引数クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsArgumentsClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		static const EsArgumentsClass* getInstance() { static EsArgumentsClass instance; return &instance; }
		static bool enumerate(EsContext* ctx, EsObject* obj) { return callEnumerate(ctx, obj); }

	private:
		EsArgumentsClass();
		static bool callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id);
		static bool callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val);
		static bool callDelProperty(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val);
		static bool callEnumerate(EsContext* ctx, EsObject* obj);
		static bool callResolve(EsObject** objOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv);
	};

	/*-----------------------------------------------------------------*//**
	 *	スクリプト クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsScriptClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		static const EsScriptClass* getInstance() { static EsScriptClass instance; return &instance; }

	private:
		EsScriptClass();
	};

	/*-----------------------------------------------------------------*//**
	 *	グローバル スクリプト クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsGlobalClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		static const EsGlobalClass* getInstance() { static EsGlobalClass instance; return &instance; }

	private:
		EsGlobalClass();
		static EsObject* initEvalMethod(EsContext* ctx, EsObject* obj); 
		static bool callEval(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callEnumerate(EsContext* ctx, EsObject* obj);
		static bool callResolve(EsObject** objParentOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static const EsClass* getClass(u16 clstype);
	static inline u16 generateClassType() { return generateClassType(0L); }
	static u16 generateClassType(const EsAtom* atomName);
	static bool isNonGlobalScope(u16 clstype);
	static s32 getStandardClassNum();
	static const EsClass* getStandardClass(s32 index);
	static void registerExternalClass(const EsClass* cls, bool isStandardClass);

	//======================================================================
	// 変数
private:
	static Array<const EsClass*> _arrExtClasses;		// 拡張クラス配列
	static Array<const EsClass*> _arrExtStdClasses;		// 拡張標準クラス配列
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_SYS_CLASSES_H_
