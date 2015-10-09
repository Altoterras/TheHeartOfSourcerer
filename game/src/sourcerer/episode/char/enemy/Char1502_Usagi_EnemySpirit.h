/***********************************************************************//**
 *	Char1502_Usagi_EnemySpirit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/07.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_ENEMY_CHAR1502_USAGI_ENEMY_SPIRIT_H_
#define _SRCR_EPISODE_CHAR_ENEMY_CHAR1502_USAGI_ENEMY_SPIRIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "SkilledEnemySpiritBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ウサギ敵スピリット
 *
**//*---------------------------------------------------------------------*/
class Char1502_Usagi_EnemySpirit : public SkilledEnemySpiritBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
protected:
	virtual bool create();

	virtual void makeSkillStartMessage(VcString* msg);
	virtual bool execSkillAct(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_ENEMY_CHAR1502_USAGI_ENEMY_SPIRIT_H_
