/***********************************************************************//**
 *	EsValue.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsValue.h"

// Friends
#include "EsAtom.h"
#include "EsContext.h"
#include "EsError.h"
#include "EsSysClasses.h"
#include "EsFunction.h"
#include "EsIterator.h"
#include "EsKeywordSet.h"
#include "EsOpcodeType.h"
#include "EsObject.h"
#include "EsObjectOp.h"
#include "EsRunEnv.h"
#include "EsStackFrame.h"

// External
#include "../../tfw/lib/TypeUtils.h"
#include "../../tfw/string/StringUtils.h"

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 静的関数

/*---------------------------------------------------------------------*//**
	比較 ⇒ EQUALITY_OP
**//*---------------------------------------------------------------------*/
static bool compare(EsContext* ctx, const EsValue* valL, const EsValue* valR, EsOpcodeType ot)
{
	EsValue::EsValueType typeL = valL->getSubstantiveType();
	EsValue::EsValueType typeR = valR->getSubstantiveType();

	const s32 RES_UNKNOW = 2;
	s32 res = RES_UNKNOW;

	if(typeL == typeR)
	{
		if(typeL == EsValue::VTYPE_STRING)
		{
			const VcString* strL = valL->getString();
			const VcString* strR = valR->getString();
			s32 is = (s32)strL->equals(strR);
			switch(ot)
			{
			case OP_EQ:	res = is == 1;	break;
			case OP_NE:	res = is != 1;	break;
			case OP_LT:	res = is <  1;	break;
			case OP_LE:	res = is <= 1;	break;
			case OP_GT:	res = is >  1;	break;
			case OP_GE:	res = is >= 1;	break;
			}

		}
		else if(typeL == EsValue::VTYPE_DOUBLE)
		{
			switch(ot)
			{
			case OP_EQ:	res = valL->getDouble() == valR->getDouble();	break;
			case OP_NE:	res = valL->getDouble() != valR->getDouble();	break;
			case OP_LT:	res = valL->getDouble() <  valR->getDouble();	break;
			case OP_LE:	res = valL->getDouble() <= valR->getDouble();	break;
			case OP_GT:	res = valL->getDouble() >  valR->getDouble();	break;
			case OP_GE:	res = valL->getDouble() >= valR->getDouble();	break;
			}
		}
		else
		{
			#if 1
				u32	nHiL = valL->getHiInt();
				s32 nLoL = valL->getInt();
				u32	nHiR = valR->getHiInt();
				s32 nLoR = valR->getInt();
				switch(ot)
				{
				case OP_EQ:
					res = (nHiL == nHiR) && (nLoL == nLoR);
					break;
				case OP_NE:
					res = (nHiL != nHiR) || (nLoL != nLoR);
					break;
				case OP_LT:
					if(nHiL < nHiR)			{	res = true;		}
					else if(nHiL > nHiR)	{	res = false;	}
					else if(nLoL < nLoR)	{	res = true;		}
					else					{	res = false;	}
					break;
				case OP_LE:
					if(nHiL < nHiR)			{	res = true;		}
					else if(nHiL > nHiR)	{	res = false;	}
					else if(nLoL <= nLoR)	{	res = true;		}
					else					{	res = false;	}
					break;
				case OP_GT:
					if(nHiL > nHiR)			{	res = true;		}
					else if(nHiL < nHiR)	{	res = false;	}
					else if(nLoL > nLoR)	{	res = true;		}
					else					{	res = false;	}
					break;
				case OP_GE:
					if(nHiL > nHiR)			{	res = true;		}
					else if(nHiL < nHiR)	{	res = false;	}
					else if(nLoL >= nLoR)	{	res = true;		}
					else					{	res = false;	}
					break;
				}
			#else
				__int64 nL = valL->getInt() | ((__int64)valL->getHiInt() << 32);
				__int64 nR = valR->getInt() | ((__int64)valR->getHiInt() << 32);

				switch(ot)
				{
				case OP_EQ:	res = (nL == nR);	break;
				case OP_NE:	res = (nL != nR);	break;
				case OP_LT:	res = (nL <  nR);	break;
				case OP_LE:	res = (nL <= nR);	break;
				case OP_GT:	res = (nL >  nR);	break;
				case OP_GE:	res = (nL >= nR);	break;
				}
			#endif
		}
	}
	else
	{
		if((typeL == EsValue::VTYPE_NULL) || (typeL == EsValue::VTYPE_VOID))
		{
			switch(ot)
			{
			case OP_EQ:	return ((typeR == EsValue::VTYPE_NULL) || (typeR == EsValue::VTYPE_VOID));
			case OP_NE:	return !((typeR == EsValue::VTYPE_NULL) || (typeR == EsValue::VTYPE_VOID));
			default:	return false;
			}
		}
		if((typeR == EsValue::VTYPE_NULL) || (typeR == EsValue::VTYPE_VOID))
		{
			switch(ot)
			{
			case OP_EQ:	return false;
			case OP_NE:	return true;
			default:	return false;
			}
		}

		EsValue valWk;
		if(typeL == EsValue::VTYPE_OBJ)
		{
			valL->getObject()->convDefaultValueOv(ctx, &valWk, LTYPE_VOID);
			typeL = valWk.getSubstantiveType();
			valL = &valWk;
		}
		else if(typeR == EsValue::VTYPE_OBJ)
		{
			static EsValue valWk;
			valR->getObject()->convDefaultValueOv(ctx, &valWk, LTYPE_VOID);
			typeR = valWk.getSubstantiveType();
			valR = &valWk;
		}

		if((typeL == EsValue::VTYPE_STRING) || (typeR == EsValue::VTYPE_STRING))
		{
			const VcString* strL = valL->getString();	ASSERT(strL != 0L);
			const VcString* strR = valR->getString();	ASSERT(strR != 0L);
			switch(ot)
			{
			case OP_EQ:	return strL->equals(strR);	// ⇒ cond = js_EqualStrings(str, str2) OP JS_TRUE;
			case OP_NE:	return !strL->equals(strR);
			default:	return false;
			}
		}

		f64 fvalL = 0.0f;
		f64 fvalR = 0.0f;
		switch(typeL)
		{
		case EsValue::VTYPE_INT:		fvalL = (f64)valL->getInt();	break;
		case EsValue::VTYPE_DOUBLE:		fvalL = valL->getDouble();		break;
		case EsValue::VTYPE_BOOLEAN:	fvalL = valL->getBoolean();		break;
		}
		switch(typeR)
		{
		case EsValue::VTYPE_INT:		fvalR = (f64)valR->getInt();	break;
		case EsValue::VTYPE_DOUBLE:		fvalR = valR->getDouble();		break;
		case EsValue::VTYPE_BOOLEAN:	fvalR = valR->getBoolean();		break;
		}
		switch(ot)
		{
		case OP_EQ:	res = fvalL == fvalR;	break;
		case OP_NE:	res = fvalL != fvalR;	break;
		case OP_LT:	res = fvalL <  fvalR;	break;
		case OP_LE:	res = fvalL <= fvalR;	break;
		case OP_GT:	res = fvalL >  fvalR;	break;
		case OP_GE:	res = fvalL >= fvalR;	break;
		}
	}

	return (res == 1);
}

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	実質的なタイプを得る
**//*---------------------------------------------------------------------*/
EsValue::EsValueType EsValue::getSubstantiveType() const
{
	if(_type == VTYPE_ATOM)
	{
		return VTYPE_STRING;
	}
	return (EsValueType)_type;
}

