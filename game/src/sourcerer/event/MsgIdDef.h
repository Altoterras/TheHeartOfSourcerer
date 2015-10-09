/***********************************************************************//**
 *	MsgIdDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/01/08.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MSG_ID_DEF_H_
#define _SRCR_EVENT_MSG_ID_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	イベント
 *
**//*---------------------------------------------------------------------*/
class MsgIdDef
{
	//======================================================================
	// 定数
public:
	// マップイベントのメッセージ ID （1 ～ 1000）
	static const u16	MSGID_MAP__START_	=     1;
	static const u16	MSGID_MAP__END_		=  1000;
	
	// ストーリーイベントのメッセージ ID （1001 ～ 10000）
	static const u16	MSGID_STORY__START_	=  1001;
	static const u16	MSGID_STORY__END_	= 10000;
	
	// クエストイベントのメッセージ ID （10001 ～ 50000）
	static const u16	MSGID_QUEST__START_	= 10001;
	static const u16	MSGID_QUEST__END_	= 50000;
	
	// テンポラリイベントのメッセージ ID （55001 ～ 56000）
	static const u16	MSGID_TMP__START_	= 55001;
	static const u16	MSGID_TMP__END_		= 56000;
	
	// 自動割り当てメッセージ ID （56001 ～ 59000）
	static const u16	MSGID_AUTO__START_	= 56001;
	static const u16	MSGID_AUTO__END_	= 59000;
	
	// システム固定メッセージ ID （60001 ～ 62000）
	static const u16	MSGID_FIXED__START_	= 60001;
	static const u16	MSGID_FIXED__END_	= 62000;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MSG_ID_DEF_H_
