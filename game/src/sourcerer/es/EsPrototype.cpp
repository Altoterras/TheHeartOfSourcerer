/***********************************************************************//**
 *	EsPrototype.cpp
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
#include "EsPrototype.h"

// Friends

// External

ES_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// EsPrototype メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
EsPrototype::EsPrototype(const EsAtom* atomName, u16 kind)
	: _kind(kind)
	, _atomName(atomName)
{

}

////////////////////////////////////////////////////////////////////////////

ES_END_NS
