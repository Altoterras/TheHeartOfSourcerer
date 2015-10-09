/***********************************************************************//**
 *	Char1504_Shika_EnemySpirit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/02.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_ENEMY_CHAR1504_SHIKA_ENEMY_SPIRIT_H_
#define _SRCR_EPISODE_CHAR_ENEMY_CHAR1504_SHIKA_ENEMY_SPIRIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "SkilledEnemySpiritBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	シカ敵スピリット
 *
**//*---------------------------------------------------------------------*/
class Char1504_Shika_EnemySpirit : public SkilledEnemySpiritBase
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

#endif	// _SRCR_EPISODE_CHAR_ENEMY_CHAR1504_SHIKA_ENEMY_SPIRIT_H_
