/***********************************************************************//**
 *	EsKeyword.cpp
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "EsKeyword.h"

// Friends
#include "EsAtom.h"
#include "EsContext.h"
#include "EsKeywordSet.h"

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	アトムの文字列を得る
**//*---------------------------------------------------------------------*/
const VcString* EsKeyword::getAtomString() const
{
	return (_param1 <= P1_HASATOM) ? getAtom()->getString() : 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	値設定
**//*---------------------------------------------------------------------*/
void EsKeyword::set(const char* name)
{
	VcString* str = new VcString(name);
	EsAtom* atom = new EsAtom(str);
	_p = atom;
	_param1 = P1_HASATOM;
	_param2 = reinterpret_cast<addr_size>(str);
}

/*---------------------------------------------------------------------*//**
	値設定
**//*---------------------------------------------------------------------*/
void EsKeyword::set(const char* name, EsTokenType tt, EsOpcodeType ot)
{
	VcString* str = new VcString(name);
	_p = str;
	_param1 = tt;
	_param2 = ot;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsKeyword::EsKeyword()
{
	init();
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsKeyword::EsKeyword(const char* name)
{
	init();
	set(name);
}

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsKeyword::EsKeyword(const char* name, EsTokenType tt, EsOpcodeType ot)
{
	init();
	set(name, tt, ot);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
EsKeyword::~EsKeyword()
{
	if(_param1 == P1_REFATOM)
	{
		delete reinterpret_cast<VcString*>(_param2);
	}
	else if(_param1 == P1_HASATOM)
	{
		delete (EsAtom*)_p;
		delete reinterpret_cast<VcString*>(_param2);
	}
	else
	{
		delete (VcString*)_p;
	}
}

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
void EsKeyword::init()
{
	_p = 0L;
	_param1 = 0;
	_param2 = 0;
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS