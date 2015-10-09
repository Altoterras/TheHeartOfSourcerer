/***********************************************************************//**
 *	ZCompress.cpp
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2010/03/17.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ZCompress.h"

// Friends

// External

// Library
#if SUPPORT_EXTLIB_ZLIB
#include <zlib/zlib.h>
#endif

////////////////////////////////////////////////////////////////////////////

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ZCompress メソッド

/*---------------------------------------------------------------------*//**
	解凍
**//*---------------------------------------------------------------------*/
bool ZCompress::uncompress(u8* dst, u32* lenDst, u8* src, u32 lenSrc)
{
	#if SUPPORT_EXTLIB_ZLIB
		s32 ret = ::uncompress(dst, (uLongf*)lenDst, src, lenSrc);
		if(ret == Z_OK)	{	return true;	}
		TRACE("{zlib@uncompress} error - %d\n", ret);
		return false;
	#else
		return false;
	#endif
}

/*---------------------------------------------------------------------*//**
	圧縮
**//*---------------------------------------------------------------------*/
bool ZCompress::compress(u8* dst, u32* lenDst, u8* src, u32 lenSrc)
{
	#if SUPPORT_EXTLIB_ZLIB
		s32 ret = ::compress(dst, (uLongf*)lenDst, src, lenSrc);
		if(ret == Z_OK)	{	return true;	}
		TRACE("{zlib@compress} error - %d\n", ret);
		return false;
	#else
		return false;
	#endif
}

/*---------------------------------------------------------------------*//**
	圧縮用メモリサイズを得る
**//*---------------------------------------------------------------------*/
u32 ZCompress::getCompressedMaxMemorySize(u32 lenSrc)
{
	#if SUPPORT_EXTLIB_ZLIB
		return (u32)::compressBound(lenSrc);
	#else
		return 0;
	#endif
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
