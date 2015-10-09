/***********************************************************************//**
 *	IngameRole.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_INGAME_INGAME_ROLE_H_
#define _SRCR_MODE_INGAME_INGAME_ROLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"
#include "../../../tfw/modemng/Role.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Game;

/*---------------------------------------------------------------------*//**
 *	モデルビュー ロール クラス
 *
**//*---------------------------------------------------------------------*/
class IngameRole : public Role
{
	//======================================================================
	// 定数
public:
	enum CommandType
	{
		START,
		END,
	};
	
	//======================================================================
	// メソッド
public:
	int execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param);

	int execCommand_start(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	int execCommand_end(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_INGAME_INGAME_ROLE_H_
