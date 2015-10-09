/***********************************************************************//**
 *	Char1403_HighSaru_EnemySpirit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/02.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_ENEMY_CHAR1403_HIGH_SARU_ENEMY_SPIRIT_H_
#define _SRCR_EPISODE_CHAR_ENEMY_CHAR1403_HIGH_SARU_ENEMY_SPIRIT_H_

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
class Char1403_HighSaru_EnemySpirit : public SkilledEnemySpiritBase
{
	//======================================================================
	// 定数
private:
	enum SkillPhase
	{
		SP_NULL,
		SP_PREPARE,
		SP_CHARGE_START,
		SP_CHARGING,
		SP_HIT_1,
		SP_HIT_2,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Char1403_HighSaru_EnemySpirit();
	virtual bool create();

protected:
	virtual bool execSkillPrepare(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep);
	virtual bool execSkillStart(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep);
	virtual bool execSkillAct(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep);

	//======================================================================
	// 変数
private:
	s32 _phaseSkill;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_ENEMY_CHAR1403_HIGH_SARU_ENEMY_SPIRIT_H_
