/***********************************************************************//**
 *	StEmp_10102_1001_BattleTutorial01.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/07/22.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10102_1001_BATTLE_TUTORIAL01_H_
#define _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10102_1001_BATTLE_TUTORIAL01_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../StEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class EvMatter;

TFW_END_NS

SRCR_BEGIN_NS

class Char1404_BattleTutorial_EnemySpirit;
class PartnerSpirit;
class PartnerUnit;
class SimpleSprite;

/*---------------------------------------------------------------------*//**
 *	 St10102 戦闘（対決）チュートリアル 1
 *
 *
**//*---------------------------------------------------------------------*/
class StEmp_10102_1001_BattleTutorial01 : public StEmp
{
	//======================================================================
	// 定数
private:
	enum Step
	{
		STEP_NULL,
		STEP_SPRITE_SOUMA_1,
		STEP_DO_SOUMA_1,
		STEP_SHOW_RESULT_SOUMA_1,
		STEP_MSG_RESULT_SOUMA_1,
		STEP_ENEMY_ATTACK_1,
		STEP_MSG_ENEMY_ATTACK_1,
		STEP_PC2_ATTACK_1,
		STEP_MSG_PC2_ATTACK_1,
		STEP_SPRITE_PC2_ATTACK_1,
		STEP_MSG_SOUMA_2,
		STEP_SPRITE_PC2WEP_FOCUS,
		STEP_WAIT_PC2WEP_FOCUS,
		STEP_SPRITE_SOUMA_2,
		STEP_WAIT_SOUMA_2,
		STEP_SHOW_FAIL_SOUMA_2,
		STEP_MSG_FAIL_SOUMA_2,
		STEP_SHOW_SUCCESS_SOUMA_2,
		STEP_MSG_SUCCESS_SOUMA_2,
		STEP_ENTER_FREE_BATTLE_1,
		STEP_DO_FREE_BATTLE_1,
		STEP_WAIT_SKILL_1,
		STEP_SHOW_RESULT_SKILL_1,
		STEP_MSG_RESULT_SKILL_1,
		STEP_SPRITE_PHYS_DAMAGE_1,
		STEP_DO_FREE_BATTLE_2,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void begin(u32 param);
	void end();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StEmp_10102_1001_BattleTutorial01();
	~StEmp_10102_1001_BattleTutorial01();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);
	void draw(const RenderCtx* rc);

	void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);
	void notifyDisappearMagicCluster(MagicCluster* mc, u32 dcflags);

private:
	void readTexture(u32 fileid);
	void setEnablePc2MenuExceptWeapon(bool isEnable);
	void jumpWaterEnchantmentSoumaEvent();

	//======================================================================
	// 変数
protected:
	SimpleSprite* _sprite;
	const EvMatter* _evmatter;
	Char1404_BattleTutorial_EnemySpirit* _sprtEnemy;
	PartnerSpirit* _sprtPtnr;
	Unit* _unitPtnrWeapon;
	f32 _frameElapsedWk;
	f32 _frameElapsedStep;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10102_1001_BATTLE_TUTORIAL01_H_
