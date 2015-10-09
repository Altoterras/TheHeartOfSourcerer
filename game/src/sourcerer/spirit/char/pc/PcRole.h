/***********************************************************************//**
 *	PcRole.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SPIRIT_CHAR_PC_PC_ROLE_H_
#define _SRCR_SPIRIT_CHAR_PC_PC_ROLE_H_

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
class PcSpirit;
class ActStat;

/*---------------------------------------------------------------------*//**
 *	プレイヤー キャララクター ロール
 *
**//*---------------------------------------------------------------------*/
class PcRole : public Role
{
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
	PcRole(PcSpirit* sprtRef);
	virtual ~PcRole();

protected:
	virtual int execCommand_nothing(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_walkNormal(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_useItem(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_pickupItem(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_enterCf(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	virtual int execCommand_leaveCf(ExecRes* res, const ExecCtx* ec, const CmdParam* param);

	//======================================================================
	// 変数
protected:
	PcSpirit*			_sprtRef;
	Confront*			_confrontRef;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SPIRIT_CHAR_PC_PC_ROLE_H_
