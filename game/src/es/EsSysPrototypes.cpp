/***********************************************************************//**
 *	EsSysPrototypes.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2011/05/10.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsSysPrototypes.h"

// Friends
#include "EsArray.h"
#include "EsBoolean.h"
#include "EsContext.h"
#include "EsSysClasses.h"
#include "EsFunction.h"
#include "EsIterator.h"
#include "EsMath.h"
#include "EsNumber.h"
#include "EsObject.h"
#include "EsResolver.h"
#include "EsString.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsSysPrototypes 変数

Array<const EsPrototype*> EsSysPrototypes::_arrExtPtypes(true, false);

//==========================================================================
// EsSysPrototypes メソッド

/*---------------------------------------------------------------------*//**
	プロトタイプインスタンスを得る
**//*---------------------------------------------------------------------*/
const EsPrototype* EsSysPrototypes::getPrototype(u16 ptypekind)
{
	// 固定プロトタイプ
	switch(ptypekind)
	{
	case PTYPEKIND_NULL:		return EsNullPrototype::getInstance();
	case PTYPEKIND_OBJECT:		return EsObject::EsObjectPrototype::getInstance();
	case PTYPEKIND_FUNCTION:	return EsFunction::EsFunctionPrototype::getInstance();
	case PTYPEKIND_ARRAY:		return EsArray::EsArrayPrototype::getInstance();
	case PTYPEKIND_BOOLEAN:		return EsBoolean::EsBooleanPrototype::getInstance();
	case PTYPEKIND_MATH:		return EsMath::EsMathPrototype::getInstance();
	case PTYPEKIND_NUMBER:		return EsNumber::EsNumberPrototype::getInstance();
	case PTYPEKIND_STRING:		return EsString::EsStringPrototype::getInstance();
	case PTYPEKIND_SCRIPT:		return EsObject::EsObjectPrototype::getInstance();
	case PTYPEKIND_ITERATOR:	return EsIterator::EsIteratorPrototype::getInstance();
	}

	// 拡張プロトタイプ
	for(u32 i = 0; i < _arrExtPtypes.getCount(); i++)
	{
		const EsPrototype* ptype = _arrExtPtypes.getAt(i);
		if(ptype->getPrototypeKind() == ptypekind)
		{
			return ptype;
		}
	}

	return 0L;
}

/*---------------------------------------------------------------------*//**
	新しいプロトタイプ種別値を生成する
**//*---------------------------------------------------------------------*/
u16 EsSysPrototypes::generatePrototypeKind(const EsAtom* atomName)
{
	u16 ptypekind = PTYPEKIND__CUSTOM_;
	for(u32 i = 0; i < _arrExtPtypes.getCount(); i++)
	{
		const EsPrototype* ptypeWk = _arrExtPtypes.getAt(i);
		if(ptypeWk == 0L)	{	continue;	}
		if(ptypeWk->getNameAtom() == atomName)
		{
			return ptypeWk->getPrototypeKind();
		}
		if(ptypekind <= ptypeWk->getPrototypeKind())
		{
			ptypekind = ptypeWk->getPrototypeKind() + 1;
		}
	}
	return ptypekind;
}

/*---------------------------------------------------------------------*//**
	拡張プロトタイプを登録する
**//*---------------------------------------------------------------------*/
void EsSysPrototypes::registerExternalPrototype(const EsPrototype* ptype)
{
	_arrExtPtypes.add(ptype);
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
