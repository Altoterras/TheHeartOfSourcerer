/***********************************************************************//**
 *	QuestIdDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/18.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_QUEST_ID_DEF_H_
#define _SRCR_EPISODE_QUEST_ID_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	クエスト ID の定義
 *
**//*---------------------------------------------------------------------*/
class QuestIdDef
{
	//======================================================================
	// 定数
public:
	static const u16	QUESTID_EP01_BASE__START_			= 10100;
	static const u16	QUESTID_OPENTHEDOOR_01				= 10107;	// 井戸舎のドアを開ける
	static const u16	QUESTID_EP01_BASE__END_				= 10199;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_QUEST_ID_DEF_H_
