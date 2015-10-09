/***********************************************************************//**
 *	ConfrontRole.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/01/25.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_INGAME_CONFRONT_CONFRONT_ROLE_H_
#define _SRCR_MODE_INGAME_CONFRONT_CONFRONT_ROLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"

#include "../../../../tfw/modemng/Role.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Confront;
class ConfrontCam;
class Game;
class Unit;
class UnitArray;

/*---------------------------------------------------------------------*//**
 *	対決ロール
 *
**//*---------------------------------------------------------------------*/
class ConfrontRole : public Role
{
	//======================================================================
	// 定数
public:
	enum CommandType
	{
		START,			// 開始
		STABLE,			// 定常
		CONCLUSION,		// 決着
		END,			// 終了
	};
	
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 対決モジュールを設定する
	inline void setConfrontModule(Confront* confrontRef) { _confrontRef = confrontRef; }
	// カメラを設定する
	inline void setCamera(ConfrontCam* camCfRef) { _camCfRef = camCfRef; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ConfrontRole() : _confrontRef(0L), _camCfRef(0L) { }

protected:
	int execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	int execCommand_start(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	int execCommand_stable(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	int execCommand_conclusion(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	int execCommand_end(ExecRes* res, const ExecCtx* ec, const CmdParam* param);

private:
	s32 countEnemy(const Unit** unitEnemyFirst) const;

	//======================================================================
	// メンバ変数
private:
	Confront*		_confrontRef;		// 対決モジュール
	ConfrontCam*	_camCfRef;			// 対決モード専用カメラ
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_INGAME_CONFRONT_CONFRONT_ROLE_H_
