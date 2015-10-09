/***********************************************************************//**
 *	SpiritMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SPIRIT_SIRIT_MODE_H_
#define _SRCR_SPIRIT_SIRIT_MODE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "Spirit.h"

#include "../../tfw/modemng/Mode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	スピリット モード
 *
**//*---------------------------------------------------------------------*/
class SpiritMode : public Mode
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual	~SpiritMode() {}
	virtual bool action() = 0;
	virtual void resetAction() {};
	virtual bool isActing() = 0;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_SIRIT_MODE_H_
