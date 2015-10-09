/***********************************************************************//**
 *	HelperSpirit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/23.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_PC_HELPER_SPIRIT_H_
#define _SRCR_EPISODE_CHAR_PC_HELPER_SPIRIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../spirit/char/pc/PcMode.h"
#include "../../../spirit/char/pc/PcRole.h"
#include "../../../spirit/char/pc/PcSpirit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class CharStat;
class HelperUnit;
class HelperMode;

/*---------------------------------------------------------------------*//**
 *	ヘルパー ロジック
 *
**//*---------------------------------------------------------------------*/
class HelperSpirit : public PcSpirit
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	ヘルパー ロール
	 *
	**//*-----------------------------------------------------------------*/
	class HelperRole : public PcRole
	{
		//======================================================================
		// 定数
	public:
		enum CommandType
		{
			NOTHING = 1,		// 何もしない
			WALK_NORMAL,		// 通常歩行
			CURE_PARTY,			// パーティ回復
			MEDITATION,			// 瞑想
			CHANGE_ATRB,		// 形態変更
			ENTER_CF,			// 対決開始
			LEAVE_CF,			// 対決終了
		};
	
		//======================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// 自らの型のスピリットを得る
		inline HelperSpirit* getThisSpirit() { return (HelperSpirit*)_sprtRef; }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		HelperRole(HelperSpirit* sprtRef) : PcRole(sprtRef) {}

	protected:
		int execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_cureParty(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_meditation(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_changeAtrb(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	};

	/*-----------------------------------------------------------------*//**
	 *	ヘルパー ロール管理
	 *
	**//*-----------------------------------------------------------------*/
	class HelperMode : public PcMode
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// 自らの型のロールを得る
		inline HelperRole* getThisRole() { return (HelperRole*)_role; }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 外部サービス
	public:
		void toCure();

		bool action();

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		bool create(HelperSpirit* sprtRef);

	protected:
		void notifyChildModeBegin(short modeid, void* objParamRef);

	private:
		bool judgeConfrontAutoBehavior();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ユニットを得る
	inline HelperUnit* getThisUnit() const { return (HelperUnit*)getUnit(); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:

	void startBehavior(Behavior behv, const BehaviorParam* bparam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	HelperSpirit();
	bool create();

protected:
	void onChangeBehavior(Spirit::Behavior behvNew, Spirit::Behavior behvOld, bool isBreak, bool duetoConfu);

	//======================================================================
	// 変数
protected:
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_PC_HELPER_SPIRIT_H_
