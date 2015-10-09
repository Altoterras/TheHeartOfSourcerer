/***********************************************************************//**
 *	WalkingRole.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/12.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_INGAME_WALKING_WALKING_ROLE_H_
#define _SRCR_MODE_INGAME_WALKING_WALKING_ROLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../Base.h"
#include "../../../../tfw/modemng/Role.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class Game;

/*---------------------------------------------------------------------*//**
 *	歩行ロール
 *
**//*---------------------------------------------------------------------*/
class WalkingRole : public Role
{
	//======================================================================
	// 定数
public:
	enum CommandType
	{
		STABLE,			// 定常
	};
	
	//======================================================================
	// メソッド
protected:
	int execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_INGAME_WALKING_WALKING_ROLE_H_
