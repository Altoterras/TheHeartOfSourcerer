/***********************************************************************//**
 *	MsgNextPage.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_MSG_NEXT_PAGE_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_MSG_NEXT_PAGE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Act.h"

#include "../ActKind.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	メッセージを次のページへ送る Act
 *	
**//*---------------------------------------------------------------------*/
class MsgNextPage : public Act
{
public:
	MsgNextPage() : Act(ACT_MSG_NEXT_PAGE) {}
	void play(ActMatter* owner);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_IMPLEMENT_MSG_NEXT_PAGE_H_
