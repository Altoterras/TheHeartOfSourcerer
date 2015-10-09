/***********************************************************************//**
 *	ZCompress.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2010/03/17.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_Z_COMPRESS_H_
#define _TFW_LIB_Z_COMPRESS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	Z 圧縮
 *
**//*---------------------------------------------------------------------*/
class ZCompress
{
public:
	static bool uncompress(u8* dst, u32* lenDst, u8* src, u32 lenSrc);
	static bool compress(u8* dst, u32* lenDst, u8* src, u32 lenSrc);
	static u32	getCompressedMaxMemorySize(u32 lenSrc);
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_LIB_Z_COMPRESS_H_

