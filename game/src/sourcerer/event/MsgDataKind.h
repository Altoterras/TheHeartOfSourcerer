/***********************************************************************//**
 *	MsgDataSet.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MSG_DATA_KIND_H_
#define _SRCR_EVENT_MSG_DATA_KIND_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	メッセージ データ種別
 *	
**//*---------------------------------------------------------------------*/
class MsgDataKind
{
	//======================================================================
	// 定数
public:
	const static u32	MSGDKIND_MAP			= 0;
	const static u32	MSGDKIND_STORY			= 1;
	const static u32	MSGDKIND_TMP			= 2;
	const static u32	MSGDKIND_QUEST__START_	= 3;
	const static u32	MSGDKIND_QUEST__END_	= 13;	// QuestManager::NUM_QUEST_SLOT_MAX + MSGDKIND_QUEST__START_

	const static u32	NUM_MSGDKIND			= 14;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MSG_DATA_KIND_H_
