/***********************************************************************//**
 *	EsObject.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_OBJECT_H_
#define _ES_ES_OBJECT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "EsClass.h"
#include "EsPrototype.h"
#include "EsObjectSlots.h"
#include "EsUnit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Array;

TFW_END_NS

ES_BEGIN_NS

class EsAtom;
class EsClass;
class EsFunction;
class EsScope;
class EsScopeProperty;
class EsObjectOp;
class EsValue;

/*---------------------------------------------------------------------*//**
 *	ES オブジェクト
 *
**//*---------------------------------------------------------------------*/
class EsObject : public EsUnit
{
	//======================================================================
	// 定数
public:
	// オブジェクトフラグ
	static const u8 OBJF_DELEGATE	= 0x01;
	static const u8 OBJF_SYSTEM		= 0x02;

	// 列挙命令
	static const u8 ENUMOP_START	= 1;	// ⇒ JSENUMERATE_INIT
	static const u8 ENUMOP_NEXT		= 2;	// ⇒ JSENUMERATE_NEXT
	static const u8 ENUMOP_END		= 3;	// ⇒ JSENUMERATE_DESTROY

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	オブジェクト プロトタイプ クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsObjectPrototype : public EsPrototype
	{
		//==================================================================
		// メソッド
	public:
		bool initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const;
		static bool initObjectAndFunctionClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj);
		static const EsObjectPrototype* getInstance() { static EsObjectPrototype instance; return &instance; }

