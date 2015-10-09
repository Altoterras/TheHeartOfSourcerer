/***********************************************************************//**
 *	EsToken.cpp
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
#include "EsToken.h"

// Friends

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsToken メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

const double* EsToken::getDoubleValue() const
{
	if(_fmt != FMT_DOUBLE)	{	return 0L;	}
	return &_v._d;
}

void EsToken::setDoubleValue(double d)
{
	_fmt = FMT_DOUBLE;
	_v._d = d;
}

const EsOpcodeType EsToken::getOpcodeTypeValue() const
{
	if(_fmt != FMT_NS)	{	return OP_NULL;	}
	return _v._ns._ot;
}

void EsToken::setOpcodeTypeValue(EsOpcodeType ot)
{
	_fmt = FMT_NS;
	_v._ns._ot = ot;
}

const EsAtom* EsToken::getAtomValue() const
{
	if(_fmt != FMT_NS)	{	return 0L;	}
	return _v._ns._atom;
}

void EsToken::setAtomValue(const EsAtom* atom)
{
	_fmt = FMT_NS;
	_v._ns._atom = atom;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsToken::EsToken()
	: _type(TKN_NULL)
	, _fmt(FMT_NULL)
{
	TFW_ZERO_MEMORY(&_v, sizeof(_v));
}

/*---------------------------------------------------------------------*//**
	コピー
**//*---------------------------------------------------------------------*/
void EsToken::copy(const EsToken* src)
{
	_type = src->_type;
	_pos.copy(&src->_pos);
	_fmt = src->_fmt;
	TFW_COPY_MEMORY(&_v, &src->_v, sizeof(_v));
}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
