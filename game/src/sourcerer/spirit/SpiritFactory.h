/***********************************************************************//**
 *	SpiritFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/23.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SPIRIT_SPIRIT_FACTORY_H_
#define _SRCR_SPIRIT_SPIRIT_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "SpiritType.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Spirit;

/*---------------------------------------------------------------------*//**
 *	ユニット ファクトリー
 *
**//*---------------------------------------------------------------------*/
class SpiritFactory : public FrameExecDraw
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static Spirit* makeSpirit(SpiritType sprttype);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_SPIRIT_FACTORY_H_
