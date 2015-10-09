/***********************************************************************//**
 *	EsObjectSlots.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/06/29.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsObjectSlots.h"

// Friends
#include "EsClass.h"
#include "EsSysClasses.h"
#include "EsObject.h"
#include "EsScope.h"
#include "EsValue.h"

// External
#include "../../tfw/collection/InsArray.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsObjectSlots メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	スロット数を得る
**//*---------------------------------------------------------------------*/
s32 EsObjectSlots::getNum() const
{
	return (_arrSlot != 0L) ? _arrSlot->getCount() : 0;
}

/*---------------------------------------------------------------------*//**
	スロットの値を得る
**//*---------------------------------------------------------------------*/
const EsValue* EsObjectSlots::getValue(u16 slotidx) const
{
	if(_arrSlot == 0L)					{	return 0L;	}
	if(_arrSlot->getCount() <= slotidx)	{	return 0L;	}
	return _arrSlot->getPointer(slotidx);
}

/*---------------------------------------------------------------------*//**
	スロットの値を得る
**//*---------------------------------------------------------------------*/
EsValue* EsObjectSlots::value(u16 slotidx)
{
	if(_arrSlot == 0L)					{	return 0L;	}
	if(_arrSlot->getCount() <= slotidx)	{	return 0L;	}
	return _arrSlot->getPointer(slotidx);
}

/*---------------------------------------------------------------------*//**
	スロットの値を設定する
**//*---------------------------------------------------------------------*/
bool EsObjectSlots::setValue(u16 slotidx, const EsValue* val)
{
	if(_arrSlot == 0L)					{	return false;	}
	if(_arrSlot->getCount() <= slotidx)	{	return false;	}
	EsValue* v = _arrSlot->getPointer(slotidx);
	v->copy(val);
	return true;
}

/*---------------------------------------------------------------------*//**
	ダイナミック スロットの値を得る
**//*---------------------------------------------------------------------*/
const EsValue* EsObjectSlots::getDvalue(u16 dslotidx) const
{
	if(_arrSlot == 0L)						{	return 0L;	}
	dslotidx += getStartDynamicIndex();
	if(_arrSlot->getCount() <= dslotidx)	{	return 0L;	}
	return _arrSlot->getPointer(dslotidx);
}

/*---------------------------------------------------------------------*//**
	ダイナミック スロットの値を得る
**//*---------------------------------------------------------------------*/
EsValue* EsObjectSlots::dvalue(u16 dslotidx)
{
	if(_arrSlot == 0L)						{	return 0L;	}
	dslotidx += getStartDynamicIndex();
	if(_arrSlot->getCount() <= dslotidx)	{	return 0L;	}
	return _arrSlot->getPointer(dslotidx);
}

/*---------------------------------------------------------------------*//**
	ダイナミック スロットの値を設定する
**//*---------------------------------------------------------------------*/
bool EsObjectSlots::setDvalue(u16 dslotidx, const EsValue* val)
{
	if(_arrSlot == 0L)						{	return false;	}
	dslotidx += getStartDynamicIndex();
	if(_arrSlot->getCount() <= dslotidx)	{	return false;	}
	EsValue* v = _arrSlot->getPointer(dslotidx);
	v->copy(val);
	return true;
}

/*---------------------------------------------------------------------*//**
	予約スロットの値を得る ⇒ JS_GetReservedSlot
**//*---------------------------------------------------------------------*/
bool EsObjectSlots::getReservedSlotValue(EsValue* val, EsContext* ctx, u16 slotidx) const
{
	u16 numLimit = _objParentRef->getClass()->getDefaultReservedSlotNum();
	if(slotidx >= numLimit)
	{
		if(!isSafeReservedSlotIndex(slotidx, numLimit))
		{
			return false;
		}
	}
	slotidx += _objParentRef->getClass()->getStartSlotIndex();
	return _objParentRef->getRequiredSlotOv(val, ctx, slotidx);
}

