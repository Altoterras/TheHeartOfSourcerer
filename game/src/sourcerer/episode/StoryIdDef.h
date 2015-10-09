/***********************************************************************//**
 *	StoryIdDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STORY_ID_DEF_H_
#define _SRCR_EPISODE_STORY_ID_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ストーリー ID の定義
 *
**//*---------------------------------------------------------------------*/
class StoryIdDef
{
	//======================================================================
	// 定数
public:
	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// ストーリー ID

	// Vol.1 オープニング イベントのストーリー ID 範囲
	static const u16	STID_EP01_OP__START_			= 10101;
	static const u16	STID_EP01_OP__END_				= 10199;
	static const u16	STID_EP01_OP_BATTLE_TUTO		= 10102;	// バトルチュートリアル

	// Vol.1 アサ イベントのストーリー ID
	static const u16	STID_EP01_ASA__START_			= 10201;
	static const u16	STID_EP01_ASA_UNGETSU_BATTLE	= 10203;
	static const u16	STID_EP01_ASA__END_				= 10299;

	// Vol.1 コルポ イベントのストーリー ID
	static const u16	STID_EP01_CORPO__START_			= 10301;
	static const u16	STID_EP01_CORPO__END_			= 10399;

	// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	// ストーリー毎のイベント ID

	// StEv10101
	static const u16	EVID_STEV10101_MEET_CORPO		= 10007;	// コルポに会う
	static const u16	EVID_STEV10101_NEXT_STORY		= 10008;	// 次のストーリーへ
	static const u16	EVID_STEV10101_ITEM_PICK_TUTO	= 10023;	// アイテム拾い／捨てチュートリアル
	
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_STORY_ID_DEF_H_
