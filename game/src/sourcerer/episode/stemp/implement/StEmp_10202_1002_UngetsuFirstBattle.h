/***********************************************************************//**
 *	StEmp_10202_1002_UngetsuFirstBattle.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/08/06.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10202_1002_UNGETSU_FIRST_BATTLE_H_
#define _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10202_1002_UNGETSU_FIRST_BATTLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../StEmp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	雲月岩との最初の戦闘
 *
**//*---------------------------------------------------------------------*/
class StEmp_10202_1002_UngetsuFirstBattle : public StEmp
{
	//======================================================================
	// 定数
public:

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
	StEmp_10202_1002_UngetsuFirstBattle();

	//======================================================================
	// 変数
protected:
	f32	_fcntBattle;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_STEMP_IMPLEMENT_ST_EMP_10202_1002_UNGETSU_FIRST_BATTLE_H_
