/***********************************************************************//**
 *	GameTaskModDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/12/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_BODY_GAME_TASK_MOD_DEF_H_
#define _SRCR_BODY_GAME_TASK_MOD_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../../etk/body/EtkTaskModDef.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	タスクの定義
 *
**//*---------------------------------------------------------------------*/
class GameTaskModDef : public EtkTaskModDef
{
	//======================================================================
	// 定数
public:
	enum GameModuleOrder
	{
		MOD__BASIC_				= 10000,
		// 3D シーン
		MOD__3D_				=  1000,
		MOD_FOCUSCURSOR			=  1101,
		MOD_MOVECURSOR			=  1201,
		MOD_INGAMEFRONT			=  1301,
		MOD_CHARSTATGAUGE		=  1401,
		MOD_ARIA_WND			=  1501,
		MOD_GAUGEANIMEFF		=  1601,
		MOD_FOCUSMENU			=  1701,
//		MOD_CFMSG_WND			=  1801,
		MOD_SUBTITLES_WND		=  1901,
		// 2D 表示物
		MOD__2D_				=  3000,
		MOD_INTVL_SAVE_WND		=  3101,
		MOD_SOURCEEDIT_WND		=  3201,
		MOD_MAGICLIST_WND		=  3301,
		MOD_ITEMLIST_WND		=  3401,
		// イベント（チュートリアル用に 2D 表示物より後に描く必要がある）
		MOD__EVENTSCENE_		=  4000,
		MOD_EVENT				=  4101,
		MOD_EVENTMVFRONT		=  4201,
		// システム表示物
		MOD__SYS_				=  5000,
//		MOD_SYS_MSG_WND			=  5101,
	};
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_BODY_GAME_TASK_MOD_DEF_H_
