/***********************************************************************//**
 *	StEmp_10203_1001_UngetsuSecondBattle.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/11.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10203_1001_UNGETSU_SECOND_BATTLE_H_
#define _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10203_1001_UNGETSU_SECOND_BATTLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../StEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	雲月岩との２回目の戦闘
 *
**//*---------------------------------------------------------------------*/
class StEmp_10203_1001_UngetsuSecondBattle : public StEmp
{
	//======================================================================
	// 定数
public:

private:
	enum SkitStep
	{
		SSTEP_NULL,		// 初期
		SSTEP_DELAY,	// 実行遅延
		SSTEP_REQ,		// イベント実行要求
		SSTEP_DONE,		// イベント実行後
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
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StEmp_10203_1001_UngetsuSecondBattle();
	void notifyUnitInfluenceStat(Unit* unitRecvRecv, const StatusChangeResult* scres, const TransStat* tstat);

	//======================================================================
	// 変数
protected:
	f32	_fcntBattle;
	f32 _fcntCool;
	SkitStep _sstepCorpoConfuEvent;
	SkitStep _sstepCorpoCoolEvent;
	bool _isLockOpPrevFrame;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10203_1001_UNGETSU_SECOND_BATTLE_H_
