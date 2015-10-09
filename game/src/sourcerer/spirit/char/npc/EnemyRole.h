/***********************************************************************//**
 *	EnemyRole.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SPIRIT_CHAR_NPC_ENEMY_ROLE_H_
#define _SRCR_SPIRIT_CHAR_NPC_ENEMY_ROLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../tfw/modemng/Role.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

TFW_END_NS

SRCR_BEGIN_NS

class Confront;
class Game;
class EnemyUnit;
class EnemySpirit;
class ActStat;

/*---------------------------------------------------------------------*//**
 *	敵ロール
 *
**//*---------------------------------------------------------------------*/
class EnemyRole : public Role
{
	//======================================================================
	// 定数
public:
	enum CommandType
	{
		WALK_NORMAL = 1,	// 通常歩行
		WALK_STANDBY,		// 待機時歩行
		WALK_CF_DEFENSE,	// 対決時防御歩行
		WALK_CF_ATTACK,		// 対決時攻撃歩行
		MOVE_TO_TRG,		// ターゲットの位置へ移動
		MOVE_TO_HOME,		// 元位置へ移動
		MOVE_AFTER_ATTACK,	// 攻撃後の移動
		ATTACK,				// 対象を攻撃
		GUARD,				// 攻撃をガード
		SKILL,				// 技を発動
	};
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 対決モジュールを設定する
	inline void setConfrontModule(Confront* confrontRef)	{	_confrontRef = confrontRef;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	explicit EnemyRole(EnemySpirit* sprtRef);
	~EnemyRole();

protected:
	bool create(Mode* modeOwnerRef);
	void destroy();
	int execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param);

protected:
	virtual int execCommand_walkNormal(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_walkStandby(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_walkCfDefense(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_walkCfAttack(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_moveToTrg(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_moveToHome(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_moveAfterAttack(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_attack(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_guard(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_skill(ExecRes* res, const ExecCtx* ec, const CmdParam* param);

	bool execWalkToTarget(const ExecCtx* ec);

	//======================================================================
	// 変数
protected:
	EnemySpirit*		_sprtRef;
	Confront*			_confrontRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_CHAR_NPC_ENEMY_ROLE_H_
