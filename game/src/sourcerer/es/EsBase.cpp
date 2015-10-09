/***********************************************************************//**
 *	EsBase.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/04/01.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsBase.h"

// Friends

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// グローバル関数

/*---------------------------------------------------------------------*//**
	ダミー ゲッター アクセサ関数
**//*---------------------------------------------------------------------*/
bool g_funcDummyPropertyGetter(EsValue* val, EsContext* ctx, EsObject* obj, const EsValue* id)
{
	return true;
}

/*---------------------------------------------------------------------*//**
	ダミー セッター アクセサ関数 ⇒ JS_PropertyStub
**//*---------------------------------------------------------------------*/
bool g_funcDummyPropertySetter(EsContext* ctx, EsObject* obj, const EsValue* id, const EsValue* val)
{
	return true;
}

/*---------------------------------------------------------------------*//**
	ダミー列挙関数
**//*---------------------------------------------------------------------*/
bool g_funcDummyEnumerate(EsContext* ctx, EsObject* obj)
{
	return true;
}

/*---------------------------------------------------------------------*//**
	ダミー列挙関数 ⇒ JS_EnumerateStub
**//*---------------------------------------------------------------------*/
bool g_funcDummyEnumerateOut(EsValue* valState, EsValue* id, EsContext* ctx, EsObject* obj, u8 enumop)
{
	return true;
}

/*---------------------------------------------------------------------*//**
	ダミー解決関数 ⇒ JS_ResolveStub
**//*---------------------------------------------------------------------*/
bool g_funcDummyResolve(EsObject** objOut, EsContext* ctx, EsObject* obj, const EsValue* id, u8 flagsRslv)
{
	return true;
}

ES_END_NS