	private:
		EsObjectPrototype();
	};

	/*-----------------------------------------------------------------*//**
	 *	オブジェクト スクリプト クラス ⇒ js_ObjectClass
	 *
	**//*-----------------------------------------------------------------*/
	class EsObjectClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		static const EsObjectClass* getInstance() { static EsObjectClass instance; return &instance; }
		static EsObject* makeClassObject(EsContext* ctx, EsObject* objParent); 

	private:
		EsObjectClass();
		static bool callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callValueOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callHasOwnProperty(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callIsPrototypeOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callPropertyIsEnumerable(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callGetPrototypeOf(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	};

private:
	/*-----------------------------------------------------------------*//**
	 *	列挙子 ⇒ JSNativeEnumerator
	 *
	**//*-----------------------------------------------------------------*/
	class EsEnumerator : public EsUnit
	{
	public:
		Array<const EsValue*>* _arr;
		s32 _cursor;

		EsEnumerator(Array<const EsValue*>* arrEntr);
		~EsEnumerator();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// クラスを得る
	inline const EsClass* getClass() const { return _class; }
	// EsObject 型判定値を得る
	inline u8 getObjType() const { return _objt; }
	// スコープを得る ⇒ OBJ_SCOPE
	inline EsScope* getScope() const { return _scope; }
	// スロット配列を取得する
	inline const EsObjectSlots* getSlots() const { return _slots; }
	inline EsObjectSlots* slots() { return _slots; }
	// オブジェクト演算子セットを取得する
	inline const EsObjectOp* getObjectOp() const { return _oop; }
	// 呼び出しオブジェクトの関数を得る
	EsFunction* getCallObjectFunction() const;

	// ネイティブオブジェクトであるかを得る
	bool isNative() const;
	// 関数オブジェクトであるかを得る
	bool isFunction() const;
	// デリゲートオブジェクトであるかを得る
	inline bool isDelegateObject() const { return TFW_IS_FLAG(_objflags, OBJF_DELEGATE); }
	// システムオブジェクトであるかを得る
	inline bool isSystemObject() const { return TFW_IS_FLAG(_objflags, OBJF_SYSTEM); }

	// デリゲートオブジェクトに設定する
	inline void setDelegateObject(bool is) { TFW_SET_FLAG(_objflags, OBJF_DELEGATE, is); }
	// システムオブジェクトに設定する
	inline void setSystemObject(bool is) { TFW_SET_FLAG(_objflags, OBJF_SYSTEM, is); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool getMethod(EsValue* val, EsContext* ctx, const EsValue* id);
	bool getConstructor(EsObject** objCtor, EsContext* ctx);
	void* getPrivateData(EsContext* ctx, const EsClass* clsInstance, const EsValue* valInstance);
	bool tryCallMethod(EsValue* valReturn, EsContext* ctx, const EsValue* id, EsValue* vaStack, u32 numArg);
	bool tryCallValueOf(EsValue* valReturn, EsContext* ctx, EsLogicalType ltype);
	bool purgePrototypeChain(EsContext* ctx, const EsValue* spropid);
	bool purgeScopeChain(EsContext* ctx, const EsValue* spropid);
	EsScope* getMutableScope();
	void writeSlotValueLockedBarrier(u32 slotidx, const EsValue* val);
	void freeSlot(u16 slotidx);
	bool isPrototypeOf(const EsValue* valObj);
	bool checkPropertyIsEnumerable(EsValue* valOut, EsContext* ctx, const EsValue* id);

	bool addProperty(EsContext* ctx, const EsAtom* atom, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, const EsValue* value, s16 shortid, u8 atrbf, u8 optf);
	bool addProperty(EsContext* ctx, const VcString* name, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, const EsValue* value, s16 shortid, u8 atrbf, u8 optf);
	EsFunction* addFunction(EsContext* ctx, const EsAtom* atom, EsNativeCall call, u8 atrbf, EsExtParam paramDef);
	EsFunction* addFunction(EsContext* ctx, const VcString* name, EsNativeCall call, u8 atrbf, EsExtParam paramDef);

	bool makeClassObject(EsObject** objPtypeOut, EsObject** objCtorOut, EsContext* ctx, const EsClass* cls, EsObject* objParentPtype, EsNativeCall constructor);
	bool defineClassProperty(EsContext* ctx, const EsAtom* atom, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, s16 shortid, u8 atrbf);
	bool defineClassFunction(EsContext* ctx, const EsAtom* atom, EsNativeCall func, u8 atrbf);

	static bool getClassPrototype(EsObject** objPtypeOut, EsContext* ctx, EsObject* objScope, const EsValue* id);
	inline bool getClassPrototype(EsObject** objPtypeOut, EsContext* ctx, const EsValue* id) { return getClassPrototype(objPtypeOut, ctx, this, id); }
	static bool setClassPrototype(EsContext* ctx, EsObject* objCtor, EsObject* objPtype, u8 atrbf);
	inline bool setClassPrototype(EsContext* ctx, EsObject* objPtype, u8 atrbf) { return setClassPrototype(ctx, this, objPtype, atrbf); }
	static bool findClassObject(EsValue* valOut, EsContext* ctx, EsObject* objStart, const EsValue* id);
	inline bool findClassObject(EsValue* valOut, EsContext* ctx, const EsValue* id) { return findClassObject(valOut, ctx, this, id); }
	static bool getClassObject(EsObject** objClassOut, EsContext* ctx, EsObject* obj, u16 ptypekind);
	inline bool getClassObject(EsObject** objClassOut, EsContext* ctx, u16 ptypekind) { return getClassObject(objClassOut, ctx, this, ptypekind); }
	static bool setClassObject(EsContext* ctx, EsObject* obj, u16 ptypekind, EsObject* objClass);
	inline bool setClassObject(EsContext* ctx, u16 ptypekind, EsObject* objClass) { return setClassObject(ctx, this, ptypekind, objClass); }
	static bool callNativeMethodDispatcher(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// 自身のオブジェクト基本操作メソッド

	EsScopeProperty* definePropertyNative(EsContext* ctx, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf, u8 optf, s16 shortid, bool isDontPurge);
	bool callGetPropertyNative(EsValue* val, EsContext* ctx, EsObject* objScopeOwn, EsScopeProperty* sprop);
	bool callSetPropertyNative(EsContext* ctx, EsObject* objScopeOwn, EsScopeProperty* sprop, const EsValue* val);
	s32 findPropertyNative(EsScopeProperty** spropOut, EsObject** objScopeOwnerOut, EsContext* ctx, const EsValue* spropid, u8 flagsResolv);

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// ベース（非オーバーライド）のオブジェクト操作メソッド

	bool definePropertyBase(EsScopeProperty** sprop, EsContext* ctx, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf);
	bool deletePropertyBase(EsValue* val, EsContext* ctx, const EsValue* spropid);
	void dropPropertyBase(EsContext* ctx, EsScopeProperty* sprop);
	bool findPropertyBase(EsScopeProperty** sprop, EsObject** objScopeOwner, EsContext* ctx, const EsValue* spropid);
	bool callGetPropertyBase(EsValue* val, EsContext* ctx, const EsValue* spropid);
	bool callSetPropertyBase(EsContext* ctx, const EsValue* spropid, const EsValue* val);
	bool getPropertyAttributeFlagsBase(u8* spatrbf, EsContext* ctx, const EsValue* spropid, EsScopeProperty* sprop);
	bool convDefaultValueBase(EsContext* ctx, EsValue* val, EsLogicalType ltypeHint);
	bool checkAccessBase(u8* spatrbf, EsValue* val, EsContext* ctx, const EsValue* spropid, u8 acsflags);
	bool enumerateBase(EsValue* valState, EsValue* id, EsContext* ctx, u8 enumop);
	bool getRequiredSlotBase(EsValue* val, EsContext* ctx, s32 slotidx);
	bool setRequiredSlotBase(EsContext* ctx, s32 slotidx, const EsValue* val);
	bool callBase(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	bool constructBase(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	bool hasInstanceBase(EsValue* valRes, EsContext* ctx, EsObject* obj, const EsValue* valInst);
	EsObject* getThisObjectBase(EsContext* ctx);

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// オーバーライドされたオブジェクト操作メソッド

	bool definePropertyOv(EsScopeProperty** sprop, EsContext* ctx, const EsValue* spropid, const EsValue* value, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, u8 atrbf);
	bool deletePropertyOv(EsValue* val, EsContext* ctx, const EsValue* spropid);
	void dropPropertyOv(EsContext* ctx, EsScopeProperty* sprop);
	bool findPropertyOv(EsScopeProperty** sprop, EsObject** objScopeOwner, EsContext* ctx, const EsValue* spropid);
	bool callGetPropertyOv(EsValue* val, EsContext* ctx, const EsValue* spropid);
	bool callSetPropertyOv(EsContext* ctx, const EsValue* spropid, const EsValue* val);
	bool getPropertyAttributeFlagsOv(u8* spatrbf, EsContext* ctx, const EsValue* spropid, EsScopeProperty* sprop);
	bool convDefaultValueOv(EsContext* ctx, EsValue* val, EsLogicalType ltype);
	bool checkAccessOv(u8* spatrbf, EsValue* val, EsContext* ctx, const EsValue* spropid, u8 acsflags);
	bool enumerateOv(EsValue* valState, EsValue* id, EsContext* ctx, u8 enumop);
	bool getRequiredSlotOv(EsValue* val, EsContext* ctx, s32 slotidx);
	bool setRequiredSlotOv(EsContext* ctx, s32 slotidx, const EsValue* val);
	bool callOv(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	bool constructOv(EsContext* ctx, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
	bool hasInstanceOv(EsValue* valRes, EsContext* ctx, EsObject* obj, const EsValue* valInst);
	EsObject* getThisObjectOv(EsContext* ctx);

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// API

	bool getMethodById(EsValue* val, EsContext* ctx, const EsValue* id);
	bool getElement(EsValue* valOut, EsContext* ctx, s32 index);
	bool setElement(EsContext* ctx, s32 index, const EsValue* val);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsObject(const EsClass* cls, u8 objct);
	virtual ~EsObject();

	bool createScope(EsObjectOp* oop);

	//======================================================================
	// 変数
private:
	const EsClass* _class;
	EsScope* _scope;
	EsObjectOp* _oop;		// ⇒ map
	EsObjectSlots* _slots;
	u8 _objflags;			// ⇒ classword の下位２ビット
	u8 _objt;				// EsObject オブジェクトの型判定値 OBJT_*
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_OBJECT_H_
