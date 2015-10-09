/***********************************************************************//**
 *	EventMatterFactory.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_EVENT_MATTER_FACTORY_H_
#define _SRCR_EVENT_MATTER_EVENT_MATTER_FACTORY_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
#include "../../../tfw/evsys/EvMatterFactory.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	イベント マター ファクトリー
 *
**//*---------------------------------------------------------------------*/
class EventMatterFactory : public EvMatterFactory
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	EvMatter* newEventMatter(const CStringBase* nameMatter);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_EVENT_MATTER_FACTORY_H_
