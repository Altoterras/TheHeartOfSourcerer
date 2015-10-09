/***********************************************************************//**
 *	EsStackRegister.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsStackRegister.h"

// Friends
#include "EsContext.h"
#include "EsObject.h"
#include "EsValue.h"
#include "EsProgramCounter.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsStackRegister メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	プログラムカウンタを進める
**//*---------------------------------------------------------------------*/
void EsStackRegister::advancePc(s32 count)
{
	_pc->advance(count);
}

/*---------------------------------------------------------------------*//**
	スタックポインタを進める
**//*---------------------------------------------------------------------*/
void EsStackRegister::advanceSp(s32 count)
{
	_idxSp += count;

	//ASSERT((0 <= _idxSp) && (_idxSp < _numSp)); 
	// _idxSp == _numSp ありうる？
	// ASSERT((0 <= _idxSp) && (_idxSp < _numSp)); 
	ASSERTM((0 <= _idxSp) && (_idxSp <= _numSp), "{EsStackRegister::advanceSp} (0 <= _idxSp) && (_idxSp <= _numSp)"); 
}

/*---------------------------------------------------------------------*//**
	値をプッシュする ⇒ PUSH
**//*---------------------------------------------------------------------*/
void EsStackRegister::push(const EsValue* val)
{
	*sp() = *val;
	advanceSp(1);
}

/*---------------------------------------------------------------------*//**
	値をポップする ⇒ POP
**//*---------------------------------------------------------------------*/
EsValue EsStackRegister::pop()
{
	advanceSp(-1);
	EsValue* val = sp();
	return *val;
}

/*---------------------------------------------------------------------*//**
	ブーリアン値をポップする ⇒ POP_BOOLEAN
**//*---------------------------------------------------------------------*/
bool EsStackRegister::popBoolean(EsValue* val)
{
	bool b;
	*val = fetchOperand(-1);
	if(val->isNull())
	{
		b = false;
	}
	else if(val->isBoolean())
	{
		b = val->getBoolean();
	}
	else
	{
		val->toBoolean(&b, _ctxRef);
	}
	advanceSp(-1);
	return b;
}

/*---------------------------------------------------------------------*//**
	スタックからオペランドを取得する ⇒ FETCH_OPND
**//*---------------------------------------------------------------------*/
EsValue EsStackRegister::fetchOperand(s32 spofs)
{
	ASSERT((0 <= (_idxSp + spofs)) && ((_idxSp + spofs) < _numSp)); 
	return *sp(spofs);
}

/*---------------------------------------------------------------------*//**
	スタックから s32 値を得る ⇒ FETCH_INT
**//*---------------------------------------------------------------------*/
s32 EsStackRegister::fetchS32(s32 spofs)
{
	EsValue* val = sp(spofs);

	s32 ival = 0;
	if(val->isInt())
	{
		ival = val->getInt();
	}
	else
	{
		if(!val->toInt32(&ival, _ctxRef))	// ⇒ i = js_ValueToECMAInt32(cx, &regs.sp[n])
		{
			ASSERT(false);
		}
	}

	return ival;
}

/*---------------------------------------------------------------------*//**
	スタックから s32 値を得る ⇒ FETCH_UINT
**//*---------------------------------------------------------------------*/
u32 EsStackRegister::fetchU32(s32 spofs)
{
	EsValue* val = sp(spofs);

	u32 ival = 0;
	if(val->isInt())
	{
		ival = (u32)val->getInt();
	}
	else
	{
		if(!val->toUint32(&ival, _ctxRef))	// ⇒ ui = js_ValueToECMAUint32(cx, &regs.sp[n])
		{
			ASSERT(false);
		}
	}

	return ival;
}

/*---------------------------------------------------------------------*//**
	スタックから f64 値を得る ⇒ FETCH_NUMBER
**//*---------------------------------------------------------------------*/
f64 EsStackRegister::fetchF64(s32 spofs)
{
	EsValue* val = sp(spofs);

	f64 fval = 0.0f;
	if(val->isInt())
	{
		fval = (f64)val->getInt();
	}
	else if(val->isDouble())
	{
		fval = val->getDouble();
	}
	else
	{
		val->toNumber(&fval, val, _ctxRef);
	}

	return fval;
}