/*---------------------------------------------------------------------*//**
	浮動小数点数値を得る
**//*---------------------------------------------------------------------*/
f64 EsValue::getDouble() const 
{
	ASSERT(isDouble()); 
	if(_type == VTYPE_DOUBLE)
	{
		return _val._d;
	}
	else if(_type == VTYPE_ATOM)
	{
		if((_val._a != 0L) && (_val._a->getType() == EsAtom::ATYPE_DOUBLE))
		{
			return _val._a->getDouble()->_v;
		}
	}
	return 0.0;
}

/*---------------------------------------------------------------------*//**
	数値を得る
**//*---------------------------------------------------------------------*/
f64 EsValue::getNumber() const
{
	if(_type == VTYPE_INT)
	{
		return (f64)getInt();
	}
	return getDouble();
}

/*---------------------------------------------------------------------*//**
	文字列値を得る ⇒ JSVAL_TO_STRING
**//*---------------------------------------------------------------------*/
const VcString* EsValue::getString() const
{
	ASSERT(isString());
	if(_type == VTYPE_STRING)
	{
		return _val._s;
	}
	else if(_type == VTYPE_ATOM)
	{
		if(_val._a->getType() == EsAtom::ATYPE_STRING)
		{
			return _val._a->getString();
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	浮動小数点数型であるかどうかを得る ⇒ JSVAL_IS_DOUBLE
**//*---------------------------------------------------------------------*/
bool EsValue::isDouble() const
{
	if(_type == VTYPE_DOUBLE)
	{
		return true;
	}
	else if(_type == VTYPE_ATOM)
	{
		if((_val._a != 0L) && (_val._a->getType() == EsAtom::ATYPE_DOUBLE))
		{
			return true;
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	文字列型であるかどうかを得る ⇒ JSVAL_IS_STRING
**//*---------------------------------------------------------------------*/
bool EsValue::isString() const
{
	if(_type == VTYPE_STRING)
	{
		return true;
	}
	else if(_type == VTYPE_ATOM)
	{
		if((_val._a != 0L) && (_val._a->getType() == EsAtom::ATYPE_STRING))
		{
			return true;
		}
	}
	return false;
}

/*---------------------------------------------------------------------*//**
	関数オブジェクトであるかどうかを得る ⇒ VALUE_IS_FUNCTION
**//*---------------------------------------------------------------------*/
bool EsValue::isFunctionObject() const
{
	if(_type != VTYPE_OBJ)	{	return false;	}
	const EsObject* obj = getObject();
	return obj->isFunction();
}

/*---------------------------------------------------------------------*//**
	void 値を設定する ⇒ JSVAL_VOID
**//*---------------------------------------------------------------------*/
void EsValue::setVoid()
{
	_type = VTYPE_VOID;
	_val._i._lo = 0;
	_val._i._hi = 0;
}

/*---------------------------------------------------------------------*//**
	NULL を設定する ⇒ JSVAL_NULL
**//*---------------------------------------------------------------------*/
void EsValue::setNull()
{
	_type = VTYPE_NULL;
	_val._i._lo = 0;
	_val._i._hi = 0;
}

/*---------------------------------------------------------------------*//**
	ホール値を設定する ⇒ JSVAL_HOLE
**//*---------------------------------------------------------------------*/
void EsValue::setHole()
{
	_type = VTYPE_VOID;
	_val._i._lo = 0;
	_val._i._hi = HI_HOLE;
}

/*---------------------------------------------------------------------*//**
	実行未終了値を設定する
**//*---------------------------------------------------------------------*/
void EsValue::setUnfinished()
{
	_type = VTYPE_VOID;
	_val._i._lo = 0;
	_val._i._hi = HI_UNFINISHED;
}

/*---------------------------------------------------------------------*//**
	オブジェクトを設定する ⇒ OBJECT_TO_JSVAL
**//*---------------------------------------------------------------------*/
void EsValue::setValue(EsObject* obj)
{
	_type = VTYPE_OBJ;
	_val._i._hi = 0;
	_val._o = obj;
}

/*---------------------------------------------------------------------*//**
	整数値を設定する
**//*---------------------------------------------------------------------*/
void EsValue::setValue(s32 val)
{
	_type = VTYPE_INT;
	_val._i._lo = val;
	_val._i._hi = 0;
}

/*---------------------------------------------------------------------*//**
	整数値を設定する
**//*---------------------------------------------------------------------*/
void EsValue::setValue(s32 ivalLo, s32 ivalHi)
{
	_type = VTYPE_INT;
	_val._i._lo = ivalLo;
	_val._i._hi = ivalHi;
}

/*---------------------------------------------------------------------*//**
	整数値を設定する
**//*---------------------------------------------------------------------*/
void EsValue::setValue(u64 val)
{
	_type = VTYPE_INT64;
	_val._u = val;
}

/*---------------------------------------------------------------------*//**
	ブーリアン値を設定する
**//*---------------------------------------------------------------------*/
void EsValue::setValue(bool b)
{
	_type = VTYPE_BOOLEAN;
	_val._i._hi = 0;
	_val._i._lo = (s32)b;
}

/*---------------------------------------------------------------------*//**
	浮動小数点数値を設定する
**//*---------------------------------------------------------------------*/
void EsValue::setValue(f64 fval)
{
	_type = VTYPE_DOUBLE;
	_val._d = fval;
}

/*---------------------------------------------------------------------*//**
	文字列を設定する
**//*---------------------------------------------------------------------*/
void EsValue::setValue(const VcString* str)
{
	_type = VTYPE_STRING;
	_val._i._lo = 0;
	_val._s = str;
}

/*---------------------------------------------------------------------*//**
	アトム値を設定する
**//*---------------------------------------------------------------------*/
void EsValue::setValue(const EsAtom* atom)
{
	_type = VTYPE_ATOM;
	_val._i._lo = 0;
	_val._a = atom;
}

/*---------------------------------------------------------------------*//**
	プライベートデータを設定する
**//*---------------------------------------------------------------------*/
void EsValue::setPrivateData(void* p)
{
	_type = VTYPE_PRIVATE;
	_val._i._lo = 0;
	_val._p = p;
}

/*---------------------------------------------------------------------*//**
	エラーコードを設定する
**//*---------------------------------------------------------------------*/
void EsValue::setErrorCode(s32 codeError, s32 codeInternal)
{
	_type = VTYPE_ERRORCODE;
	_val._i._lo = codeError;
	_val._i._hi = codeInternal;
}

/*---------------------------------------------------------------------*//**
	型を設定する
**//*---------------------------------------------------------------------*/
void EsValue::setType(EsValueType type)
{
	_type = type;
	_val._i._lo = 0;
	_val._i._hi = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	==
**//*---------------------------------------------------------------------*/
bool EsValue::compareEq(EsContext* ctx, const EsValue* valR) const
{
	return compare(ctx, this, valR, OP_EQ);
}

/*---------------------------------------------------------------------*//**
	!=
**//*---------------------------------------------------------------------*/
bool EsValue::compareNe(EsContext* ctx, const EsValue* valR) const
{
	return compare(ctx, this, valR, OP_NE);
}

/*---------------------------------------------------------------------*//**
	<
**//*---------------------------------------------------------------------*/
bool EsValue::compareLt(EsContext* ctx, const EsValue* valR) const
{
	return compare(ctx, this, valR, OP_LT);
}

/*---------------------------------------------------------------------*//**
	<=
**//*---------------------------------------------------------------------*/
bool EsValue::compareLe(EsContext* ctx, const EsValue* valR) const
{
	return compare(ctx, this, valR, OP_LE);
}

/*---------------------------------------------------------------------*//**
	>
**//*---------------------------------------------------------------------*/
bool EsValue::compareGt(EsContext* ctx, const EsValue* valR) const
{
	return compare(ctx, this, valR, OP_GT);
}

/*---------------------------------------------------------------------*//**
	>=
**//*---------------------------------------------------------------------*/
bool EsValue::compareGe(EsContext* ctx, const EsValue* valR) const
{
	return compare(ctx, this, valR, OP_GE);
}

/*---------------------------------------------------------------------*//**
	同一判定
**//*---------------------------------------------------------------------*/
bool EsValue::equals(const EsValue* valR) const
{
	return
		(this->getType() == valR->getType()) &&
		(this->_val._i._hi == valR->_val._i._hi) &&
		(this->_val._i._lo == valR->_val._i._lo);
}

/*---------------------------------------------------------------------*//**
	厳密な同一判定 ⇒ js_StrictlyEqual
**//*---------------------------------------------------------------------*/
bool EsValue::equalsStrict(const EsValue* valR) const
{
	const EsValue* valL = this;
	EsValue::EsValueType typeL = valL->getSubstantiveType();
	EsValue::EsValueType typeR = valR->getSubstantiveType();

	const s32 RES_UNKNOW = 2;
	s32 res = RES_UNKNOW;

	if(typeL == typeR)
	{
		if(typeL == EsValue::VTYPE_STRING)
		{
			const VcString* strL = valL->getString();
			const VcString* strR = valR->getString();
			return strL->equals(strR);
		}
		else if(typeL == EsValue::VTYPE_DOUBLE)
		{
			f64 fvalL = valL->getDouble();
			f64 fvalR = valR->getDouble();
			return TFW_F64_IS_EQUAL(fvalL, fvalR);
		}
		else if(typeL == EsValue::VTYPE_OBJ)
		{
			// ※※※ 実装不要 ※※※ ⇒ js_GetWrappedObject, JSCLASS_IS_EXTENDED
			return valL->getObject() == valR->getObject();
		}
		else
		{
			return (valL->getHiInt() == valR->getHiInt()) && (valL->getInt() == valR->getInt());
		}
	}
	else if((typeL == EsValue::VTYPE_DOUBLE) && (typeR == EsValue::VTYPE_INT))
	{
		f64 fvalL = valL->getDouble();
		f64 fvalR = (f64)valR->getInt();
		return TFW_F64_IS_EQUAL(fvalL, fvalR);
	}
	else if((typeL == EsValue::VTYPE_INT) && (typeR == EsValue::VTYPE_DOUBLE))
	{
		f64 fvalL = (f64)valL->getInt();
		f64 fvalR = valR->getDouble();
		return TFW_F64_IS_EQUAL(fvalL, fvalR);
	}

	return valL->equals(valR);
}

/*---------------------------------------------------------------------*//**
	インクリメント（１加算）／デクリメント（１減算）⇒ js_DoIncDec

	@param valRes 加算／減算の結果値が後置（x++/x--）の場合でも入る
	@param isDecrement デクリメント（１減算）
	@param isPost 後置インクリメント／デクリメント（x++/x--）
**//*---------------------------------------------------------------------*/
bool EsValue::incdec(EsValue* valRes, EsContext* ctx, bool isDecrement, bool isPost)
{
	f64 fval;
	if(isDouble())
	{
		fval = getDouble();
	}
	else if(isInt())
	{
		fval = getInt();
	}
	else
	{
		EsValue valNumber;
		toNumber(&fval, &valNumber, ctx);
		if(valNumber.isNull())
		{
			return false;
		}
		ASSERT(valNumber.isNumber() || (valNumber.isBoolean() && (valNumber.getBoolean() == true)));

		if(isPost && (valNumber.isBoolean() && (valNumber.getBoolean() == true)))
		{
			setNumber(fval);
		}
	}

	// 計算処理
	isDecrement ? fval-- : fval++;

	// 結果を反映
	valRes->setNumber(fval);

	// 前置の場合は自身の値も変更する
	if(!isPost)
	{
		copy(valRes);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	倍精度浮動小数点化と数値取得 ⇒ js_ValueToNumber
**//*---------------------------------------------------------------------*/
bool EsValue::toNumber(f64* fval, EsValue* valConv, EsContext* ctx) const
{
	EsValue valWk(*this);
	do
	{
		if(valWk.isDouble())
		{
			if(valConv != 0L) { valConv->copy(&valWk); }
			if(fval != 0L)
			{
				*fval = valWk.getDouble();
			}
			return true;
		}
		if(valWk.isInt())
		{
			if(valConv != 0L) { valConv->copy(&valWk); }
			if(fval != 0L)
			{
				*fval = (f64)valWk.getInt();
			}
			return true;
		}
		if(valWk.isString())
		{
			if(valConv != 0L) { valConv->setValue(true); }
			if(fval != 0L)
			{
				const VcString* str = valWk.getString();
				s32 index;
				ASSERT(ctx != 0L);
				*fval = StringUtils::toDouble(&index, str, ctx->getKeywordSet()->getInfinity()->getName());
				if(StringUtils::skipSpace(str, index) != str->getLength())
				{
					VcString strWk(str->substring(index));
					index = 0;
					*fval = StringUtils::toInteger(&index, &strWk);
					if(StringUtils::skipSpace(&strWk, index) != strWk.getLength())
					{
						break;	// エラー
					}
				}
			}
			return true;
		}
		if(valWk.isBoolean())
		{
			if(valWk.getBoolean())
			{
				if(valConv != 0L) { valConv->setValue(1);	}
				if(fval != 0L) { *fval = 1.0; }
				return true;
			}
			else
			{
				if(valConv != 0L) { valConv->setValue(0);	}
				if(fval != 0L) { *fval = 0.0; }
				return true;
			}
		}
		if(valWk.isVoid())
		{
			break;
		}

		ASSERT(isObject());
		ASSERT(ctx != 0L);
		EsObject* obj = getObject();
		if(!obj->convDefaultValueOv(ctx, &valWk, LTYPE_NUMBER))
		{
			if(valConv != 0L) { valConv->setNull();	}
			if(fval != 0L) { *fval = 0.0; }
			return false;
		}
		if(valConv != 0L) { valConv->copy(&valWk); }

		if(!valWk.isPrimitive())
		{
			break;
		}
	}
	while(false);

	// 非数を返す
	if(valConv != 0L) { valConv->setValue(TypeUtils::getF64NaN());	}
	if(fval != 0L) { *fval = TypeUtils::getF64NaN(); }
	return true;
}

/*---------------------------------------------------------------------*//**
	固定文字列化 ⇒ js_ValueToString, JS_ValueToString
**//*---------------------------------------------------------------------*/
bool EsValue::toString(const VcString** cstr, EsContext* ctx) const
{
	EsValue valWk(*this);
	if(isObject())
	{
		EsObject* obj = getObject();
		if(obj == 0L)
		{
			*cstr = EsKeywordSet::getStatic(EsKeywordSet::S_TYPE_NULL)->getAtomString();	// ⇒ ATOM_TO_STRING(cx->runtime->atomState.nullAtom)
			return true;
		}
		if(!obj->convDefaultValueOv(ctx, &valWk, LTYPE_STRING))
		{
			return false;
		}
	}

	if(valWk.isString())
	{
		*cstr = valWk.getString();	// ⇒ JSVAL_TO_STRING
		return true;
	}
	if(valWk.isInt())
	{
		VcString strWk;
		*cstr = ctx->newString(VcString::format(&strWk, "%f", valWk.getInt()));		// ⇒ js_NumberToString(cx, JSVAL_TO_INT(v))
		return true;
	}
	if(valWk.isDouble())
	{
		VcString strWk;
		*cstr = ctx->newString(VcString::format(&strWk, "%f", valWk.getDouble()));	// ⇒ js_NumberToString(cx, *JSVAL_TO_DOUBLE(v))
		return true;
	}
	if(valWk.isBoolean())
	{
		*cstr = valWk.getBoolean() ? EsKeywordSet::getStatic(EsKeywordSet::S_VALUE_TRUE)->getAtomString() : EsKeywordSet::getStatic(EsKeywordSet::S_VALUE_FALSE)->getAtomString();	// ⇒ js_BooleanToString(cx, JSVAL_TO_BOOLEAN(v))
		return true;
	}

	*cstr = ctx->getKeywordSet()->getLogicalType(LTYPE_VOID)->getName();	// ⇒ str = ATOM_TO_STRING(cx->runtime->atomState.typeAtoms[JSTYPE_VOID])
	return true;
}

/*---------------------------------------------------------------------*//**
	文字列化
**//*---------------------------------------------------------------------*/
bool EsValue::toString(VcString* str, EsContext* ctx) const
{
	EsValue valWk(*this);
	if(isObject())
	{
		EsObject* obj = getObject();
		if(obj == 0L)
		{
			str->copy(EsKeywordSet::getStatic(EsKeywordSet::S_TYPE_NULL)->getAtomString());
			return true;
		}
		if(!obj->convDefaultValueOv(ctx, &valWk, LTYPE_STRING))
		{
			return false;
		}
	}

	if(valWk.isString())
	{
		str->copy(valWk.getString());
		return true;
	}
	if(valWk.isInt())
	{
		VcString strWk;
		str->copy(ctx->newString(VcString::format(&strWk, "%d", valWk.getInt())));
		return true;
	}
	if(valWk.isDouble())
	{
		VcString strWk;
		str->copy(ctx->newString(VcString::format(&strWk, "%f", valWk.getDouble())));
		return true;
	}
	if(valWk.isBoolean())
	{
		str->copy(valWk.getBoolean() ? EsKeywordSet::getStatic(EsKeywordSet::S_VALUE_TRUE)->getAtomString() : EsKeywordSet::getStatic(EsKeywordSet::S_VALUE_FALSE)->getAtomString());
		return true;
	}

	str->copy(ctx->getKeywordSet()->getLogicalType(LTYPE_VOID)->getName());
	return true;
}

/*---------------------------------------------------------------------*//**
	真偽値化 ⇒ js_ValueToBoolean
**//*---------------------------------------------------------------------*/
bool EsValue::toBoolean(bool* bval, EsContext* ctx) const
{
	if(isNull() || isVoid())
	{
		*bval = false;
		return true;
	}
	else if(isObject())
	{
		*bval = true;
		return true;
	}
	else if(isString())
	{
		const VcString* str = getString();
		*bval = (str->getLength() > 0);
		return true;
	}
	else if(isInt())
	{
		*bval = (getInt() != 0);
		return true;
	}
	else if(isDouble())
	{
		f64 d = getDouble();
		*bval = !TFW_F64_IS_NAN(d) && (d != 0);
		return true;
	}
	else if(isBoolean())
	{
		*bval = getBoolean();
		return true;
	}
	ASSERT(false);
	return false;
}

/*---------------------------------------------------------------------*//**
	整数化 ⇒ js_ValueToECMAInt32
**//*---------------------------------------------------------------------*/
bool EsValue::toInt32(s32* ival, EsContext* ctx) const
{
	if(isInt())
	{
		s32 iwk = getInt();
		*ival = iwk;
		return true;
	}

	f64 fval;
	if(isDouble())
	{
		fval = getDouble();
	}
	else
	{
		if(!toNumber(&fval, ctx))
		{
			*ival = 0;
			return false;
		}
	}

	*ival = convF64ToS32(fval);
	return true;
}

/*---------------------------------------------------------------------*//**
	正数化 ⇒ js_ValueToECMAUint32
**//*---------------------------------------------------------------------*/
bool EsValue::toUint32(u32* uival, EsContext* ctx) const
{
	if(isInt())
	{
		s32 iwk = getInt();
		*uival = (u32)iwk;
		return true;
	}

	f64 fval;
	if(isDouble())
	{
		fval = getDouble();
	}
	else
	{
		if(!toNumber(&fval, ctx))
		{
			*uival = 0;
			return false;
		}
	}

	*uival = convF64ToU32(fval);
	return true;
}

/*---------------------------------------------------------------------*//**
	浮動小数点値を整数化 ⇒ js_DoubleToECMAInt32
**//*---------------------------------------------------------------------*/
s32 EsValue::convF64ToS32(f64 fval)
{
	if(TFW_F64_IS_FINITE(fval))
	{
		return 0;
	}

	s32 iwk = (s32)fval;
	if((f64)iwk == fval)
	{
		return iwk;
	}

	const f64 fval32bitI = 4294967296.0;
	const f64 fval31bitI = 2147483648.0;
	fval = ::fmod(fval, fval32bitI);
	fval = (fval >= 0) ? ::floor(fval) : (::ceil(fval) + fval32bitI);
	iwk = (s32)(fval >= fval31bitI ? (fval - fval32bitI) : fval);
	return iwk;
}

/*---------------------------------------------------------------------*//**
	浮動小数点値を正数化 ⇒ js_DoubleToECMAUint32
**//*---------------------------------------------------------------------*/
u32 EsValue::convF64ToU32(f64 fval)
{
	if(TFW_F64_IS_FINITE(fval))
	{
		return 0;
	}

	u32 uiwk = (u32)fval;
	if((f64)uiwk == fval)
	{
		return uiwk;
	}

	bool isNegative = (fval < 0);
	fval = ::floor(isNegative ? - fval : fval);
	fval = isNegative ? - fval : fval;

	const f64 fval32bitUi = 4294967296.0;
	fval = ::fmod(fval, fval32bitUi);
	uiwk = (u32)(fval >= 0 ? fval : (fval + fval32bitUi));
	return uiwk;
}

/*---------------------------------------------------------------------*//**
	整数と浮動小数点数の適した型で値を設定する
	⇒ js_NewNumberInRootedValue, js_NewWeaklyRootedNumber
**//*---------------------------------------------------------------------*/
void EsValue::setNumber(f64 fval)
{
	if(TFW_F64_IS_INT(fval))
	{
		// 整数値として保存
		setValue((s32)fval);
	}
	else
	{
		// 浮動小数点値として保存
		setValue(fval);
	}
}

/*---------------------------------------------------------------------*//**
	論理的な型を得る ⇒ JS_TypeOfValue
**//*---------------------------------------------------------------------*/
EsLogicalType EsValue::getLogicalType() const
{
	EsLogicalType ltype;
	if(isObject())
	{
		ltype = LTYPE_OBJ;
		EsObject* obj = getObject();
		if(obj != 0L)
		{
			// ※※※ 実装不要 ※※※ ⇒ js_GetWrappedObject, JSCLASS_IS_EXTENDED

			const EsObjectOp* oop = obj->getObjectOp();
			bool isFunction = false;
			if(oop->getCallFunc() != 0L)
			{
				isFunction = true;
			}
			else
			{
				u32 clstype = obj->getClass()->getClassType();
				if(obj->getClass()->getCallFunc() != 0L)
				{
					isFunction = (clstype == EsSysClasses::CLASSTYPE_SCRIPT);
				}
				else
				{
					isFunction = (clstype == EsSysClasses::CLASSTYPE_FUNC);
				}
			}
			if(isFunction)
			{
				ltype = LTYPE_FUNC;
			}
		}
	}
	else if(isNumber())
	{
		ltype = LTYPE_NUMBER;
	}
	else if(isBoolean())
	{
		ltype = LTYPE_BOOLEAN;
	}
	else if(isString())
	{
		ltype = LTYPE_STRING;
	}
	else
	{
		ltype = LTYPE_VOID;
	}

	return ltype;
}

/*---------------------------------------------------------------------*//**
	ハッシュコードを計算する
**//*---------------------------------------------------------------------*/
u32 EsValue::calcHashCode() const
{
	return _val._i._hi ^ _val._i._lo;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void EsValue::copy(const EsValue* src)
{
	_type = src->_type;
	TFW_COPY_MEMORY(&_val, &src->_val, sizeof(Value));
}

//==========================================================================
// EsValue::EsValueAtomHashOp メソッド

/*---------------------------------------------------------------------*//**
	エントリのサイズを返す
**//*---------------------------------------------------------------------*/
u32 EsValue::EsValueAtomHashOp::getEntrySize()
{
	return sizeof(EsAtom::EsAtomHashEntry);
}

/*---------------------------------------------------------------------*//**
	エントリバッファを確保する
**//*---------------------------------------------------------------------*/
void* EsValue::EsValueAtomHashOp::newEntryBuffer(u32 numEntry)
{
	EsAtom::EsAtomHashEntry* arr = new EsAtom::EsAtomHashEntry[numEntry];
	return arr;
}

/*---------------------------------------------------------------------*//**
	エントリバッファを削除する
**//*---------------------------------------------------------------------*/
void EsValue::EsValueAtomHashOp::deleteEntryBuffer(void* entrybuf)
{
	EsAtom::EsAtomHashEntry* arr = (EsAtom::EsAtomHashEntry*)entrybuf;
	delete[] arr;
}

/*---------------------------------------------------------------------*//**
	ハッシュコードを算出する
**//*---------------------------------------------------------------------*/
u32 EsValue::EsValueAtomHashOp::hashCode(const void* key)
{
	const EsValue* val = (const EsValue*)key;
	u32 h = val->calcHashCode();
	return h;
}

/*---------------------------------------------------------------------*//**
	エントリが同一かどうか判定する
**//*---------------------------------------------------------------------*/
bool EsValue::EsValueAtomHashOp::matchEntry(HashEntry* entry, const void* key)
{
	const EsAtom* atom = ((EsAtom::EsAtomHashEntry*)entry)->getAtom();
	const EsValue* val1 = atom->getValue();
	const EsValue* val2 = (const EsValue*)key;
	return val1->equals(val2);
}

/*---------------------------------------------------------------------*//**
	エントリ初期化
**//*---------------------------------------------------------------------*/
bool EsValue::EsValueAtomHashOp::initEntry(HashEntry* entry, const void* key)
{
	EsAtom::EsAtomHashEntry* ae = (EsAtom::EsAtomHashEntry*)entry;
	const EsValue* val = (const EsValue*)key;
	EsAtom* atom = new EsAtom(val);
	ae->setAtom(atom);
	return true;
}

/*---------------------------------------------------------------------*//**
	エントリ破棄
**//*---------------------------------------------------------------------*/
void EsValue::EsValueAtomHashOp::cleanupEntry(HashEntry* entry)
{
	EsAtom::EsAtomHashEntry* ae = (EsAtom::EsAtomHashEntry*)entry;
	delete ae->getAtom();
	entry->clear();
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
