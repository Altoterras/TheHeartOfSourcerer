/***********************************************************************//**
 *	QuestDefTbl.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/25.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_QUEST_QUEST_DEF_TBL_H_
#define _SRCR_QUEST_QUEST_DEF_TBL_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "QuestDef.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	パーティクル定義テーブル
 *	
**//*---------------------------------------------------------------------*/
class QuestDefTbl
{
	//======================================================================
	// 定数
private:
	static const s32	NUM_QUESTDEF_MAX	= 256;

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	const QuestDef* getDef(u16 questid) const;

	s32 getDefNum() const { return NUM_QUESTDEF_MAX; }
	const QuestDef* getDefFromIndex(s32 index) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QuestDefTbl();
	~QuestDefTbl();
	bool init(u32 fileidData, u32 fileidTxt);
	void cleanup();

	//======================================================================
	// 変数
private:
	QuestDef _qsdefarr[NUM_QUESTDEF_MAX];
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_QUEST_QUEST_DEF_TBL_H_
