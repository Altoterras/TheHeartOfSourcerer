/***********************************************************************//**
 *  CStringBase.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/20.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_C_STRING_BASE_H_
#define _TFW_STRING_C_STRING_BASE_H_

#if 1
////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../Base.h"

// Friends
#include "StringBase.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	文字列基底クラス
 *
**//*---------------------------------------------------------------------*/
class CStringBase : public StringBase<char>
{
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
#else
////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{

/*---------------------------------------------------------------------*//**
 *	文字列基底クラス
 *
**//*---------------------------------------------------------------------*/
class CStringBase
{
public:
	virtual const char* getRaw(int index = 0) const = 0;
	virtual operator const char*() const = 0;
	virtual	int getLength() const = 0;
	virtual	char getAt(int index) const = 0;
};

}	// namespace tfw

////////////////////////////////////////////////////////////////////////////
#endif

#endif	// _TFW_STRING_C_STRING_BASE_H_
