/***********************************************************************//**
 *	SkilledEnemySpiritBase.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/03/02.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_ENEMY_SKILLED_ENEMY_SPIRIT_BASE_H_
#define _SRCR_EPISODE_CHAR_ENEMY_SKILLED_ENEMY_SPIRIT_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../spirit/char/npc/EnemyRole.h"
#include "../../../spirit/char/npc/EnemySpirit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	汎用スキル敵スピリット基底クラス
 *
**//*---------------------------------------------------------------------*/
class SkilledEnemySpiritBase : public EnemySpirit
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	汎用スキル敵ロール
	 *
	**//*-----------------------------------------------------------------*/
	class SkilledEnemyRole : public EnemyRole
	{
	public:
		SkilledEnemyRole(EnemySpirit* sprtRef) : EnemyRole(sprtRef) {}
	protected:
		int execCommand_skill(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual s16 decideNextConfrontAction();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
protected:
	SkilledEnemySpiritBase();
	virtual bool create();

	virtual bool execCheckReach(ExecRes* res, const ExecCtx* ec, s32 step, s32 cntStep, f32 frmcntStep);
	virtual bool execSkillPrepare(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep);
	virtual bool execSkillStart(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep);
	virtual void makeSkillStartMessage(VcString* msg) {}
	virtual bool execSkillAct(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep);
	virtual bool execSkillWait(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep);

	//======================================================================
	// 変数
protected:
	f32 _reach;
	f32 _reachSq;
	u16 _animidSkillStart;
	u16 _seidSkillStart;
	u16 _ptcldidSkillStart;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_ENEMY_SKILLED_ENEMY_SPIRIT_BASE_H_
