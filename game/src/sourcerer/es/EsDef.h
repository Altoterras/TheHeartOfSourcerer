/***********************************************************************//**
 *	EsDef.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/03/25.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_DEF_H_
#define _ES_ES_DEF_H_

////////////////////////////////////////////////////////////////////////////
// マクロ

#define ESCASSERT	///ASSERT

////////////////////////////////////////////////////////////////////////////
// 定数

namespace es {

/*---------------------------------------------------------------------*//**
 *	論理的な型 ⇒ JSType, JSTYPE_*
 *
**//*---------------------------------------------------------------------*/
enum EsLogicalType
{
	LTYPE_NULL = 0,
	LTYPE_VOID,
	LTYPE_OBJ,
	LTYPE_FUNC,
	LTYPE_STRING,
	LTYPE_NUMBER,
	LTYPE_BOOLEAN,
	LTYPE_ATOM,
	LTYPE_PRIVATE,
	NUM_LTYPE
};

/*---------------------------------------------------------------------*//**
 *	アクセスフラグ ⇒ JSType, JSTYPE_*
 *
**//*---------------------------------------------------------------------*/
enum AccessFlags
{
	ACSF_PTYPE		= 0x01,		// プロトタイプ ⇒ JSACC_PROTO
	ACSF_PARENT		= 0x02,		// 親 ⇒ JSACC_PARENT
	ACSF_WATCH		= 0x04,		// ⇒ JSACC_WATCH
	ACSF_TYPEMASK	= 0x0f,		// タイプに対するビットマスク ⇒ JSACC_TYPEMASK
	ACSF_READ		= 0x10,		// 読み込み ⇒ JSACC_READ
	ACSF_WRITE		= 0x20,		// 書き込み ⇒ JSACC_WRITE
	ACSF_MODEMASK	= 0xf0,		// モードに対するビットマスク
};

/*---------------------------------------------------------------------*//**
 *	オブジェクトの型
 *
**//*---------------------------------------------------------------------*/
enum EsObjectType
{
	OBJT_NULL = 0,
	OBJT_ARRAY,
	OBJT_BLOCK,
	OBJT_FUNC,
	OBJT_ITER,
	OBJT_OBJECT,
	OBJT_WITH,
	NUM_OBJT
};

}

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_DEF_H_
