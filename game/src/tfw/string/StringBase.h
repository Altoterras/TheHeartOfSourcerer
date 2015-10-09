/***********************************************************************//**
 *  CStringBase.h
 *  Terras Framework
 *
 *  Created by Ryoutarou Kishi on 2009/05/20.
 *  Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_STRING_STRING_BASE_H_
#define _TFW_STRING_STRING_BASE_H_

////////////////////////////////////////////////////////////////////////////
// クラス

namespace tfw
{

/*---------------------------------------------------------------------*//**
 *	文字列基底クラス
 *
**//*---------------------------------------------------------------------*/
template <class TYPE>
class StringBase
{
public:
	virtual const TYPE* getRaw(int index = 0) const = 0;
	virtual	int getLength() const = 0;
	virtual	TYPE getAt(int index) const = 0;
};

}	// namespace tfw

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_STRING_STRING_BASE_H_
