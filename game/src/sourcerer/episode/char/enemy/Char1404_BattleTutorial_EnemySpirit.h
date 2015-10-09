/***********************************************************************//**
 *	Char1404_BattleTutorial_EnemySpirit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_ENEMY_CHAR1404_BATTLE_TUTORIAL_ENEMY_SPIRIT_H_
#define _SRCR_EPISODE_CHAR_ENEMY_CHAR1404_BATTLE_TUTORIAL_ENEMY_SPIRIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "Char1502_Usagi_EnemySpirit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class CharStat;
class EnemyUnit;
class EnemyMode;

/*---------------------------------------------------------------------*//**
 *	バトルチュートリアル敵スピリット
 *
**//*---------------------------------------------------------------------*/
class Char1404_BattleTutorial_EnemySpirit : public Char1502_Usagi_EnemySpirit
{
	//======================================================================
	// 定数
public:
	// チュートリアルのステップ
	enum TutoStep
	{
		TSTEP_FIRST,
		TSTEP_ATTACK_1,
		TSTEP_WAIT_1,
		TSTEP_ATTACK_2,
		TSTEP_SKILL_1,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setStep(TutoStep tstep);
	s16 decideNextConfrontAction();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Char1404_BattleTutorial_EnemySpirit();
	void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition);
	void notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat);

	//======================================================================
	// 変数
protected:
	TutoStep _tstep;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_ENEMY_CHAR1404_BATTLE_TUTORIAL_ENEMY_SPIRIT_H_
