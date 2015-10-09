/***********************************************************************//**
 *	ActFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_ACT_FACTORY_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_ACT_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Act;

/*---------------------------------------------------------------------*//**
 *	Act ファクトリー
 *	
**//*---------------------------------------------------------------------*/
class ActFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	static Act* makeAct(const VcString* nameAct);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_ACT_FACTORY_H_
