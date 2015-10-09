/***********************************************************************//**
 *  TypeUtils.cpp
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2011/03/23.
 *  Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TypeUtils.h"

// Friends
#include "../string/VcString.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TypeUtils 定数

const TypeUtils::Union32 TypeUtils::s_f32PosInf	= { 0x7f800000 };
const TypeUtils::Union32 TypeUtils::s_f32NegInf	= { 0xff800000 };
const TypeUtils::Union32 TypeUtils::s_f32Nan	= { 0xffffffff };
const TypeUtils::Union64 TypeUtils::s_f64PosInf	= {        0x0, 0x7ff00000 };
const TypeUtils::Union64 TypeUtils::s_f64NegInf	= {        0x0, 0xfff00000 };
const TypeUtils::Union64 TypeUtils::s_f64Nan	= { 0xffffffff, 0xffffffff };

//==========================================================================
// TypeUtils 静的変数

VcString* TypeUtils::_strEmpty;

//==========================================================================
// TypeUtils メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	初期化
**//*---------------------------------------------------------------------*/
bool TypeUtils::init()
{
	// 空文字列の作成
	_strEmpty = new VcString();

	return true;
}

/*---------------------------------------------------------------------*//**
	後処理
**//*---------------------------------------------------------------------*/
void TypeUtils::cleanup()
{
	// 空文字列の削除
	delete _strEmpty;
	_strEmpty = 0L;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS