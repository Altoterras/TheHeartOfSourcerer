/***********************************************************************//**
 *	EsMath.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/10/24.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsMath.h"

// Friends
#include "EsContext.h"
#include "EsError.h"
#include "EsSysClasses.h"
#include "EsKeywordSet.h"
#include "EsObject.h"
#include "EsObjectSlots.h"
#include "EsResolver.h"
#include "EsScopeProperty.h"

// External
#include "../../tfw/lib/TypeUtils.h"
#include <math.h>

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#if __GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
	#define COPYSIGN __builtin_copysign
#elif defined WINCE
	#define COPYSIGN _copysign
#elif defined _WIN32
	#define COPYSIGN _copysign
#else
	#define COPYSIGN copysign
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsMath::EsMathPrototype メソッド

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathPrototype::initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const
{
	EsObject* objPtype = EsMath::EsMathClass::makeClassObject(ctx, obj);
	*objPrototypeOut = objPtype;
	return objPtype != 0L;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsMath::EsMathPrototype::EsMathPrototype()
	: EsPrototype(EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_MATH)->getAtom(), EsSysPrototypes::PTYPEKIND_MATH)
{
}


//==========================================================================
// EsMath::EsMathClass メソッド

/*---------------------------------------------------------------------*//**
	数値処理スクリプトクラスの作成 ⇒ js_InitMathClass
**//*---------------------------------------------------------------------*/
EsObject* EsMath::EsMathClass::makeClassObject(EsContext* ctx, EsObject* objParent)
{
	// クラスオブジェクト作成
	EsObject* objMath = ctx->newObject(getInstance(), 0L, objParent);
	if(objMath == 0L)
	{
		return 0L;
	}

	// グローバルオブジェクトに数値処理クラスを定義する
	EsValue valMathObj(objMath);
	if(!objParent->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_MATH)->getAtom(), g_funcDummyPropertyGetter, g_funcDummyPropertySetter, &valMathObj, 0, 0, 0))
	{
		return 0L;
	}

	EsValue valWk;
	// 静的関数定義 ⇒ math_static_methods
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ABS)->getAtom(), callAbs, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ACOS)->getAtom(), callAcos, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ASIN)->getAtom(), callAsin, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ATAN)->getAtom(), callAtan, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ATAN2)->getAtom(), callAtan2, 0))	{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_CEIL)->getAtom(), callCeil, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_COS)->getAtom(), callCos, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_EXP)->getAtom(), callExp, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_FLOOR)->getAtom(), callFloor, 0))	{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_LOG)->getAtom(), callLog, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_MAX)->getAtom(), callMax, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_MIN)->getAtom(), callMin, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_POW)->getAtom(), callPow, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_RANDOM)->getAtom(), callRandom, 0))	{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_ROUND)->getAtom(), callRound, 0))	{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_SIN)->getAtom(), callSin, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_SQRT)->getAtom(), callSqrt, 0))		{	return 0L;	}	
	if(!objMath->defineClassFunction(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_PROP_TAN)->getAtom(), callTan, 0))		{	return 0L;	}	
	// 静的特殊定数アクセサ定義 ⇒ math_constants
	if(!objMath->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_E)->getAtom(), 0L, 0L, EsValue::setout(&valWk, (f64)2.7182818284590452354), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))			{	return 0L;	}
	if(!objMath->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_LN10)->getAtom(), 0L, 0L, EsValue::setout(&valWk, (f64)2.30258509299404568402), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))		{	return 0L;	}
	if(!objMath->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_LN2)->getAtom(), 0L, 0L, EsValue::setout(&valWk, (f64)0.69314718055994530942), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))		{	return 0L;	}
	if(!objMath->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_LOG10E)->getAtom(), 0L, 0L, EsValue::setout(&valWk, (f64)0.43429448190325182765), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))	{	return 0L;	}
	if(!objMath->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_LOG2E)->getAtom(), 0L, 0L, EsValue::setout(&valWk, (f64)1.4426950408889634074), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))		{	return 0L;	}
	if(!objMath->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_PI)->getAtom(), 0L, 0L, EsValue::setout(&valWk, (f64)3.14159265358979323846), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))		{	return 0L;	}
	if(!objMath->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_SQRT1_2)->getAtom(), 0L, 0L, EsValue::setout(&valWk, (f64)0.70710678118654752440), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))	{	return 0L;	}
	if(!objMath->addProperty(ctx, EsKeywordSet::getStatic(EsKeywordSet::S_CONST_SQRT2)->getAtom(), 0L, 0L, EsValue::setout(&valWk, (f64)1.41421356237309504880), 0, EsScopeProperty::ATRBF_READONLY | EsScopeProperty::ATRBF_PERMANENT, 0))	{	return 0L;	}

	return objMath;
}

