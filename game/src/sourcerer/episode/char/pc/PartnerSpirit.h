/***********************************************************************//**
 *	PartnerSpirit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_PC_PARTNER_SPIRIT_H_
#define _SRCR_EPISODE_CHAR_PC_PARTNER_SPIRIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../spirit/char/pc/PcMode.h"
#include "../../../spirit/char/pc/PcRole.h"
#include "../../../spirit/char/pc/PcSpirit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class CharStat;
class PartnerUnit;
class PartnerMode;

/*---------------------------------------------------------------------*//**
 *	準主人公（プレイヤーキャラクタ２）ロジック
 *
**//*---------------------------------------------------------------------*/
class PartnerSpirit : public PcSpirit
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	準主人公（プレイヤーキャラクタ２）ロール
	 *
	**//*-----------------------------------------------------------------*/
	class PartnerRole : public PcRole
	{
		//==================================================================
		// 定数
	public:
		enum CommandType
		{
			NOTHING = 1,		// 何もしない
			WALK_NORMAL,		// 通常歩行
			WALK_CF_AUTO,		// 対決時自動歩行
			WALK_CF_ATTACK,		// 対決時攻撃歩行
			MOVE_TO_MYU,		// 自キャラの位置へ移動
			ATTACK,				// 対象を攻撃
			GUARD_CHARGE,		// 防御とチャージ
			ATTRACT,			// 挑発
			USE_ITEM,			// アイテム使用
			PICKUP_ITEM,		// アイテム拾い
			ENTER_CF,			// 対決開始
			LEAVE_CF,			// 対決終了
		};
	
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// 自らの型のスピリットを得る
		inline PartnerSpirit* getThisSpirit() { return (PartnerSpirit*)_sprtRef; }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		PartnerRole(PartnerSpirit* sprtRef) : PcRole(sprtRef) {}

	protected:
		int execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_walkNormal(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_walkCfAuto(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_walkCfAttack(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_moveToMyu(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_attack(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_guardChage(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_attract(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	};

	/*-----------------------------------------------------------------*//**
	 *	準主人公（プレイヤーキャラクタ２）ロール管理
	 *
	**//*-----------------------------------------------------------------*/
	class PartnerMode : public PcMode
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// 自らの型のロールを得る
		inline PartnerRole* getThisRole() { return (PartnerRole*)_role; }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 外部サービス
	public:
		void toGuardCharge();

		bool action();

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		bool create(PartnerSpirit* sprtRef);

	protected:
		void notifyChildModeBegin(short modeid, void* objParamRef);

	private:
		bool judgeConfrontAutoBehavior();
	};

	//======================================================================
	// 定数
public:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ユニットを得る
	inline PartnerUnit*	getThisUnit() const { return (PartnerUnit*)getUnit(); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void startBehavior(Behavior behv, const BehaviorParam* bparam);
	void breakBehavior(Behavior behv, bool isByGui, bool duetoConfu);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	PartnerSpirit();
	virtual bool	create();
	virtual void	exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御

protected:
	void onChangeBehavior(Spirit::Behavior behvNew, Spirit::Behavior behvOld, bool isBreak, bool duetoConfu);

	//======================================================================
	// 変数
protected:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_CHAR_PC_IMPLEMENT_PARTNER_SPIRIT_H_
