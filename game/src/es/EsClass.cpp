/***********************************************************************//**
 *	EsClass.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/20.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsClass.h"

// Friends
#include "EsAtom.h"
#include "EsContext.h"
#include "EsSysPrototypes.h"
#include "EsDefaultObjectOp.h"
#include "EsObject.h"
#include "EsValue.h"

// External
#include "../tfw/lib/TypeUtils.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsClass メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	名称を得る
**//*---------------------------------------------------------------------*/
const VcString* EsClass::getName() const
{
	ASSERT(_atomName != 0L);
	if(_atomName == 0L)									{	return TypeUtils::getEmptyString();	}
	ASSERT(_atomName->getType() == EsAtom::ATYPE_STRING);
	if(_atomName->getType() != EsAtom::ATYPE_STRING)	{	return TypeUtils::getEmptyString();	}
	return _atomName->getString();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	追加の予約スロット数を得る
	追加がない場合は false を返す
	その場合デフォルト値（getDefaultReservedSlotNum() の値）をそのまま使う
	⇒ JSClass::reserveSlots, JSCLASS_NO_OPTIONAL_MEMBERS
**//*---------------------------------------------------------------------*/
bool EsClass::getAdditionalReservedSlotNum(u16* slotnumOut, EsObject* obj) const
{
	return false;
}

/*---------------------------------------------------------------------*//**
	オブジェクト演算子セットを得る
	⇒ JSClass::getObjectOps
**//*---------------------------------------------------------------------*/
EsObjectOp* EsClass::getObjectOp() const
{
	return new EsDefaultObjectOp();
}

/*---------------------------------------------------------------------*//**
	クラス ID を得る ⇒ js_GetClassId
**//*---------------------------------------------------------------------*/
bool EsClass::getClassId(EsValue* id, EsContext* ctx) const
{
	if(_ptypekind != EsSysPrototypes::PTYPEKIND_NULL)
	{
		id->setValue((s32)_ptypekind);
	}
	else if(TFW_IS_FLAG(_flags, F_IS_ANONYMOUS))
	{
		id->setValue((s32)EsSysPrototypes::PTYPEKIND_OBJECT);
	}
	else
	{
		id->setValue(_atomName);
	}
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsClass::EsClass()
{
	clear();
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsClass::EsClass(const EsAtom* atomName, u16 clstype, u16 flags, u16 ptypekind, u16 numDefaultReservedSlot)
{
	clear();
	_atomName = atomName;
	_clstype = clstype;
	_ptypekind = ptypekind;
	_flags = flags;
	_numDefaultRsvSlot = numDefaultReservedSlot;
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void EsClass::clear()
{
	_atomName = 0L;
	_clstype = 0;
	_ptypekind = EsSysPrototypes::PTYPEKIND_NULL;
	_flags = 0;
	_numDefaultRsvSlot = 0;
	_funcGetProp = g_funcDummyPropertyGetter;
	_funcSetProp = g_funcDummyPropertySetter;
	_funcAddProp = g_funcDummyPropertySetter;
	_funcEnumerate = g_funcDummyEnumerate;
	_funcEnumOutParam = g_funcDummyEnumerateOut;
	_funcResolv = g_funcDummyResolve;
	_funcConvert = convertDefaultFunc;
	_funcCheckAccess = 0L;
	_funcCall = 0L;
	_funcConstruct = 0L;
	_funcHasInst = 0L;
}

/*---------------------------------------------------------------------*//**
	デフォルトの変換関数 ⇒ JS_ConvertStub
**//*---------------------------------------------------------------------*/
bool EsClass::convertDefaultFunc(EsValue* val, EsContext* ctx, EsObject* obj, EsLogicalType ltype)
{
	return obj->tryCallValueOf(val, ctx, ltype);
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
