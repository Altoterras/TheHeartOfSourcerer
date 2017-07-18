/***********************************************************************//**
 *	SsBase.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/11/09.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SsBase.h"

// Friends

// External
#include "../../../es/EsContext.h"
#include "../../../es/EsKeyword.h"
#include "../../../es/EsKeywordSet.h"
#include "../../../es/EsObject.h"
#include "../../../es/EsScopeProperty.h"
#include "../../../es/EsSysClasses.h"
#include "../../../es/EsSysPrototypes.h"
#include "../../../es/EsValue.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SsBase::BasePrototype メソッド

/*---------------------------------------------------------------------*//**
	クラスの初期化
**//*---------------------------------------------------------------------*/
bool SsBase::BasePrototype::initClass(EsObject** objPrototypeOut, EsContext* ctx, EsObject* obj) const
{
	EsObject* objPtype = makeClassObject(ctx, obj);
	*objPrototypeOut = objPtype;
	return objPtype != 0L;
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SsBase::BasePrototype::BasePrototype(SoumaScriptKeywords::Keyword kwd)
	: EsPrototype(
		SoumaScriptKeywords::getKeyword(kwd)
		, EsSysPrototypes::generatePrototypeKind(SoumaScriptKeywords::getKeyword(kwd))	)
{
}


//==========================================================================
// SsBase::BaseClass メソッド

/*---------------------------------------------------------------------*//**
	ソウマスクリプトクラスのコンストラクタ
**//*---------------------------------------------------------------------*/
SsBase::BaseClass::BaseClass(SoumaScriptKeywords::Keyword kwd, u16 flags, u16 numDefaultReservedSlot)
	: EsClass(
		SoumaScriptKeywords::getKeyword(kwd)
		, EsSysClasses::generateClassType(SoumaScriptKeywords::getKeyword(kwd))
		, flags
		, EsSysPrototypes::generatePrototypeKind(SoumaScriptKeywords::getKeyword(kwd))
		, numDefaultReservedSlot )
{
}

//==========================================================================
// SsBase メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool SsBase::init()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
