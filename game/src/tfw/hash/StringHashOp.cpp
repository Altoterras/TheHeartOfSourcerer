/***********************************************************************//**
 *	StringHashOp.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/10/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "StringHashOp.h"

// Friends
#include "StringHashEntry.h"
#include "../string/VcString.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// StringHashOp メソッド

/*---------------------------------------------------------------------*//**
	エントリのサイズを返す
**//*---------------------------------------------------------------------*/
u32 StringHashOp::getEntrySize()
{
	return sizeof(StringHashEntry);
}

/*---------------------------------------------------------------------*//**
	エントリバッファを確保する
**//*---------------------------------------------------------------------*/
void* StringHashOp::newEntryBuffer(u32 numEntry)
{
	StringHashEntry* arr = new StringHashEntry[numEntry];
	return arr;
}

/*---------------------------------------------------------------------*//**
	エントリバッファを削除する
**//*---------------------------------------------------------------------*/
void StringHashOp::deleteEntryBuffer(void* entrybuf)
{
	StringHashEntry* arr = (StringHashEntry*)entrybuf;
	delete[] arr;
}

/*---------------------------------------------------------------------*//**
	ハッシュコードを算出する
**//*---------------------------------------------------------------------*/
u32 StringHashOp::hashCode(const void* key)
{
	const VcString* str = (const VcString*)key;

	u32 h = 0;
	for(int i = 0; i < str->getLength(); i++)
	{
		h = TFW_BIT_ROT_L_32(h, 4) ^ str->getAt(i);
	}
	return h;
}

/*---------------------------------------------------------------------*//**
	エントリが同一かどうか判定する
**//*---------------------------------------------------------------------*/
bool StringHashOp::matchEntry(HashEntry* entry, const void* key)
{
	VcString* str1 = ((StringHashEntry*)entry)->getString();
	VcString* str2 = (VcString*)key;
	if(str1 == str2)					{	return true;	}
	if((str1 == 0L) && (str2 != 0L))	{	return false;	}
	if((str1 != 0L) && (str2 == 0L))	{	return false;	}
	return str1->equals(str2);
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
