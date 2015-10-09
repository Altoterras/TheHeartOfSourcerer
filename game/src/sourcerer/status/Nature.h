/***********************************************************************//**
 *	Nature.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/19.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_NATURE_H_
#define _SRCR_STATUS_NATURE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	３つの質および機能
 *
**//*---------------------------------------------------------------------*/
class Nature
{
	//======================================================================
	// 定数
public:
	enum Trinity
	{
		NT_PURE,	// 浄性，創造，調和，癒し
		NT_EXCT,	// 激性，破壊，移動，攻撃
		NT_DULL,	// 鈍性，維持，停滞，守備
	};

	static const u32 NUM_NATURE = 3;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_NATURE_H_