/*---------------------------------------------------------------------*//**
	予約スロットの値を設定する ⇒ JS_SetReservedSlot
**//*---------------------------------------------------------------------*/
bool EsObjectSlots::setReservedSlotValue(EsContext* ctx, u16 slotidx, const EsValue* val)
{
	u16 numLimit = _objParentRef->getClass()->getDefaultReservedSlotNum();
	if(slotidx >= numLimit)
	{
		if(!isSafeReservedSlotIndex(slotidx, numLimit))
		{
			return false;
		}
	}
	slotidx += _objParentRef->getClass()->getStartSlotIndex();
	return _objParentRef->setRequiredSlotOv(ctx, slotidx, val);
}

/*---------------------------------------------------------------------*//**
	プロトタイプ オブジェクトを得る ⇒ STOBJ_GET_PROTO, OBJ_GET_PROTO
**//*---------------------------------------------------------------------*/
const EsObject* EsObjectSlots::getPrototypeObject() const
{
	const EsValue* v = getValue(getPrototypeIndex());
	if(v == 0L)	{	return 0L;	}
	return v->isVoid() ? 0L : v->getObject();
}

/*---------------------------------------------------------------------*//**
	プロトタイプ オブジェクトを得る ⇒ STOBJ_GET_PROTO, OBJ_GET_PROTO
**//*---------------------------------------------------------------------*/
EsObject* EsObjectSlots::prototypeObject()
{
	EsValue* v = value(getPrototypeIndex());
	if(v == 0L)	{	return 0L;	}
	return v->isVoid() ? 0L : v->getObject();
}

/*---------------------------------------------------------------------*//**
	プロトタイプオブジェクトを設定する ⇒ STOBJ_SET_PARENT, OBJ_SET_PROTO
**//*---------------------------------------------------------------------*/
void EsObjectSlots::setPrototypeObject(EsObject* obj)
{
	EsValue* v = value(getPrototypeIndex());
	if(v == 0L)	{	return;	}
	v->setValue(obj);
}

/*---------------------------------------------------------------------*//**
	親オブジェクトを得る ⇒ STOBJ_GET_PARENT, OBJ_GET_PARENT
**//*---------------------------------------------------------------------*/
const EsObject* EsObjectSlots::getParentObject() const
{
	const EsValue* v = getValue(getParentIndex());
	if(v == 0L)	{	return 0L;	}
	return v->isVoid() ? 0L : v->getObject();
}

/*---------------------------------------------------------------------*//**
	親オブジェクトを得る ⇒ STOBJ_GET_PARENT, OBJ_GET_PARENT
**//*---------------------------------------------------------------------*/
EsObject* EsObjectSlots::parentObject()
{
	EsValue* v = value(getParentIndex());
	if(v == 0L)	{	return 0L;	}
	return v->isVoid() ? 0L : v->getObject();
}

/*---------------------------------------------------------------------*//**
	親オブジェクトを設定する ⇒ STOBJ_SET_PARENT, STOBJ_CLEAR_PARENT
**//*---------------------------------------------------------------------*/
void EsObjectSlots::setParentObject(EsObject* obj)
{
	EsValue* v = value(getParentIndex());
	if(v == 0L)	{	return;	}
	v->setValue(obj);
}

/*---------------------------------------------------------------------*//**
	プライベートスロットのデータを得る
		⇒ STOBJ_GET_PRIVATE, OBJ_GET_PRIVATE, JS_GetPrivate
**//*---------------------------------------------------------------------*/
const void* EsObjectSlots::getPrivateData() const
{
	ASSERT(TFW_IS_FLAG(_objParentRef->getClass()->getFlags(), EsClass::F_HAS_PRIVATE));
	const EsValue* v = getValue(getPrivateIndex());
	if(v == 0L)			{	return 0L;	}
	if(!v->isInt64())	{	return 0L;	}
	return reinterpret_cast<const void*>(v->getInt64());
}