/*---------------------------------------------------------------------*//**
	スタックからオブジェクトを取得する ⇒ FETCH_OBJECT
**//*---------------------------------------------------------------------*/
bool EsStackRegister::fetchObject(EsObject** objOut, EsValue* valOut, s32 spofs)
{
	EsObject* obj;
	if(valOut != 0L)
	{
		*valOut = fetchOperand(spofs);
		if(!convValueToObject(&obj, spofs, valOut))		{	return false;	}
	}
	else
	{
		EsValue valTmp = fetchOperand(spofs);
		if(!convValueToObject(&obj, spofs, &valTmp))	{	return false;	}
	}
	*objOut = obj;
	return true;
}

/*---------------------------------------------------------------------*//**
	スタックからアトムを取得する
**//*---------------------------------------------------------------------*/
bool EsStackRegister::fetchAtom(const EsAtom** atomOut, s32 spofs)
{
	EsValue val = fetchOperand(spofs);
	if(val.isAtom())
	{
		*atomOut = val.getAtom();
		return true;
	}
	else
	{
		EsValue id;
		if(!_ctxRef->convValueToStringId(&id, &val))	// ⇒ if (!js_InternNonIntElementId(cx, obj, idval_, &id))
		{
			return false;
		}
		ASSERT(id.isAtom());
		*atomOut = id.getAtom();
		return true;
	}
}

/*---------------------------------------------------------------------*//**
	スタックから ID を取得する ⇒ FETCH_ELEMENT_ID
**//*---------------------------------------------------------------------*/
bool EsStackRegister::fetchId(EsValue* id, s32 spofs)
{
	EsValue val = fetchOperand(spofs);
	if(val.isInt())
	{
		id->copy(&val);
		return true;
	}

	const EsAtom* atom;
	if(!fetchAtom(&atom, spofs))	{	return false;	}
	id->setValue(atom);
	return true;
}

/*---------------------------------------------------------------------*//**
	オペランドをスタックに格納する ⇒ STORE_OPND
**//*---------------------------------------------------------------------*/
void EsStackRegister::storeOperand(s32 spofs, const EsValue* val)
{
	ASSERT((0 <= (_idxSp + spofs)) && ((_idxSp + spofs) < _numSp)); 
	*sp(spofs) = *val;
}

/*---------------------------------------------------------------------*//**
	s32 値をスタックに格納する ⇒ STORE_INT
**//*---------------------------------------------------------------------*/
bool EsStackRegister::storeS32(s32 spofs, s32 ival)
{
	// 整数値として保存
	EsValue* val = sp(spofs);
	val->setValue(ival);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	u32 値をスタックに格納する ⇒ STORE_UINT
**//*---------------------------------------------------------------------*/
bool EsStackRegister::storeU32(s32 spofs, u32 ival)
{
	// 整数値として保存
	EsValue* val = sp(spofs);
	val->setValue((s32)ival);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	f64 値をスタックに格納する ⇒ STORE_NUMBER
**//*---------------------------------------------------------------------*/
bool EsStackRegister::storeF64(s32 spofs, f64 fval)
{
	EsValue* val = sp(spofs);
	val->setNumber(fval);
	
	return true;
}

/*---------------------------------------------------------------------*//**
	EsValue からオブジェクトを作成する ⇒ VALUE_TO_OBJECT
**//*---------------------------------------------------------------------*/
bool EsStackRegister::convValueToObject(EsObject** objOut, s32 offset, EsValue* val)
{
	if(!val->isPrimitive())
	{
		*objOut = val->getObject();
	}
	else
	{
		if(!_ctxRef->convValueToObject(objOut, val))	// ⇒ obj = js_ValueToNonNullObject(cx, v);
		{
			return false;
		}
		EsValue valOutObj(*objOut);
		storeOperand(offset, &valOutObj);	// ⇒ STORE_OPND(n, OBJECT_TO_JSVAL(obj));
	}
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsStackRegister::EsStackRegister(EsContext* ctx, EsProgramCounter* pc, EsValue* sp, s32 numSp, s32 idxSpInit)
	: _pc(pc)
	, _spTop(sp)
	, _numSp(numSp)
	, _idxSp(idxSpInit)
	, _ctxRef(ctx)
{
}

/*---------------------------------------------------------------------*//**
	コピーコンストラクタ
**//*---------------------------------------------------------------------*/
EsStackRegister::EsStackRegister(const EsStackRegister& src)
{
	copy(&src);
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void EsStackRegister::copy(const EsStackRegister* src)
{
	_pc = src->_pc;
	_spTop = src->_spTop;
	_numSp = src->_numSp;
	_idxSp = src->_idxSp;
	_ctxRef = src->_ctxRef;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
