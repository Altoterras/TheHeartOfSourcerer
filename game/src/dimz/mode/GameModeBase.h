/***********************************************************************//**
 *	GameModeBase.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/06/29.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _DIMZ_MODE_GAME_MODE_BASE_H_
#define _DIMZ_MODE_GAME_MODE_BASE_H_

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

DIMZ_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	ゲームモード基底クラス
 *
**//*---------------------------------------------------------------------*/
class GameModeBase : public Mode
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// プロパティ

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	GameModeBase();
	virtual void notifyChangedMap(bool isDeserializeCase);
	void notifyChildModeChanged(short modeid, void* objParamRef);

protected:
	bool create(short modeidStart, short modeidEnd, Role* roleSelf);
	virtual bool registerMode(short modeid, Mode* mode);
	virtual bool changeModeDive(short modeid);
	short getLastReservedModeDive();

	//======================================================================
	// 変数
private:
	short _modeidStart;
	short _modeidEnd;
};

DIMZ_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _DIMZ_MODE_GAME_MODE_BASE_H_