/*---------------------------------------------------------------------*//**
	数値処理オブジェクトクラスのコンストラクタ ⇒ js_WithClass
**//*---------------------------------------------------------------------*/
EsMath::EsMathClass::EsMathClass()
	: EsClass(
		EsKeywordSet::getStatic(EsKeywordSet::S_CLASS_MATH)->getAtom()
		, EsSysClasses::CLASSTYPE_MATH
		, 0
		, EsSysPrototypes::PTYPEKIND_MATH
		, 1 )
{
}

/*---------------------------------------------------------------------*//**
	絶対値取得 ⇒ math_abs
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callAbs(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::fabs(a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	アークコサイン ⇒ math_acos
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callAcos(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::acos(a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	アークサイン ⇒ math_asin
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callAsin(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::asin(a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	アークタンジェント ⇒ math_atan
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callAtan(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::atan(a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	２項アークタンジェント ⇒ math_atan2
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callAtan2(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, b, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}
	vaArg[1].toNumber(&b, &vaArg[1], ctx);
	if(vaArg[1].isNull())
	{
		return false;
	}

	r = ::atan2(a, b);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	端数繰上げ ⇒ math_ceil
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callCeil(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::ceil(a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	コサイン ⇒ math_cos
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callCos(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::cos(a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	e の累乗 ⇒ math_exp
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callExp(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::exp(a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	端数切り捨て ⇒ math_floor
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callFloor(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::floor(a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	自然対数 ⇒ math_log
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callLog(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::log(a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	最大値 ⇒ math_max
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callMax(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NegativeInfinity());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNegativeInfinity);
		return true;
	}

	f64 a, r;
	r = TypeUtils::getF64NegativeInfinity();
	for(u32 i = 0; i < numArg; i++)
	{
		vaArg[i].toNumber(&a, &vaArg[i], ctx);
		if(vaArg[i].isNull())
		{
			return false;
		}
		if(TFW_F64_IS_NAN(a))
		{
			valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
			return true;
		}

		if((a == 0.0) && (a == r))
		{
			if(COPYSIGN(1.0, r) == -1)
			{
				r = a;
			}
		}
		else if(a > r)
		{
			r = a;
		}
	}

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	最小値 ⇒ math_min
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callMin(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64PositiveInfinity());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsPositiveInfinity);
		return true;
	}

	f64 a, r;
	r = TypeUtils::getF64PositiveInfinity();
	for(u32 i = 0; i < numArg; i++)
	{
		vaArg[i].toNumber(&a, &vaArg[i], ctx);
		if(vaArg[i].isNull())
		{
			return false;
		}
		if(TFW_F64_IS_NAN(a))
		{
			valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
			return true;
		}

		if((a == 0.0) && (a == r))
		{
			if(COPYSIGN(1.0, r) == -1)
			{
				r = a;
			}
		}
		else if(a < r)
		{
			r = a;
		}
	}

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	累乗 ⇒ math_pow
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callPow(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, b, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}
	vaArg[1].toNumber(&b, &vaArg[1], ctx);
	if(vaArg[1].isNull())
	{
		return false;
	}
	
	// 引数チェック
	if(!TFW_F64_IS_FINITE(b) && ((a == 1.0) || (a == -1.0)))	// ⇒ if (!JSDOUBLE_IS_FINITE(y) && (x == 1.0 || x == -1.0))
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}
	if(b == 0.0)
	{
		valRet->setValue((f64)1.0);	// ⇒ *vp = JSVAL_ONE;
		return true;
	}

	r = ::atan2(a, b);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	乱数 ⇒ math_random
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callRandom(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	f64 r = ::rand() / (f64)RAND_MAX;

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	四捨五入 ⇒ math_round
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callRound(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = COPYSIGN(::floor(a + 0.5), a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	サイン ⇒ math_sin
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callSin(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::sin(a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	平方根 ⇒ math_sqrt
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callSqrt(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::sqrt(a);

	valRet->setNumber(r);
	return true;
}

/*---------------------------------------------------------------------*//**
	タンジェント ⇒ math_tan
**//*---------------------------------------------------------------------*/
bool EsMath::EsMathClass::callTan(EsContext* ctx, EsObject* objThis, EsValue* valCallee, EsValue* valThis, EsValue* vaArg, u32 numArg, EsValue* valRet, const EsCallExtParam* exprm)
{
	if(numArg <= 0)
	{
		valRet->setValue(TypeUtils::getF64NaN());	// ⇒ *vp = DOUBLE_TO_JSVAL(cx->runtime->jsNaN);
		return true;
	}

	f64 a, r;
	vaArg[0].toNumber(&a, &vaArg[0], ctx);
	if(vaArg[0].isNull())
	{
		return false;
	}

	r = ::tan(a);

	valRet->setNumber(r);
	return true;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