/*---------------------------------------------------------------------*//**
	プライベートスロットのデータを得る ⇒ STOBJ_GET_PRIVATE, OBJ_GET_PRIVATE
**//*---------------------------------------------------------------------*/
void* EsObjectSlots::privateData()
{
	ASSERT(TFW_IS_FLAG(_objParentRef->getClass()->getFlags(), EsClass::F_HAS_PRIVATE));
	EsValue* v = value(getPrivateIndex());
	if(v == 0L)	{	return 0L;	}
	return reinterpret_cast<void*>(v->getInt64());
}

/*---------------------------------------------------------------------*//**
	プライベートスロットにデータを設定 ⇒ JS_SetPrivate
**//*---------------------------------------------------------------------*/
void EsObjectSlots::setPrivateData(void* data)
{
	ASSERT(TFW_IS_FLAG(_objParentRef->getClass()->getFlags(), EsClass::F_HAS_PRIVATE));
	EsValue* v = value(getPrivateIndex());
	if(v == 0L)	{	return;	}
	v->setValue(reinterpret_cast<u64>(data));
}

/*---------------------------------------------------------------------*//**
	プライベートスロットの値を得る
**//*---------------------------------------------------------------------*/
const EsValue* EsObjectSlots::getPrivateValue() const
{
	ASSERT(TFW_IS_FLAG(_objParentRef->getClass()->getFlags(), EsClass::F_HAS_PRIVATE));
	const EsValue* v = getValue(getPrivateIndex());
	return v;
}

/*---------------------------------------------------------------------*//**
	プライベートスロットの値を得る
**//*---------------------------------------------------------------------*/
EsValue* EsObjectSlots::privateValue()
{
	ASSERT(TFW_IS_FLAG(_objParentRef->getClass()->getFlags(), EsClass::F_HAS_PRIVATE));
	EsValue* v = value(getPrivateIndex());
	return v;
}

/*---------------------------------------------------------------------*//**
	プライベートスロットに値を設定
**//*---------------------------------------------------------------------*/
void EsObjectSlots::setPrivateValue(const EsValue* val)
{
	ASSERT(TFW_IS_FLAG(_objParentRef->getClass()->getFlags(), EsClass::F_HAS_PRIVATE));
	EsValue* v = value(getPrivateIndex());
	if(v == 0L)	{	return;	}
	v->copy(val);
}

/*---------------------------------------------------------------------*//**
	ブロック深度を得る ⇒ OBJ_BLOCK_DEPTH
**//*---------------------------------------------------------------------*/
s32 EsObjectSlots::getBlockDepth() const
{
	const EsValue* v = getValue(_objParentRef->getClass()->getBlockDepthSlotIndex());
	if(v == 0L)		{	ASSERT(false);	return -1;	}
	if(!v->isInt())	{	ASSERT(false);	return -1;	}
	return v->getInt();
}

/*---------------------------------------------------------------------*//**
	ブロック深度を設定 ⇒ OBJ_SET_BLOCK_DEPTH
**//*---------------------------------------------------------------------*/
void EsObjectSlots::setBlockDepth(s32 depth)
{
	EsValue* v = value(_objParentRef->getClass()->getBlockDepthSlotIndex());
	if(v == 0L)		{	ASSERT(false);	return;		}
	v->setValue(depth);
}

/*---------------------------------------------------------------------*//**
	プロトタイプ オブジェクト スロットのインデックスを得る ⇒ JSSLOT_PROTO
**//*---------------------------------------------------------------------*/
u16 EsObjectSlots::getPrototypeIndex() const
{
	return _objParentRef->getClass()->getPrototypeSlotIndex();
}

/*---------------------------------------------------------------------*//**
	親オブジェクト スロットのインデックスを得る ⇒ JSSLOT_PARENT
**//*---------------------------------------------------------------------*/
u16 EsObjectSlots::getParentIndex() const
{
	return _objParentRef->getClass()->getParentSlotIndex();
}

/*---------------------------------------------------------------------*//**
	プライベート スロットのインデックスを得る ⇒ JSSLOT_PRIVATE
**//*---------------------------------------------------------------------*/
u16 EsObjectSlots::getPrivateIndex() const
{
	return _objParentRef->getClass()->getPrivateSlotIndex();
}

