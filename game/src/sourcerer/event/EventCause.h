/***********************************************************************//**
 *	EventCause.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_EVENT_CAUSE_H_
#define _SRCR_EVENT_EVENT_CAUSE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../tfw/evsys/EvCause.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	イベント起動原因
 *
**//*---------------------------------------------------------------------*/
class EventCause : public EvCause
{
public:
	static const short	CID_MAPSTART		= 30001;
	static const short	CID_STORYSTART		= 30002;
	static const short	CID_INTVLEVSTART	= 30003;
	static const short	CID_FOCUSMENU		= 30004;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_EVENT_CAUSE_H_
