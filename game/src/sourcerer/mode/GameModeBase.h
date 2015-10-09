/***********************************************************************//**
 *	GameModeBase.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_GAME_MODE_BASE_H_
#define _SRCR_MODE_GAME_MODE_BASE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "GameModeParam.h"
#include "../../tfw/modemng/Mode.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Role;

TFW_END_NS

SRCR_BEGIN_NS

class StatusChangeResult;
class Game;
class Souma;
class TransStat;
class Unit;

/*---------------------------------------------------------------------*//**
 *	ゲームモード基底クラス
 *
**//*---------------------------------------------------------------------*/
class GameModeBase : public Mode
{
	//======================================================================
	// 定数
public:
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
#if defined(_DEBUG)
	void straceModeStatus();
	void straceModeTreeDive(int dive = 0);
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	GameModeBase();
	virtual void notifyChangedMap(bool isDeserializeCase);
	virtual void notifyDischargeSouma(Souma* souma, u16 mcid);
	virtual void notifyUnitInfluenceStat(Unit* unitRecv, const StatusChangeResult* scres, const TransStat* tstat);
	void notifyChildModeChanged(short modeid, void* objParamRef);

protected:
	bool create(short modeidStart, short modeidEnd, Role* roleSelf);
	virtual bool registerMode(short modeid, Mode* mode);
	virtual short changeModeDive(short modeid);
	virtual short getDefaultChildMode() { return 0; }
	short getLastReservedModeDive();
	short reserveDefaultChildModeDive(short modeid, int divecnt);
	void changeChildMode(short modeid);

private:
	void changeMode(short modeid, bool isReset)	{ Mode::changeMode(modeid, isReset); }
	void changePrevMode()						{ Mode::changePrevMode(); }
	void reserveMode(short modeid)				{ Mode::reserveMode(modeid); }
	void reserveModeCancel(short modeidCancel)	{ Mode::reserveModeCancel(modeidCancel); }
	void changeFromReservedMode()				{ Mode::changeFromReservedMode(); }

	//======================================================================
	// 変数
private:
	short _modeidStart;
	short _modeidEnd;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_GAME_MODE_BASE_H_
