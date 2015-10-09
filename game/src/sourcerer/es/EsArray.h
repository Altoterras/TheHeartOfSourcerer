/***********************************************************************//**
 *	EsArray.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/07/27.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_ARRAY_H_
#define _ES_ES_ARRAY_H_

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

template<class TYPE> class List;
class BitFlagArray;

TFW_END_NS

ES_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	イテレータ
 *
**//*---------------------------------------------------------------------*/
class EsArray : public EsObject
{
	//======================================================================
	// 定数
private:
	static const s32 INDEX_INTIDX_MAX	= 65530;

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	配列プロトタイプ クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsArrayPrototype : public EsPrototype
	{
		//==================================================================
		// メソッド
	public:
		bool initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const;
		static const EsArrayPrototype* getInstance() { static EsArrayPrototype instance; return &instance; }

	private:
		EsArrayPrototype();
	};

	/*-----------------------------------------------------------------*//**
	 *	配列スクリプト クラス ⇒ js_ArrayClass
	 *
	**//*-----------------------------------------------------------------*/
	class EsArrayClass : public EsClass
	{
		//==================================================================
		// メソッド
	public:
		// 配列容量数スロットのインデックスを得る ⇒ dslots[-1] 
		inline u16 getArrayCapacitySlotIndex() const { return getPrivateSlotIndex(); }
		// 配列長さスロットのインデックスを得る ⇒ JSSLOT_ARRAY_LENGTH
		inline u16 getArrayLengthSlotIndex() const { return getPrivateSlotIndex() + 1; }
		// 配列使用数スロットのインデックスを得る ⇒ JSSLOT_ARRAY_COUNT
		inline u16 getArrayCountSlotIndex() const { return getPrivateSlotIndex() + 2; }
		// ダイナミックスロットの開始インデックスを得る
		virtual u16 getStartDynamicSlotIndex() const { return getPrivateSlotIndex() + 3; }

		virtual EsObjectOp* getObjectOp() const;
		static const EsArrayClass* getInstance() { static EsArrayClass instance; return &instance; }
		static EsObject* makeClassObject(EsContext* ctx, EsObject* objParent); 

	private:
		EsArrayClass();
		static bool callConstructor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callAddPropertySlowArray(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val);

	public:
		static bool callLengthGetter(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id);
		static bool callLengthSetter(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val);
		static bool callToString(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callJoin(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callReverse(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callSort(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callPush(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callPop(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callShift(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callUnshift(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callSplice(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callConcat(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);
		static bool callSlice(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm);

	private:
		static bool joinSub(EsValue* valRet, EsContext* ctx, EsObject* obj, const VcString* strSeparator);
		static int compareStrings(const void* a, const void* b, void* param);
	};

	/*-----------------------------------------------------------------*//**
	 *	配列オブジェクト操作子 ⇒ js_ArrayObjectOps
	 *
	**//*-----------------------------------------------------------------*/
	class EsArrayObjectOp : public EsObjectOp
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

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		EsArrayObjectOp();
	};

	/*-----------------------------------------------------------------*//**
	 *	状態クラス
	 *
	**//*-----------------------------------------------------------------*/
	class EsArrayStat
	{
	public:
		BitFlagArray* _bfarr;
		s32 _index;
		s32 _capacity;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		EsArrayStat(s32 capacity);
		~EsArrayStat();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	s32 getIntIndexedCapacity() const;
	void setIntIndexedCapacity(s32 size);

	bool getElementForRunOp(EsValue* val, s32 index);
	bool setElementForRunOp(s32 index, const EsValue* val);
	bool isPrototypeHasIndexedPropeties() const;

	static bool isIntIndexedArray(const EsObject* obj);
	inline bool isIntIndexedArray() const { return isIntIndexedArray(this); }
	static bool getElement(EsValue* val, bool* isHole, EsContext* ctx, EsObject* obj, f64 index);
	inline bool getElement(EsValue* val, bool* isHole, EsContext* ctx, f64 index) { return getElement(val, isHole, ctx, this, index); }
	static bool setElement(EsContext* ctx, EsObject* obj, f64 index, const EsValue* val);
	inline bool setElement(EsContext* ctx, f64 index, const EsValue* val) { return setElement(ctx, this, index, val); }
	static bool deleteElement(EsContext* ctx, EsObject* obj, f64 index);
	inline bool deleteElement(EsContext* ctx, f64 index) { return deleteElement(ctx, this, index); }
	static bool getArrayLengthProperty(s32* lengthOut, EsContext* ctx, EsObject* obj);
	inline bool getArrayLengthProperty(s32* lengthOut, EsContext* ctx) { return getArrayLengthProperty(lengthOut, ctx, this); }
	static bool setArrayLengthProperty(EsContext* ctx, EsObject* obj, f64 length);
	inline bool setArrayLengthProperty(EsContext* ctx, f64 length) { return setArrayLengthProperty(ctx, this, length); }
	static bool getArrayLengthFromValue(s32* lengthOut, EsContext* ctx, const EsValue* val);
	static bool isArrayIndexId(s32* index, const EsValue* id);
	static bool enumerate(EsValue* valState, EsValue* id, EsContext* ctx, EsObject* obj, u8 enumop);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsArray();
	~EsArray();
	bool create(EsContext* ctx, EsValue* valarr, s32 lenArr);

	static bool setArrayElements(EsContext* ctx, EsObject* obj, EsValue* valarr, s32 indexStart, s32 count, bool isAllHoles);
	static bool pushForNoIntIndexedArray(EsContext* ctx, EsObject* obj, EsValue* valarr);

	static s32 getLengthValueFromSlot(const EsObject* obj);
	inline s32 getLengthValueFromSlot() const { return getLengthValueFromSlot(this); }
	static s32 getLengthValueFromSlot(EsValue** valLength, EsObject* obj);
	inline s32 getLengthValueFromSlot(EsValue** valLength) { return getLengthValueFromSlot(valLength, this); }
	static s32 getCountValueFromSlot(const EsObject* obj);
	inline s32 getCountValueFromSlot() const { return getCountValueFromSlot(this); }
	static s32 getCountValueFromSlot(EsValue** valCount, EsObject* obj);
	inline s32 getCountValueFromSlot(EsValue** valCount) { return getCountValueFromSlot(valCount, this); }
	bool isIntIndexedArrayIndex(EsContext* ctx, const EsValue* id) const;

private:
	bool changeNoIntIndexedArray(EsContext* ctx);
	bool ensureCapacity(s32 size);
	bool resizeSlots(s32 size);

	//======================================================================
	// 変数
private:
	bool _isIntIndexed;		// 整数インデックスからダイナミックスロットにアクセスする配列 ⇒ OBJ_IS_DENSE_ARRAY
	List<EsArrayStat*>* _listPreventLeak;	// EsArrayStat のメモリリーク防止リスト
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_ARRAY_H_
