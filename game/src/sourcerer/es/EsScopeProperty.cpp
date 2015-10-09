/***********************************************************************//**
 *	EsScopeProperty.cpp
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
#include "EsScopeProperty.h"

// Friends
#include "EsSysClasses.h"
#include "EsObject.h"
#include "EsValue.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	ユーザー ID を取得 ⇒ SPROP_USERID
**//*---------------------------------------------------------------------*/
EsValue EsScopeProperty::getUserId() const
{
	if(TFW_IS_FLAG(_optf, OPTF_HAS_SHORTID))
	{
		return EsValue((s32)_shortid);
	}
	else
	{
		return _id;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ゲットのコール ⇒ js_GetSprop
**//*---------------------------------------------------------------------*/
bool EsScopeProperty::callGetProperty(EsValue* val, EsContext* ctx, EsObject* obj)
{
	ASSERT(_funcGetter != 0L);
	if(_funcGetter == 0L)	{	return false;	}

	if(TFW_IS_FLAG(_atrbf, ATRBF_OBJ_GETTER))
	{
		// ※※※ 実装保留 ※※※ ⇒ js_CastAsObjectJSVal, js_InternalGetOrSet
		ASSERT(false);
		return false;
	}

	if(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_WITH)
	{
		obj = obj->getThisObjectOv(ctx);
	}

	EsValue valUserId(getUserId());
	return _funcGetter(val, ctx, obj, &valUserId);
}

/*---------------------------------------------------------------------*//**
	セットのコール ⇒ js_SetSprop
**//*---------------------------------------------------------------------*/
bool EsScopeProperty::callSetProperty(EsContext* ctx, EsObject* obj, const EsValue* val)
{
	ASSERT(_funcSetter != 0L);
	if(_funcSetter == 0L)	{	return false;	}

	if(TFW_IS_FLAG(_atrbf, ATRBF_OBJ_SETTER))
	{
		// ※※※ 実装保留 ※※※ ⇒ js_CastAsObjectJSVal, js_InternalGetOrSet
		ASSERT(false);
		return false;
	}
	if(TFW_IS_FLAG(_atrbf, ATRBF_OBJ_GETTER))
	{
		// ※※※ 実装保留 ※※※ ⇒ js_ReportGetterOnlyAssignment
		ASSERT(false);
		return false;
	}

	if(obj->getClass()->getClassType() == EsSysClasses::CLASSTYPE_WITH)
	{
		obj = obj->getThisObjectOv(ctx);
	}

	EsValue valUserId(getUserId());
	return _funcSetter(ctx, obj, &valUserId, val);
}

/*---------------------------------------------------------------------*//**
	同じパラメータのプロパティかを調べる ⇒ SPROP_MATCH
**//*---------------------------------------------------------------------*/
bool EsScopeProperty::isMatchParams(const EsScopeProperty* s)
{
	return isMatchParams(&s->_id, s->_funcGetter, s->_funcSetter, s->_slotidx, s->_atrbf, s->_optf, s->_shortid);
}

/*---------------------------------------------------------------------*//**
	同じパラメータのプロパティかを調べる ⇒ SPROP_MATCH_PARAMS
**//*---------------------------------------------------------------------*/
bool EsScopeProperty::isMatchParams(const EsValue* id, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, s32 slotidx, u8 atrbf, u8 optf, s16 shortid)
{
	return id->equals(id) && isMatchParams(funcGetter, funcSetter, slotidx, atrbf, optf, shortid);
}

/*---------------------------------------------------------------------*//**
	ID 以外が同じパラメータのプロパティかを調べる ⇒ SPROP_MATCH_PARAMS_AFTER_ID
**//*---------------------------------------------------------------------*/
bool EsScopeProperty::isMatchParams(EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, s32 slotidx, u8 atrbf, u8 optf, s16 shortid)
{
	return
		(this->_funcGetter == funcGetter) &&
		(this->_funcSetter == funcSetter) &&
		(this->_slotidx == slotidx) &&
		(this->_atrbf == atrbf) &&
		(this->_shortid == shortid)	&&
		(((this->_optf ^ optf) & (OPTF_MARK | OPTF_FLAG_SHAPE_REGEN)) == 0) ;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsScopeProperty::EsScopeProperty()
{
	clear();
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsScopeProperty::EsScopeProperty(const EsValue* id, EsPropertyGetterFunc funcGetter, EsPropertySetterFunc funcSetter, s32 slotidx, u8 atrbf, u8 optf)
{
	clear();
	_id.copy(id);
	_funcGetter = funcGetter;
	_funcSetter = funcSetter;
	_slotidx = slotidx;
	_atrbf = atrbf;
	_optf = optf;
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsScopeProperty::~EsScopeProperty()
{
}

/*---------------------------------------------------------------------*//**
	クリア
**//*---------------------------------------------------------------------*/
void EsScopeProperty::clear()
{
	_shortid = 0;
	_atrbf = 0;
	_optf = 0;
	_slotidx = SLOTIDX_INVALUD;
	_funcGetter = 0L;
	_funcSetter = 0L;
	_parent = 0L;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
