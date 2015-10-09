/***********************************************************************//**
 *	Char1406_HighKabutomushi_EnemySpirit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2011/01/07.
 *	Copyright 2011 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_ENEMY_CHAR1406_HIGHT_KABUTOMUSHI_ENEMY_SPIRIT_H_
#define _SRCR_EPISODE_CHAR_ENEMY_CHAR1406_HIGHT_KABUTOMUSHI_ENEMY_SPIRIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "SkilledEnemySpiritBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	大カブトムシ敵スピリット
 *
**//*---------------------------------------------------------------------*/
class Char1406_HighKabutomushi_EnemySpirit : public SkilledEnemySpiritBase
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Char1406_HighKabutomushi_EnemySpirit();

protected:
	virtual bool create();

	virtual bool execSkillPrepare(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep);
	virtual void makeSkillStartMessage(VcString* msg);
	virtual bool execSkillAct(ExecRes* res, const ExecCtx* ec, s32 cntStep, f32 frmcntStep);

	//======================================================================
	// 変数
private:
	s32 _cntCharge;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_ENEMY_CHAR1406_HIGHT_KABUTOMUSHI_ENEMY_SPIRIT_H_