/*---------------------------------------------------------------------*//**
	ブロック深度スロットのインデックスを得る ⇒ JSSLOT_BLOCK_DEPTH
**//*---------------------------------------------------------------------*/
u16 EsObjectSlots::getBlockDepthIndex() const
{
	return _objParentRef->getClass()->getBlockDepthSlotIndex();
}

/*---------------------------------------------------------------------*//**
	開始スロットインデックスを得る ⇒ JSSLOT_START
**//*---------------------------------------------------------------------*/
u16 EsObjectSlots::getStartIndex() const
{
	return _objParentRef->getClass()->getStartSlotIndex();
}
/*---------------------------------------------------------------------*//**
	ダイナミックスロット開始インデックスを得る 【お試し予想仕様 r-kishi 2011/4/1】 ⇒ dslots
**//*---------------------------------------------------------------------*/
u16 EsObjectSlots::getStartDynamicIndex() const
{
	return _objParentRef->getClass()->getStartDynamicSlotIndex();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	スロットを作成 ⇒ js_AllocSlot
**//*---------------------------------------------------------------------*/
bool EsObjectSlots::create()
{
	EsScope* scope = _objParentRef->getScope();
	const EsClass* cls = _objParentRef->getClass();

	u16 numReservedSlot;
	if(cls->getAdditionalReservedSlotNum(&numReservedSlot, _objParentRef))
	{
		if(scope->getFreeSlotIndex() == cls->getFreeSlotIndex())
		{
			scope->setFreeSlotIndex(scope->getFreeSlotIndex() + numReservedSlot);
		}
	}

	if(scope->getFreeSlotIndex() >= getNum())
	{
		if(!recreate(scope->getFreeSlotIndex() + 1))
		{
			return false;
		}
	}

	ASSERT(getValue(scope->getFreeSlotIndex())->isVoid());
	scope->setFreeSlotIndex(scope->getFreeSlotIndex() + 1);
	return true;
}

/*---------------------------------------------------------------------*//**
	スロットを再作成 ⇒ js_ReallocSlots
**//*---------------------------------------------------------------------*/
bool EsObjectSlots::recreate(s32 numSlot)
{
	if(_arrSlot == 0L)	{	_arrSlot = new InsArray<EsValue>;	}
	if(_arrSlot == 0L)						{	return false;	}
	s32 numSlotOld = _arrSlot->getCount();
	if(numSlotOld >= numSlot)				{	return true;	}
	if(!_arrSlot->realloc(numSlot))			{	return false;	}

	for(int i = numSlotOld; i < numSlot; i++)
	{
		_arrSlot->setAt(i, EsValue());
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	スロットに値を追加
**//*---------------------------------------------------------------------*/
s32 EsObjectSlots::addValue(const EsValue* val)
{
	if(_arrSlot == 0L)	{	_arrSlot = new InsArray<EsValue>;	}
	if(_arrSlot == 0L)	{	return -1;	}

	_arrSlot->addArray(val, 1);
	return _arrSlot->getCount() - 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsObjectSlots::EsObjectSlots(EsObject* objParentRef)
	: _arrSlot(0L)
	, _objParentRef(objParentRef)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsObjectSlots::~EsObjectSlots()
{
	delete _arrSlot;
}

/*---------------------------------------------------------------------*//**
	予約スロットのインデックスが安全かを調べる ⇒ ReservedSlotIndexOK
**//*---------------------------------------------------------------------*/
bool EsObjectSlots::isSafeReservedSlotIndex(u16 slotidx, u16 numLimit) const
{
	u16 numAdd = 0;
	if(_objParentRef->getClass()->getAdditionalReservedSlotNum(&numAdd, _objParentRef))
	{
		numLimit += numAdd;	// ⇒ JS_LOCK_OBJ_VOID(cx, obj, limit += clasp->reserveSlots(cx, obj));
	}
	if(slotidx >= numLimit)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
