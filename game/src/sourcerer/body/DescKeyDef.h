/***********************************************************************//**
 *	DescKeyDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BODY_DESC_KEY_DEF_H_
#define _SRCR_BODY_DESC_KEY_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	識別キーの定義
 *
**//*---------------------------------------------------------------------*/
class DescKeyDef
{
	//======================================================================
	// 定数
public:
	static const u32	DESCKEY_HBIT_MASK					= 0xffff0000;
	static const u32	DESCKEY_LBIT_MASK					= 0x0000ffff;
	static const u32	DESCKEY_QUEST_HBIT					= 0x00010000;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_BODY_DESC_KEY_DEF_H_
