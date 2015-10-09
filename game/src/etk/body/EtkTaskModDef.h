/***********************************************************************//**
 *	EtkTaskModDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/05/29.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_BODY_ETK_TASK_MOD_DEF_H_
#define _ETK_BODY_ETK_TASK_MOD_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	タスクの定義
 *
**//*---------------------------------------------------------------------*/
class EtkTaskModDef
{
	//======================================================================
	// 定数
public:
	// タスクモジュールオーダー ID
	enum EtkModuleOrderId
	{
		MOD__BASIC_				= 10000,
		// 3D シーン
		MOD__3D_				=  1000,
		MOD_ACTMSG_WND			=  1801,
		MOD_SCREFF_SCENE		=  2001,
		// 2D 表示物
		MOD__2D_				=  3000,
		// イベント（チュートリアル用に 2D 表示物より後に描く必要がある）
		MOD__EVENTSCENE_		=  4000,
		// システム表示物
		MOD__SYS_				=  5000,
		MOD_SYS_MSG_WND			=  5101,
		MOD_SCREFF_WHOLE		=  5501,
	};

	// タスクモジュール汎用フラグ
	enum EtkModuleGeneralFlags
	{
		MGF_WINDOW = 0x0001,	// Window である
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
	static s32 calcExecOrder(s32 modid) { return EtkTaskModDef::MOD__BASIC_ - modid; }
	static s32 calcDrawOrder(s32 modid) { return EtkTaskModDef::MOD__BASIC_ + modid; }
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_BODY_ETK_TASK_MOD_DEF_H_
